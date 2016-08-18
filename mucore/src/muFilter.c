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
 * Module: muFilter.c
 * Author: Joe Lin
 *
 * Description:
 *    Filters
 *
 -------------------------------------------------------------------------- */

#include "muCore.h"

/*===========================================================================================*/
/*   muFilter55                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a 5x5 filter convolution.                                         */
/*                                                                                           */
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*   muImage_t *dst --> output image                                                         */
/*   selection 1~2, two difference masks are able to use                                     */
/*   kernel is stucture element                                                              */
/*   norm is sum of all the kernel value                                                     */
/*===========================================================================================*/
muError_t muFilter55( const muImage_t* src, muImage_t* dst, const MU_8S kernel[], const MU_8U norm)
{
	MU_32S i, j, temp;
	MU_32S width, height;
	muError_t ret;
	MU_8U* in, *out; 

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

			out[j+2+width*(i+2)] = (MU_8U)(temp/(MU_32F)norm);
		}
	}

	return MU_ERR_SUCCESS;
}


/*===========================================================================================*/
/*   muFilter33                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a 3x3 filter convolution.                                         */
/*                                                                                           */
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*   muImage_t *dst --> output image                                                         */
/*   selection 1~2, two difference masks are able to use                                     */
/*   kernel is stucture element                                                              */
/*   norm is sum of all the kernel value                                                     */
/*===========================================================================================*/
muError_t muFilter33( const muImage_t* src, muImage_t* dst, const MU_8S kernel[], const MU_8U norm)
{
	MU_32S i, j, temp;
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

	
	for( i=1; i<(src->height-1); i++ )
	{
		for( j=1; j<(src->width-1); j++ )
		{
			temp = 0;
			temp += (src->imagedata[(i-1)*src->width+(j-1)]*kernel[0]);
			temp += (src->imagedata[(i-1)*src->width+(j+0)]*kernel[1]);
			temp += (src->imagedata[(i-1)*src->width+(j+1)]*kernel[2]);
			temp += (src->imagedata[(i+0)*src->width+(j-1)]*kernel[3]);
			temp += (src->imagedata[(i+0)*src->width+(j+0)]*kernel[4]);
			temp += (src->imagedata[(i+0)*src->width+(j+1)]*kernel[5]);
			temp += (src->imagedata[(i+1)*src->width+(j-1)]*kernel[6]);
			temp += (src->imagedata[(i+1)*src->width+(j+0)]*kernel[7]);
			temp += (src->imagedata[(i+1)*src->width+(j+1)]*kernel[8]);

			dst->imagedata[i*src->width+j] = temp/(MU_32F)norm;
		}
	}

	return MU_ERR_SUCCESS;
}


muError_t muMedian33(const muImage_t *src, muImage_t *dst)
{
	MU_8U temp, flag;
	MU_32S i,j;
	MU_32S x,y;
	MU_32S width, height;
	muError_t ret;
	MU_8U *in, *out;
	MU_8U data[9];

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
			data[0] = in[i+width*j];
			data[1] = in[i+1+width*j];
			data[2] = in[i+2+width*j];

			data[3] = in[i+width*(j+1)];
			data[4] = in[i+1+width*(j+1)];
			data[5] = in[i+2+width*(j+1)];

			data[6] = in[i+width*(j+2)];
			data[7] = in[i+1+width*(j+2)];
			data[8] = in[i+2+width*(j+2)];

			for(x=0; x<9-1; x++)
			{
				flag = 0;
				for(y=0; y<9-x-1; y++)
				{
					if(data[y] > data[y+1])
					{
						flag = 1;
						temp = data[y];
						data[y] = data[y+1];
						data[y+1] = temp;
					}
				}

				if(!flag)
				break;
			}
			
			out[i+1+width*(j+1)] = data[4];
		}

	return MU_ERR_SUCCESS;
}

static MU_8U search_median_value(MU_8U Numarry[])
{
	// bitonic sorting.	
	MU_8U med; 
	MU_32S i=0, temp;
	MU_32S j=0;
	MU_32S k=0, l=0; 
	MU_32S m=0;

	while(i<4)
	{
		if(i%2==0)
		{
			if( Numarry[i*2] > Numarry[i*2+1])
			{
				temp = Numarry[i*2];
				Numarry[i * 2] = Numarry[i * 2 +1];
				Numarry[i * 2 +1] = temp;
			}
		}
		else
		{
			if( Numarry[ i * 2] < Numarry[i * 2 + 1])
			{
				temp = Numarry[ i * 2];
				Numarry[i * 2] = Numarry[i * 2 +1];
				Numarry[i * 2 +1] = temp;
			}
		}
		++i;
	}

	i=0;
	while( i < 2)
	{
		while( j < 2)
		{
			if( i % 2 == 0)
			{
				if( Numarry[ i * 4 + j] >  Numarry[ i * 4 + j + 2])
				{
					temp = Numarry[ i * 4 + j];
					Numarry[ i * 4 + j] = Numarry[ i * 4 + j + 2];
					Numarry[ i * 4 + j + 2] = temp;
				}
			}
			else
			{
				if( Numarry[ i * 4 + j] <  Numarry[ i * 4 + j + 2])
				{
					temp = Numarry[ i * 4 + j];
					Numarry[ i * 4 + j] = Numarry[ i * 4 + j + 2];
					Numarry[ i * 4 + j + 2] = temp;
				}
			}// end else
			++j;
		}// end while loop j.
		j = 0; // clesr value j. 
		++i;
	}

	i = 0, j = 0;

	while(j < 4)
	{
		if(j < 2 )
		{
			if( Numarry[ j * 2 ] > Numarry[ j * 2 + 1] )
			{
				temp = Numarry[ j * 2 ];
				Numarry[ j * 2 ] = Numarry[ j * 2 + 1];
				Numarry[ j * 2 + 1] = temp;
			}
		}
		else
		{
			if( Numarry[ j * 2 ] < Numarry[ j * 2 + 1] )
			{
				temp = Numarry[ j * 2 ];\
				Numarry[ j * 2 ] = Numarry[ j * 2 + 1];
				Numarry[ j * 2 + 1] = temp;
			}
		}
		++j;
	}
	j = 0;
	while( j < 4)
	{
		if( Numarry[ j ] > Numarry[ j + 4])
		{
			temp = Numarry[ j ];
			Numarry[ j ] = Numarry[ j + 4];
			Numarry[ j + 4] = temp;
		}
		++j;
	}// get the bitonic sequence that is dived into min and max group.

	while( k < 2)
	{
		while( l < 2)
		{
			if( Numarry[ k * 4 + l] > Numarry[ k * 4 + l + 2])
			{
				temp = Numarry[ k * 4 + l];
				Numarry[ k * 4 + l] = Numarry[ k * 4 + l + 2];
				Numarry[ k * 4 + l + 2] = temp;
			}
			++l;
		}
		l = 0;
		++k;
	}

	k = 0, l = 0;

	while( m < 4)
	{
		if( Numarry[ m * 2] > Numarry[ m * 2 + 1])
		{
			temp = Numarry[ m * 2];
			Numarry[ m * 2 ] = Numarry[ m * 2 + 1];
			Numarry[ m * 2 + 1] = temp;
		}
		++m;
	}

	//decide the median value.
	if( Numarry[8] <= Numarry[3])
		med = Numarry[3];
	else if( Numarry[8] >= Numarry[4])
		med = Numarry[4];
	else
		med = Numarry[8];

	return med;
}

/*  Fast Median Filter by biotonic search */
muError_t muFastMedian33(muImage_t * src, muImage_t * dst)
{
	MU_32S i,j,ret;
	MU_32S width, height;
	MU_8U data[9];
	MU_8U *in, *out;
	MU_8U median;

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
	width = src->width;
	height = src->height;

	for(j=0; j<(height-2); j++)
		for(i=0; i<(width-2); i++)
		{
			data[0] = in[i+width*j];
			data[1] = in[i+1+width*j];
			data[2] = in[i+2+width*j];

			data[3] = in[i+width*(j+1)];
			data[4] = in[i+1+width*(j+1)];
			data[5] = in[i+2+width*(j+1)];

			data[6] = in[i+width*(j+2)];
			data[7] = in[i+1+width*(j+2)];
			data[8] = in[i+2+width*(j+2)];
		
			median = search_median_value(data);//get the median value.
			
			out[i+1+width*(j+1)] = median;
		}

	return MU_ERR_SUCCESS;
}
