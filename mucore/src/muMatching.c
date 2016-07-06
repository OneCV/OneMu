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


// https://en.wikipedia.org/wiki/Mean_squared_error
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

//https://en.wikipedia.org/wiki/Root-mean-square_deviation
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

//https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio
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
// http://moodlearchive.epfl.ch/2007-2008/mod/resource/view.php?id=115231
// http://scribblethink.org/Work/nvisionInterface/nip.html
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


static MU_64F calNbyN(MU_8U *src1, MU_8U *src2)
{
	MU_32S i,j;
	MU_64F sum1=0, sum2=0, sum3=0;
	MU_64F mean1, mean2, mean3;
	MU_64F var1, var2, var3;
	MU_64F covar1;
	MU_64F temp1, temp2;
	const MU_64F c1 = 6.5025; //c1=(k1L)^2, k1=0.01,L=bits per pixel
	const MU_64F c2 = 58.5225; //c2 = (k2L)^2 k2=0.03
	MU_64F ssim;

	//NxN = 8x8 = 64;
	for(i=0; i<64; i++)
	{
		sum1 += *(src1+i);
		sum2 += *(src2+i);
	}

	mean1 = (MU_64F)sum1/(MU_64F)64.0; 
	mean2 = (MU_64F)sum2/(MU_64F)64.0;

	sum1 = 0; 
	sum2 = 0;
		
	for(i=0; i<64; i++)
	{
		//get variance	
		sum1 += pow((*(src1+i) - mean1), 2);
		sum2 += pow((*(src2+i) - mean2), 2);

		//get co-variance
		sum3 += (*(src1+i)-mean1)*(*(src2+i)-mean2);
	}
	
	var1 = sum1/(MU_64F)64.0; 
	var2 = sum2/(MU_64F)64.0;

	covar1 = sum3/(MU_64F)64.0;
		
	//R, B, G. 3 dimenson ssim
	//(2uxuy+c1)*(2covarxy+c2)/(ux^2+uy^2+C1)(varx+vary+c2)
	temp1 = (2*mean1*mean2+c1);
	temp2 = (2*covar1+c2);
	ssim = (temp1*temp2)/(((mean1*mean1)+(mean2*mean2)+c1)*(var1+var2+c2));

	return ssim;

}

// https://en.wikipedia.org/wiki/Structural_similarity
// mean
// variance
// covariance(x-x_bar)*(y-y_bar)/n
muError_t muSSIM(const muImage_t *src1, const muImage_t *src2, MU_64F *ssim)
{
	MU_32S i,j;
	MU_32S area, count = 0;
	muImage_t *r1, *g1, *b1;
	muImage_t *r2, *g2, *b2;
	muImage_t *r1temp, *g1temp, *b1temp;
	muImage_t *r2temp, *g2temp, *b2temp;

	muSize_t size, subSize;
	muRect_t rect;
	MU_64F ssimR, ssimB, ssimG;
	MU_64F sumR = 0, sumB = 0, sumG = 0;

	if((src1->channels != src2->channels) || \
	(src1->width != src2->width) || (src1->height != src2->height))
	{
		return MU_ERR_NOT_SUPPORT;
	}

	area = src1->width * src1->height;
	size.width = src1->width;
	size.height = src1->height;
	if(src1->width < 8 && src1->height < 8)
	{
		MU_DBG("width & height  <8,  not support!\n");
		return MU_ERR_NOT_SUPPORT;
	}

	if(src1->channels == 1)
	{
		for(i=0; i<size.width-7; i++)
			for(j=0; j<size.height-7; j++)
			{
				rect.x = i;
				rect.y = j;
				rect.width = 8;
				rect.height = 8;
				subSize.width = rect.width;
				subSize.height = rect.height;

				r1temp = muCreateImage(subSize, MU_IMG_DEPTH_8U, 1);
				r2temp = muCreateImage(subSize, MU_IMG_DEPTH_8U, 1);
				muGetSubImage(src1, r1temp, rect);
				muGetSubImage(src2, r2temp, rect);
				sumR += calNbyN(r1temp->imagedata, r2temp->imagedata);
		
				muReleaseImage(&r1temp);
				muReleaseImage(&r2temp);
				count++;
			}

		*ssim = sumR/(MU_64F)count;
	}
	else if(src1->channels == 3)
	{
		r1 = muCreateImage(size, MU_IMG_DEPTH_8U, 1);
		b1 = muCreateImage(size, MU_IMG_DEPTH_8U, 1);
		g1 = muCreateImage(size, MU_IMG_DEPTH_8U, 1);
		r2 = muCreateImage(size, MU_IMG_DEPTH_8U, 1);
		b2 = muCreateImage(size, MU_IMG_DEPTH_8U, 1);
		g2 = muCreateImage(size, MU_IMG_DEPTH_8U, 1);

		for(i=0, j=0; j<area; j++, i+=3)
		{
			//seperate RGB
			r1->imagedata[j] = src1->imagedata[i];
			b1->imagedata[j] = src1->imagedata[i+1];
			g1->imagedata[j] = src1->imagedata[i+2];
			r2->imagedata[j] = src2->imagedata[i];
			b2->imagedata[j] = src2->imagedata[i+1];
			g2->imagedata[j] = src2->imagedata[i+2];

		}
		
		for(i=0; i<size.width-7; i++)
			for(j=0; j<size.height-7; j++)
			{
				rect.x = i;
				rect.y = j;
				rect.width = 8;
				rect.height = 8;
				subSize.width = rect.width;
				subSize.height = rect.height;

				r1temp = muCreateImage(subSize, MU_IMG_DEPTH_8U, 1);
				r2temp = muCreateImage(subSize, MU_IMG_DEPTH_8U, 1);
				muGetSubImage(r1, r1temp, rect);
				muGetSubImage(r2, r2temp, rect);
				sumR += calNbyN(r1temp->imagedata, r2temp->imagedata);
		
				b1temp = muCreateImage(subSize, MU_IMG_DEPTH_8U, 1);
				b2temp = muCreateImage(subSize, MU_IMG_DEPTH_8U, 1);
				muGetSubImage(b1, b1temp, rect);
				muGetSubImage(b2, b2temp, rect);
				sumB += calNbyN(b1temp->imagedata, b2temp->imagedata);

				g1temp = muCreateImage(subSize, MU_IMG_DEPTH_8U, 1);
				g2temp = muCreateImage(subSize, MU_IMG_DEPTH_8U, 1);
				muGetSubImage(g1, g1temp, rect);
				muGetSubImage(g2, g2temp, rect);
				sumG += calNbyN(g1temp->imagedata, g2temp->imagedata);

				muReleaseImage(&r1temp);
				muReleaseImage(&r2temp);
				muReleaseImage(&b1temp);
				muReleaseImage(&b2temp);
				muReleaseImage(&g1temp);
				muReleaseImage(&g2temp);
				count++;
			}


		ssimR = sumR/(MU_64F)count;
		ssimB = sumB/(MU_64F)count;
		ssimG = sumG/(MU_64F)count;

		//MU_DBG("ssimR = %f ssimB = %f ssimG = %f\n", ssimR, ssimB, ssimG);	
		*ssim = ((ssimR+ssimB+ssimG)/(MU_64F)3.0);

		muReleaseImage(&r1);
		muReleaseImage(&b1);
		muReleaseImage(&g1);
		muReleaseImage(&r2);
		muReleaseImage(&b2);
		muReleaseImage(&g2);
	}

	return;
}