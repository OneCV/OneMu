
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

/* ------------------------------------------------------------------------------- /
 *
 * Module: muType.h
 * Author: Joe Lin, Chao-Ting Hong
 *
 * Description:
 *    define the image processing, structural analysis, object detection functions
 *
 -------------------------------------------------------------------------------- */


#ifndef _MU_TYPES_H_
#define _MU_TYPES_H_


/* Basic header file fallow with ANSI C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#ifndef MU_INLINE
#if defined __cplusplus
    #define MU_INLINE inline
#elif (defined WIN32 || defined WIN64) && !defined __GNUC__
    #define MU_INLINE __inline
#else
    #define MU_INLINE static
#endif
#endif /* MU_INLINE */


#if defined WIN32 || defined WIN64
    #define MU_CDECL __cdecl
    #define MU_STDCALL __stdcall
#else
    #define MU_CDECL
    #define MU_STDCALL
#endif

#ifndef MU_EXTERN_C
    #ifdef __cplusplus
        #define MU_EXTERN_C extern "C"
        #define MU_DEFAULT(val) = val
    #else
        #define MU_EXTERN_C
        #define MU_DEFAULT(val)
    #endif
#endif

#if (defined WIN32 || defined WIN64) && defined MU_API_EXPORTS
    #define MU_EXPORTS __declspec(dllexport)
#else
    #define MU_EXPORTS
#endif

#ifndef MU_API
    #define MU_API(rettype) MU_EXTERN_C MU_EXPORTS rettype MU_CDECL
#endif

/********************************* Basic type definitions ********************************/

#define MU_VOID	void
#define MU_8U	unsigned char
#define MU_8S	char
#define MU_16U	unsigned short
#define MU_16S	short
#define MU_32U	unsigned int
#define MU_32S	int
#define MU_32F	float
#define MU_64F	double
#define MU_64S	long long
#define MU_64U	unsigned long long

typedef enum
{
	MU_FALSE = 0,
	MU_TRUE = 1,
}muBool_t;

typedef union _mu32suf
{
    MU_32S i;
    MU_32U u;
    MU_32F f;
}mu32suf_t;

typedef union _mu64suf
{
    MU_64S i;
    MU_64U u;
    MU_64F f;
}mu64suf_t;

/********************************* Error code definitions ********************************/

typedef enum _muError
{
    MU_ERR_SUCCESS = 0,
    MU_ERR_INVALID_PARAMETER,
    MU_ERR_NULL_POINTER,
    MU_ERR_OUT_OF_MEMORY,
    MU_ERR_NOT_SUPPORT,
    MU_ERR_UNKNOWN = 100,
}muError_t;

/************************************ video resolution ***********************************/

typedef enum _muResolution
{
    MU_RES_NONE = 0,
	MU_RES_FHD = 2073600,		//1920*1080
    MU_RES_FD1 = 340000,            
    MU_RES_D1 = 330000,
    MU_RES_CIF = 80000,
    MU_RES_QCIF = 21000,
    MU_RES_FD1_PAL = 414720,     //720*576
    MU_RES_FD1_NTSC = 345600,    //720*480
	MU_RES_D1_PAL = 405504,      //704*576
	MU_RES_D1_NTSC = 337920,     //704*480
    MU_RES_2CIF_PAL = 202752,    //704*288
    MU_RES_2CIF_NTSC = 168960,   //704*240
    MU_RES_CIF_PAL = 101376,     //352*288
    MU_RES_CIF_NTSC = 84480,     //352*240
    MU_RES_QCIF_PAL = 25344,     //176*144
    MU_RES_QCIF_NTSC = 21120,    //176*120
    MU_RES_VGA = 307200,         //640*480
    MU_RES_QVGA = 76800,         //320*240
}muResolution_t;

/**************************** Common macros and inline functions *************************/
#define MU_FALSE 0
#define MU_TRUE  1

#define MU_PI   3.1415926535897932384626433832795F
#define MU_LOG2 0.69314718055994530941723212145818F

#define MU_MAX(a,b) a>=b?a:b
#define MU_MIN(a,b) a<=b?a:b
#define MU_IMIN(a,b) ((a) ^ (((a)^(b)) & (((a) < (b)) - 1)))

#define MU_DBG(fmt, ...)	printf("[MULIB]"fmt,##__VA_ARGS__)

MU_INLINE MU_32S muRound( MU_64F value )
{
    mu64suf_t temp;
    temp.f = value + 6755399441055744.0;
    return (int)temp.u;
}


MU_INLINE MU_32S muFloor( MU_64F value )
{
    int temp = muRound(value);
    mu32suf_t diff;
    diff.f = (float)(value - temp);
    return temp - (diff.i < 0);
}


MU_INLINE MU_32S muCeil( MU_64F value )
{
    int temp = muRound(value);
    mu32suf_t diff;
    diff.f = (float)(temp - value);
    return temp + (diff.i < 0);
}


/************************************ Image type ****************************************/
#define MU_IMG_DEPTH_8U		0x001
#define MU_IMG_DEPTH_8S		0x101
#define MU_IMG_DEPTH_16U	0x002
#define MU_IMG_DEPTH_16S	0x102
#define MU_IMG_DEPTH_32U	0x004
#define MU_IMG_DEPTH_32S	0x104
#define MU_IMG_DEPTH_32F	0x204
#define MU_IMG_DEPTH_64U	0x008
#define MU_IMG_DEPTH_64S	0x108
#define MU_IMG_DEPTH_64F	0x208

#define MU_IMG_DATAORDER_PIXEL  0
#define MU_IMG_DATAORDER_PLANE  1

#define MU_IMG_ORIGIN_TL 0
#define MU_IMG_ORIGIN_BL 1

typedef struct _muROI
{
    MU_32S  coi; /* 0 - no COI (all channels are selected), 1 - 0th channel is selected ...*/
    MU_32S  xoffset;
    MU_32S  yoffset;
    MU_32S  width;
    MU_32S  height;

}muROI_t;

typedef struct _muImage
{
    MU_32S channels;     /* support 1,2,3 or 4 channels */
    MU_32S depth;        /* pixel depth in bits: MU_IMG_DEPTH_8U, MU_IMG_DEPTH_8S, MU_IMG_DEPTH_16S,
                             MU_IMG_DEPTH_32S, MU_IMG_DEPTH_32F and MU_IMG_DEPTH_64F are supported */
    MU_32S dataorder;    /* 0 - interleaved color channels, 1 - separate color channels.
                             muCreateImage can only create interleaved images */
    MU_32S origin;       /* 0 - top-left origin,
                             1 - bottom-left origin (Windows bitmaps style) */
    MU_32S width;        /* image width in pixels */
    MU_32S height;       /* image height in pixels */
    muROI_t* roi;          /* image ROI. if NULL, the whole image is selected */
    MU_8U* imagedata;    /* pointer to aligned image data */
	MU_32U phyaddr;

}muImage_t;

/*************************************** muRect *****************************************/

typedef struct _muRect
{
    MU_32S x;
    MU_32S y;
    MU_32S width;
    MU_32S height;

}muRect_t;

MU_INLINE muRect_t muRect( MU_32S x, MU_32S y, MU_32S width, MU_32S height )
{
    muRect_t r;

    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;

    return r;
}

/******************************* muPoint and variants ***********************************/

typedef struct _muPoint
{
    MU_32S x;
    MU_32S y;

}muPoint_t;


MU_INLINE muPoint_t muPoint( MU_32S x, MU_32S y )
{
    muPoint_t p;

    p.x = x;
    p.y = y;

    return p;
}

typedef struct _muPoint2D32f
{
    MU_32F x;
    MU_32F y;

}muPoint2D32f_t;

MU_INLINE muPoint2D32f_t muPoint2D32f( MU_64F x, MU_64F y )
{
    muPoint2D32f_t p;

    p.x = (MU_32F)x;
    p.y = (MU_32F)y;

    return p;
}


MU_INLINE muPoint2D32f_t muPointTo32f( muPoint_t point )
{
    return muPoint2D32f( (MU_32F)point.x, (MU_32F)point.y );
}


MU_INLINE muPoint_t muPointFrom32f( muPoint2D32f_t point )
{
    muPoint_t ipt;
    ipt.x = muRound(point.x);
    ipt.y = muRound(point.y);

    return ipt;
}


typedef struct _muPoint3D32f
{
    MU_32F x;
    MU_32F y;
    MU_32F z;

}muPoint3D32f_t;


MU_INLINE muPoint3D32f_t muPoint3D32f( MU_64F x, MU_64F y, MU_64F z )
{
    muPoint3D32f_t p;

    p.x = (MU_32F)x;
    p.y = (MU_32F)y;
    p.z = (MU_32F)z;

    return p;
}


typedef struct _muPoint2D64f
{
    MU_64F x;
    MU_64F y;

}muPoint2D64f_t;


MU_INLINE muPoint2D64f_t muPoint2D64f( MU_64F x, MU_64F y )
{
    muPoint2D64f_t p;

    p.x = x;
    p.y = y;

    return p;
}


typedef struct _muPoint3D64f
{
    MU_64F x;
    MU_64F y;
    MU_64F z;

}muPoint3D64f_t;


MU_INLINE muPoint3D64f_t muPoint3D64f( MU_64F x, MU_64F y, MU_64F z )
{
    muPoint3D64f_t p;

    p.x = x;
    p.y = y;
    p.z = z;

    return p;
}

MU_INLINE MU_32S muCrossProduct( muPoint_t vec1, muPoint_t vec2 )
{
    return ((vec1.x*vec2.y)-(vec2.x*vec1.y));
}

/************************************** muLine *******************************************/

typedef struct _muLine
{
    muPoint_t start;
    muPoint_t end;

}muLine_t;

MU_INLINE muLine_t muLine( muPoint_t start, muPoint_t end )
{
    muLine_t line;

    line.start = start;
    line.end = end;

    return line;
}

/******************************** muSize's & muBox **************************************/

typedef struct _muBoundingBox
{
	MU_32S minx, miny, maxx, maxy;
	MU_32S overlap, label;
	MU_32S width, height, area;

}muBoundingBox_t;


typedef struct _muSize
{
    MU_32S width;
    MU_32S height;

}muSize_t;

MU_INLINE muSize_t muSize( MU_32S width, MU_32S height )
{
    muSize_t s;

    s.width = width;
    s.height = height;

    return s;
}

typedef struct _muSize2D32f
{
    MU_32F width;
    MU_32F height;

}muSize2D32f_t;


MU_INLINE muSize2D32f_t muSize2D32f( MU_64F width, MU_64F height )
{
    muSize2D32f_t s;

    s.width = (MU_32F)width;
    s.height = (MU_32F)height;

    return s;
}

typedef struct _muBox2D
{
    muPoint2D32f_t center;  /* center of the box */
    muSize2D32f_t  size;    /* box width and length */
    MU_32F angle;          /* angle between the horizontal axis
                             and the first side (i.e. length) in degrees */
}muBox2D_t;

/****mu integral image****/ //chaotien 2015/2/8
typedef struct _muIntegralImg
{
    int *sum;
    double *sqsum;
    double *tilted;
    muSize_t sumSize;
    muSize_t imgSize;
} muIntegralImg_t;


/************************************* muParameter *****************************************/
typedef struct _muDoubleThreshold
{
	MU_32S min;
	MU_32S max;

}muDoubleThreshold_t;

/*********************************** Sequence *******************************************/

typedef struct _muSeqBlock
{
    struct _muSeqBlock*  prev; /* previous sequence block */
    struct _muSeqBlock*  next; /* next sequence block */
    MU_VOID* data;

}muSeqBlock_t;

/*
   Read/Write sequence.
   Elements can be dynamically inserted to or deleted from the sequence.
*/
#define MU_SEQUENCE_FIELDS()                                               \
    MU_32S          total;          /* total number of elements */          \
    MU_32S			elem_size;      /* size of sequence element in bytes */ \
    muSeqBlock_t*	first;          /* pointer to the first sequence block */

typedef struct _muSeq
{
    MU_SEQUENCE_FIELDS()

}muSeq_t;

/* TODO AF Structure */
typedef struct _muAfInfo
{
	muRect_t rect[5];
	MU_32U	afSum[5];
}muAfInfo_t;


#endif /*_MU_TYPES_H_*/

/* End of file. */
