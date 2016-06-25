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
 * Module: muLogic.c
 * Author: Joe Lin
 *
 * Description:
 *  This file is presented the logic operation and calculation of image
 *
 -------------------------------------------------------------------------- */

/* MU include files */
#include "muCore.h"


/*===========================================================================================*/
/*   muAnd                                                                                  */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   MU_8U th1 --> threshold1                                                                */
/*   MU_8U th2 --> threshold2                                                                */
/*===========================================================================================*/
muError_t muAnd(const muImage_t *src1, muImage_t *src2, muImage_t *dst)
{
	MU_8U *in1, *in2, *out;
	MU_32S i;
	MU_32S width,height;
	muError_t ret;

	ret = muCheckDepth(6, src1, MU_IMG_DEPTH_8U, src2, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src1->channels != 1 || src2->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	in1 = src1->imagedata;
	in2 = src2->imagedata;

	out = dst->imagedata;

	width = dst->width;
	height = dst->height;

	i = width*height;
	do
	{
		*out = (MU_8U)(*in1 & *in2);
		in1++;
		in2++;
		out++;

	}while(--i);

	return MU_ERR_SUCCESS;
}





/*===========================================================================================*/
/*   muSub                                                                                  */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   MU_8U th1 --> threshold1                                                                */
/*   MU_8U th2 --> threshold2                                                                */
/*===========================================================================================*/
muError_t muSub(const muImage_t *src1, muImage_t *src2, muImage_t *dst)
{
	MU_8U *in1, *in2, *out;
	MU_32S i;
	MU_32S width,height;
	muError_t ret;

	ret = muCheckDepth(6, src1, MU_IMG_DEPTH_8U, src2, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src1->channels != 1 || src2->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	in1 = src1->imagedata;
	in2 = src2->imagedata;

	out = dst->imagedata;

	width = dst->width;
	height = dst->height;

	i = width*height;
	do
	{
		*out = (MU_8U)abs((*in1 - *in2));
		in1++;
		in2++;
		out++;

	}while(--i);

	return MU_ERR_SUCCESS;
}




/*===========================================================================================*/
/*   muOr                                                                                  */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   MU_8U th1 --> threshold1                                                                */
/*   MU_8U th2 --> threshold2                                                                */
/*===========================================================================================*/
muError_t muOr(const muImage_t *src1, muImage_t *src2, muImage_t *dst)
{
	MU_8U *in1, *in2, *out;
	MU_32S i;
	MU_32S width,height;
	muError_t ret;

	ret = muCheckDepth(6, src1, MU_IMG_DEPTH_8U, src2, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src1->channels != 1 || src2->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	in1 = src1->imagedata;
	in2 = src2->imagedata;

	out = dst->imagedata;

	width = dst->width;
	height = dst->height;

	i = width*height;
	do
	{
		*out = (MU_8U)(*in1 | *in2);
		in1++;
		in2++;
		out++;

	}while(--i);

	return MU_ERR_SUCCESS;
}


