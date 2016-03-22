/* ------------------------------------------------------------------------- /
 *
 * Module: muHistogram.c
 * Author: Joe Lin
 *
 * Description:
 *    Histogram, Histogram equlization.
 *
 -------------------------------------------------------------------------- */
#include "muCore.h"



typedef struct _Link
{
	MU_32S data;
	struct _Link *add;

}Link;

typedef struct _Grayvalue
{
	MU_32S count;
	Link *ptr;

}grayvalue;


/*===========================================================================================*/
/*   muEqualization                                                                         */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   The procedure increases the contrast of the input image. (ex. over exposure, etc)       */
/*   The histogram equalization would be utilized.                                           */
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/

muError_t muEqualization( const muImage_t* src, muImage_t* dst)
{
	MU_32S width, height, i, j;

	MU_8U *in, *out;

	MU_64F pr[256] = {0};

	MU_32S idx2=0, idx_num = 0;

	MU_64F sk[256] = {0};

	MU_32S transfernum[256] = {0}, kdx, sidx;

	Link *current = NULL, *temp[256] = {NULL}, *buffer = NULL;//declare the buffer to delete the linklist.

	grayvalue rk[256] = { 0, NULL};

	if(src->depth != MU_IMG_DEPTH_8U ||
			dst->depth != MU_IMG_DEPTH_8U) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}


	width = dst->width;

	height = dst->height;

	in = src->imagedata;
	out = dst->imagedata;

	/*!
	 ****************************************************************************************************************
	 * \brief
	 *    construt the Histogram parameter. 
	 *    
	 *    in: image data in[height][width]          out: rk[0].count: the total number of ro,
	 *                                                      rk[1].count: the total number of r1,
	 *                                                                        .
	 *                                                                        .
	 *                                                                        .
	 *                                                      rk[255].ocunt: the total number of r255
	 *                                                      and the rk[num].ptr is the first node 
	 *                                                      which could use the link visitation to search each positin
	 *                                                      of gray scale num.
	*/


	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			rk[ in[ i * width + j ] ].count += 1;

			current = (Link *) malloc ( sizeof(Link) );

			current->data = i * width + j; // record the positin.

			/* construt the link list visitation  */

			if(rk[ in[ i * width + j ] ].ptr == NULL)
				rk[ in[ i * width + j ] ].ptr = current;

			else
				temp[ in[ i * width + j ] ] = current;

			current->add = NULL;

			temp[ in[ i * width + j ] ] = current;
		}
	}

	/*!
	 ********************************************************************************************
	 * \brief
	 *    find the probability of each pixel value.
	 *    
	 *  in: rk[0].count, rk[2].count,... rk[255].count    out: pr[0]~pr[255]
	 ********************************************************************************************
	 */	
	while(idx2 < 256)
	{
		pr[idx2] = (double) ( rk[idx2].count ) / (width * height);

		++idx2;
	}//get probability.

	/*!
	 ********************************************************************************************
	 * \brief
	 *    find the CDF value.
	 *    
	 *  in: pr[0]~pr[255]    out: new gray scale:trnasfernum[0]~transfernum[255]
	 ********************************************************************************************
	 */	
	for(kdx = 0; kdx < 256; kdx++)
	{
		for(sidx = 0; sidx <= kdx; sidx++)
			sk[kdx] += pr[sidx];

		/* find the zone in the  mapping line. */

		transfernum[kdx] = sk[kdx] >= 0.9945 ? 255 : (int) (sk[kdx] / 0.0039 +1) - 1;

	}//get the replace new grayscale.

	/*!
	 ********************************************************************************************
	 * \brief
	 *    put the new gray scale back to the record position.
	 *    
	 *  in: rk[0~255].ptr, transfernum[0~255]    out: out image: out[height][width]
	 ********************************************************************************************
	 */	
	while(idx_num < 256)
	{
		while(rk[idx_num].ptr != NULL)
		{
			buffer = rk[idx_num].ptr;

			out[ rk[idx_num].ptr->data ] = transfernum[idx_num];

			rk[idx_num].ptr = rk[idx_num].ptr->add;

			free( buffer );//free each node of the linklist.
		}

		++idx_num;
	}

	return MU_ERR_SUCCESS;

}

/*===========================================================================================*/
/*   muHistogram                                                                            */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   output = 32bits level = 256 --> stride  = 256*4 byte                                    */
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/
MU_32U* muCreateHistogram()
{
	MU_32U* histValue;
	histValue = (MU_32U *)malloc(256*sizeof(MU_32U));
	return histValue;
}
muError_t muHistogram( const muImage_t* src, MU_32U *dst)
{
	muError_t ret;
    MU_32S i,j;
	ret = muCheckDepth(2, src, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}
	/*if(src->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}*/
/*
	if(src->depth & MU_IMG_HW_ACCE)
	{
			platform_histogram(src, dst);
			return MU_ERR_SUCCESS;
	}
*/  
    for( i=0;i<256;i++)
    {
        dst[i]=0;
    }
 	if( src==NULL )
	{
		return MU_ERR_NULL_POINTER;
	}   
	for( i=0; i!=src->height; i++)
    {
		for(j=0; j!=src->width;j++)
		{
            dst[src->imagedata[i*src->width+j]]++;
        }
    }

	return MU_ERR_SUCCESS;
}

/*****************************************************************************************/
/*  blk_num_h:The number of block (horizontal)                                            */
/*  blk_num_v:The number of block (vertical)                                              */
/*  win_h_s:The height of the block(exchange to shift type)                               */
/*  win_w_s:The width of the block(exchange to shift type)                                */
/*  hist_blk_result:The result of histogram (for block)                                    */
/*                                                                                       */
/*  about hist_blk_result:                                                                 */ 
/*  size:total number of block*16                                                        */
/*  save type:                                                                           */
/*  |block0|block1|..............|block(total number of block)|                          */
/*                                                                                       */
/*  block:devide the 256bits color into 16 regions                                       */
/*  Ex: How many pixels of the graylevel from 0 to 15 in the block 5?                    */
/*  Ans:                                                                                 */
/*  Because we devide the 256bits color into 16 regions,the graylevel from 0 to 15       */
/*  belong to region 0,                                                                  */  
/*  answer=hist_blk_result[5*16+0]                                                         */
/*****************************************************************************************/
MU_16U* muCreateHistogramBlk(MU_32S blk_num_h,MU_32S blk_num_v)
{
	MU_16U* histValue;
	histValue = (MU_16U *)malloc(blk_num_h*blk_num_v*16*sizeof(MU_16U));
	return histValue;
}
      
muError_t muHistogramBlk(muImage_t* src, MU_16U* hist_blk_result, MU_8U win_h_s, MU_8U win_w_s)
{
    MU_32S i,j,win_h=1,win_w=1,blk_num_v,blk_num_h;
	MU_32S two_2_one_index;
	MU_32S true_index;
    MU_32S hist_blk_size;
	muError_t ret;

	ret = muCheckDepth(2, src, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}
	/*if(src->channels != 1)
	{
		printf("src->channels != 1\n");
		return MU_ERR_NOT_SUPPORT;
	}*/

	if( src==NULL )
	{
		return MU_ERR_NULL_POINTER;
	}
	for(i=0;i<win_h_s;i++)
	{
		win_h*=2;
	}
	for(i=0;i<win_w_s;i++)
	{
		win_w*=2;
	}
    if (src->height%win_h  != 0)
    {
        blk_num_v=src->height/win_h+1;
    }
    else
    {
        blk_num_v=src->height/win_h;
    }
    if (src->width%win_w != 0)
    {
        blk_num_h=src->width/win_w+1;
    }
    else
    {
        blk_num_h=src->width/win_w;
    }
	hist_blk_size=blk_num_h*blk_num_v<<4;
	for (i=0; i!=hist_blk_size; i++)
	{
		hist_blk_result[i]=0;
	}
	for( i=0; i!=src->height; i++)
    {
		for(j=0; j!=src->width;j++)
		{
			two_2_one_index=(i>>win_h_s)*blk_num_h+(j>>win_w_s);
			true_index=(two_2_one_index<<4)+(src->imagedata[i*src->width+j]>>4);
            if (true_index > hist_blk_size)
			{
				return MU_ERR_INVALID_PARAMETER;
            }
			hist_blk_result[true_index]++;
		}
	}
    return MU_ERR_SUCCESS;
}
