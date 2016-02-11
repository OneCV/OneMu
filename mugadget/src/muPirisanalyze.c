//********************************************************/
//lum_mean_blk:calculate the the mean graylevel of the block
//muPIrisAnalyze:
//output:over_exp_blk_num, lum_value, the max color region (except noise), image change or not
//********************************************************/
#include "muGadget.h"
static MU_16U *hist_value;
static MU_32U *tr_hist_value;
static MU_32U **y_mean_blk;
static MU_8U win_h_s, win_w_s;
static MU_32S win_h, win_w;
static MU_32S blk_num_h, blk_num_v;
static MU_32S height,width;
static MU_8U blk_total_pixel;//the size of block
static MU_8U over_exp_th;
static MU_8U max_regionTh;
static MU_8U over_exp_range_l;//1~15
static MU_8U over_exp_range_h;//1~15
static MU_8U enable_var_anal;
static MU_8U var_th;//1~255
static muImage_t *prev_img;
static MU_8U prev_lumvalue;
static MU_32S pts;
static MU_8U sample_freq;
static time_t begin, end;
static float total_time;
	/*	
	begin = curtime();
	end = curtime();
	total_time = ((end - begin)/(double)1000);
	printf("time = %f\n", total_time);
	*/
	
static MU_32U*  create_y_blk(MU_32S x, MU_32S y)
{
	MU_32U* y_mean_blk;
	MU_32S i;
	y_mean_blk = (MU_32U **)malloc(y * sizeof(MU_32U *));
	for(i = 0; i != y; i++)
	{
		y_mean_blk[i] = (MU_32U*)malloc(x * sizeof(MU_32U *));
	}
	return y_mean_blk;
}
static MU_32U  average_lumvalue(const muImage_t *src)
{
	MU_32U y=0;
	MU_32S i,j;	
	if( src==NULL  )
	{
		return MU_ERR_NULL_POINTER;
	}
	for( i=0; i<src->height; i+=2)
	{
		for(j=0; j<src->width; j+=2)
		{
			y+=src->imagedata[i*src->width+j];	
		}
	}
	y=4*y/(src->width*src->height);
	return y;
}
//get average y in every block
static MU_32U  lum_mean_blk(const muImage_t *src, MU_32U **y_mean_blk, MU_32S blk_num_h, MU_32S blk_num_v)
{
	MU_32S i,j;
	MU_8U x=(src->width%win_w),y=(src->height%win_h);
	if( src==0 || y_mean_blk==0 )
	{
		return MU_ERR_NULL_POINTER;
	}
	
	for(i = 0; i != blk_num_v; i++)
	{
		for(j = 0; j != blk_num_h; j++)
		{
			y_mean_blk[i][j]= 0;
		}
	}
	for (i=0;i!=src->height;i++)
	{	
		for (j=0;j!=src->width;j++)
		{
			y_mean_blk[i>>win_h_s][j>>win_w_s]+=src->imagedata[i*src->width+j];
		}
	}
	switch (x)
	{
		case 0 :
			switch (y)
			{
				case 0 :
					for (i=0;i!=blk_num_v;i++)
					{
						for (j=0;j!=blk_num_h;j++)
						{
							y_mean_blk[i][j]=((y_mean_blk[i][j]>>win_w_s)>>win_h_s);
						}				
					}
					break;
				default :
					for (i=0;i!=blk_num_v-1;i++)
					{
						for (j=0;j<blk_num_h;j++)
						{
							y_mean_blk[i][j]=((y_mean_blk[i][j]>>win_w_s)>>win_h_s);
						}				
					}
					for (j=0;j!=blk_num_h;j++)
					{
						y_mean_blk[blk_num_v-1][j]/=(y<<win_w_s);
					}
					break;
			}
            break;
		default :
			switch (y)
			{
				case 0 :
					for (i=0;i!=blk_num_v;i++)
					{
						for (j=0;j<blk_num_h-1;j++)
						{
							y_mean_blk[i][j]=((y_mean_blk[i][j]>>win_w_s)>>win_h_s);
						}				
					}
					for (i=0;i!=blk_num_v;i++)
					{
						y_mean_blk[i][blk_num_h-1]/=(x<<win_h_s);
					}
					break;
				default :
					for (i=0;i!=blk_num_v-1;i++)
					{
						for (j=0;j<blk_num_h-1;j++)
						{
							y_mean_blk[i][j]=((y_mean_blk[i][j]>>win_w_s)>>win_h_s);
						}				
					}
					for (j=0;j!=blk_num_h-1;j++)
					{
						y_mean_blk[blk_num_v-1][j]/=(y<<win_w_s);
					}
					for (i=0;i!=blk_num_v-1;i++)
					{
						y_mean_blk[i][blk_num_h-1]/=(x<<win_h_s);
					}
					y_mean_blk[blk_num_v-1][blk_num_h-1]/=(x*y);
					break;
			}
			break;
	}
	return 0;
}
static MU_32U  image_match(const muImage_t *p,const muImage_t *q, const MU_8U lum, const MU_8U prev_lum)
{
	MU_32S i,different_pixel=0;
	for (i=0;i<p->width*p->height;i++)
	{
		if (abs(p->imagedata[i]-q->imagedata[i])>4)
		{
			different_pixel++;
		}
	}	
	if (different_pixel==0)
	{
		return 1;
	}
	else if (p->width*p->height/different_pixel<20 && abs(lum-prev_lum)>1)
	{
		//printf("[PIRIS ANALYZE]different_pixel =%d\n",different_pixel);
		return 0;
	}		
	return 1;
}
MU_API(MU_8S) muConfigPIrisAnalyze( muPIrisAnalzeParameter_t* pIrisAnalyeParameter )
{
	if( pIrisAnalyeParameter==NULL )
	{
		printf("No image or no pIrisAnalyzeValue\n");
		return MU_ERR_NULL_POINTER;
	}
	enable_var_anal=pIrisAnalyeParameter->enable_var_anal;
    sample_freq=pIrisAnalyeParameter->sample_freq;
 	if (pIrisAnalyeParameter->over_exp_range_h>=pIrisAnalyeParameter->over_exp_range_l)
	{
		over_exp_range_l=pIrisAnalyeParameter->over_exp_range_l;
		over_exp_range_h=pIrisAnalyeParameter->over_exp_range_h;		 
	}
	else
	{
		over_exp_range_h=pIrisAnalyeParameter->over_exp_range_l;
		over_exp_range_l=pIrisAnalyeParameter->over_exp_range_h;	 
	}

	over_exp_range_l=pIrisAnalyeParameter->over_exp_range_l>>4;
	over_exp_range_h=pIrisAnalyeParameter->over_exp_range_h>>4;   
    
	if (enable_var_anal!=1 && enable_var_anal!=0)
	{
		printf("please input 0 or 1 for enable_var_anal\n");
		return MU_ERR_INVALID_PARAMETER;	
	}	
	var_th=pIrisAnalyeParameter->var_th;	
	return 0;
}
MU_API(MU_8S) muInitPIrisAnalyze( muPIrisAnalzeParameter_t* pIrisAnalyeParameter )
{

	MU_32S i;
	height=pIrisAnalyeParameter->height;
	width=pIrisAnalyeParameter->width;
	win_h_s=4;
	win_w_s=4;	
	pts=0;
	sample_freq=1;

	if( pIrisAnalyeParameter==NULL )
	{
		printf("No image or no pIrisAnalyzeValue\n");
		return MU_ERR_NULL_POINTER;
	}

	muConfigPIrisAnalyze(pIrisAnalyeParameter);
	
	win_h=1,win_w=1;
	for (i=0;i<win_h_s;i++)
	{
		win_h*=2;
	}
	for (i=0;i<win_w_s;i++)
	{
		win_w*=2;
	}
	if (win_h > height)
	{
		return MU_ERR_INVALID_PARAMETER;
	}
	if (win_w > width)
	{
		return MU_ERR_INVALID_PARAMETER;	
	}
    if (height%win_h  != 0)
    {
        blk_num_v=height/win_h+1;
    }
    else
    {
        blk_num_v=height/win_h;
    }
    if (width%win_w != 0)
    {
        blk_num_h=width/win_w+1;
    }
    else
    {
        blk_num_h=width/win_w;
    }
    tr_hist_value=NULL;	
	hist_value=NULL;	
	y_mean_blk=NULL;
	prev_img=NULL;

	blk_total_pixel=255;//16*16
	over_exp_th=blk_total_pixel-4;//16*16-5
	max_regionTh=blk_total_pixel/16;//blk_total_pixel/16;
	
	y_mean_blk=create_y_blk(blk_num_h, blk_num_v);
	if(y_mean_blk==NULL)
	{
		return MU_ERR_NULL_POINTER;
	}		

	tr_hist_value=muCreateHistogram();
	if( tr_hist_value==NULL)
	{
		printf("create Histogram buffer error\n");
        return MU_ERR_NULL_POINTER;
	}
	
	hist_value=muCreateHistogramBlk(blk_num_h, blk_num_v);
	if( hist_value==NULL)
	{
		return MU_ERR_NULL_POINTER;
	}
	prev_img = muCreateImage(muSize(width,height), MU_IMG_DEPTH_8U, 1);
	return 0;	
}
MU_API(MU_VOID) muRleasePIrisAnalyze()
{
	MU_32S i;
	if (y_mean_blk!=NULL)
	{
		for(i = 0; i != blk_num_v; i++)
			free(y_mean_blk[i]);
		free(y_mean_blk);
	}
	if (hist_value!=NULL)
	{
		free(hist_value);	
	}
	if (tr_hist_value!=NULL)
	{
		free(tr_hist_value);	
	}
	if (prev_img!=NULL)
	{
		muReleaseImage(&prev_img);
	}
	y_mean_blk=NULL;
	hist_value=NULL;
	tr_hist_value=NULL;
	prev_img=NULL;
	return;
}
MU_API(MU_8S) muPIrisAnalyze(const muImage_t *src, muPIrisAnalzeValue_t* pIrisAnalyzeValue, muPIrisAnalzeParameter_t* pIrisAnalyeParameter)
{
	MU_8U lumvalue;
	MU_32S i,j;
	MU_32U avg_lum_modify=0;
	MU_32S over_exp_blk_num=0;//the number of over-exposure blocks
	MU_32S noise_blk=0;//the number of noise blocks
	MU_32S max_region=0;//0~15	the most bright graylevel in the frame
	MU_32S u=0,k=0,pixelNum=0;
	muError_t ret;
	pIrisAnalyzeValue->max_region=0;
	pIrisAnalyzeValue->img_match=1;//the image change or not, 1:match , 0:change


	enable_var_anal=pIrisAnalyeParameter->enable_var_anal;
	if (enable_var_anal!=1 && enable_var_anal!=0)
	{
		printf("please input 0 or 1 for enable_var_anal\n");
		return MU_ERR_INVALID_PARAMETER;	
	}

	if( src==NULL || pIrisAnalyzeValue==NULL )
	{
		return MU_ERR_NULL_POINTER;
	}

	//get histogram
    ret=muHistogram(src,tr_hist_value);

	if (ret!=MU_ERR_SUCCESS){
		printf("something wrong in muHistogram!!\n");
		return ret;
	}
    
    
    
	//get histogram in every block


	ret=muHistogramBlk(src,hist_value,win_h_s,win_w_s);

	if (ret!=MU_ERR_SUCCESS){
		printf("something wrong in muHistogramBlk!!\n");
		return ret;
	}
	pIrisAnalyzeValue->lum_value=average_lumvalue(src);

	lumvalue=pIrisAnalyzeValue->lum_value;
	
	//check the frame is different or not
	pIrisAnalyzeValue->img_match=image_match(src,prev_img,lumvalue,prev_lumvalue);
	
	prev_lumvalue=lumvalue;
	memcpy(prev_img->imagedata,src->imagedata, src->width*src->height*sizeof(MU_8U ));

	if (enable_var_anal)
	{		
		ret=lum_mean_blk(src,y_mean_blk,blk_num_h,blk_num_v);
		if (ret!=MU_ERR_SUCCESS){
			printf("something wrong in lum_mean_blk\n");
			return ret;
		}
	}
	
	for(i = 0; i != blk_num_v; i++)
	{
		for(j = 0; j != blk_num_h; j++)
		{
			u=((i*blk_num_h)+j)<<4;
			pixelNum = 0;
			for (k = over_exp_range_l; k <= over_exp_range_h; k++){
				pixelNum += hist_value[u+k];
			}
			//calculate the number of overexp block
			if(pixelNum >= (over_exp_th))
			{
				//new , refer to variance
				if (enable_var_anal == 1)
				{
					//if the block is not belong to the noise block
					if (y_mean_blk[i][j]-lumvalue <= var_th && y_mean_blk[i][j]>lumvalue)
					{
						over_exp_blk_num++;
					}
					else if(y_mean_blk[i][j]-lumvalue > var_th && y_mean_blk[i][j]>lumvalue)
					{
						noise_blk++;
						avg_lum_modify+=y_mean_blk[i][j];
					}
				}
				else{
					over_exp_blk_num++;
				}
			}
			//find the max lume region
			for(k = 0; k < 16; k++)
			{
				pixelNum = hist_value[u+15-k];
				if(pixelNum > (max_regionTh))
				{
					if (enable_var_anal != 1)
					{
						if(max_region < (15 - k))
							max_region = 15 - k;
						break;
					}
					else{
						if ( (y_mean_blk[i][j]-lumvalue) <= var_th && (y_mean_blk[i][j]>lumvalue))
						{
							if(max_region < (15 - k))
								max_region = 15 - k;
							break;						
						}
					}
				}
			}
		}
	}
	
	pIrisAnalyzeValue->max_region=max_region;
	pIrisAnalyzeValue->over_exp_blk_num=over_exp_blk_num;

    memcpy(pIrisAnalyzeValue->hist,tr_hist_value,256*sizeof(unsigned int));
    
    pIrisAnalyzeValue->pix_num=(src->width)*(src->height);
	if (enable_var_anal == 1)
	{
		//ignore noise and get average lume again
		if (noise_blk==blk_num_h*blk_num_v) noise_blk=blk_num_h*blk_num_v-1;
		pIrisAnalyzeValue->lum_value=((lumvalue*blk_num_h*blk_num_v-avg_lum_modify)/(blk_num_h*blk_num_v-noise_blk));
	}
	//total_time = ((end - begin)/(double)1000);
	//printf("time = %fs\n", total_time);
	return 0;
}	

