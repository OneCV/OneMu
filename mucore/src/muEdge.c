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
/*   muLaplace                                                                               */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a edge detection by Laplace operator.                             */
/*                                                                                           */
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*   muImage_t *dst --> output image                                                         */
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
/*   muSobel                                                                                 */
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
/*   muImage_t *src --> input image                                                          */
/*   muImage_t *dst --> output image                                                         */
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
/*   muPrewitt                                                                               */
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
/*   muImage_t *src --> input image                                                          */
/*   muImage_t *dst --> output image                                                         */
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
	MU_8U curData, lastData = 0;
	MU_8U maxData = 0, minData = 255;
	MU_8U conFlag = 0;
	MU_8U *data;
	MU_8U firstFlag = 1;
	muSeqBlock_t *current;
	muSeq_t *localList;
	localList_t listData, *list;
	MU_32S maxPos, minPos;

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
muError_t muNoRefBlurMetric(muImage_t *src, MU_64F *bm)
{
	MU_16S temp; 
	MU_32S i,j, index;
	MU_32S gy;
	MU_32S width, height;
	MU_8U *in, *out;
	muImage_t *dst, *gray, *edgeImg;
	muSize_t size;
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
		MU_DBG("total edge : nan");
	else
		*bm = (totalEdgeWidth/(MU_64F)totalEdge);

	muReleaseImage(&dst);
	muReleaseImage(&gray);
	muReleaseImage(&edgeImg);

	return MU_ERR_SUCCESS;
}


#define NOEDGE        0
#define EDGECANDIDATE 128
#define EDGE          255
static muError_t nonMaxSuppress(const muImage_t *magImg, muImage_t *dirImg, muImage_t *cannyImg, muDoubleThreshold_t th, MU_32S offset)
{

	MU_32S i, j;
	MU_32S width, height;
	muSize_t size;
	muError_t ret;
	muImage_t *tempImg;
	MU_16S *mag, magData[9], magTemp;
	MU_8U *out, *dir, dirTemp, *tmp, mgTemp[9]; 
	MU_16S leftPix, rightPix;

	ret = muCheckDepth(6, magImg, MU_IMG_DEPTH_16S, dirImg, MU_IMG_DEPTH_8U, cannyImg, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(magImg->channels != 1 || dirImg->channels != 1 || cannyImg->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}
	
	size.width = magImg->width;
	size.height = magImg->height;

	tempImg = muCreateImage(size, MU_IMG_DEPTH_8U, 1);
	memset(magData, 0, 9*sizeof(MU_16S));
	memset(mgTemp, 0, 9*sizeof(MU_8U));
	mag = (MU_16S *)magImg->imagedata;
	out = (MU_8U *)tempImg->imagedata;
	dir = (MU_8U *)dirImg->imagedata;

	width = magImg->width;
	height = magImg->height;

	for(j=offset; j<(height-offset-2); j++)
		for(i=offset; i<(width-offset-2); i++)
		{
			magData[0] = mag[i+width*j];     magData[1] = mag[i+1+width*j];     magData[2] = mag[i+2+width*j];
			magData[3] = mag[i+width*(j+1)]; magData[4] = mag[i+1+width*(j+1)]; magData[5] = mag[i+2+width*(j+1)];
			magData[6] = mag[i+width*(j+2)]; magData[7] = mag[i+1+width*(j+2)]; magData[8] = mag[i+2+width*(j+2)];

			dirTemp = dir[i+1+width*(j+1)];
			magTemp = magData[4];

			if(dirTemp == 0)
			{
				leftPix = magData[1]; rightPix = magData[7];
			}
			else if(dirTemp == 45)
			{
				leftPix = magData[0]; rightPix = magData[8];
			}
			else if(dirTemp == 90)
			{
				leftPix = magData[3]; rightPix = magData[5];
			}
			else
			{
				leftPix = magData[2]; rightPix = magData[6];
			}

			//to place the edge and trace the edge by hysteresis function ;
			if(magTemp == NOEDGE)
				out[i+1+width*(j+1)] = NOEDGE;
			else if(magTemp < leftPix || magTemp < rightPix)
				out[i+1+width*(j+1)] = NOEDGE;
			else //maybe edge occur
			{
				if(magTemp > th.max )  
					out[i+1+width*(j+1)] = EDGE;
				else if(magTemp <= th.max && magTemp >= th.min)
					out[i+1+width*(j+1)] = EDGECANDIDATE;
				else
					out[i+1+width*(j+1)] = NOEDGE;
			}

		}

	tmp = tempImg->imagedata;
	out = cannyImg->imagedata;
	// HysteresisTh
	offset = offset+1;
	for(j=offset; j<(height-offset-2); j++)
		for(i=offset; i<(width-offset-2); i++)
		{
			mgTemp[0] = tmp[i+width*j];     mgTemp[1] = tmp[i+1+width*j];     mgTemp[2] = tmp[i+2+width*j];
			mgTemp[3] = tmp[i+width*(j+1)]; mgTemp[4] = tmp[i+1+width*(j+1)]; mgTemp[5] = tmp[i+2+width*(j+1)];
			mgTemp[6] = tmp[i+width*(j+2)]; mgTemp[7] = tmp[i+1+width*(j+2)]; mgTemp[8] = tmp[i+2+width*(j+2)];

			if(mgTemp[4] == EDGE)
			{
				//Row1
				if(mgTemp[0]==EDGECANDIDATE){tmp[i+width*j] = EDGE;out[i+width*j] = EDGE;}
				if(mgTemp[1]==EDGECANDIDATE){tmp[i+1+width*j] = EDGE;out[i+1+width*j] = EDGE;}
				if(mgTemp[2]==EDGECANDIDATE){tmp[i+2+width*j] = EDGE;out[i+2+width*j] = EDGE;}
				//Row2
				if(mgTemp[3]==EDGECANDIDATE){tmp[i+width*(j+1)] = EDGE;out[i+width*(j+1)] = EDGE;}
				if(mgTemp[5]==EDGECANDIDATE){tmp[i+2+width*(j+1)] = EDGE;out[i+2+width*(j+1)] = EDGE;}
				//Row3
				if(mgTemp[6]==EDGECANDIDATE){tmp[i+width*(j+2)] = EDGE;out[i+width*(j+2)] = EDGE;}
				if(mgTemp[7]==EDGECANDIDATE){tmp[i+1+width*(j+2)] = EDGE;out[i+1+width*(j+2)] = EDGE;}
				if(mgTemp[8]==EDGECANDIDATE){tmp[i+2+width*(j+2)] = EDGE;out[i+2+width*(j+2)] = EDGE;}

				out[i+1+width*(j+1)] = EDGE;
			}

		}

	muReleaseImage(&tempImg);

	return MU_ERR_SUCCESS;
}


static muError_t muFilter16S55( const muImage_t* src, muImage_t* dst, const MU_8S kernel[], const MU_8U norm)
{
	MU_32S i, j, temp;
	MU_32S width, height;
	muError_t ret;
	MU_8U *in;
	MU_16S *out;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_16S);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	in = src->imagedata;
	out = (MU_16S *)dst->imagedata;

	width = src->width;
	height = src->height;
	
	for(i=0; i<height-4; i++)
	{
		for(j=0; j<width-4; j++)
		{
			temp = 0;
			//Row1
			temp += in[j+width*i]*kernel[0]; 
			temp += in[j+1+width*i]*kernel[1]; 
			temp += in[j+2+width*i]*kernel[2]; 
			temp += in[j+3+width*i]*kernel[3]; 
			temp += in[j+4+width*i]*kernel[4];
			//Row2
			temp += in[j+width*(i+1)]*kernel[5]; 
			temp += in[j+1+width*(i+1)]*kernel[6]; 
			temp += in[j+2+width*(i+1)]*kernel[7]; 
			temp += in[j+3+width*(i+1)]*kernel[8]; 
			temp += in[j+4+width*(i+1)]*kernel[9];
			//Row3
			temp += in[j+width*(i+2)]*kernel[10]; 
			temp += in[j+1+width*(i+2)]*kernel[11]; 
			temp += in[j+2+width*(i+2)]*kernel[12]; 
			temp += in[j+3+width*(i+2)]*kernel[13]; 
			temp += in[j+4+width*(i+2)]*kernel[14];
			//Row4
			temp += in[j+width*(i+3)]*kernel[15]; 
			temp += in[j+1+width*(i+3)]*kernel[16]; 
			temp += in[j+2+width*(i+3)]*kernel[17]; 
			temp += in[j+3+width*(i+3)]*kernel[18]; 
			temp += in[j+4+width*(i+3)]*kernel[19];
			//Row5
			temp += in[j+width*(i+4)]*kernel[20]; 
			temp += in[j+1+width*(i+4)]*kernel[21]; 
			temp += in[j+2+width*(i+4)]*kernel[22]; 
			temp += in[j+3+width*(i+4)]*kernel[23]; 
			temp += in[j+4+width*(i+4)]*kernel[24];

			out[j+2+width*(i+2)] = (temp/(MU_32F)norm);
		}
	}

	return MU_ERR_SUCCESS;
}

muError_t edgeFilter(muImage_t *src, muImage_t *mag, muImage_t *dirImg, const MU_8S kx[], const MU_8S ky[], MU_32S offset)
{
	muError_t ret;
	MU_32S i,j;
	MU_32S width, height;
	MU_32S gx, gy, gm;
	MU_16S *in, *out;
	MU_8U *dir, degree;
	MU_64F degreeTemp;
	
	ret = muCheckDepth(6, src, MU_IMG_DEPTH_16S, mag, MU_IMG_DEPTH_16S, dirImg, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || mag->channels != 1 || dirImg->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	in = (MU_16S*)src->imagedata;
	out = (MU_16S*)mag->imagedata;
	dir = (MU_8U *)dirImg->imagedata;

	width = src->width;
	height = src->height;

	for(j=offset; j<(height-offset-2); j++)
		for(i=offset; i<(width-offset-2); i++)
		{
			
			gx =  in[i+width*j]    *kx[0] + in[i+1+width*j]    *kx[1]  + in[i+2+width*j]    *kx[2]
				 +in[i+width*(j+1)]*kx[3] + in[i+1+width*(j+1)]*kx[4]  + in[i+2+width*(j+1)]*kx[5]
				 +in[i+width*(j+2)]*kx[6] + in[i+1+width*(j+2)]*kx[7]  + in[i+2+width*(j+2)]*kx[8];

			gy =  in[i+width*j]    *ky[0] + in[i+1+width*j]    *ky[1]  + in[i+2+width*j]    *ky[2]
				 +in[i+width*(j+1)]*ky[3] + in[i+1+width*(j+1)]*ky[4]  + in[i+2+width*(j+1)]*ky[5]
				 +in[i+width*(j+2)]*ky[6] + in[i+1+width*(j+2)]*ky[7]  + in[i+2+width*(j+2)]*ky[8];
			
			gm = abs(gx)+abs(gy);

			if(gx == 0)
   	       	{
              if(gy == 0)
              {
              	degree = 0;
              	degreeTemp = 0;
              }          
              else
              {
              	degree = 90;
              	degreeTemp = 90;
              }              
           	}
           	else
           	{
            	
				degreeTemp = atan2(gy, gx)*180/MU_PI;
				if (((degreeTemp < 22.5) && (degreeTemp > -22.5)) || (degreeTemp > 157.5) || (degreeTemp < -157.5))
					degreeTemp = 0;
				if (((degreeTemp > 22.5) && (degreeTemp < 67.5)) || ( (degreeTemp < -112.5) && (degreeTemp > -157.5)))
					degreeTemp = 45;
				if (((degreeTemp > 67.5) && (degreeTemp < 112.5)) || ( (degreeTemp < -67.5) && (degreeTemp > -112.5)))
					degreeTemp = 90;
				if (((degreeTemp > 112.5) && (degreeTemp < 157.5)) || ( (degreeTemp < -22.5) && (degreeTemp > -67.5)))
					degreeTemp = 135;
				//LUT include atan.h
				//tempx = abs(gx); tempy = abs(gy);
            	//degree = atantbl[tempy][tempx];
				//printf("tbl:%d atan2:%f \n", degree, degreeTemp);
              	//if(((gx<0 && gy> 0)||(gx > 0 && gy < 0))&&(degree==45))
              	//	degree = 135;
           	}

           	dir[i+1+width*(j+1)] = degreeTemp;
           	out[i+1+width*(j+1)] = gm;	
		}
}



/*===========================================================================================*/
/*   muCannyEdge																             */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   https://en.wikipedia.org/wiki/Canny_edge_detector                                       */
/*   http://www.pages.drexel.edu/~nk752/cannyTut2.html                                       */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   double threshold could be using other methodology to find                               */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*   *dst --> canny img                                                                      */
/*   th --> hysterisis thresholding                                                          */
/*===========================================================================================*/
muError_t muCannyEdge(const muImage_t *src, muImage_t *dst, muDoubleThreshold_t th)
{
	muError_t ret;
	muImage_t *gausImg, *magImg, *dirImg;
	muSize_t size;

	MU_8U kernel[25] = {2,4,5,4,2,4,9,12,9,4,5,12,15,12,5,4,9,12,9,4,2,4,5,4,2};
	MU_8S gx[9] = {-1,-1,-1,0,0,0,1,1,1};
	MU_8S gy[9] = {-1,0,1,-1,0,1,-1,0,1};

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	size.width = src->width;
	size.height = src->height;

	gausImg = muCreateImage(size, MU_IMG_DEPTH_16S, 1);
	magImg = muCreateImage(size, MU_IMG_DEPTH_16S, 1);
	dirImg = muCreateImage(size, MU_IMG_DEPTH_8U, 1);

	muSetZero(gausImg);
	muSetZero(magImg);
	muSetZero(gausImg);

	muFilter16S55(src, gausImg, kernel, 159);
	edgeFilter(gausImg, magImg, dirImg, gx, gy, 2);
	nonMaxSuppress(magImg, dirImg, dst, th, 3);
	
	muReleaseImage(&gausImg);
	muReleaseImage(&magImg);
	muReleaseImage(&dirImg);

	return MU_ERR_SUCCESS;
}

