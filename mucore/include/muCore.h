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
 * Module: muCore.h
 * Author: Joe Lin
 *
 * Description:
 *    define the basic data types
 *
 -------------------------------------------------------------------------- */

#ifndef _MU_CORE_H_
#define _MU_CORE_H_

#include "muBase.h"

/****************************************************************************************\
*                                    Image Processing                                    *
\****************************************************************************************/


/************************* Gradients, Edges and Corners *********************************/

/* Calculates an image derivative using generalized Sobel
   (aperture_size = 1,3,5,7) or Scharr (aperture_size = -1) operator.
   Scharr can be used only for the first dx or dy derivative */
MU_API(muError_t) muSobel( const muImage_t* src, muImage_t* dst);

/* Calculates the image Laplacian: (d2/dx + d2/dy)I */
MU_API(muError_t) muLaplace( const muImage_t* src, muImage_t* dst, MU_8U selection);

/* Edge detection by Prewitt operator */
MU_API(muError_t) muPrewitt( const muImage_t* src, muImage_t* dst);

/* Canny edge detection*/
MU_API(muError_t) muCannyEdge(const muImage_t *src, muImage_t *dst, muDoubleThreshold_t th);

/* Edge-based no reference blur metric */
MU_API(muError_t) muNoRefBlurMetric(muImage_t *src, MU_64F *bm);

/****************** Sampling, Interpolation and Geometrical Transforms ******************/

#define  MU_INTER_NN        0
#define  MU_INTER_LINEAR    1 // not support

/* Resizes image (input array is resized to fit the destination array) */
MU_API(muError_t) muResize( const muImage_t* src, muImage_t* dst,
                             MU_32S interpolation MU_DEFAULT( MU_INTER_NN ));

/* Down scale image, e.g. v_scale=1, h_scale=2: 2CIF->CIF; v_scale=2, h_scale=2: CIF->QCIF */
MU_API(muError_t) muDownScale( const muImage_t* src, muImage_t* dst, MU_32S v_scale, MU_32S h_scale);

/* Down scale image, e.g. v_scale=1, h_scale=2: 2CIF->CIF; v_scale=2, h_scale=2: CIF->QCIF */
MU_API(muError_t) muDownScaleMemcpy( const muImage_t* src, muImage_t* dst, MU_32S v_scale, MU_32S h_scale);

/* Bilinear Scaling support down/up scale by bi-linear */
MU_API(muError_t) muBilinearScale(muImage_t *in, muImage_t *out);

/* DownScale */
MU_API(muError_t) muDownScaleMemcpy422( const muImage_t* src, muImage_t* dst, MU_32S v_scale, MU_32S h_scale);

/* DownScale */
MU_API(muError_t) muDownScaleMemcpy420( const muImage_t* src, muImage_t* dst, MU_32S v_scale, MU_32S h_scale);

/* Neareast Image Rotation */
MU_API(muImage_t*) muImageRotate(const muImage_t *src, MU_64F angle);

/*************************** Filters and Color Conversion *******************************/

#define MU_BORDER_NONE      0 // (default and only support currently)
                               // not treat the border pixels, the output image will have black border
#define MU_BORDER_CONSTANT  1 // border is filled with the fixed value, passed as last parameter of the function.
#define MU_BORDER_REPLICATE 2 // the pixels from the top and bottom rows, the left-most and right-most columns are replicated to fill the border.

/* Convolves the image with the 5*5 kernelv*/
MU_API(muError_t) muFilter55( const muImage_t* src, muImage_t* dst, const MU_8S kernel[], const MU_8U norm);
/* Convolves the image with the 3*3 kernel */
MU_API(muError_t) muFilter33( const muImage_t* src, muImage_t* dst, const MU_8S kernel[], const MU_8U norm);

/* 3x3 median filter by bubble sort */
MU_API(muError_t) muMedian33( const muImage_t *src, muImage_t *dst);

MU_API(muError_t) muFastMedian33(muImage_t *src, muImage_t *dst);


/* This routine stretchs the original data to the user-defined value. */
MU_API(muError_t) muContraststretching(muImage_t *src, muImage_t *dst, MU_8U maxvalue);


/* This routine performs a thresholding on an input data, and the thresholded pixels are   
   overwritten to the input buffer. */
MU_API(muError_t) muThresholding(const muImage_t * src, muImage_t * dst,  muDoubleThreshold_t th);

MU_API(muError_t) muOtsuThresholding(const muImage_t *src, muImage_t *dst);

MU_API(muError_t) muISOThresholding(const muImage_t *src, muImage_t *dst);

MU_API(muError_t) muMeanThresholding(const muImage_t *src, muImage_t *dst, MU_8U offset);

/* This routine transform the RGB plane to the Y plane. */
MU_API(muError_t) muRGB2GrayLevel(const muImage_t * src, muImage_t * dst);

/* This routine transform the Y plane to the RGB plane. */
MU_API(muError_t) muGrayLevel2RGB(const muImage_t *src, muImage_t *dst);

/* This routine transform the yuv422 plane to the RGB. */
MU_API(muError_t) muYUV422toRGB(const muImage_t *src, muImage_t *dst);

/* This routine transform the yuv420 plane to the RGB. */
MU_API(muError_t) muYUV420toRGB(const muImage_t *src, muImage_t *dst);

/* This routine transform the RGB plane to the Hue plane. */
MU_API(muError_t) muRGB2Hue(const muImage_t * src, muImage_t * dst);

/* This routine transform the RGB plane to the HSV/HSB plane. */
MU_API(muError_t) muRGB2HSV(const muImage_t *rgb, muImage_t *hsv);

/* This routine transform the gray to RGBA plane to the Hue plane. */
MU_API(muError_t) muGraytoRGBA(const muImage_t *src, muImage_t *dst);

/* This routine transform the RGB to XYZ plane. */
MU_API(muError_t) muRGB2XYZ(const muImage_t *src, muImage_t *dst);

/* This routine transform the RGB to XYZ plane. */
MU_API(muError_t) muXYZ2LAB(const muImage_t *src, muImage_t *dst);

/********* Image Segmentation, Connected Components and Contour Retrieval ***************/

/* Retrieves bounding boxes of white (non-zero) connected
   components in the black (zero) background */
MU_API(muError_t) mu4ConnectedComponent8u(muImage_t * src, muImage_t * dst, MU_8U *numlabel);

/* ?? */
MU_API(muSeq_t*) muFindBoundingBox(const muImage_t * image, MU_8U numlabel,  muDoubleThreshold_t th);

/* */
MU_API(muError_t) muFindOverlapSize(muBoundingBox_t *B1, muBoundingBox_t *B2, MU_32S* overlapsize);

/* */
MU_API(muPoint_t) muFindGravityCenter(muImage_t *binary_img);

/* */
MU_API(muError_t) muHoleFillingByLabelImage(muImage_t *label_img, muImage_t *binary_img, muBoundingBox_t *box);

/* */
MU_API(muError_t) muIntegralImage(const muImage_t *src, muImage_t *ii);

/********* Morphological processing ***************/

/* erodes input image (applies minimum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
MU_API (muError_t) muErode33(const muImage_t *src, muImage_t *dst);


/* dilates input image (applies maximum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
MU_API (muError_t) muDilate33(const muImage_t *src, muImage_t *dst);


/* erodes input image (applies minimum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
MU_API (muError_t) muErode55(const muImage_t *src, muImage_t *dst);


/* dilates input image (applies maximum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
MU_API (muError_t) muDilate55(const muImage_t *src, muImage_t *dst);


/* Dilates input image (applies maximum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
MU_API (muError_t) muGrayDilate33(const muImage_t *src, muImage_t *dst, MU_8U *se);

/* Erode input image (applies maximum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
MU_API (muError_t) muGrayErode33(const muImage_t *src, muImage_t *dst, MU_8U *se);


/********* Logic processing ***************/

/* And operation between two images */
MU_API (muError_t) muAnd(const muImage_t *src1, muImage_t *src2, muImage_t *dst);

/* OR operation between images */
MU_API (muError_t) muOr(const muImage_t *src1, muImage_t *src2, muImage_t *dst);

/* Sub operation between images */
MU_API (muError_t) muSub(const muImage_t *src1, muImage_t *src2, muImage_t *dst);


/********* Histogram-based processing ***************/
MU_API (muError_t) muHistogram(const muImage_t *src, MU_32U *dst);

MU_API (muError_t) muEqualization(const muImage_t *src, muImage_t *dst);

MU_API (muError_t) muHistogramBlk(muImage_t* src, MU_16U* histBlkResult, MU_8U winHS, MU_8U winWS);

MU_API (MU_16U*) muCreateHistogramBlk(MU_32S blkNumH, MU_32S blkNumV);

/******** Motion detection ********/
MU_API (muError_t) muLKOpticalFlow(muImage_t *imageI, muImage_t *imageJ, MU_32S *vectorX, MU_32S *vectorY, MU_32S *lostTable);

MU_API (muError_t) muTransVector2Angle(muImage_t *curFrame, MU_32S *vectorX, MU_32S *vectorY, MU_32S *lostTable, MU_32S *angleTable);

MU_API (muError_t) muGetVectorImage(MU_32S *angleMap, muImage_t *src, muImage_t *dst);


/******** Image Matching ********/
typedef struct _muMSEInfo
{
  MU_64F oriData; // original data
  MU_64F norData; // normalized data 0~1
}muMSEInfo_t;

typedef struct _muSearchMatching
{
  muPoint_t point;
  MU_64F    data;
}muSearchMatching_t;

/**only support Y data and RGB channel**/
MU_API (muError_t) muMSE(const muImage_t *src1, const muImage_t *src2, muMSEInfo_t *mseInfo);

/**only support Y data and RGB channel**/
MU_API (muError_t) muRMSE(const muImage_t *src1, const muImage_t *src2, muMSEInfo_t *rmse);

/**only support Y data and RGB channel**/
MU_API (muError_t) muPSNR(const muImage_t *src1, const muImage_t *src2, MU_64F *psnr);

/**only support Y data and RGB channel**/
MU_API (muError_t) muNCC(const muImage_t *src1, const muImage_t *src2, MU_64F *ncc);

/**only support Y data and RGB channel**/
MU_API (muError_t) muSSIM(const muImage_t *src1, const muImage_t *src2, MU_64F *ssim);

/** full search find the similar matching **/
MU_API (muError_t) exhaustiveMatching(muImage_t *gold, muImage_t *test, muPoint_t stPoint, muPoint_t endPoint, MU_8S *alg, muSearchMatching_t *out);

#endif /*_MU_CORE_H_*/

/* End of file. */
