
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
 * Module: muBase.h
 * Author: Joe Lin
 *
 * Description:
 *    define the basic functions
 *
 -------------------------------------------------------------------------- */

#ifndef _MU_BASE_H_
#define _MU_BASE_H_

#include "muTypes.h"

/**********************************************\
*          Operations on Arrays(Image)         *
\**********************************************/

/* Allocates and initializes muImage_t header (not allocates data) */
MU_API(muImage_t*)  muCreateImageHeader( muSize_t size, MU_32S depth, MU_32S channels );

/* Allocates and initializes muImage_t header and data */
MU_API(muImage_t*)  muCreateImage( muSize_t size, MU_32S depth, MU_32S channels );

/* Releases image header */
MU_API(muError_t)  muReleaseImageHeader( muImage_t** image );

/* Releases image header and data */
MU_API(muError_t)  muReleaseImage( muImage_t** image );

/* Returns width and height of image */
MU_API(muSize_t)  muGetSize( const muImage_t* image );

/* Returns resolution of image */
//MU_API(EcvResolution)  muGetResolution( const muImage_t* image );

/* Clears all the image array elements (sets them to 0) */
MU_API(muError_t) muSetZero( muImage_t* image );

/* Get the sub image to dst */
MU_API(muError_t) muGetSubImage(const muImage_t *src, muImage_t *dst, const muRect_t rect);

/* Get the RGB image to dst */
MU_API(muError_t) muGetRGBImage(const muImage_t *src, muImage_t *dst, const muRect_t rect);

/* Remove the sub image from src */
MU_API(muError_t) muRemoveSubImage(muImage_t *src, const muRect_t rect, const MU_32S replaceVal);

/* Get the sub image to dst YUV422*/
MU_API(muError_t)  muGetSubYUV422Image(const muImage_t *src, muImage_t *dst, const muRect_t rect);

/* Return the particular element of image (idx0:height, idx1:width, idx2:channel) */
/* The functions return 0 if the requested node does not exist */
MU_API(MU_32S)  muGet2D(const muImage_t *image, MU_32S idx0, MU_32S idx1);
MU_API(MU_32S)  muGet3D(const muImage_t *image, MU_32S idx0, MU_32S idx1, MU_32S idx2);


/* Debug function for error code displaying */
MU_API(MU_VOID) muDebugError(muError_t errorcode);

/* Debug function for error code displaying */
MU_API(muError_t) muCheckDepth(MU_32S amount, ...);

/*Draw Rectangle*/
MU_API(muError_t) muRectangle(muImage_t *SrcImg, muPoint_t p1, muPoint_t p2, MU_8S color);

/**********************************************\
*          Dynamic Structures(Sequence)        *
\**********************************************/

/* dynamic structure, create sequence */
MU_API(muSeq_t*) muCreateSeq(MU_32S elementsize);

/* dynamic structure, inset the new sequence block to the last list */
MU_API(muSeqBlock_t*) muPushSeq(muSeq_t *seq, MU_VOID* element);

/* dynamic structure, clear the input sequence */
MU_API(MU_VOID) muClearSeq(muSeq_t **seq);

/* dynamic structure, remove the index node from sequence*/
MU_API(muError_t) muRemoveIndexNode(muSeq_t **seq, MU_32S index);

/* dynamic structure, delete the node by address */
MU_API(muError_t) muRemoveAddressNode(muSeq_t **seq, muSeqBlock_t *ptr);

/* dynamic structure, remove the last node of sequence */ 
MU_API(muError_t) muSeqPop(muSeq_t **seq, MU_VOID *element);

/* dynamic structure, insert the sequence block to the front */
MU_API(muSeqBlock_t*) muPushSeqFront(muSeq_t *seq, MU_VOID* element);

/* dynamic structure, get element by index */
MU_API(MU_VOID*) muGetSeqElement(muSeq_t **seq, MU_32S index);

/**********************************************\
*          Loading and Saving Images           *
\**********************************************/
/* load bmp */
MU_API(muImage_t*) muLoadBMP(const char *filename);

/* save bmp */
MU_API(muError_t) muSaveBMP(const char *filename, muImage_t *image);

#endif /* _MU_BASE_H_ */

