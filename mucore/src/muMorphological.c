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
 * Module: muMorphological.c
 * Author: Joe Lin
 *
 * Description:
 *  This file is presented the morphological processing  
 *
 -------------------------------------------------------------------------- */

/* MU include files */
#include "muCore.h"


/*===========================================================================================*/
/*   muDilate33                                                                             */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a dilation of morphological processing with 3*3 square kernel.    */
/*                                                                                           */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/
muError_t muDilate33(const muImage_t *src, muImage_t *dst)
{
	MU_8U *in, *out;
	MU_8U center;
	MU_32S x,y;
	MU_32S width, height;
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

	in = src->imagedata;
	out = dst->imagedata;

	if(in == out)
	{
		return MU_ERR_INVALID_PARAMETER;
	}

	width = dst->width;
	height = dst->height;

	for(y=0; y<(height-2); y++)
	{
		for(x=0; x<(width-2); x++)
		{
			center = in[(y+1)*width+x+1];

			if(center == 255)
			{
				out[y*width+x]   = 255;
				out[y*width+x+1] = 255;
				out[y*width+x+2] = 255;

				out[(y+1)*width+x]   = 255;
				out[(y+1)*width+x+1] = 255;
				out[(y+1)*width+x+2] = 255;

				out[(y+2)*width+x]   = 255;
				out[(y+2)*width+x+1] = 255;
				out[(y+2)*width+x+2] = 255;
			}
		}
	}

	return MU_ERR_SUCCESS;
}



/*===========================================================================================*/
/*   muErode33                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a erosion of morphological processing with 3*3 square kernel.     */
/*                                                                                           */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/ 
muError_t muErode33(const muImage_t *src, muImage_t *dst)
{
	MU_8U *in, *out;
	MU_32S x,y;
	MU_32S width, height;
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

	in = src->imagedata;
	out = dst->imagedata;

	if(in == out)
	{
		return MU_ERR_INVALID_PARAMETER;
	}

	width = dst->width;
	height = dst->height;

	for(y=0; y<(height-2); y++)
	{
		for(x=0; x<(width-2); x++)
		{
			if((in[y*width+x]==255)&(in[y*width+x+1]==255)&(in[y*width+x+2]==255)
					&(in[(y+1)*width+x]==255)&(in[(y+1)*width+x+1]==255)&(in[(y+1)*width+x+2]==255)
					&(in[(y+2)*width+x]==255)&(in[(y+2)*width+x+1]==255)&(in[(y+2)*width+x+2]==255))
			{
				out[(y+1)*width+x+1] = 255;                                                                                                                                                              
			}	
		}
	}

	return MU_ERR_SUCCESS;

}





/*===========================================================================================*/
/*   muDilate55                                                                             */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a dilation of morphological processing with 5*5 square kernel.    */
/*                                                                                           */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/
muError_t muDilate55(const muImage_t *src, muImage_t *dst)
{
	MU_8U *in, *out;
	MU_8U center;
	MU_32S x,y;
	MU_32S width, height;
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

	in = src->imagedata;
	out = dst->imagedata;

	if(in == out)
	{
		return MU_ERR_INVALID_PARAMETER;
	}

	width = dst->width;
	height = dst->height; 

	for(y=0; y<(height-4); y++)
		for(x=0; x<(width-4); x++)
		{
			center=in[(y+2)*width+x+2];

			if(center == 255)
			{
				out[y*width+x]   = 255;
				out[y*width+x+1] = 255;
				out[y*width+x+2] = 255;
				out[y*width+x+3] = 255;
				out[y*width+x+4] = 255;

				out[(y+1)*width+x]   = 255;
				out[(y+1)*width+x+1] = 255;
				out[(y+1)*width+x+2] = 255;
				out[(y+1)*width+x+3] = 255;
				out[(y+1)*width+x+4] = 255;

				out[(y+2)*width+x]   = 255;
				out[(y+2)*width+x+1] = 255;
				out[(y+2)*width+x+2] = 255;
				out[(y+2)*width+x+3] = 255;
				out[(y+2)*width+x+4] = 255;

				out[(y+3)*width+x]   = 255;
				out[(y+3)*width+x+1] = 255;
				out[(y+3)*width+x+2] = 255;
				out[(y+3)*width+x+3] = 255;
				out[(y+3)*width+x+4] = 255;

				out[(y+4)*width+x]   = 255;
				out[(y+4)*width+x+1] = 255;
				out[(y+4)*width+x+2] = 255;
				out[(y+4)*width+x+3] = 255;
				out[(y+4)*width+x+4] = 255;
			} 
		}

	return MU_ERR_SUCCESS;

}



/*===========================================================================================*/
/*   muErode55                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a erosion of morphological processing with 5*5 square kernel.     */
/*                                                                                           */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/ 
muError_t muErode55(const muImage_t *src, muImage_t *dst)
{
	MU_8U *in, *out;
	MU_32S x,y;
	MU_32S width, height;
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

	in = src->imagedata;
	out = dst->imagedata;

	if(in == out)
	{
		return MU_ERR_INVALID_PARAMETER;
	}

	width = dst->width;
	height = dst->height;

	for(y=0; y<(height-4); y++)
		for(x=0; x<(width-4); x++)
		{

			if((in[y*width+x]==255)&(in[y*width+x+1]==255)&(in[y*width+x+2]==255)&(in[y*width+x+3]==255)
					&(in[y*width+x+4]==255)&(in[(y+1)*width+x]==255)&(in[(y+1)*width+x+1]==255)
					&(in[(y+1)*width+x+2]==255)&(in[(y+1)*width+x+3]==255)&(in[(y+1)*width+x+4]==255)
					&(in[(y+2)*width+x]==255)&(in[(y+2)*width+x+1]==255)&(in[(y+2)*width+x+2]==255)&(in[(y+2)*width+x+3]==255)
					&(in[(y+2)*width+x+4]==255)&(in[(y+3)*width+x]==255)&(in[(y+3)*width+x+1]==255)&(in[(y+3)*width+x+2]==255)
					&(in[(y+3)*width+x+3]==255)&(in[(y+3)*width+x+4]==255)&(in[(y+4)*width+x]==255)&(in[(y+4)*width+x+1]==255)
					&(in[(y+4)*width+x+2]==255)&(in[(y+4)*width+x+3]==255)&(in[(y+4)*width+x+4]==255))
			{
				out[(y+2)*width+x+2] = 255;                                                                                                                                                              
			}
		}

	return MU_ERR_SUCCESS;

}

/*===========================================================================================*/
/*   muDilateCross33                                                                        */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a dilation of morphological processing with 3*3 cross kernel.    */
/*                                                                                           */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/

muError_t muDilateCross33(const muImage_t *src, muImage_t *dst)
{
	MU_8U *in, *out;
	MU_8U center;
	MU_32S x,y;
	MU_32S width, height;

	if(src->depth != MU_IMG_DEPTH_8U &&
			dst->depth != MU_IMG_DEPTH_8U) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}

	in = src->imagedata;
	out = dst->imagedata;

	if(in == out)
	{
		return MU_ERR_INVALID_PARAMETER;
	}

	width = dst->width;
	height = dst->height;

	for(y = 0; y < (height-2); y++)
	{
		for(x = 0; x < (width-2); x++)
		{
			center = in[ (y+1) * width + (x+1) ];

			if(center == 255)
			{
				out[ y * width + (x+1)] = 255;

				out[(y+1) * width + x ] = 255;
				out[(y+1) * width + (x+1) ] = 255;
				out[(y+1) * width + (x+2) ] = 255;

				out[(y+2) * width + (x+1)] = 255;
			}
		}
	}

	return MU_ERR_SUCCESS;

}

/*===========================================================================================*/
/*   muErodeCross33                                                                         */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a erosion of morphological processing with 3*3 square kernel.     */
/*                                                                                           */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/ 

muError_t muErodeCross33(const muImage_t *src, muImage_t *dst)
{
	MU_8U *in, *out;
	MU_32S x,y;
	MU_32S width, height;

	if(src->depth != MU_IMG_DEPTH_8U &&
			dst->depth != MU_IMG_DEPTH_8U) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}


	in = src->imagedata;
	out = dst->imagedata;

	if(in == out)
	{
		return MU_ERR_INVALID_PARAMETER;
	}

	width = dst->width;
	height = dst->height;

	for(y = 0; y < (height - 2); y++)
	{
		for(x = 0; x < (width - 2); x++)
		{
			if((in[y * width + (x+1)] == 255)
				&(in[(y+1) * width + x] == 255)&(in[(y+1) * width + (x+1)] == 255)&(in[(y+1) * width + (x+2)] == 255)
				&(in[(y+2) * width + (x+1)] == 255) )
			{
				out[(y+1) * width + (x+1)] = 255;
			}
		}
	}

	return MU_ERR_SUCCESS;

}

/*===========================================================================================*/
/*   muGrayDilate33                                                                         */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a erosion of morphological processing with 3*3 square kernel.     */
/*                                                                                           */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/ 
muError_t muGrayDilate33(const muImage_t *src, muImage_t *dst, MU_8U *se)
{
	MU_8U max;
	MU_8U *in, *out;
	MU_8U se_array[9];
	MU_8U data[9];
	MU_32S x,y;
	MU_32S i;
	MU_32S width, height;
	MU_32S ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	in = src->imagedata;
	out = dst->imagedata;

	if(in == out)
	{
		return MU_ERR_INVALID_PARAMETER;
	}

	width = dst->width;
	height = dst->height;

	if(se == NULL)
	{
		for(i=0; i<9; i++)
		{
			se_array[i] = 0xFF;
		}
	}

	for(y=0; y<(height-2); y++)
	{
		for(x=0; x<(width-2); x++)
		{
			max = 0;

			data[0] = in[x+width*y];
			data[1] = in[x+1+width*y];
			data[2] = in[x+2+width*y];
			
			data[3] = in[x+width*(y+1)];
			data[4] = in[x+1+width*(y+1)];
			data[5] = in[x+2+width*(y+1)];

			data[6] = in[x+width*(y+2)];
			data[7] = in[x+1+width*(y+2)];
			data[8] = in[x+2+width*(y+2)];

			for(i=0; i<9; i++)
			{
				if((data[i]>max)&&se_array[i])
				max = data[i];
			}

			out[x+1+width*(y+1)] = max;
		}
	}

	return MU_ERR_SUCCESS;

}

/*===========================================================================================*/
/*   muGrayErode33                                                                          */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a erosion of morphological processing with 3*3 square kernel.     */
/*                                                                                           */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/ 
muError_t muGrayErode33(const muImage_t *src, muImage_t *dst, MU_8U *se)
{
	MU_8U min;
	MU_8U *in, *out;
	MU_8U se_array[9];
	MU_8U data[9];
	MU_32S x,y;
	MU_32S i;
	MU_32S width, height;
	MU_32S ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	in = src->imagedata;
	out = dst->imagedata;

	if(in == out)
	{
		return MU_ERR_INVALID_PARAMETER;
	}

	width = dst->width;
	height = dst->height;

	if(se == NULL)
	{
		for(i=0; i<9; i++)
		{
			se_array[i] = 0xFF;
		}
	}

	for(y=0; y<(height-2); y++)
	{
		for(x=0; x<(width-2); x++)
		{
			min = 0xFF;

			data[0] = in[x+width*y];
			data[1] = in[x+1+width*y];
			data[2] = in[x+2+width*y];
			
			data[3] = in[x+width*(y+1)];
			data[4] = in[x+1+width*(y+1)];
			data[5] = in[x+2+width*(y+1)];

			data[6] = in[x+width*(y+2)];
			data[7] = in[x+1+width*(y+2)];
			data[8] = in[x+2+width*(y+2)];

			for(i=0; i<9; i++)
			{
				if((data[i]<min)&&se_array[i])
				min = data[i];
			}

			out[x+1+width*(y+1)] = min;
		}
	}

	return MU_ERR_SUCCESS;

}
