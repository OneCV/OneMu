/* ------------------------------------------------------------------------- /
 *
 * Module: mu_backgroundmodeling.c
 * Author: Joe Lin
 * Create date: 06/13/2012
 *
 * Description:
 *
 -------------------------------------------------------------------------- */
 


#include "muGadget.h"

#define	INIT_STD 36 
#define STD_WEIGHT 3	
#define ALPHA 0.01

static MU_32U gmm_init_flag = 0;
static MU_32U isb_init_flag = 0;
static MU_32U frame_count_isb = 0, frame_count_gmm = 0;
static MU_32U g_height = 0;
static MU_64F pre_entropy = 0;

typedef struct gmm_buf
{
	MU_64F *mean;
	MU_64F *std;
	MU_64F *weight;
}gmm_buf_t;

typedef struct isb_buf
{
	MU_8U *pre_bg;
	MU_8U *bg_light;
	MU_8U *bg_dark;

}isb_buf_t;

static gmm_buf_t gmm_buf;
static isb_buf_t isb_buf;


static muError_t muBackgroundModelingISB(muImage_t *curimg, muImage_t *bkimg, isb_buf_t *isb_buf)
{
	MU_8U min_l, max_l;
	MU_32U temp;
	MU_32U i, j;
	MU_32U width, height;
	MU_32U index;
	MU_32U mean_bg, mean_in, mean_bl, mean_bd;
	MU_32U sum_bg, sum_in, sum_bl, sum_bd;
	MU_8U *in, *bg;
	MU_8U *pre_bg;
	MU_8U *bg_light;
	MU_8U *bg_dark;
	MU_8U luma[256];
	MU_64F *luma_pdf;
	MU_64F entropy;

	FILE *img;

	width  =  curimg->width;
	height = curimg->height;
	
	in = curimg->imagedata;
	bg = bkimg->imagedata;
	pre_bg = isb_buf->pre_bg;
	bg_light = isb_buf->bg_light;
	bg_dark = isb_buf->bg_dark;

	if(frame_count_isb == 0)
	{
		printf("background modeling init  first\n");

		for(i=0; i<width*height; i++)
		{
			*(pre_bg+i) = *(in+i);
			*(bg_light+i) = *(in+i);
			*(bg_dark+i) = *(in+i);
			*(bg+i) = *(in+i);
		}
		
		return MU_ERR_SUCCESS;
	}
	else if(frame_count_isb > 0)
	{
		sum_in = 0;
		sum_bg = 0;
		sum_bl = 0;
		sum_bd = 0;
		memset(luma, 0, 256*sizeof(MU_8U));
		
		//background model using constant calculation
		for(j=0; j<height; j++)
			for(i=0; i<width; i++)
			{
				index = i+width*j;
					
				if(*(in+index) > *(pre_bg+index))
				{
					temp = *(pre_bg+index) + 1;
					temp = temp > 255 ? 255 : temp;
					*(bg+index) = temp;
				}
				else if(*(in+index) < *(pre_bg+index))
				{
					temp = *(pre_bg+index) - 1;
					temp = temp < 0 ? 0 : temp;
					*(bg+index) = temp;
				}

				sum_in += *(in+index);
				sum_bg += *(bg+index);
				sum_bl += *(bg_light+index);
				sum_bd += *(bg_dark+index);

				luma[*(in+index)]++;
			}

		//entropy calculation
		min_l = 255;
		max_l = 0;
		entropy = 0;
		luma_pdf = (MU_64F *)calloc(256, sizeof(MU_64F));

		for(i=0; i<256; i++)
		{
			if(luma[i])
			{
				//find pdf(probability of density function)

				if(i < min_l)
				{
					min_l = i;
				}

				if(i > max_l)
				{
					max_l = i;
				}
				
				luma_pdf[i] = (luma[i])/(double)(width*height);
			}
		}
		
		for(j=min_l; j<max_l+1; j++)
		{
			if(luma_pdf[j])
			{
				entropy += (luma_pdf[j]*log((double)luma_pdf[j]));
			}
		}

		entropy = -entropy;
		//printf("entropy = %lf\n", entropy);

		free(luma_pdf);
		
		mean_in = sum_in/(double)(width*height);
		mean_bg = sum_bg/(double)(width*height);
		mean_bl = sum_bl/(double)(width*height);
		mean_bd = sum_bd/(double)(width*height);

		//update background to light or dark
		if((entropy - pre_entropy) > 0.15f)
		{
			//printf("update background\n");
			if(mean_bl < mean_bg)
			{
				memcpy(bg_light, bg, width*height*sizeof(MU_8U));
			}

			if(mean_bd > mean_bg)
			{
				memcpy(bg_dark, bg, width*height*sizeof(MU_8U));
			}

			if(mean_bg < mean_in)
			{
				memcpy(bg, bg_light, width*height*sizeof(MU_8U));
			}
			else
			{
				memcpy(bg, bg_dark, width*height*sizeof(MU_8U));
			}
		}

		pre_entropy = entropy;
	}
	
	return MU_ERR_SUCCESS;
}



static muError_t muBackgroundModelingGMM(muImage_t *curimg, muImage_t *bkimg, gmm_buf_t *gmm_buf)
{
	MU_32U i, j;
	MU_32U width, height;
	MU_32U index;
	MU_8U *in;
	MU_8U *bg;
	MU_64F *mean;
	MU_64F *std;
	MU_64F *weight;
	MU_64F img_in, img_mean, img_weight, img_std;
	MU_64F diff, p;

	width = curimg->width;
	height = curimg->height;

	in = (MU_8U *)curimg->imagedata;
	bg = (MU_8U *)bkimg->imagedata;
	mean = (MU_64F *)gmm_buf->mean;
	std = (MU_64F *)gmm_buf->std;
	weight = (MU_64F *)gmm_buf->weight;

	if(frame_count_gmm == 0)
	{
		
		for(i=0; i<width*height; i++)
		{
			*(mean+i) = *(in+i);
			*(weight+i) = 1.0F;
			*(std+i) = INIT_STD;
			*(bg+i) = *(in+i);
		}

		return MU_ERR_SUCCESS;
	}
	else if(frame_count_gmm > 0)
	{
		for(j=g_height; j<(1+g_height); j++)
			for(i=0; i<width; i++)
				{
					index = i+width*j;

					img_in = (double)*(in+index);
					img_mean = *(mean+index);
					img_std = *(std+index);
					img_weight = *(weight+index);
					diff = abs((int)img_in-(int)img_mean);

					//update threshold
					if(diff <= STD_WEIGHT*img_std)
					{
						img_weight = (1-ALPHA)*img_weight+ALPHA;

						p = ALPHA/img_weight;
						img_mean = (1-p)*img_mean+p*img_in;

						img_std = sqrt((1-p)*img_std*img_std + p*(img_in-img_mean)*(img_in-img_mean));
						//printf("std = %f\n", img_std);
						*(weight+index) = img_weight;
						*(mean+index) = img_mean;
						*(std+index) = img_std;
					}
					else
					{
						img_weight = (1-ALPHA)*img_weight;
						*(weight+index) = img_weight;
					}

					//img_weight = img_weight/img_weight;

					*(bg+index) = (MU_8U)(img_mean/**img_weight*/);

				}

		g_height++;

		if(g_height == height)
			g_height = 0;
	}
	
	return MU_ERR_SUCCESS;
}

static muError_t muGMMBackgroundInit(MU_32U width, MU_32U height, gmm_buf_t *gmm_buf)
{
	if(!gmm_init_flag)
	{
		printf("[MUGADGET] GMM Background modeling init\n");
		gmm_buf->mean = (MU_64F *)malloc(width*height*sizeof(MU_64F));
		gmm_buf->std = (MU_64F *)malloc(width*height*sizeof(MU_64F));
		gmm_buf->weight = (MU_64F *)malloc(width*height*sizeof(MU_64F));
	}
	
	gmm_init_flag = 1;
	frame_count_gmm = 0;
	return MU_ERR_SUCCESS;
}

static muError_t muISBBackgroundInit(MU_32U width, MU_32U height, isb_buf_t *isb_buf)
{
	if(!isb_init_flag)
	{
		printf("[MUGADGET] ISB Background modeling init\n");
		isb_buf->pre_bg = (MU_8U *)malloc(width*height*sizeof(MU_8U));
		isb_buf->bg_light = (MU_8U *)malloc(width*height*sizeof(MU_8U));
		isb_buf->bg_dark = (MU_8U *)malloc(width*height*sizeof(MU_8U));
	}
	
	isb_init_flag = 1;
	frame_count_isb = 0;
	return MU_ERR_SUCCESS;
}

muError_t muBackgroundModelingRelease()
{
	if(gmm_init_flag)
	{
		free(gmm_buf.mean);
		free(gmm_buf.std);
		free(gmm_buf.weight);
	}
	else if(isb_init_flag)
	{
		free(isb_buf.pre_bg);
		free(isb_buf.bg_light);
		free(isb_buf.bg_dark);
	}
	
	return MU_ERR_SUCCESS;
}


/* TODO reset type for moultiple background modeling*/
muError_t muBackgroundModelingReset()
{
	frame_count_gmm = 0;
	frame_count_isb = 0;
}


muError_t muBackgroundModelingInit(MU_32U width, MU_32U height, MU_32U type)
{
	switch(type)
	{
		case MU_BGM_GMM:
			muGMMBackgroundInit(width, height, &gmm_buf);
			break;
		case MU_BGM_ISB:
			muISBBackgroundInit(width, height, &isb_buf);
			break;
		default:
			printf("none support this type %d\n", type);
			break;
	}
		
	return MU_ERR_SUCCESS;
}


muError_t muBackgroundModeling(muImage_t *curimg, muImage_t *bkimg)
{
	if(gmm_init_flag)
	{
		if(muBackgroundModelingGMM(curimg, bkimg, &gmm_buf))
		{
			printf("[MUGADGET] GMM init bg Error\n");
		}

		frame_count_gmm++;
	}
	
	if(isb_init_flag)
	{
		if(muBackgroundModelingISB(curimg, bkimg, &isb_buf))
		{
			printf("[MUGADGET] GMM init ISB Error\n");
		}

		frame_count_isb++;
	}

	if(!isb_init_flag && !gmm_init_flag)
	{
		printf("[MUGADGET] background modeling must init first\n");
	}

	return MU_ERR_SUCCESS;
}
