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

			for(j=0; j<(height-1); j++)
				for(i=0; i<(width-1); i++)
				{
					index = i+width*j;

					temp = abs((in[index+1]+in[index+width]+in[index+width+2]+in[index+(width<<1)+1])-(in[index+width+1]<<2));
					temp = temp > 255 ? 255 : temp;
					out[index+width+1]  = (MU_8U)temp;

				}
			break;

		case 2:

			for(j=0; j<(height-1); j++)
				for(i=0; i<(width-1); i++)
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

	for(j=0; j<(height-1); j++)
		for(i=0; i<(width-1); i++)
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

	for(j=0; j<(height-1); j++)
		for(i=0; i<(width-1); i++)
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

