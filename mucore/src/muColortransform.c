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
 * Module: muColortransform.c
 * Author: Joe Lin
 *
 * Description:
 *  This file is presented the color space transformation of image processing 
 *
 -------------------------------------------------------------------------- */

/* MU include files */
#include "muCore.h"

/*===========================================================================================*/
/*   muContraststretch                                                                       */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine stretchs the original data to the user-defined value.                      */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   note that the user-defined value only support 0~255                                     */
/*                                                                                           */ 
/*   This function can be expressed as:                                                      */                    
/*   dst[x,y] = src[x,y]-min/max-min*(2^n-1)                                                 */
/*	                                                                                         */ 
/*   USAGE                                                                                   */
/*   muImage_t *src --> src image                                                            */
/*   muImage_t *dst --> dst image                                                            */
/*   MU_8U stretchvalue --> this is the maximum value for normalization                      */
/*===========================================================================================*/

muError_t muContraststretching(muImage_t *src, muImage_t *dst, MU_8U maxvalue)
{
	MU_8U maxtemp = 0, mintemp = 255;
	MU_8U *in, *out;
	MU_32S i;
	MU_32S width,height;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || src->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	in = src->imagedata;
	out = dst->imagedata;

	width  = dst->width;
	height = dst->height;

	i = width*height-1;
	do
	{
		maxtemp = in[i] > maxtemp ? in[i] : maxtemp;
		mintemp = in[i] < mintemp ? in[i] : mintemp;
	}while(i--);


	i = width*height;
	do
	{
		*out = (MU_8U)(((MU_32F)(*in-mintemp)/(MU_32F)(maxtemp-mintemp))*maxvalue);
		in++;
		out++;
	}while(--i);


	return MU_ERR_SUCCESS; 
}


typedef struct _muSepImage_t
{
	MU_32S width, height;
	MU_8U* cha;
	MU_8U* chb;
	MU_8U* chc;
}muSepImage_t;


/*===========================================================================================*/
/*   muSeparateChannel                                                                      */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine divide the semi-planer data structure into 3 components.                   */
/*	 code:1->yuv422 2->RGB																	 */
/*                                                                                           */
/*===========================================================================================*/

static muError_t muSeparateChannel(const muImage_t *src, muSepImage_t *dst, MU_32S code)
{
	if((src->channels != 3)) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}

	switch(code)
	{
		case 1:
		dst->cha  = src->imagedata;
		dst->chb  = src->imagedata + (src->width * src->height);
		dst->chc  = src->imagedata + (src->width * src->height) + ((src->width*src->height)/2);
		break;

		case 2:
		dst->cha  = src->imagedata;
		dst->chb  = src->imagedata + (src->width * src->height);
		dst->chc  = src->imagedata + (src->width * src->height) + (src->width*src->height);
		break;

		default:
		return MU_ERR_NOT_SUPPORT;
	}

	return MU_ERR_SUCCESS;

}

//TODO
static muError_t muYUV422p2sp(muImage_t *src)
{
	muImage_t *temp;
	muSepImage_t ssrc;
	MU_32S w,h;
	MU_32S i,j = 0;

	w = src->width;
	h = src->height;
	
	temp = muCreateImage(muSize(w,h), MU_IMG_DEPTH_8U, 3);
	
	//memcpy(temp->imagedata, src->imagedata, w*h*src->channels);
	//memcpy(temp->imagedata, src->imagedata, w*h);

	for(i=0; i<w*h; i++)
	{
		temp->imagedata[i] = src->imagedata[i];
	}
	ssrc.width = src->width;
	ssrc.height = src->height; 
	
	muSeparateChannel(src, &ssrc, 1);
	for(i=0, j=0; i<(w*h/2); i++, j+=2)
	{
		temp->imagedata[w*h+j] = ssrc.chb[i];
		temp->imagedata[w*h+j+1] = ssrc.chc[i];
	}
	memcpy(src->imagedata, temp->imagedata, w*h*src->channels);

	
	muReleaseImage(&temp);

	return MU_ERR_SUCCESS;
}


/*===========================================================================================*/
/*   muYUV422toRGB                                                                          */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*===========================================================================================*/
muError_t muYUV422toRGB(const muImage_t *src, muImage_t *dst)
{
	MU_8U *data;
	MU_32S u,v;
	MU_32S i,j;
	MU_32S rdif,gdif,bdif;
	MU_32S r,g,b;
	MU_32S width, height, channels;
	muSepImage_t ssrc;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 3 || dst->channels != 3)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	channels = dst->channels;

	ssrc.width = src->width;
	ssrc.height = src->height; 

	width = src->width;
	height = src->height;

	muSeparateChannel(src, &ssrc, 1);
	
	data = dst->imagedata;

	for(i=0; i<width*height; i++)
	{
		j = i/2;

		u = ssrc.chb[j]-128;
		v = ssrc.chc[j]-128;

		rdif = v + ((v*103) >> 8);
		gdif = ((u*88)>>8)+((v*183)>>8);
		bdif = u+((u*198)>>8);
	

		r = ssrc.cha[i] + rdif;
		//r = ssrc.cha[i] + 1.4075*(ssrc.chc[j]-128);
		if(r > 255)
		r=255;
		if(r < 0)
		r=0;

		g = ssrc.cha[i] - gdif;
		//g = ssrc.cha[i] - 0.3455*(ssrc.chb[j]-128) - (ssrc.chc[j]-128);
		if(g > 255)
		g = 255;
		if(g < 0)
		g = 0;

		b = ssrc.cha[i] + bdif;
		//b = ssrc.cha[i] + 1.7790*(ssrc.chb[j]-128);
		if(b > 255)
		b = 255;
		if(b < 0)
		b = 0;
	
		//printf("r = %d g= %d b = %d\n", r, g, b);

		data[i*3] = b;
		data[i*3+1] = g;
		data[i*3+2] = r;
	}

	return MU_ERR_SUCCESS;

}



/*===========================================================================================*/
/*   muRGB2GaryLevel                                                                        */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*===========================================================================================*/

muError_t muRGB2GrayLevel(const muImage_t *src, muImage_t *dst)
{

	MU_8U r,g,b;
	MU_16U y;
	MU_32S i, x;
	MU_32S width, height;
	MU_8U *in, *out;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 3 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	width = src->width;
	height = src->height;
	in = src->imagedata;
	out = dst->imagedata;

	for(i=0,x=0; i<(width*height)*3; i+=3,x++)
	{
		r = in[i]; b = in[i+1]; g = in[i+2];
		y = (MU_16U)((((0.299*r) + (0.587*g) + (0.114*b))));
		out[x] = y >= 255 ? 255 : y;
	}

	return MU_ERR_SUCCESS;
}




/*===========================================================================================*/
/*   muRGB2Hue                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*===========================================================================================*/

muError_t muRGB2Hue(const muImage_t * src, muImage_t * dst)
{
	unsigned char *in;
	unsigned short *outU16;
	int width, height;
	unsigned char b, g, r;
	int i, x, j;
	unsigned short int pass = 0, idx = 0;
	unsigned char sort[3] = {0};
	unsigned char hold = 0;
	float bufferval = 0;
	short int *Hnum = 0;


	if(src->depth != MU_IMG_DEPTH_8U ||
			dst->depth != MU_IMG_DEPTH_16U ||
			src->channels != 3) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}

	width = src->width;
	height = src->height;

	Hnum = (short int *) calloc(height * width, sizeof(short int));

	in = src->imagedata;
	outU16 = (unsigned short *) dst->imagedata;

	for(i = 0, x = 0; i < (width * height) * 3; i+=3, x++)
	{
		b = in[i]; g = in[i+1]; r = in[i+2];

		sort[0] = b; sort[1] = g; sort[2] = r;
	
		for(pass = 1; pass < 3 ; pass++)// find the min , mean, max : sort[0], sort[1], sort[2]
		{
			for( idx = 0; idx < 2; idx++)
			{
				if( sort[ idx ] > sort[ idx + 1 ])
				{
					hold = sort[ idx ];

					sort[ idx ] = sort[ idx + 1];

					sort[ idx + 1 ] = hold;
				}
			}

			idx = 0;
		}

		pass = 1;

		if( (sort[2] == r) & (g >= b ) )
			bufferval = (float)( 60 * ( g - b ) ) / (float)( sort[2] - sort[0]);

		else if( (sort[2] == r) & (g < b ) )
			bufferval = (float)( 60 * ( g - b ) ) / (float)( sort[2] - sort[0]) + 360;

		else if( sort[2] == g)
			bufferval = (float)( 60 * ( b - r ) ) / (float)( sort[2] - sort[0]) + 120;

		else if( sort[2] == b)
			bufferval = (float)( 60 * ( r - g ) ) / (float)( sort[2] - sort[0]) + 240;

		else if(sort[2] == sort[0])
			bufferval = 0.;

		Hnum[x] = (short int)( bufferval + 0.5 );

	}

	for( i = 0; i < height; i++)
		for( j = 0; j < width; j++)
		{
			outU16[i*width+j] = (unsigned short)Hnum[i*width+j];
		}

	free(Hnum);

	return MU_ERR_SUCCESS;


}


/*===========================================================================================*/
/*   muGraytoRGBA                                                                           */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   transfer the Gray level image to RGBA RGB8888 fomat                                     */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*===========================================================================================*/
muError_t muGraytoRGBA(const muImage_t *src, muImage_t *dst)
{
	MU_32U *outData;
	MU_8U *inData;
	MU_32S i,j;
	MU_32S width, height, channels;
	MU_8U tempData;
	MU_32U outTempData;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_32U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 4)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	width = src->width;
	height = src->height;

	
	outData = (MU_32U *)dst->imagedata;
	inData = (MU_8U *)src->imagedata;

	for(i=0; i<width*height; i++)
	{
		tempData = inData[i];
		outTempData = (0xFF000000 | (tempData<<16) | (tempData<<8) | tempData);
		outData[i] = outTempData;	
	}

	return MU_ERR_SUCCESS;

}

