/* ------------------------------------------------------------------------- /
 *
 * Module: muPtztracking.c
 * Author: Joe Lin
 * Create date: 03/22/2012
 *
 * Description:
 *    This application is presented the PTZ Get Target by image difference.
 *
 -------------------------------------------------------------------------- */
 
#include "muGadget.h"

#define DEBUG_W 0
#define CHOOSE_DENSITY 0

#define MANUAL_WIDTH	26
#define MANUAL_HEIGHT	40

/* For Debug */
static FILE *img = NULL;
static FILE *img1 = NULL;
static FILE *img2 = NULL;



static int g_frame_count = 0, g_no_seq_count = 0;
static int g_maximum_size;



MU_8S gKernel[9] = {7,7,7,7,8,7,7,7,7};
static muSeq_t *g_head_tracking_seq = NULL;
static muImage_t *g_mhi_img = NULL, *g_bg = NULL;
static MU_32S g_width, g_height;
/*     --[seq]-->[seq1]-->[seq2]--> ... -->[seqN]
 *                 |         |               |
 *              [objseq]  [objseq]        [objseq]
 *                 |         |               |
 *               [obj1]    [obj1]          [obj1]
 *                 |         |               |
 *               [obj2]    [obj2]          [obj2]
 *                 |         |               |
 *               [objN]    [objN]          [objN]
 *
 *       3 conditions 
 *       1-> g_tracking_seq = NULL
 *       2-> g_tracking_seq != NULL (tracking judgement enable)
 *       3-> g_tracking_seq IDLE (delete linker list)
*/

typedef struct _seq_info_t
{
	int seq_no;
	int target;
	int overlap_count;
	float density;
	MU_32S area;
	muBoundingBox_t box;
	muImage_t *sub_bin;
	muImage_t *sub_yuv422;
}seq_info_t;


static MU_VOID show_seq(muSeq_t *seq)
{
	muSeqBlock_t *seq_head;
	seq_info_t *seq_info;

	if(seq)
	{
		seq_head = seq->first;
		
		while(seq_head)
		{
			seq_info = (seq_info_t *)seq_head->data;
			MU_DBG("seq_no = %d target = %d area = %d  box x =%d box y = %d  overlap count =%d\n", seq_info->seq_no, seq_info->target, seq_info->area, seq_info->box.minx, seq_info->box.miny, seq_info->overlap_count);
			seq_head = seq_head->next;
		}

	}
}


MU_VOID delete_seq(muSeq_t *seq)
{
	muSeqBlock_t *seq_block;
	seq_info_t *seq_info;
	muImage_t *sub_bin_temp;
	muImage_t *sub_yuv422_temp;

	if(seq == NULL)
	{
		MU_DBG("seq = NULL \n");
		return;
	}

	//MU_DBG("delete seq\n");

	seq_block = seq->first;

	while(seq_block)
	{
		seq_info = (seq_info_t *)seq_block->data;
		
		sub_bin_temp = seq_info->sub_bin;
		sub_yuv422_temp = seq_info->sub_yuv422;


		if(sub_bin_temp != NULL)
		muReleaseImage(&sub_bin_temp);

		if(sub_yuv422_temp != NULL)
		muReleaseImage(&sub_yuv422_temp);

		
		seq_block = seq_block->next;
	}

	muClearSeq(&seq);
}



static MU_32S find_target(muSeq_t *seq)
{
	MU_32S target = 0;
	muSeqBlock_t *seq_block;
	seq_info_t *seq_info;
	float max_density = 0;
	MU_32S max_area = 0;
	MU_32S target_seq_no;

	if(seq ==  NULL)
	{
		MU_DBG("seq  = NULL\n");
		return -1;
	}

	if(seq)
	{
		seq_block = seq->first;

		while(seq_block)
		{
			seq_info = (seq_info_t *)seq_block->data;

			if(seq_info->seq_no > 5)
			{
#if CHOOSE_DENSITY
				if(seq_info->density > max_density)
				{
					max_density = seq_info->density;
					target_seq_no = seq_info->seq_no;
				}
#else
				if(seq_info->area > max_area)
				{
					max_density = seq_info->density;
					max_area = seq_info->area;
					target_seq_no = seq_info->seq_no;
				}
#endif
			}

			if(seq_info->overlap_count > 25)
			{
				target = 1;
			}

			seq_block = seq_block->next;
		}
	}

	if(target)
	{
		MU_DBG("[MU_PTZTARGET]find target seq = %d\n", target_seq_no);
		return target_seq_no;
	}

	return -1;
}

#define BEST_ROI	1
#define LAST_ROI	0

static MU_VOID get_target_info(MU_32S seq_no, muSeq_t *seq, muPtzTargetInfo_t *info)
{
 	muSeqBlock_t *seq_block;
	seq_info_t *seq_info;
	muImage_t *sub_bin;
	muImage_t *sub_yuv422;
	MU_32S width, height;
	muBoundingBox_t box;


	seq_block = seq->first;

	while(seq_block)
	{
		seq_info = (seq_info_t *)seq_block->data;

#if BEST_ROI
		if(seq_info->seq_no == seq_no)
		{
			sub_bin = seq_info->sub_bin;
			sub_yuv422 = seq_info->sub_yuv422;

			width = sub_bin->width;
			height = sub_bin->height;
			
			info->bin = muCreateImage(muSize(width, height), MU_IMG_DEPTH_8U, 1);
			info->yuv422 = muCreateImage(muSize(width, height), MU_IMG_DEPTH_8U, 3);

			memcpy(info->bin->imagedata, sub_bin->imagedata, width*height);
			memcpy(info->yuv422->imagedata, sub_yuv422->imagedata, width*height*3);
				
		}
#endif

		// ROI of currently Image
		if(seq_block->next == NULL)
		{

#if LAST_ROI
			sub_bin = seq_info->sub_bin;
			sub_yuv422 = seq_info->sub_yuv422;

			width = sub_bin->width;
			height = sub_bin->height;
			
			info->bin = muCreateImage(muSize(width, height), MU_IMG_DEPTH_8U, 1);
			info->yuv422 = muCreateImage(muSize(width, height), MU_IMG_DEPTH_8U, 3);

			memcpy(info->bin->imagedata, sub_bin->imagedata, width*height);
			memcpy(info->yuv422->imagedata, sub_yuv422->imagedata, width*height*3);
			
#endif
			memcpy(&box, &seq_info->box, sizeof(muBoundingBox_t));
			info->rect.x = box.minx;
			info->rect.y = box.miny;
			info->rect.width = box.width;
			info->rect.height = box.height;
		}

		seq_block = seq_block->next;
	}

}

MU_VOID muPtzGetTargetReset()
{
	if(g_head_tracking_seq != NULL)
	{
		delete_seq(g_head_tracking_seq);
	}

	g_head_tracking_seq = NULL;
	g_frame_count  = 0;
	g_no_seq_count = 0;
}


// release recorded sequence
MU_VOID muPtzGetTargetInit(MU_32S width, MU_32S height)
{
	g_width = width;
	g_height = height;

#if OPTICAL_FLOW
#if defined(HISI3516) || defined(HISI3531)
	if(g_mhi_img == NULL)
	g_mhi_img = muCreateImage(muSize(width,height), MU_IMG_DEPTH_8U, 1);
#else
	if(g_mhi_img == NULL)
	g_mhi_img = muCreateImage(muSize(width,height), MU_IMG_DEPTH_8U, 1);
#endif
#endif

	g_bg = muCreateImage(muSize(width, height), MU_IMG_DEPTH_8U, 1);
	muBackgroundModelingInit(width, height, MU_BGM_ISB);

	g_maximum_size = (width*height/14);

#if DEBUG_W
	memset(name, 0, 50);
	MU_DBG(name, "%d.yuv", g_frame_count);

	if(img != NULL)
	{
		fclose(img);
		img = NULL;
	}

	if(img1 != NULL)
	{
		fclose(img1);
		img1 = NULL;
	}

	if(img2 != NULL)
	{
		fclose(img2);
		img2 = NULL;
	}

	if(img == NULL)
	{
		img = fopen("176x120-1.yuv", "wb");
	}

	if(img1 == NULL)
	{
		img1 = fopen("176x120-2.yuv", "wb");
	}

	if(img2 == NULL)
	{
		img2 = fopen("176x120-3.yuv", "wb");
	}
#endif

}


// cpnt_seq = after connected component sequence
static MU_8S tracking_judgement(muSeq_t *cpnt_seq, const muImage_t *yuv422_img, muImage_t *bin_img, muPtzTargetInfo_t *info)
{
	muSeqBlock_t *box_seq, *seq_block;
	muSeqBlock_t *curseq;
	muBoundingBox_t *box, *box_temp;
	muSeq_t *seq;
	seq_info_t *seq_info;
	seq_info_t seq_info_temp;
	MU_32S target_seq;
	MU_32S max_area = 0, overlap_size, max_overlap_size = 0;
	MU_32F density, max_density = 0;
	MU_8S target = 0;
	muRect_t rect;
	MU_32S w,h;
	MU_32S area;
	muImage_t *sub_bin_temp, *sub_yuv422_temp;
	MU_32S b_width, b_height, b_area, b_minx, b_miny;
	MU_32S b_center_x, b_center_y;
	MU_32F ratio, s_ratio;


	seq = g_head_tracking_seq;

	w = yuv422_img->width;
	h = yuv422_img->height;


	// first step don't care aobut the object refining
	if(seq == NULL && (cpnt_seq->first) != NULL)
	{
		seq = muCreateSeq(sizeof(seq_info_t));

		curseq = muPushSeq(seq, NULL);

		seq_info = (seq_info_t *)curseq->data;

		// analyze candidate
		box_seq = cpnt_seq->first;
		while(box_seq)
		{
			box = (muBoundingBox_t*)box_seq->data;

			density = (box->area/(float)(box->width*box->height));
			area = box->width*box->height;
#if CHOOSE_DENSITY
			if(density > max_density)
			{
				seq_info->density = density;
				seq_info->seq_no = 1;
				seq_info->target = 0;
				seq_info->overlap_count = 0;
				seq_info->sub_bin = NULL;
				seq_info->sub_yuv422 = NULL;
				box_temp = (muBoundingBox_t *)&seq_info->box;
				memcpy(box_temp, box, sizeof(muBoundingBox_t));
			}
#else
			if(area > max_area)
			{
				seq_info->density = density;
				seq_info->area = area;
				seq_info->seq_no = 1;
				seq_info->target = 0;
				seq_info->overlap_count = 0;
				seq_info->sub_bin = NULL;
				seq_info->sub_yuv422 = NULL;
				box_temp = (muBoundingBox_t *)&seq_info->box;
				memcpy(box_temp, box, sizeof(muBoundingBox_t));
			}
#endif
			box_seq = box_seq->next;
		}

		// Get Sub Image 
		
		// just support even number
		if(box_temp->width%2 != 0)
		{
			box_temp->width = box_temp->width+1;
			if(box_temp->width > w)
			box_temp->width = w;
		}

		if(box_temp->height%2 != 0)
		{
			box_temp->height = box_temp->height+1;
			if(box_temp->height > h)
				box_temp->height = h;
		}

		if(box_temp->minx%2 != 0)
		{
			box_temp->minx = box_temp->minx-1;
			if(box_temp->minx < 0)
			box_temp->minx = 0;
		}

		if(box_temp->miny%2 != 0)
		{
			box_temp->miny = box_temp->miny+1;
			if(box_temp->miny > h)
			box_temp->miny = h;
		}

		if((box_temp->miny+box_temp->height) > h)
		{
			box_temp->height -= 2;
		}
		
		seq_info->sub_bin = muCreateImage(muSize(box_temp->width, box_temp->height), MU_IMG_DEPTH_8U, 1);
		seq_info->sub_yuv422 = muCreateImage(muSize(box_temp->width, box_temp->height), MU_IMG_DEPTH_8U, 3);
			
		rect = muRect(box_temp->minx, box_temp->miny, box_temp->width, box_temp->height);
		
		sub_bin_temp = seq_info->sub_bin;
		sub_yuv422_temp = seq_info->sub_yuv422;

		muGetSubImage(bin_img, sub_bin_temp, rect);
		muGetSubYUV422Image(yuv422_img, sub_yuv422_temp, rect);
			
		g_head_tracking_seq = seq;

	}
	else
	{
		
		if(seq != NULL && (cpnt_seq->first) != NULL)
		{
			memset(&seq_info_temp, 0, sizeof(seq_info_t));
			seq_block = seq->first;
			while(seq_block)
			{
				// find the tail
				if(seq_block->next == NULL)
				{
					seq_info = (seq_info_t *)seq_block->data;
					box_temp = (muBoundingBox_t *)&seq_info->box;
					//find the overlap size
					box_seq = cpnt_seq->first;
					while(box_seq)
					{

						box = (muBoundingBox_t *)box_seq->data;
						muFindOverlapSize(box_temp, box, &overlap_size);

						if(overlap_size > 0)
						{
							if(overlap_size > max_overlap_size)
							{
								max_overlap_size = overlap_size;
								seq_info_temp.seq_no = seq_info->seq_no+1;
								seq_info_temp.overlap_count = seq_info->overlap_count+1;
								density = (box->area/(float)(box->width*box->height));
								area = box->width*box->height;
								seq_info_temp.density = density;
								seq_info_temp.area = area;
								memcpy(&seq_info_temp.box, box, sizeof(muBoundingBox_t));
								seq_info_temp.target = 1;
								seq_info_temp.sub_bin = NULL;
								seq_info_temp.sub_yuv422 = NULL;
							}
						}

						box_seq = box_seq->next;
					}

				}
				seq_block = seq_block->next;
			}
		}

		if(seq_info_temp.target)
		{
			//push seq, find candidate

			// Get Sub Image, just support yuv422 
			box_temp = (muBoundingBox_t *)&seq_info_temp.box;
			
			b_width = box_temp->width;
			b_height = box_temp->height;
			b_area = box_temp->width*box_temp->height;
			b_minx = box_temp->minx;
			b_miny = box_temp->miny;
			
			b_area = b_width*b_height;

			if((b_area - g_maximum_size) > 500)
			{
				ratio = (b_area/(float)g_maximum_size);
				s_ratio = sqrt(ratio);
				//b_center_x = (b_minx+b_width-1)/2;
				b_center_y = (b_miny+b_height-1)/2; 
				b_width = b_width/s_ratio;
				b_height = b_height/s_ratio;
				b_minx = b_center_x-(b_width>>1);
				b_area = b_width*b_height;
				//b_miny = b_center_y-(b_height>>1);
				//MU_DBG("[VA_SERVER] refined target size new area = %d\n", b_width*b_height);
			}
			
			if(b_width%2 != 0)
			{
				b_width = b_width+1;
				if(b_width > w)
					b_width = w;
			}

			if(b_height%2 != 0)
			{
				b_height = b_height+1;
				if(b_height > h)
					b_height = h;
			}

			if(b_minx%2 != 0)
			{
				b_minx = b_minx-1;
				if(b_minx < 0)
					b_minx = 0;
			}

			if(b_miny%2 != 0)
			{
				b_miny = b_miny+1;
				if(b_miny > h)
					b_miny = h;
			}
			
			if((b_miny+b_height) > h)
			{
				b_height -= 2;
			}

			seq_info_temp.sub_bin = muCreateImage(muSize(b_width, b_height), MU_IMG_DEPTH_8U, 1);
			seq_info_temp.sub_yuv422 = muCreateImage(muSize(b_width, b_height), MU_IMG_DEPTH_8U, 3);

			muSetZero(seq_info_temp.sub_bin);
			muSetZero(seq_info_temp.sub_yuv422);

			//rect = muRect(box_temp->minx, box_temp->miny, box_temp->width, box_temp->height);
			rect = muRect(b_minx, b_miny, b_width, b_height);
			
			sub_bin_temp = seq_info_temp.sub_bin;
			sub_yuv422_temp = seq_info_temp.sub_yuv422;

			muGetSubImage(bin_img, sub_bin_temp, rect);
			muGetSubYUV422Image(yuv422_img, sub_yuv422_temp, rect);
			
			muPushSeq(seq, &seq_info_temp);
		}

		if(max_overlap_size == 0) // no target --> delete seq
		{
			delete_seq(seq);
			g_head_tracking_seq  = NULL;
		}

	} //else
	
	//find real target
	if(seq != NULL)
	{
		target_seq = find_target(seq);
		if(target_seq > 0)
		{
			//show_seq(seq);
			get_target_info(target_seq, seq, info);
			delete_seq(seq);
			g_head_tracking_seq = NULL;
			target = 1;
		}
	}

	return target;
}

static MU_8S horn_optical_flow(const muImage_t *pre_img, const muImage_t *cur_img, muImage_t *mhi_img)
{
	MU_32S x,y;
	MU_32S width, height;
	MU_8U *pre, *mhi, *cur;
	MU_32S result;
	MU_32S index1, index2, index3, index4;
	width = cur_img->width;
	height = cur_img->height;

	mhi = mhi_img->imagedata;
	pre = pre_img->imagedata;
	cur = cur_img->imagedata;

	for(y=0; y<(height-2); y++)
		for(x=0; x<(width-2); x++)
		{
			index1 = x+width*y;
			index2 = index1+2;
			index3 = index1+width+1;
			index4 = index1+width+width;

			result = ((*(cur+index1)-*(pre+index1))+(*(cur+index2)-*(pre+index2))+
					(*(cur+index3)-*(pre+index3))+(*(cur+index4)-*(pre+index4)))>>2;

			*(mhi+index3) = abs(result) + (*(mhi+index3)>>1);
		}

	return 0;
}

static MU_VOID muPTZTargetCCA(muSeq_t *seq)
{
	muSeqBlock_t *box_seq;
	muBoundingBox_t *box;
	float asp_ratio, density;	
	
	if(seq)
	{
		box_seq = seq->first;
		
		while(box_seq)
		{
		
			box = (muBoundingBox_t*)box_seq->data;
		
			asp_ratio = box->width/(float)box->height;
			density = box->area/(float)(box->width*box->height);

			if(asp_ratio > 14 || asp_ratio < 0.06F)
			{
				//MU_DBG("aspect ratio remove\n");
				muRemoveAddressNode(&seq, box_seq);
			}
			else if(density < 0.25F)
			{
				//MU_DBG("density remove density = %f\n", density);
				muRemoveAddressNode(&seq, box_seq);
			}

			box_seq = box_seq->next;
		}
	}
}

static MU_32S add_mask(muImage_t *cur_img, muImage_t *bg_img, const muRect_t mask)
{
	MU_32S width, height;
	MU_32S i,j;
	MU_8U *cur, *bg;

	if(cur_img == NULL || bg_img == NULL)
	{
		MU_DBG("[MU_PTZGETTARGET] add_mask error\n");
		return -1;
	}

	width = cur_img->width;
	height = cur_img->height;

	cur =  cur_img->imagedata;
	bg = bg_img->imagedata;

	//MU_DBG(" mask add x= %d y = %d w =%d h = %d\n", mask.x, mask.y, mask.width, mask.height);

	for(j=mask.y; j<mask.y+mask.height; j++)
		for(i=mask.x; i<mask.x+mask.width; i++)
		{
			*(cur+i+width*j) = 0;
			*(bg+i+width*j) = 0;
		}

	return 0;
}



static MU_VOID get_manual_select_object(const muImage_t *yuv422_img, muPoint_t center, muPtzTargetInfo_t *info)
{
	muRect_t rect;
	
	rect.x = center.x - (MANUAL_WIDTH/2);
	
	rect.y = center.y - (MANUAL_HEIGHT/2);
	
	rect.width = MANUAL_WIDTH;
	
	rect.height = MANUAL_HEIGHT;

	if(rect.x%2 != 0)
	{
		rect.x = rect.x-1;
		if(rect.x < 0)
			rect.x = 0;
	}

	if(rect.y%2 != 0)
	{
		rect.y = rect.y-1;
		if(rect.y < 0)
			rect.y = 0;
	}
	
	if(rect.x+rect.width > g_width)
	{
		rect.width = rect.width - ((rect.x+rect.width) - g_width) - 1;
	}

	if(rect.y+rect.height > g_height)
	{
		rect.height = rect.height - ((rect.y+rect.height) - g_height) - 1;
	}

	if(rect.width%2 != 0)
	{
		rect.width = rect.width-1;
	}

	if(rect.height%2 != 0)
	{
		rect.height = rect.height-1;
	}

	info->bin = NULL;
	info->yuv422 = muCreateImage(muSize(rect.width, rect.height), MU_IMG_DEPTH_8U, 3);

	MU_DBG("rect x=%d y=%d w=%d h=%d\n", rect.x, rect.y, rect.width, rect.height);
	muGetSubYUV422Image(yuv422_img, info->yuv422, rect);

	info->rect.x = rect.x;
	info->rect.y = rect.y;
	info->rect.width = rect.width;
	info->rect.height = rect.height;

	return;
}



//Get target by image diff
MU_32S muPtzGetTarget(const muImage_t *pre_img, const muImage_t *cur_yuv422, muPtzGetTargetParas_t *para)
{
	MU_32S width, height, ret;
	muImage_t *cur_img;
	muImage_t *diff_img, *binary_img, *binary_img2, *label_img;
	muDoubleThreshold_t objth;
	MU_8U label = 0;
	MU_32S target = 0;
	muSeq_t *seq;
	muRect_t	mask[NUM_MASK];
	muPoint_t m_center;
	MU_32S manual_select = 0;
	MU_32S i;

	if(pre_img == NULL || cur_yuv422 == NULL)
	{
		MU_DBG("input image = NULL\n");
		return MU_ERR_NULL_POINTER;
	}

	manual_select = para->manual_select;
	g_frame_count++;

	width = cur_yuv422->width;
	height = cur_yuv422->height;

	if(g_frame_count < 30 && (!manual_select))
	{
		return 0;
	}

	cur_img = muCreateImage(muSize(width, height), MU_IMG_DEPTH_8U, 1);
	binary_img = muCreateImage(muSize(width, height), MU_IMG_DEPTH_8U, 1);
	binary_img2 = muCreateImage(muSize(width, height), MU_IMG_DEPTH_8U, 1);
	diff_img = muCreateImage(muSize(width, height), MU_IMG_DEPTH_8U, 1);

	memcpy(cur_img->imagedata, cur_yuv422->imagedata, width*height);
	
	muBackgroundModeling(cur_img, g_bg);
	
	//aim by manual selection
	if(manual_select)
	{
		MU_DBG("[mu_ptzgettarget] manual aiming start\n");
		m_center.x = (para->manual_center.x/(float)255)*width;
		m_center.y = (para->manual_center.y/(float)255)*height;
		get_manual_select_object(cur_yuv422, m_center, &para->info);

		target = 1;
		return target;
	}

	//check mask
	for(i=0; i<NUM_MASK-2; i++)
	{
		if(para->mask[i].onoff)
		{
			//MU_DBG("%d %d %d %d \n", para->mask[i].start_x, para->mask[i].start_y, para->mask[i].width, para->mask[i].height);
			mask[i].x = (para->mask[i].start_x/(float)255)*g_width;
			mask[i].y = (para->mask[i].start_y/(float)255)*g_height;
			mask[i].width = (para->mask[i].width/(float)255)*g_width;
			mask[i].height = (para->mask[i].height/(float)255)*g_height;
			add_mask(cur_img, g_bg, mask[i]);
		}
	}

	muSub(cur_img, g_bg, diff_img);
	muMeanThresholding(diff_img, binary_img, 3);	
	
	if(diff_img != NULL)
		muReleaseImage(&diff_img);

	if(cur_img != NULL)
		muReleaseImage(&cur_img);
	
	// open+close the binary image
    muSetZero(binary_img2);
    muErode33(binary_img, binary_img2);
    muSetZero(binary_img);
    muDilate33(binary_img2, binary_img);
    muSetZero(binary_img2);
    muDilate33(binary_img, binary_img2);
    muSetZero(binary_img);
    muErode33(binary_img2, binary_img);
	
	if(img2)
	fwrite(binary_img->imagedata, 1, width*height, img2);

	label_img = muCreateImage(muSize(width, height), MU_IMG_DEPTH_8U, 1);

    ret = mu4ConnectedComponent8u(binary_img, label_img, &label);
	if(ret != MU_ERR_SUCCESS)
	{
	    MU_DBG("[MU_APP] Connected Component failed\n");
		return 0;
	}

    // find bounding box 
	objth.max = ((width*height)/6);

	objth.min = 200;

	seq = muFindBoundingBox(label_img, label, objth);

	if(binary_img2 != NULL)
		muReleaseImage(&binary_img2);

	if(seq)
	muPTZTargetCCA(seq);

	if(label_img != NULL)
		muReleaseImage(&label_img);

	if(seq && seq->first != NULL)
	{
		target = tracking_judgement(seq, cur_yuv422, binary_img, &para->info);	
	}
	else
	{
		g_no_seq_count++;
	}

	muClearSeq(&seq);

	if(binary_img != NULL)
		muReleaseImage(&binary_img);

	return target;
}



