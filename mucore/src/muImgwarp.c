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
 * Module: muImgwarp.c
 * Author: Joe Lin
 *
 * Description:
 *    Sampling, Interpolation and Geometrical Transforms
 *
 -------------------------------------------------------------------------- */

#include "muCore.h"

/* Resizes image (input array is resized to fit the destination array) */
muError_t muResize( const muImage_t* src, muImage_t* dst,
		MU_32S interpolation MU_DEFAULT( MU_INTER_NN ) )
{
	int i,j,k;
	MU_32S depth = src->depth;
	MU_32S src_x, src_y;
	muError_t ret;

	ret = muCheckDepth(4, src, depth, dst, depth);
	if(ret)
	{
		return ret;
	}

	// nearest-neigbor interpolation
	for( i=0; i<dst->height; i++ )
	{
		for( j=0; j<dst->width; j++ )
		{
			src_x = j*src->width/dst->width;
			src_y = i*src->height/dst->height;

			for( k=0; k<depth; k++ )
			{
				dst->imagedata[(i*dst->width*depth)+j+k] = src->imagedata[(src_y*src->width*depth)+src_x+k];
			}
		}
	}

	return MU_ERR_SUCCESS;
}

/* Down scale image, e.g. v_scale=1, h_scale=2: 2CIF->CIF; v_scale=2, h_scale=2: CIF->QCIF */
//TODO scale = 0 --> prevent this condition
muError_t muDownScale( const muImage_t* src, muImage_t* dst, MU_32S v_scale, MU_32S h_scale)
{
	int i, j, k;
	int x=0, y=0;
	int src_depth = src->depth & 0x00F;
	int dst_depth = dst->depth & 0x00F;
	muError_t ret;

	ret = muCheckDepth(4, src, src_depth, dst, src_depth);
	if(ret)
	{
		return ret;
	}

	else if( (src->width!=(h_scale*dst->width)) || 
			(src->height!=(v_scale*dst->height)) )
	{

		return MU_ERR_INVALID_PARAMETER;
	}


	for( i=0; i<src->height; i+=v_scale )
	{
		x=0;

		for( j=0; j<src->width; j+=h_scale )
		{
			for( k=0; k<src_depth; k++ )
			{
				dst->imagedata[(y*dst->width*dst_depth)+x+k] = src->imagedata[(i*src->width*src_depth)+j+k];
			}

			x++;
		}

		y++;
	}

	return MU_ERR_SUCCESS;
}


/* Down scale image by memcpy, e.g. v_scale=1, h_scale=2: 2CIF->CIF; v_scale=2, h_scale=2: CIF->QCIF */
muError_t muDownScaleMemcpy( const muImage_t* src, muImage_t* dst,  MU_32S v_scale, MU_32S h_scale)
{
    MU_32S i, j, k;
	MU_8U *temp;
	MU_8U *out;
	MU_8U *in;
	MU_32S src_depth = src->depth & 0x00F;
	muError_t ret;

	ret = muCheckDepth(4, src, src_depth, dst, src_depth);
	if(ret)
	{
		return ret;
	}

	if((src->width!=(h_scale*dst->width)) || 
		(src->height!=(v_scale*dst->height)) )
	{

		return MU_ERR_INVALID_PARAMETER;
	}

	out = dst->imagedata;
	in = src->imagedata;

	temp = (MU_8U *)calloc(src->width, sizeof(MU_8U));

	for( i=0; i<src->height; i+=v_scale,  out+=(src->width>>1))
	{
		memcpy(temp, in+(i*src->width), src->width*sizeof(MU_8U ));
		for(j=0, k=0; j<src->width; j+=h_scale, k++)
		{
			out[k] = temp[j];
		}
		memset(temp, 0, src->width*sizeof(MU_8U));
	}

	free(temp);

	return MU_ERR_SUCCESS;
}


muError_t muDownScaleMemcpy422( const muImage_t* src, muImage_t* dst,  MU_32S v_scale, MU_32S h_scale)
{
	MU_32S i, j, k;
	MU_8U *temp;
	MU_8U *out;
	MU_8U *in;
	MU_8U *out_uv;
	MU_8U *in_uv;
	MU_32S src_depth = src->depth & 0x00F;
	muError_t ret;

	ret = muCheckDepth(4, src, src_depth, dst, src_depth);
	if(ret)
	{
		return ret;
	}
	if (src->channels !=3 || dst->channels!=3) return MU_ERR_INVALID_PARAMETER;
	if (src->width%2!=0 || dst->width%2!=0) return MU_ERR_NOT_SUPPORT;
	if((src->width!=(h_scale*dst->width)) || 
			(src->height!=(v_scale*dst->height)) )
	{

		return MU_ERR_INVALID_PARAMETER;
	}

	out = dst->imagedata;
	in = src->imagedata;
	out_uv = out+dst->width*dst->height;
	in_uv = in+src->width*src->height;

	temp = (MU_8U *)calloc(src->width, sizeof(MU_8U));

	for( i=0; i<src->height; i+=v_scale,  out+=(dst->width))
	{
		memcpy(temp, in+(i*src->width), src->width*sizeof(MU_8U ));
		for(j=0, k=0; j<src->width; j+=h_scale, k++)
		{
			out[k] = temp[j];
		}
		memset(temp, 0, src->width*sizeof(MU_8U));
	}
	free(temp);
	temp = (MU_8U *)calloc(src->width/2, sizeof(MU_8U));
	for( i=0; i<src->height*2; i+=v_scale,  out_uv+=((src->width/2)>>1))
	{
		memcpy(temp, in_uv+i*(src->width/2), (src->width/2)*sizeof(MU_8U ));
		for(j=0, k=0; j<src->width/2; j+=h_scale, k++)
		{
			out_uv[k] = temp[j];
		}
		memset(temp, 0, (src->width/2)*sizeof(MU_8U));
	}	

	free(temp);
	return MU_ERR_SUCCESS;
}

muError_t muDownScaleMemcpy420( const muImage_t* src, muImage_t* dst,  MU_32S v_scale, MU_32S h_scale)
{
	MU_32S i, j, k;
	MU_8U *temp;
	MU_8U *out;
	MU_8U *in;
	MU_8U *out_uv;
	MU_8U *in_uv;
	MU_32S src_depth = src->depth & 0x00F;
	muError_t ret;

	ret = muCheckDepth(4, src, src_depth, dst, src_depth);
	if(ret)
	{
		return ret;
	}
	if (src->channels !=3 || dst->channels!=3) return MU_ERR_INVALID_PARAMETER;
	if (src->width%2!=0 || dst->width%2!=0 || src->height%2!=0 || dst->height%2!=0 ) return MU_ERR_NOT_SUPPORT;
	if((src->width!=(h_scale*dst->width)) || 
			(src->height!=(v_scale*dst->height)) )
	{

		return MU_ERR_INVALID_PARAMETER;
	}

	out = dst->imagedata;
	in = src->imagedata;
	out_uv = out+dst->width*dst->height;
	in_uv = in+src->width*src->height;

	temp = (MU_8U *)calloc(src->width, sizeof(MU_8U));

	for( i=0; i<src->height; i+=v_scale,  out+=(dst->width))
	{
		memcpy(temp, in+(i*src->width), src->width*sizeof(MU_8U ));
		for(j=0, k=0; j<src->width; j+=h_scale, k++)
		{
			out[k] = temp[j];
		}
		memset(temp, 0, src->width*sizeof(MU_8U));
	}
	free(temp);
	temp = (MU_8U *)calloc(src->width/2, sizeof(MU_8U));
	for( i=0; i<src->height; i+=v_scale,  out_uv+=((src->width/2)>>1))
	{
		memcpy(temp, in_uv+i*(src->width/2), (src->width/2)*sizeof(MU_8U ));
		for(j=0, k=0; j<src->width/2; j+=h_scale, k++)
		{
			out_uv[k] = temp[j];
		}
		memset(temp, 0, (src->width/2)*sizeof(MU_8U));
	}	

	free(temp);
	return MU_ERR_SUCCESS;
}

muError_t muBilinearScale(muImage_t *in, muImage_t *out)
{
	MU_8U a,b,c,d;
	MU_32S ret;
	MU_32S ix,iy,i,j;
	MU_32S new_w, new_h;
	MU_32S width,height;
	MU_32S index;
	MU_32S src_depth = in->depth & 0x00F;
	MU_8U *inbuf, *outbuf;
	MU_32F fwratio, fhratio;
	MU_32F fx, fy;

	ret = muCheckDepth(4, in, src_depth, out, src_depth);
	if(ret)
	{
		return ret;
	}

	width = in->width;
	height = in->height;

	new_w = out->width;
	new_h = out->height;

	fwratio = width/(float)new_w;
	fhratio = height/(float)new_h;

	inbuf = in->imagedata;
	outbuf = out->imagedata;

	for(j=0; j<new_h; j++)
		for(i=0; i<new_w; i++)
		{
			fx = fwratio*(float)i;	fy = fhratio*(float)j;
			ix = (int)fx;			iy = (int)fy;
			fx = fx - ix;			fy = fy - iy;

			if((ix+1) >= width)
				ix = width-2;

			if((iy+1 >= height))
				iy = height-2;

			index = ix+width*iy;

			a = *(inbuf+index);
			b = *(inbuf+index+1);
			c = *(inbuf+index+width);
			d = *(inbuf+index+width+1);
		
			// unit square f(x,y) = (1-x)(1-y)f(0,0)+ (1-y)xf(1,0)+(1-x)yf(0,1)+xyf(1,1)
			*(outbuf+(i+new_w*j)) = (MU_8U)((1.0-fx)*(1.0-fy)*(float)a+(fx)*(1.0-fy)*(float)b+
									(1.0-fx)*fy*(float)c+fx*fy*(float)d);

		}
	
		return MU_ERR_SUCCESS;
}
