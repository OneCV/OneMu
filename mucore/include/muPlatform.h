/* ------------------------------------------------------------------------------------------
 *
 * Module: muPlatform.h
 *
 * Author: Joe Lin
 *
 * Description:
 *	define the platform based image process function, please place this header to the buttom
 *
 -------------------------------------------------------------------------------------------- */

#ifndef _MU_PLATFORM_H
#define	_MU_PLATFORM_H


#if defined(HISI3516) || defined(HISI3531) 

extern MU_32S platformThresholding(const muImage_t *in, muImage_t *out, muDoubleThreshold_t th);

extern MU_32S platformDilate33(const muImage_t *in, muImage_t *out);

extern MU_32S platformErode33(const muImage_t *in, muImage_t *out);

extern MU_32S platformFilter33(const muImage_t *in, muImage_t *out, const MU_8S kernel[], MU_8U norm);

extern MU_32S platformSobel(const muImage_t *in, muImage_t *out);

extern MU_32S platformAFSum(const muImage_t *src, muAfInfo_t *afInfo);

extern MU_32S platformAFSumInit(MU_32S width, MU_32S height);

extern MU_32S platformAFRelease();

extern MU_32S platformAEInit();

extern muImage_t *platformAEProcess(muImage_t *src);

extern MU_32S platformAnd(const muImage_t *in1, const muImage_t *in2, muImage_t *dst);

extern MU_32S platformSub(const muImage_t *in1, const muImage_t *in2, muImage_t *dst);

extern MU_32S platformOr(const muImage_t *in1, const muImage_t *in2, muImage_t *dst);

extern MU_32S platformIntegral(const muImage_t *in, muImage_t *ii);

extern muImage_t *platformGetLuma(int vichn, muResolution_t res);

extern muImage_t *platformGetYUV422(int vichn, muResolution_t res);

extern muImage_t *platformGetYUV420(int vichn, muResolution_t res);

extern MU_32S platformDmaInit(MU_32S viChn);

extern MU_32S platformMalloc(muImage_t *img, MU_32S channels);

extern MU_VOID platformFree(muImage_t *image);

#endif


#endif /* _MU_PLATFORM_H */
