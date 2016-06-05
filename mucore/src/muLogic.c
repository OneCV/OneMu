/* ------------------------------------------------------------------------- /
 *
 * Module: muLogic.c
 * Author: Joe Lin
 * Create date: 01/16/2012
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


