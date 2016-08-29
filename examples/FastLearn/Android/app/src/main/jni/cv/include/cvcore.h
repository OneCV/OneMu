/* ------------------------------------------------------------------------- /
 *
 * Module: cvcore.h
 * Author: Joe Lin
 *
 * Description:
 *    define the basic data types
 *
 -------------------------------------------------------------------------- */

#ifndef _CV_CORE_H_
#define _CV_CORE_H_

#include "cvbase.h"

/****************************************************************************************\
*                                    Image Processing                                    *
\****************************************************************************************/


/************************* Gradients, Edges and Corners *********************************/

/* Calculates an image derivative using generalized Sobel
   (aperture_size = 1,3,5,7) or Scharr (aperture_size = -1) operator.
   Scharr can be used only for the first dx or dy derivative */
CV_API(cvError_t)  cvSobel( const cvImage_t* src, cvImage_t* dst);

/* Calculates the image Laplacian: (d2/dx + d2/dy)I */
CV_API(cvError_t)  cvLaplace( const cvImage_t* src, cvImage_t* dst, CV_8U selection);

/* Edge detection by Prewitt operator */
CV_API(cvError_t) cvPrewitt( const cvImage_t* src, cvImage_t* dst);

/* Canny edge detection*/
CV_API(cvError_t) cvCanny( const cvImage_t* src, cvImage_t* dst, cvImage_t *ang);


/****************** Sampling, Interpolation and Geometrical Transforms ******************/

#define  CV_INTER_NN        0
#define  CV_INTER_LINEAR    1 // not support

/* Resizes image (input array is resized to fit the destination array) */
CV_API(cvError_t) cvResize( const cvImage_t* src, cvImage_t* dst,
                             CV_32S interpolation CV_DEFAULT( CV_INTER_NN ));

/* Down scale image, e.g. v_scale=1, h_scale=2: 2CIF->CIF; v_scale=2, h_scale=2: CIF->QCIF */
CV_API(cvError_t) cvDownScale( const cvImage_t* src, cvImage_t* dst, CV_32S v_scale, CV_32S h_scale);

/* Down scale image, e.g. v_scale=1, h_scale=2: 2CIF->CIF; v_scale=2, h_scale=2: CIF->QCIF */
CV_API(cvError_t) cvDownScaleMemcpy( const cvImage_t* src, cvImage_t* dst, CV_32S v_scale, CV_32S h_scale);

/* Bilinear Scaling support down/up scale by bi-linear */
CV_API(cvError_t) cvBilinearScale(cvImage_t *in, cvImage_t *out);

/* DownScale */
CV_API(cvError_t) cvDownScaleMemcpy422( const cvImage_t* src, cvImage_t* dst, CV_32S v_scale, CV_32S h_scale);

/* DownScale */
CV_API(cvError_t) cvDownScaleMemcpy420( const cvImage_t* src, cvImage_t* dst, CV_32S v_scale, CV_32S h_scale);
/*************************** Filters and Color Conversion *******************************/

#define CV_BORDER_NONE      0 // (default and only support currently)
                               // not treat the border pixels, the output image will have black border
#define CV_BORDER_CONSTANT  1 // border is filled with the fixed value, passed as last parameter of the function.
#define CV_BORDER_REPLICATE 2 // the pixels from the top and bottom rows, the left-most and right-most columns are replicated to fill the border.

/* Convolves the image with the 3*3 kernel and
   border effect will be handle by specified type (CV_BORDER_*) */
CV_API(cvError_t) cvFilter33( const cvImage_t* src, cvImage_t* dst, const CV_8S kernel[], const CV_8U norm);

/* Convolves the image with the 5*5 kernel and
   border effect will be handle by specified type (CV_BORDER_*) */
CV_API(cvError_t) cvFilter55( const cvImage_t* src, cvImage_t* dst, const CV_32S* kernel,
                              CV_8U bordertype CV_DEFAULT(CV_BORDER_NONE),
                              cvScalar_t value CV_DEFAULT(cvScalarAll(0)));

/* 3x3 median filter by bubble sort */
CV_API(cvError_t) cvMedian33( const cvImage_t *src, cvImage_t *dst);

CV_API(cvError_t) cvFastMedian33(cvImage_t *src, cvImage_t *dst);


/* This routine stretchs the original data to the user-defined value. */
CV_API(cvError_t) cvContraststretching(cvImage_t *src, cvImage_t *dst, CV_8U maxvalue);


/* This routine performs a thresholding on an input data, and the thresholded pixels are   
   overwritten to the input buffer. */
CV_API(cvError_t) cvThresholding(const cvImage_t * src, cvImage_t * dst,  cvDoubleThreshold_t th);

CV_API(cvError_t) cvOtsuThresholding(const cvImage_t *src, cvImage_t *dst);

CV_API(cvError_t) cvISOThresholding(const cvImage_t *src, cvImage_t *dst);

CV_API(cvError_t) cvMeanThresholding(const cvImage_t *src, cvImage_t *dst, CV_8U offset);

/* This routine transform the RGB plane to the Y plane. */
CV_API(cvError_t) cvRGB2GrayLevel(const cvImage_t * src, cvImage_t * dst);

/* This routine transform the yuv422 plane to the RGB. */
CV_API(cvError_t) cvYUV422toRGB(const cvImage_t *src, cvImage_t *dst);

/* This routine transform the RGB plane to the Hue plane. */
CV_API(cvError_t) cvRGB2Hue(const cvImage_t * src, cvImage_t * dst);

/* This routine transform the gray to RGBA plane to the Hue plane. */
CV_API(cvError_t) cvGraytoRGBA(const cvImage_t *src, cvImage_t *dst);


/********* Image Segmentation, Connected Components and Contour Retrieval ***************/

/* Retrieves bounding boxes of white (non-zero) connected
   components in the black (zero) background */
CV_API(cvError_t) cv4ConnectedComponent8u(cvImage_t * src, cvImage_t * dst, CV_8U *numlabel);


/* ?? */
CV_API(cvSeq_t*) cvFindBoundingBox(const cvImage_t * image, CV_8U numlabel,  cvDoubleThreshold_t th);

/* */
CV_API(cvError_t) cvFindOverlapSize(cvBoundingBox_t *B1, cvBoundingBox_t *B2, CV_32S* overlapsize);

/* */
CV_API(cvPoint_t) cvFindGravityCenter(cvImage_t *binary_img);

/* */
CV_API(cvError_t) cvHoleFillingByLabelImage(cvImage_t *label_img, cvImage_t *binary_img, cvBoundingBox_t *box);

/* */
CV_API(cvError_t) cvIntegralImage(const cvImage_t *src, cvImage_t *ii);

/********* Morphological processing ***************/

/* erodes input image (applies minimum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
CV_API (cvError_t) cvErode33(const cvImage_t *src, cvImage_t *dst);


/* dilates input image (applies maximum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
CV_API (cvError_t) cvDilate33(const cvImage_t *src, cvImage_t *dst);


/* erodes input image (applies minimum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
CV_API (cvError_t) cvErode55(const cvImage_t *src, cvImage_t *dst);


/* dilates input image (applies maximum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
CV_API (cvError_t) cvDilate55(const cvImage_t *src, cvImage_t *dst);


/* Dilates input image (applies maximum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
CV_API (cvError_t) cvGrayDilate33(const cvImage_t *src, cvImage_t *dst, CV_8U *se);

/* Erode input image (applies maximum filter) one or more times.
   If element pointer is NULL, 3x3 rectangular element is used */
CV_API (cvError_t) cvGrayErode33(const cvImage_t *src, cvImage_t *dst, CV_8U *se);


/********* Logic processing ***************/

/* And operation between two images */
CV_API (cvError_t) cvAnd(const cvImage_t *src1, cvImage_t *src2, cvImage_t *dst);

/* OR operation between images */
CV_API (cvError_t) cvOr(const cvImage_t *src1, cvImage_t *src2, cvImage_t *dst);

/* Sub operation between images */
CV_API (cvError_t) cvSub(const cvImage_t *src1, cvImage_t *src2, cvImage_t *dst);


/********* Histogram-based processing ***************/
CV_API (cvError_t) cvHistogram(const cvImage_t *src, CV_32U *dst);

CV_API (cvError_t) cvEqualization(const cvImage_t *src, cvImage_t *dst);

CV_API (cvError_t) cvHistogramBlk(cvImage_t* src, CV_16U* histBlkResult, CV_8U winHS, CV_8U winWS);

CV_API (CV_16U*) cvCreateHistogramBlk(CV_32S blkNumH, CV_32S blkNumV);

/******** Motion detection ********/
CV_API (cvError_t) cvLKOpticalFlow(cvImage_t *imageI, cvImage_t *imageJ, CV_32S *vectorX, CV_32S *vectorY, CV_32S *lostTable);

CV_API (cvError_t) cvTransVector2Angle(cvImage_t *curFrame, CV_32S *vectorX, CV_32S *vectorY, CV_32S *lostTable, CV_32S *angleTable);

CV_API (cvError_t) cvGetVectorImage(CV_32S *angleMap, cvImage_t *src, cvImage_t *dst);

#endif /*_CV_CORE_H_*/

/* End of file. */
