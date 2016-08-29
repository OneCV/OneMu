/* ------------------------------------------------------------------------- /
 *
 * Module: cvtype.h
 * Author: Joe Lin
 *
 * Description:
 *    define the image processing, structural analysis functions
 *
 -------------------------------------------------------------------------- */


#ifndef _CV_TYPES_H_
#define _CV_TYPES_H_


/* Basic header file fallow with ANSI C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#ifndef CV_INLINE
#if defined __cplusplus
    #define CV_INLINE inline
#elif (defined WIN32 || defined WIN64) && !defined __GNUC__
    #define CV_INLINE __inline
#else
    #define CV_INLINE static
#endif
#endif /* CV_INLINE */


#if defined WIN32 || defined WIN64
    #define CV_CDECL __cdecl
    #define CV_STDCALL __stdcall
#else
    #define CV_CDECL
    #define CV_STDCALL
#endif

#ifndef CV_EXTERN_C
    #ifdef __cplusplus
        #define CV_EXTERN_C extern "C"
        #define CV_DEFAULT(val) = val
    #else
        #define CV_EXTERN_C
        #define CV_DEFAULT(val)
    #endif
#endif

#if (defined WIN32 || defined WIN64) && defined CV_API_EXPORTS
    #define CV_EXPORTS __declspec(dllexport)
#else
    #define CV_EXPORTS
#endif

#ifndef CV_API
    #define CV_API(rettype) CV_EXTERN_C CV_EXPORTS rettype CV_CDECL
#endif

/********************************* Basic type definitions ********************************/

#define CV_VOID	void
#define CV_8U	unsigned char
#define CV_8S	char
#define CV_16U	unsigned short
#define CV_16S	short
#define CV_32U	unsigned int
#define CV_32S	int
#define CV_32F	float
#define CV_64F	double

#if defined _MSC_VER || defined __BORLANDC__
#define CV_64S	__int64
#define CV_64U	unsigned __int64
#else
#define CV_64S	long long
#define CV_64U	unsigned long long
#endif

typedef enum
{
	CV_FALSE = 0,
	CV_TRUE = 1,
}cvBool_t;

typedef union _cv32suf
{
    CV_32S i;
    CV_32U u;
    CV_32F f;
}cv32suf_t;

typedef union _cv64suf
{
    CV_64S i;
    CV_64U u;
    CV_64F f;
}cv64suf_t;

/********************************* Error code definitions ********************************/

typedef enum _cvError
{
    CV_ERR_SUCCESS = 0,
    CV_ERR_INVALID_PARAMETER,
    CV_ERR_NULL_POINTER,
    CV_ERR_OUT_OF_MEMORY,
    CV_ERR_NOT_SUPPORT,
    CV_ERR_UNKNOWN = 100,
}cvError_t;

/************************************ video resolution ***********************************/

typedef enum _cvResolution
{
    CV_RES_NONE = 0,
	CV_RES_FHD = 2073600,		//1920*1080
    CV_RES_FD1 = 340000,            
    CV_RES_D1 = 330000,
    CV_RES_CIF = 80000,
    CV_RES_QCIF = 21000,
    CV_RES_FD1_PAL = 414720,     //720*576
    CV_RES_FD1_NTSC = 345600,    //720*480
	CV_RES_D1_PAL = 405504,      //704*576
	CV_RES_D1_NTSC = 337920,     //704*480
    CV_RES_2CIF_PAL = 202752,    //704*288
    CV_RES_2CIF_NTSC = 168960,   //704*240
    CV_RES_CIF_PAL = 101376,     //352*288
    CV_RES_CIF_NTSC = 84480,     //352*240
    CV_RES_QCIF_PAL = 25344,     //176*144
    CV_RES_QCIF_NTSC = 21120,    //176*120
    CV_RES_VGA = 307200,         //640*480
    CV_RES_QVGA = 76800,         //320*240
}cvResolution_t;


/**************************** Common macros and inline functions *************************/
#define CV_FALSE 0
#define CV_TRUE  1

#define CV_PI   3.1415926535897932384626433832795F
#define CV_LOG2 0.69314718055994530941723212145818F

#define CV_MAX(a,b) a>=b?a:b
#define CV_MIN(a,b) a<=b?a:b

#if defined WIN32 || defined WIN64
#define	CV_DBG	printf
#else
#define CV_DBG(fmt, str...)	printf("[CVLIB]"fmt,##str)
#endif

CV_INLINE CV_32S cvRound( CV_64F value )
{
#if CV_SSE2
    __m128d t = _mm_load_sd( &value );
    return _mm_cvtsd_si32(t);
#elif defined WIN32 && !defined WIN64 && defined _MSC_VER
    int t;
    __asm
    {
        fld value;
        fistp t;
    }
    return t;
#elif (defined HAVE_LRINT) || (defined WIN64 && !defined EM64T && defined CV_ICC)
    return (int)lrint(value);
#else
    /*
     the algorithm was taken from Agner Fog's optimization guide
     at http://www.agner.org/assem
     */
    cv64suf_t temp;
    temp.f = value + 6755399441055744.0;
    return (int)temp.u;
#endif
}


CV_INLINE CV_32S cvFloor( CV_64F value )
{
#if CV_SSE2
    __m128d t = _mm_load_sd( &value );
    int i = _mm_cvtsd_si32(t);
    return i - _mm_movemask_pd(_mm_cmplt_sd(t,_mm_cvtsi32_sd(t,i)));
#else
    int temp = cvRound(value);
    cv32suf_t diff;
    diff.f = (float)(value - temp);
    return temp - (diff.i < 0);
#endif
}


CV_INLINE CV_32S cvCeil( CV_64F value )
{
#if CV_SSE2
    __m128d t = _mm_load_sd( &value );
    int i = _mm_cvtsd_si32(t);
    return i + _mm_movemask_pd(_mm_cmplt_sd(_mm_cvtsi32_sd(t,i),t));
#else
    int temp = cvRound(value);
    cv32suf_t diff;
    diff.f = (float)(temp - value);
    return temp + (diff.i < 0);
#endif
}

/************************************ HW engine buffer name *****************************/
#define CV_IMG_HW_ACCE		0xC00

/************************************ Image type ****************************************/
#define CV_IMG_DEPTH_8U		0x001
#define CV_IMG_DEPTH_8S		0x101
#define CV_IMG_DEPTH_16U	0x002
#define CV_IMG_DEPTH_16S	0x102
#define CV_IMG_DEPTH_32U	0x004
#define CV_IMG_DEPTH_32S	0x104
#define CV_IMG_DEPTH_32F	0x204
#define CV_IMG_DEPTH_64U	0x008
#define CV_IMG_DEPTH_64S	0x108
#define CV_IMG_DEPTH_64F	0x208
#define CV_IMG_HW_ACCE_8U	0xC01
#define CV_IMG_HW_ACCE_8S	0xD01
#define CV_IMG_HW_ACCE_16U	0xC02
#define CV_IMG_HW_ACCE_16S	0xD02
#define CV_IMG_HW_ACCE_32U	0xC04
#define CV_IMG_HW_ACCE_32S	0xD04
#define CV_IMG_HW_ACCE_64U	0xC08
#define CV_IMG_HW_ACCE_64S	0xD08

#define CV_IMG_DATAORDER_PIXEL  0
#define CV_IMG_DATAORDER_PLANE  1

#define CV_IMG_ORIGIN_TL 0
#define CV_IMG_ORIGIN_BL 1

typedef struct _cvROI
{
    CV_32S  coi; /* 0 - no COI (all channels are selected), 1 - 0th channel is selected ...*/
    CV_32S  xoffset;
    CV_32S  yoffset;
    CV_32S  width;
    CV_32S  height;

}cvROI_t;

typedef struct _cvImage
{
    CV_32S channels;     /* support 1,2,3 or 4 channels */
    CV_32S depth;        /* pixel depth in bits: CV_IMG_DEPTH_8U, CV_IMG_DEPTH_8S, CV_IMG_DEPTH_16S,
                             CV_IMG_DEPTH_32S, CV_IMG_DEPTH_32F and CV_IMG_DEPTH_64F are supported */
    CV_32S dataorder;    /* 0 - interleaved color channels, 1 - separate color channels.
                             cvCreateImage can only create interleaved images */
    CV_32S origin;       /* 0 - top-left origin,
                             1 - bottom-left origin (Windows bitmaps style) */
    CV_32S width;        /* image width in pixels */
    CV_32S height;       /* image height in pixels */
    cvROI_t* roi;          /* image ROI. if NULL, the whole image is selected */
    CV_8U* imagedata;    /* pointer to aligned image data */
	CV_32U phyaddr;

}cvImage_t;

/*************************************** cvRect *****************************************/

typedef struct _cvRect
{
    CV_32S x;
    CV_32S y;
    CV_32S width;
    CV_32S height;

}cvRect_t;

CV_INLINE cvRect_t cvRect( CV_32S x, CV_32S y, CV_32S width, CV_32S height )
{
    cvRect_t r;

    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;

    return r;
}

/******************************* cvPoint and variants ***********************************/

typedef struct _cvPoint
{
    CV_32S x;
    CV_32S y;

}cvPoint_t;


CV_INLINE cvPoint_t cvPoint( CV_32S x, CV_32S y )
{
    cvPoint_t p;

    p.x = x;
    p.y = y;

    return p;
}


typedef struct _cvPoint2D32f
{
    CV_32F x;
    CV_32F y;

}cvPoint2D32f_t;


CV_INLINE cvPoint2D32f_t cvPoint2D32f( CV_64F x, CV_64F y )
{
    cvPoint2D32f_t p;

    p.x = (CV_32F)x;
    p.y = (CV_32F)y;

    return p;
}


CV_INLINE cvPoint2D32f_t cvPointTo32f( cvPoint_t point )
{
    return cvPoint2D32f( (CV_32F)point.x, (CV_32F)point.y );
}


CV_INLINE cvPoint_t cvPointFrom32f( cvPoint2D32f_t point )
{
    cvPoint_t ipt;
    ipt.x = cvRound(point.x);
    ipt.y = cvRound(point.y);

    return ipt;
}


typedef struct _cvPoint3D32f
{
    CV_32F x;
    CV_32F y;
    CV_32F z;

}cvPoint3D32f_t;


CV_INLINE cvPoint3D32f_t cvPoint3D32f( CV_64F x, CV_64F y, CV_64F z )
{
    cvPoint3D32f_t p;

    p.x = (CV_32F)x;
    p.y = (CV_32F)y;
    p.z = (CV_32F)z;

    return p;
}


typedef struct _cvPoint2D64f
{
    CV_64F x;
    CV_64F y;

}cvPoint2D64f_t;


CV_INLINE cvPoint2D64f_t cvPoint2D64f( CV_64F x, CV_64F y )
{
    cvPoint2D64f_t p;

    p.x = x;
    p.y = y;

    return p;
}


typedef struct _cvPoint3D64f
{
    CV_64F x;
    CV_64F y;
    CV_64F z;

}cvPoint3D64f_t;


CV_INLINE cvPoint3D64f_t cvPoint3D64f( CV_64F x, CV_64F y, CV_64F z )
{
    cvPoint3D64f_t p;

    p.x = x;
    p.y = y;
    p.z = z;

    return p;
}

CV_INLINE CV_32S cvCrossProduct( cvPoint_t vec1, cvPoint_t vec2 )
{
    return ((vec1.x*vec2.y)-(vec2.x*vec1.y));
}

/************************************** cvLine *******************************************/

typedef struct _cvLine
{
    cvPoint_t start;
    cvPoint_t end;

}cvLine_t;

CV_INLINE cvLine_t cvLine( cvPoint_t start, cvPoint_t end )
{
    cvLine_t line;

    line.start = start;
    line.end = end;

    return line;
}

/******************************** cvSize's & cvBox **************************************/

typedef struct _cvBoundingBox
{
	CV_32S minx, miny, maxx, maxy;
	CV_32S overlap, label;
	CV_32S width, height, area;

}cvBoundingBox_t;


typedef struct _cvSize
{
    CV_32S width;
    CV_32S height;

}cvSize_t;

CV_INLINE cvSize_t cvSize( CV_32S width, CV_32S height )
{
    cvSize_t s;

    s.width = width;
    s.height = height;

    return s;
}

typedef struct _cvSize2D32f
{
    CV_32F width;
    CV_32F height;

}cvSize2D32f_t;


CV_INLINE cvSize2D32f_t cvSize2D32f( CV_64F width, CV_64F height )
{
    cvSize2D32f_t s;

    s.width = (CV_32F)width;
    s.height = (CV_32F)height;

    return s;
}

typedef struct _cvBox2D
{
    cvPoint2D32f_t center;  /* center of the box */
    cvSize2D32f_t  size;    /* box width and length */
    CV_32F angle;          /* angle between the horizontal axis
                             and the first side (i.e. length) in degrees */
}cvBox2D_t;

/************************************* cvScalar *****************************************/

typedef struct _cvScalar
{
    CV_64F val[4];

}cvScalar_t;

CV_INLINE cvScalar_t cvScalar( CV_64F val0, CV_64F val1 CV_DEFAULT(0),
                               CV_64F val2 CV_DEFAULT(0), CV_64F val3 CV_DEFAULT(0))
{
    cvScalar_t scalar;
    scalar.val[0] = val0; scalar.val[1] = val1;
    scalar.val[2] = val2; scalar.val[3] = val3;
    return scalar;
}


CV_INLINE cvScalar_t cvRealScalar( CV_64F val0 )
{
    cvScalar_t scalar;
    scalar.val[0] = val0;
    scalar.val[1] = scalar.val[2] = scalar.val[3] = 0;
    return scalar;
}

CV_INLINE cvScalar_t cvScalarAll( CV_64F val0123 )
{
    cvScalar_t scalar;
    scalar.val[0] = val0123;
    scalar.val[1] = val0123;
    scalar.val[2] = val0123;
    scalar.val[3] = val0123;
    return scalar;
}

/****cv integral image****/ //chaotien 2015/2/8
typedef struct _cvIntegralImg
{
    int *sum;
    double *sqsum;
    double *tilted;
    cvSize_t sumSize;
    cvSize_t imgSize;
} cvIntegralImg_t;


/************************************* cvParameter *****************************************/
typedef struct _cvDoubleThreshold
{
	CV_32S min;
	CV_32S max;

}cvDoubleThreshold_t;

/*********************************** Sequence *******************************************/

typedef struct _cvSeqBlock
{
    struct _cvSeqBlock*  prev; /* previous sequence block */
    struct _cvSeqBlock*  next; /* next sequence block */
    CV_VOID* data;

}cvSeqBlock_t;

/*
   Read/Write sequence.
   Elements can be dynamically inserted to or deleted from the sequence.
*/
#define CV_SEQUENCE_FIELDS()                                               \
    CV_32S          total;          /* total number of elements */          \
    CV_32S			elem_size;      /* size of sequence element in bytes */ \
    cvSeqBlock_t*	first;          /* pointer to the first sequence block */

typedef struct _cvSeq
{
    CV_SEQUENCE_FIELDS()

}cvSeq_t;

/* TODO AF Structure */
typedef struct _cvAfInfo
{
	cvRect_t rect[5];
	CV_32U	afSum[5];
}cvAfInfo_t;


#endif /*_CV_TYPES_H_*/

/* End of file. */
