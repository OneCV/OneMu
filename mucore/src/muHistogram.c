/*
% MIT License
%
% Copyright (c) 2016 OneCV
%
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, including without limitation the rights
% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
% copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included in all
% copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
% OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
% SOFTWARE.
*/

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
/*   muEqualization                                                                        	 */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   The procedure increases the contrast of the input image. (ex. over exposure, etc)       */
/*   The histogram equalization would be utilized.                                           */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   https://zh.wikipedia.org/wiki/%E7%9B%B4%E6%96%B9%E5%9B%BE%E5%9D%87%E8%A1%A1%E5%8C%96    */   
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*   muImage_t *dst --> output image                                                         */
/*                                                                                           */
/*===========================================================================================*/

muError_t muEqualization( const muImage_t* src, muImage_t* dst)
{
	MU_32S i, temp=0;
	MU_32U *his;
	MU_32U *cdfHis;
	MU_32S cdfMin = 0x3FFFFFFFF;
	MU_32S area;

	if(src->depth != MU_IMG_DEPTH_8U || dst->depth != MU_IMG_DEPTH_8U) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}

	his = (MU_32U *)malloc(256*sizeof(MU_32U));
	memset(his, 0, 256*sizeof(MU_32U));

	cdfHis = (MU_32U *)malloc(256*sizeof(MU_32U));
	memset(cdfHis, 0, 256*sizeof(MU_32U));

	muHistogram(src, his);
	//find CDF
	for(i=0; i<256; i++)
	{
		if(his[i])
		{
			cdfHis[i] = temp+his[i];
			temp = cdfHis[i];
			if(cdfHis[i] < cdfMin)
				cdfMin = cdfHis[i];
		}
	}
	area = src->width*src->height;

	//round((cdf(v) - cdfMin)/(area-cdfMin) x 255)
	for(i=0; i<256; i++)
	{
		if(cdfHis[i])
		{
			cdfHis[i] = muRound(((MU_64F)(((cdfHis[i]-cdfMin)/(MU_64F)(area-cdfMin)))*255.0));
		}
	}

	for(i=0; i<area; i++)
	{
		dst->imagedata[i] = cdfHis[src->imagedata[i]];
	}

	if(his)
		free(his);
	if(cdfHis)
		free(cdfHis);

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
