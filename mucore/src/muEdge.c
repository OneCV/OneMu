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
 * Module: muEdge.c
 * Author: Joe Lin
 *
 * Description:
 *    Gradients, Edges and Corners
 *
 -------------------------------------------------------------------------- */

#include "muCore.h"

/*===========================================================================================*/
/*   muLaplace                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a edge detection by Laplace operator.                             */
/*                                                                                           */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*   selection 1~2, two difference masks are able to use                                     */
// 1 = 0, 1, 0,      2 = 1, 1, 1,                                                            */
//     1,-4, 1,          1,-8, 1,                                                            */
//     0, 1, 0           1, 1, 1                                                             */
//                                                                                           */
/*===========================================================================================*/
muError_t muLaplace( const muImage_t* src, muImage_t* dst, MU_8U selection)
{
	MU_16S temp; 
	MU_32S i,j, index;
	MU_32S width, height;
	MU_8U *in, *out;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}


	width = src->width;
	height = src->height;

	in = src->imagedata;
	out= dst->imagedata;

	switch (selection)
	{
		case 1:

			for(j=0; j<(height-2); j++)
				for(i=0; i<(width-2); i++)
				{
					index = i+width*j;

					temp = abs((in[index+1]+in[index+width]+in[index+width+2]+in[index+(width<<1)+1])-(in[index+width+1]<<2));
					temp = temp > 255 ? 255 : temp;
					out[index+width+1]  = (MU_8U)temp;

				}
			break;

		case 2:

			for(j=0; j<(height-2); j++)
				for(i=0; i<(width-2); i++)
				{
					index = i+width*j;

					temp =  abs((in[index]+in[index+1]+in[index+2]+in[index+width]+in[index+width+2]+in[index+(width<<1)]+in[index+(width<<1)+1]
								+in[index+(width<<1)+2]) - (in[index+width+1]<<3));
					temp = temp > 255 ? 255 : temp;
					out[index+width+1] = (MU_8U)temp; 
				}
			break;
	}

	return MU_ERR_SUCCESS;
}


/*===========================================================================================*/
/*   muSobel                                                                                */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a edge detection by Sobel operator.                               */
/*                                                                                           */
/*   Gx = 1, 2, 1,    Gy = 1, 0, -1,    out = abs(Gx)+abs(Gy)                                */
/*        0, 0, 0,         2, 0, -2,                                                         */
/*       -1,-2,-1          1, 0, -1                                                          */ 
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/
muError_t muSobel( const muImage_t* src, muImage_t* dst)
{
	MU_16S temp; 
	MU_32S i,j, index;
	MU_32S gx,gy;
	MU_32S width, height;
	MU_8U *in, *out;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	width = src->width;
	height = src->height;

	in = src->imagedata;
	out = dst->imagedata;

	for(j=0; j<(height-2); j++)
		for(i=0; i<(width-2); i++)
		{
			index = i+width*j;

			gx = (in[index]+(in[index+1]<<1)+(in[index+2]))-
				(in[index+(width<<1)]+(in[index+(width<<1)+1]<<1)+in[index+(width<<1)+2]);

			gy = (in[index]+(in[index+width]<<1)+in[index+(width<<1)])-
				(in[index+2]+(in[index+width+2]<<1)+in[index+(width<<1)+2]);

			temp = abs(gx)+abs(gy);

			temp = temp > 255 ? 255 : temp;
			temp = temp < 0 ? 0 : temp;

			out[index+width+1] = (MU_8U)temp;

		}

	return MU_ERR_SUCCESS;
}



/*===========================================================================================*/
/*   muPrewitt                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a edge detection by Prewitt operator.                             */
/*                                                                                           */
/*   Gx = 1, 1, 1,    Gy = 1, 0, -1,    out = abs(Gx)+abs(Gy)                                */
/*        0, 0, 0,         1, 0, -1,                                                         */
/*       -1,-1,-1          1, 0, -1                                                          */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/
muError_t muPrewitt( const muImage_t* src, muImage_t* dst)
{
	MU_16S temp; 
	MU_32S i,j, index;
	MU_32S gx,gy;
	MU_32S width, height;
	MU_8U *in, *out;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}


	width = src->width;
	height = src->height;

	in = src->imagedata;
	out = dst->imagedata;

	for(j=0; j<(height-2); j++)
		for(i=0; i<(width-2); i++)
		{
			index = i+width*j;

			gx = (in[index]+(in[index+1])+(in[index+2]))-
				(in[index+(width<<1)]+(in[index+(width<<1)+1])+in[index+(width<<1)+2]);

			gy = (in[index]+(in[index+width])+in[index+(width<<1)])-
				(in[index+2]+(in[index+width+2])+in[index+(width<<1)+2]);

			temp = abs(gx)+abs(gy);

			temp = temp > 255 ? 255 : temp;
			temp = temp < 0 ? 0 : temp;

			out[index+width+1] = (MU_8U)temp;

		}

	return MU_ERR_SUCCESS;
}


/*===========================================================================================*/
/*   muCanny TODO																	         */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a edge detection by Prewitt operator.                             */
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*   muImage_t *dst --> output image                                                         */
/*   muImage_t *ang --> direction image                                                      */
/*===========================================================================================*/
muError_t muCanny( const muImage_t* src, muImage_t* dst, muImage_t *ang)
{
/*
	if(src->depth != MU_IMG_DEPTH_8U ||
			dst->depth != MU_IMG_DEPTH_8U) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}
*/
	return MU_ERR_SUCCESS;
}


typedef struct _blurInfo
{
	MU_32S totalEdge;
	MU_32S totalEdgeWidth;
}blurInfo_t;

typedef struct _localList
{
	MU_32S max;
	MU_32S min;
}localList_t;

// Run-length based calculation of the edge width
static blurInfo_t *calEdgeWidth(MU_8U *content, MU_32S length)
{
	blurInfo_t *info;
	MU_32S i;
	MU_32S totalEdge = 0, edgeWidth = 0;
	MU_8U curData, lastData = 0, nextData;
	MU_8U maxData = 0, minData = 255;
	MU_8U conFlag = 0;
	MU_8U *data;
	MU_8U firstFlag = 1;
	muSeqBlock_t *current, *head;
	muSeq_t *localList;
	localList_t listData, *list;
	MU_32S maxPos, minPos, diff, lastMax, lastMin;

	data = content;
	localList = muCreateSeq(sizeof(localList_t));
	listData.min = 0;
	listData.max = 0;
	for(i=0; i<length; i++)
	{
		curData = data[i];
	
		if(lastData == 0 && curData > 0)
		{
			//start edge position
			listData.min = i;
		}
		else if(curData == 0 && lastData > 0)
		{
			listData.max = i-1;
		}
		else if(curData > 0 && i == (length-1))
		{
			listData.max = i;
		}

		if(listData.min != 0 && listData.max != 0)
		{
			muPushSeq(localList, &listData);
			listData.min = 0;
			listData.max = 0;
		}

		lastData = curData;

	}

	//matching the data;
	for(i=0; i<length; i++)
	{
		curData = data[i];
		if(curData > 0)
		{
			//search each data in list to find the edgeWidth
			current = localList->first;
			while(current)
			{
				list = (localList_t *)current->data;	
				if(i <= list->max && i >= list->min)
				{
					minPos = list->min;
					maxPos = list->max;
					break;
				}
				current = current->next;
			}
			totalEdge++;
			edgeWidth+=abs(minPos-maxPos);
		}
	}

	muClearSeq(&localList);

	info = (blurInfo_t *)malloc(sizeof(blurInfo_t));

	info->totalEdge = totalEdge;
	info->totalEdgeWidth = edgeWidth;

	return info;
}

/*===========================================================================================*/
/*   muNoRefBlurMetric																         */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   pina marziliano a no reference perceptual blur metric                                   */
/*   This metod refer to professor marziliano's blur metric by edge width                    */
/*   The implementation is using the run length based to calculate the edge width            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   return bm value, larger is more blur, smaller is more sharpness                         */ 
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*   *bm --> return blur metric value                                                        */
/*===========================================================================================*/
muError_t muNoRefBlurMetric(muImage_t *src, MU_32S *bm)
{
	MU_16S temp; 
	MU_32S i,j, index;
	MU_32S gx,gy;
	MU_32S width, height;
	MU_8U *in, *out;
	muImage_t *dst, *gray, *edgeImg;
	muSize_t size;
	muError_t ret;
	blurInfo_t *blurInfo;
	MU_32U totalEdge = 0;
	MU_32U totalEdgeWidth = 0;

	width = src->width;
	height = src->height;
	size.width = width;
	size.height = height;

	gray = muCreateImage(size, MU_IMG_DEPTH_8U, 1);
	dst = muCreateImage(size, MU_IMG_DEPTH_8U, 1);
	edgeImg = muCreateImage(size, MU_IMG_DEPTH_8U, 1);

	muSetZero(dst);
	if(src->channels == 3)
	{
		muRGB2GrayLevel(src, gray);
		in = gray->imagedata;
	}
	else
	{
		in = src->imagedata;
	}
		
	out = dst->imagedata;
	
	for(j=0; j<(height-2); j++)
		for(i=0; i<(width-2); i++)
		{
			index = i+width*j;

			gy = (in[index]+(in[index+width]<<1)+in[index+(width<<1)])-
				(in[index+2]+(in[index+width+2]<<1)+in[index+(width<<1)+2]);

			temp = abs(gy);

			temp = temp > 255 ? 255 : temp;
			temp = temp < 0 ? 0 : temp;

			out[index+width+1] = (MU_8U)temp;
		
		}

	muOtsuThresholding(dst, edgeImg);

	out = edgeImg->imagedata;	
	for(i=0; i<height; i++)
	{
		blurInfo = calEdgeWidth(out+(i*width), width);
		totalEdge += blurInfo->totalEdge;
		totalEdgeWidth += blurInfo->totalEdgeWidth;
		free(blurInfo);
	}

	if(totalEdge <= 0)
		printf("nan");
	else
		MU_DBG("blurmetric: %f\n", ((MU_64F)totalEdgeWidth/(MU_64F)totalEdge));

	muReleaseImage(&dst);
	muReleaseImage(&gray);
	muReleaseImage(&edgeImg);
}