/* ------------------------------------------------------------------------------------------
 *
 * Module: cvplatform.h
 *
 * Author: Joe Lin
 *
 * Description:
 *	define the platform based image process function, please place this header to the buttom
 *
 -------------------------------------------------------------------------------------------- */

#ifndef _CV_PLATFORM_H
#define	_CV_PLATFORM_H


#if defined(HISI3516) || defined(HISI3531) 

extern CV_32S platformThresholding(const cvImage_t *in, cvImage_t *out, cvDoubleThreshold_t th);

extern CV_32S platformDilate33(const cvImage_t *in, cvImage_t *out);

extern CV_32S platformErode33(const cvImage_t *in, cvImage_t *out);

extern CV_32S platformFilter33(const cvImage_t *in, cvImage_t *out, const CV_8S kernel[], CV_8U norm);

extern CV_32S platformSobel(const cvImage_t *in, cvImage_t *out);

extern CV_32S platformAFSum(const cvImage_t *src, cvAfInfo_t *afInfo);

extern CV_32S platformAFSumInit(CV_32S width, CV_32S height);

extern CV_32S platformAFRelease();

extern CV_32S platformAEInit();

extern cvImage_t *platformAEProcess(cvImage_t *src);

extern CV_32S platformAnd(const cvImage_t *in1, const cvImage_t *in2, cvImage_t *dst);

extern CV_32S platformSub(const cvImage_t *in1, const cvImage_t *in2, cvImage_t *dst);

extern CV_32S platformOr(const cvImage_t *in1, const cvImage_t *in2, cvImage_t *dst);

extern CV_32S platformIntegral(const cvImage_t *in, cvImage_t *ii);

extern cvImage_t *platformGetLuma(int vichn, cvResolution_t res);

extern cvImage_t *platformGetYUV422(int vichn, cvResolution_t res);

extern cvImage_t *platformGetYUV420(int vichn, cvResolution_t res);

extern CV_32S platformDmaInit(CV_32S viChn);

extern CV_32S platformMalloc(cvImage_t *img, CV_32S channels);

extern CV_VOID platformFree(cvImage_t *image);

#endif


#endif /* _CV_PLATFORM_H */
