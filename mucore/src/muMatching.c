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
 * Module: muMatching.c
 * Author: Joe Lin
 *
 * Description:
 *  This file is presented the logic operation and calculation of image
 *
 -------------------------------------------------------------------------- */

/* MU include files */
#include "muCore.h"

typedef struct _muMSEInfo
{
	MU_64F oriData; // original data
	MU_64F norData; // normalized data 0~1
}muMSEInfo_t;

// normalized mean squared error -> nmse
// mse/max_mse
muError_t muMSE(const muImage_t *src1, const muImage_t *src2, muMSEInfo_t *mseInfo)
{
	MU_32S i,j;
	MU_32S area;
	MU_64F mse, maxmse, nmse;
	MU_64F bmse, gmse, rmse;
	MU_64F rsum, gsum, bsum;
	MU_64F sum, nsum;

	if((src1->channels != src2->channels) || \
	(src1->width != src2->width) || (src1->height != src2->height))
		{
		return MU_ERR_NOT_SUPPORT;
	}

	area = src1->width * src1->height;
	sum = 0;
	nsum = 0;
	rsum = 0;
	gsum = 0;
	bsum = 0;
	if(src1->channels == 1)
	{
		for(i=0; i < area; i++)
		{
			sum += pow((src1->imagedata[i] - src2->imagedata[i]), 2);
			nsum += pow(255, 2);
		}
		mse = (sum/(MU_64F)area);
		maxmse = (nsum/(MU_64F)area);
		nmse = (mse/maxmse);
	}
	else
	{
		for(j=0, i=0; j < area; j++, i+=3)
		{
			rsum += pow((src1->imagedata[i] - src2->imagedata[i]), 2);
			bsum += pow((src1->imagedata[i+1] - src2->imagedata[i+1]), 2);
			gsum += pow((src1->imagedata[i+2] - src2->imagedata[i+2]), 2);
			nsum += pow(255,2);
		}
		rmse = (rsum/(MU_64F)area);
		gmse = (gsum/(MU_64F)area);
		bmse = (bsum/(MU_64F)area);

		sum = rmse + gmse + bmse;
		mse = (sum/(MU_64F)(3.0F));
		maxmse = (nsum/(MU_64F)area);
		nmse = (mse/maxmse);
	}

	mseInfo->oriData = mse;
	mseInfo->norData = nmse;

	return MU_ERR_SUCCESS;
}

//normalized root mean squared error -> rmse
muError_t muRMSE(const muImage_t *src1, const muImage_t *src2, muMSEInfo_t *rmse)
{
	muMSEInfo_t mse;
	if((src1->channels != src2->channels) || \
	(src1->width != src2->width) || (src1->height != src2->height))
	{
		return MU_ERR_NOT_SUPPORT;
	}

	muMSE(src1, src2, &mse);

	rmse->oriData = (MU_64F)sqrt((MU_64F)(mse.oriData));
	rmse->norData = (MU_64F)sqrt((MU_64F)(mse.norData));

	return MU_ERR_SUCCESS;
}

muError_t muPSNR(const muImage_t *src1, const muImage_t *src2, MU_64F *psnr)
{
	muMSEInfo_t mse;
	if((src1->channels != src2->channels) || \
	(src1->width != src2->width) || (src1->height != src2->height))
	{
		return MU_ERR_NOT_SUPPORT;
	}

	muMSE(src1, src2, &mse);
	//8bit image 255^2 = 65025
	*psnr = 10*log10((MU_64F)(65025.0/mse.oriData));

	return MU_ERR_SUCCESS;
}

// normalized cross correlation - behavior code
// channel fixed due to decrease the while loop
// f(x,y) t(x,y) f window and t window matching
// N = w x h (image area)
// bar = mean
// std = standard deviation
// (f(x,y) - f_bar/f_std x t(x,y) - t_bar/t_std)/N
muError_t muNCC(const muImage_t *src1, const muImage_t *src2, MU_64F *ncc)
{
	MU_32S i,j;
	MU_32S area;
	MU_64F sum1 = 0, sum2 = 0, sum3 = 0;
	MU_64F sum4 = 0, sum5 = 0, sum6 = 0;
	MU_64F mean1, mean2, mean3;
	MU_64F mean4, mean5, mean6;
	MU_64F var1, var2, var3;
	MU_64F var4, var5, var6;
	MU_64F std1, std2, std3;
	MU_64F std4, std5, std6;
	MU_64F ncc1, ncc2, ncc3, nncc;

	if((src1->channels != src2->channels) || \
	(src1->width != src2->width) || (src1->height != src2->height))
	{
		return MU_ERR_NOT_SUPPORT;
	}

	area = (src1->width * src1->height);

	if(src1->channels == 1)
	{
		// get mean
		for(i=0; i < area; i++)
		{
			sum1 += src1->imagedata[i];
			sum2 += src2->imagedata[i];
		}
		mean1 = sum1/(MU_64F)area;
		mean2 = sum2/(MU_64F)area;

		sum1 = 0;
		sum2 = 0;
		//get variance
		for(i=0; i < area; i++)
		{
			sum1 += pow((src1->imagedata[i] - mean1), 2);
			sum2 += pow((src2->imagedata[i] - mean2), 2);
		}

		var1 = sum1/(MU_64F)area;
		var2 = sum2/(MU_64F)area;
		
		std1 = sqrt(var1);
		std2 = sqrt(var2);

		sum1 = 0;
		for(i=0; i < area; i++)
		{
			sum1 += ((src1->imagedata[i] - mean1)*(src2->imagedata[i] - mean2))/(std1*std2);
		}

		nncc = sum1/((MU_64F)area);
		*ncc = (nncc+1)/(MU_64F)(2.0);
	}
	else
	{
		//RGB channels, rms_rgb_ncc = sqrt((ncc-r^2 + ncc-g^2+ncc-b^2)/3)
		// get mean
		for(j=0, i=0; j < area; j++, i+=3)
		{
			sum1 += src1->imagedata[i];
			sum2 += src1->imagedata[i+1];
			sum3 += src1->imagedata[i+2];
			sum4 += src2->imagedata[i];
			sum5 += src2->imagedata[i+1];
			sum6 += src2->imagedata[i+2];
		}
	
		mean1 = sum1/(MU_64F)area;
		mean2 = sum2/(MU_64F)area;
		mean3 = sum3/(MU_64F)area;
		mean4 = sum4/(MU_64F)area;	
		mean5 = sum5/(MU_64F)area;
		mean6 = sum6/(MU_64F)area;

		sum1 = 0; sum2 = 0; sum3 = 0;
		sum4 = 0; sum5 = 0; sum6 = 0;
		//get variance
		for(i=0, j=0; j < area; j++, i+=3)
		{
			sum1 += pow((src1->imagedata[i] - mean1), 2);
			sum2 += pow((src1->imagedata[i+1] - mean2), 2);
			sum3 += pow((src1->imagedata[i+2] - mean3), 2);
			sum4 += pow((src2->imagedata[i] - mean4), 2);
			sum5 += pow((src2->imagedata[i+1] - mean5), 2);
			sum6 += pow((src2->imagedata[i+2] - mean6), 2);
		}

		var1 = sum1/(MU_64F)area;
		var2 = sum2/(MU_64F)area;
		var3 = sum3/(MU_64F)area;
		var4 = sum4/(MU_64F)area;
		var5 = sum5/(MU_64F)area;
		var6 = sum6/(MU_64F)area;
		
		std1 = sqrt(var1); std2 = sqrt(var2); std3 = sqrt(var3);
		std4 = sqrt(var4); std5 = sqrt(var5); std6 = sqrt(var6);

		sum1 = 0;
		sum2 = 0;
		sum3 = 0;
		for(i=0, j=0; j < area; j++, i+=3)
		{
			sum1 += ((src1->imagedata[i] - mean1)*(src2->imagedata[i] - mean4))/(std1*std4);
			sum2 += ((src1->imagedata[i+1] - mean2)*(src2->imagedata[i+1] - mean5))/(std2*std5);	
			sum3 += ((src1->imagedata[i+2] - mean3)*(src2->imagedata[i+2] - mean6))/(std3*std6);
		}

		ncc1 = sum1/((MU_64F)area);
		ncc2 = sum2/((MU_64F)area);
		ncc3 = sum3/((MU_64F)area);
		
		nncc = sqrt((pow(ncc1,2)+pow(ncc2,2)+pow(ncc3,2))/(MU_64F)3.0);
		//*ncc = nncc-(-1.0)/(MU_64F)(2); //(xi-xmin/xmax-xmin); (0~1)
		*ncc = nncc;
	}

	return MU_ERR_SUCCESS;
}


