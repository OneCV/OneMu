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
/*	 code:1->yuv422 2->RGB 3->yuv420														 */
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
	
		case 3:
		dst->cha  = src->imagedata;
		dst->chb  = src->imagedata + (src->width * src->height);
		dst->chc  = src->imagedata + (src->width * src->height) + (src->width*src->height)/4;
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
/*   muYUV420toRGB                                                                          */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*===========================================================================================*/
muError_t muYUV420toRGB(const muImage_t *src, muImage_t *dst)
{
	MU_8U *data;
	MU_32S u,v;
	MU_32S i,j,k;
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

	muSeparateChannel(src, &ssrc, 3);
	
	data = dst->imagedata;

	k=0;
	for(j=0; j<height; j++)
		for(i=0; i<width; i++)
		{

			u = ssrc.chb[(j/2)*(width/2)+(i/2)]-128;
			v = ssrc.chc[(j/2)*(width/2)+(i/2)]-128;

			rdif = v + ((v*103) >> 8);
			gdif = ((u*88)>>8)+((v*183)>>8);
			bdif = u+((u*198)>>8);


			r = ssrc.cha[i+width*j] + rdif;
			//r = ssrc.cha[i] + 1.4075*(ssrc.chc[j]-128);
			if(r > 255)
				r=255;
			if(r < 0)
				r=0;

			g = ssrc.cha[i+width*j] - gdif;
			//g = ssrc.cha[i] - 0.3455*(ssrc.chb[j]-128) - (ssrc.chc[j]-128);
			if(g > 255)
				g = 255;
			if(g < 0)
				g = 0;

			b = ssrc.cha[i+width*j] + bdif;
			//b = ssrc.cha[i] + 1.7790*(ssrc.chb[j]-128);
			if(b > 255)
				b = 255;
			if(b < 0)
				b = 0;

			//printf("r = %d g= %d b = %d\n", r, g, b);

			data[k*3] = b;
			data[k*3+1] = g;
			data[k*3+2] = r;
			k++;
		}

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

static MU_32S getH(MU_32S r, MU_32S g, MU_32S b, MU_32S min, MU_32S max)
{
	MU_32S h = 0;

	if(min == max)
	{
		return 0;
	}
	else if((max == r) && (g >= b))
	{
		h = 60.0 * ((g-b)/(MU_32F)(max-min));
	}
	else if((max == r) && (g < b))
	{
		h = 60.0 * ((g-b)/(MU_32F)(max-min)) + 360;
	}
	else if((max == g))
	{
		h = 60.0 * ((b-r)/(MU_32F)(max-min)) + 120;
	}
	else if((max == b))
	{
		h = 60.0 * ((r-g)/(MU_32F)(max-min)) + 240;
	}	

	return h;
}

static MU_32S getS(MU_32S min, MU_32S max)
{
	MU_32S s = 0;

	if(max != 0)
	{
		s = ((max-min)/(MU_32F)max) * 100;
	}

	return s;
}

typedef struct _colorDataInfo
{
	MU_32S data1, data2, data3;

}colorDataInfo_t; 

static colorDataInfo_t rgb2hsv(MU_32S r, MU_32S g, MU_32S b, MU_32S min, MU_32S max)
{
	colorDataInfo_t hsv;

	//h 0~360
	hsv.data1 = getH(r, g, b, min, max);
	//s 0~100
	hsv.data2 = getS(min, max);
	//v 0~100
	hsv.data3 = (MU_32F)(max/(MU_32F)255.0)*100.0;
	
	return hsv;
}

static muError_t muBubbleSort(MU_32S data[], MU_32S length)
{
	MU_32S i, j, temp;

	// sort
	for(j=0; j<length-1; j++)
		for(i=0; i<length-1-j; i++)
		{
			if(data[i] > data[i+1])
			{
				temp = data[i];
				data[i] = data[i+1];
				data[i+1] = temp; 
			}
		}

	
	return MU_ERR_SUCCESS;
}

/*===========================================================================================*/
/*   muRGB2HSV                                                                               */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*   muImage_t *hsv --> output image                                                         */
/*                                                                                           */
/*	 method:                                                                                 */
/*	 https://zh.wikipedia.org/wiki/HSL_and_HSV                                               */
/*	 verify by www.rapidtables.com/convert/color/rgb-to-hsv.htm                              */
/*===========================================================================================*/
muError_t muRGB2HSV(const muImage_t *rgb, muImage_t *hsv)
{
	MU_32S i, width, height;
	MU_32S b, g, r;
	MU_32S max, min;
	MU_32S data[3];
	MU_8U *rgbData;
	MU_16U *hsvData, h, s, v;
	colorDataInfo_t ret;

	if((rgb->depth != MU_IMG_DEPTH_8U) || (hsv->depth != MU_IMG_DEPTH_16U) || (rgb->channels != 3)) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}
	
	width = rgb->width;
	height = rgb->height;
	rgbData = (MU_8U *)rgb->imagedata;
	hsvData = (MU_16U *)hsv->imagedata;
	for(i=0; i<width*height*3; i+=3)
	{
		b = rgbData[i];
		g = rgbData[i+1];
		r = rgbData[i+2];

		data[0] = r; data[1] = g; data[2] = b;
		
		muBubbleSort(data, 3);
		//sort to find min and max;
		min = data[0]; max = data[2];

		ret = rgb2hsv(r, g, b, min, max);
		//h
		hsvData[i] = ret.data1;
		//s
		hsvData[i+1] = ret.data2;
		//v
		hsvData[i+2] = ret.data3;
	}

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


/*===========================================================================================*/
/*   muRGBtoXYZ                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   reference: http://www.easyrgb.com                                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*===========================================================================================*/

muError_t muRGB2XYZ(const muImage_t *src, muImage_t *dst)
{
	MU_16U y;
	MU_32S i, x;
	MU_32S width, height;
	MU_32F r,g,b;
	MU_8U *in, *out;
	MU_32F *data;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_32F);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 3 || dst->channels != 3)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	width = src->width;
	height = src->height;
	in = (MU_8U *)src->imagedata;
	data = (MU_32F *)dst->imagedata;

	for(i=0; i<width*height; i++)
	{
		// gamma correction
		b = ((MU_32F)in[i*3]/(MU_32F)255);
		g = ((MU_32F)in[i*3+1]/(MU_32F)255);
		r = ((MU_32F)in[i*3+2]/(MU_32F)255);
			
		if(r > 0.04045)
			r = pow(((r+0.055)/(MU_32F)1.055), 2.4F);
		else
			r = (r/(MU_32F)12.92);

		if(g > 0.04045)
			g = pow(((g+0.055)/(MU_32F)1.055), 2.4F);
		else
			g = (g/(MU_32F)12.92);

		if(b > 0.04045)
			b = pow(((b+0.055)/(MU_32F)1.055), 2.4F);
		else
			b = (b/(MU_32F)12.92);

		r = r*100; g = g*100; b = b*100;
		
		data[i*3] = (r * 0.4124F) + (g * 0.3576F) + (b * 0.1805F); //X
		data[i*3+1] = (r * 0.2126F) + (g * 0.7152F) + (b * 0.0722F); //Y
		data[i*3+2] = (r * 0.0193F) + (g * 0.1192F) + (b * 0.9505F); //Z

	}

	return MU_ERR_SUCCESS;
}

/*===========================================================================================*/
/*   muXYZtoLab                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   reference: http://www.easyrgb.com                                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*===========================================================================================*/

muError_t muXYZ2LAB(const muImage_t *src, muImage_t *dst)
{
	MU_32S i;
	MU_32S width, height;
	MU_32F x,y,z;
	MU_32F *in;
	MU_32F *data;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_32F, dst, MU_IMG_DEPTH_32F);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 3 || dst->channels != 3)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	width = src->width;
	height = src->height;
	
	in = (MU_32F*)src->imagedata;
	data = (MU_32F*)dst->imagedata;
	
	for(i=0; i<width*height; i++)
	{
		x = ((MU_32F)in[i*3]/(MU_32F)95.047F);
		y = ((MU_32F)in[i*3+1]/(MU_32F)100.0F);
		z = ((MU_32F)in[i*3+2]/(MU_32F)108.883F);

		if(x > 0.008856)
			x = pow(x, (0.3333F));
		else
			x = (7.787*x) + (16/(MU_32F)116.0F);

		if(y > 0.008856)
			y = pow(y, (0.3333F));
		else
			y = (7.787*y) + (16/(MU_32F)116.0F);

		if(z > 0.008856)
			z = pow(z, (0.3333F));
		else
			z = (7.787*z) + (16/(MU_32F)116.0F);

		data[i*3]   = (116.0F*y) - 16.0F; //L*
		data[i*3+1] = 500.0F * (x-y); //a*
		data[i*3+2] = 200.0F * (y-z); //b*
	}

	return MU_ERR_SUCCESS;
}
