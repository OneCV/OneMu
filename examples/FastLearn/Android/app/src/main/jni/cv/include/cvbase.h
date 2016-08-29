/* ------------------------------------------------------------------------- /
 *
 * Module: cvbase.h
 * Author: Joe Lin
 *
 * Description:
 *    define the basic functions
 *
 -------------------------------------------------------------------------- */

#ifndef _CV_BASE_H_
#define _CV_BASE_H_

#include "cvtypes.h"
//#include "cvplatform.h"

/**********************************************\
*          Operations on Arrays(Image)         *
\**********************************************/

/* Allocates and initializes cvImage_t header (not allocates data) */
CV_API(cvImage_t*)  cvCreateImageHeader( cvSize_t size, CV_32S depth, CV_32S channels );

/* Allocates and initializes cvImage_t header and data */
CV_API(cvImage_t*)  cvCreateImage( cvSize_t size, CV_32S depth, CV_32S channels );

/* Releases image header */
CV_API(cvError_t)  cvReleaseImageHeader( cvImage_t** image );

/* Releases image header and data */
CV_API(cvError_t)  cvReleaseImage( cvImage_t** image );

/* Returns width and height of image */
CV_API(cvSize_t)  cvGetSize( const cvImage_t* image );

/* Returns resolution of image */
//CV_API(EcvResolution)  cvGetResolution( const cvImage_t* image );

/* Clears all the image array elements (sets them to 0) */
CV_API(cvError_t)  cvSetZero( cvImage_t* image );

/* Get the sub image to dst */
CV_API(cvError_t)  cvGetSubImage(const cvImage_t *src, cvImage_t *dst, const cvRect_t rect);

/* Remove the sub image from src */
cvError_t cvRemoveSubImage(cvImage_t *src, const cvRect_t rect, const CV_32S replaceVal);
/* Get the sub image to dst YUV422*/
CV_API(cvError_t)  cvGetSubYUV422Image(const cvImage_t *src, cvImage_t *dst, const cvRect_t rect);

/* Return the particular element of image (idx0:height, idx1:width, idx2:channel) */
/* The functions return 0 if the requested node does not exist */
CV_API(CV_32S)  cvGet2D(const cvImage_t *image, CV_32S idx0, CV_32S idx1);
CV_API(CV_32S)  cvGet3D(const cvImage_t *image, CV_32S idx0, CV_32S idx1, CV_32S idx2);


/* Debug function for error code displaying */
CV_API(CV_VOID) cvDebugError(cvError_t errorcode);

/* Debug function for error code displaying */
CV_API(cvError_t) cvCheckDepth(CV_32S amount, ...);

/**********************************************\
*          Dynamic Structures(Sequence)        *
\**********************************************/

/* dynamic structure, create sequence */
CV_API(cvSeq_t*) cvCreateSeq(CV_32S elementsize);

/* dynamic structure, inset the new sequence block to the last list */
CV_API(cvSeqBlock_t*) cvPushSeq(cvSeq_t *seq, CV_VOID* element);

/* dynamic structure, clear the input sequence */
CV_API(CV_VOID) cvClearSeq(cvSeq_t **seq);

/* dynamic structure, remove the index node from sequence*/
CV_API(cvError_t) cvRemoveIndexNode(cvSeq_t **seq, CV_32S index);

/* dynamic structure, delete the node by address */
CV_API(cvError_t) cvRemoveAddressNode(cvSeq_t **seq, cvSeqBlock_t *ptr);

/* dynamic structure, remove the last node of sequence */ 
CV_API(cvError_t) cvSeqPop(cvSeq_t **seq, CV_VOID *element);

/* dynamic structure, insert the sequence block to the front */
CV_API(cvSeqBlock_t*) cvPushSeqFront(cvSeq_t *seq, CV_VOID* element);

/* dynamic structure, get element by index */
CV_API(CV_VOID*) cvGetSeqElement(cvSeq_t **seq, CV_32S index);


/**********************************************\
*          Loading and Saving Images           *
\**********************************************/

/* load bmp */
CV_API(cvImage_t*) cvLoadBMP(const char *filename);

/* save bmp */
CV_API(cvError_t) cvSaveBMP(const char *filename, cvImage_t *image);



#endif /* _CV_BASE_H_ */

