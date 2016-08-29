/* ------------------------------------------------------------------------- /
 *
 * Module: cvapp.h
 * Author: Joe Lin
 *
 * Description:
 *    define the video analytics functions
 *
 -------------------------------------------------------------------------- */

#ifndef _CVAPP_H_
#define _CVAPP_H_

#include "cvcore.h"
#include "cvaux.h"

/* cvDetectCamTampering()
*
* Detect whether the input image is covered or lost focus.
*
* Parameters
* src:            the source image frame, it must be 1 channel (gray-level) image.
* flags:          the situations want to detect (CV_CAM_LOSTFOCUS/ CV_CAM_OCCLUSION)
* sensitivity:    from 1 to 5
* return value:   detection result (CV_CAM_LOSTFOCUS/ CV_CAM_OCCLUSION)
*
*/

#define CV_CAM_NORMAL      0x00
#define CV_CAM_LOSTFOCUS   0x01
#define CV_CAM_OCCLUSION   0x02

CV_API(CV_32S) cvDetectCamTampering( const cvImage_t* src, CV_32S flags, CV_32S sensitivity);

/* end of cvDetectCamTampering */



/* the direction of cvSADObjectCounting()*/
#define CV_ONE_WAY 0x00
#define CV_TWO_WAY 0x01

typedef enum
{
	CV_TRIPWIRE=1,  
	CV_HEADCOUNTING=2,
}cvTrackingAppMode_t;


typedef struct _cvSADTrackingParam
{
	CV_32S sadsensitive; // SAD threshold
	CV_32U label; // the number of label for history list
	CV_32S timeth; // the threshold of time to delete
	cvSeq_t *historyhead; // the pointer buffer of history header
	cvSeqBlock_t *historylast; // the pointer buffer of history last address
	cvDoubleThreshold_t th; // the basic object size, if objectsize bigger than th.max  or small than th.min will discard
	cvTrackingAppMode_t mode; // CV_TRIPWIRE=1; CV_HEADCOUNTING=2;

}cvSADTrackingParam_t;


typedef struct _cvSADTripwireCounting
{
	CV_32S way;  //one-way or two-way
	CV_32S incount, outcount; // object counting output
	CV_32S objectsize; // pre-define object size
	CV_32S dirsensitive; // the threshold of sensitive for direction
	CV_32S linenum;
	cvLine_t line; // pre-define the axis of tripwire

}cvSADTripwireCounting_t;


typedef struct _cvSADTripZone
{
	cvRect_t rect; //pre-define rectangle position 
	CV_32S alarm, updateth;  //output alarm and the threshold of update times
	CV_32S zonenum;

}cvSADTripZone_t;


typedef struct _cvSADApp
{
	cvBool_t tripwire;
	cvBool_t obcounting;
	cvBool_t tripzone;
	cvResolution_t mode;
	CV_8U *saddata;
	cvSize_t sadsize;
}cvSADApp_t;


/* ------------------------------------------------------------------------- /
 *
 * Application: SAD-Based object tracking 
 * Author: Nobel Hsu, Joe Lin
 * Create date: 03/04/2010
 *
 * Description:
 *  This application is presented the SAD-Based tracking.
 *  The flow chart is shown as follow
 *
 *  [SAD RAW data in]->[cvContraststretching]->[SADFindThreshold]->
 *
 *  [cvThresholding]->[CLOSING(D->E)]->[OPENING(E->D)]->[cv4ConnectedComponent8u]->
 *
 *  [cvFindBoundingBox]->[ObjectCounting]->[counting output]
 *
 * Usage:
 *  cvSADTracking(cvImage_t *src, // input image
 *                       cvImage_t *dst,  // output image
 *                       cvTrackingParam param, // parameters of object counting
 *
 *  The more detail of parameter setting and technical please see the CVLIB user guide.
 *
 -------------------------------------------------------------------------- */
CV_API(cvError_t) cvSADTracking(cvImage_t *src, cvImage_t *dst, cvSADTrackingParam_t *param);



/* ------------------------------------------------------------------------- /
 *
 * Application: SAD-Based tripwire and object counting 
 * Author: Nobel Hsu, Joe Lin
 * Create date: 03/16/2010
 *
 * Description:
 *  This application is presented the SAD-Based object counting.
 *  Please note that this function must work after cvSADTracking. 
 *
 * Usage:
 *  cvSADTripwireCounting(cvSeqBlock *first, // please feed the first sequence block address of history header list  
 *                         cvTripwireCounting *param); // the parameter setting of tripwire and objectcounting
 *                        
 *                       
 *  The more detail of parameter setting and technical please see the CVLIB user guide.
 *
 -------------------------------------------------------------------------- */
CV_API(cvError_t) cvSADTripwireCounting(cvSeqBlock_t **first, cvSADTripwireCounting_t *param);


/* ------------------------------------------------------------------------- /
 *
 * Application: SAD-Based tripzone 
 * Author: Nobel Hsu, Joe Lin
 * Create date: 03/16/2010
 *
 * Description:
 *  This application is presented the SAD-Based tripzone.
 *  Please note that this function must work after cvSADTracking. 
 *
 * Usage:
 *  cvSADTripZone(cvSeqBlock *first, // please feed the first sequence block address of history header list  
 *                         cvTripwireZone *param); // the parameter setting of tripzone
 *                        
 *                       
 *  The more detail of parameter setting and technical please see the CVLIB user guide.
 *
 -------------------------------------------------------------------------- */
CV_API(cvError_t) cvSADTripZone(cvSeqBlock_t *first, cvSADTripZone_t *param);

// This function must call after finish cvSADTracking
CV_API(CV_VOID) cvSADDeleteObject(cvSeq_t *);
/* end of cvSADtracking Based algorithm */


/* ------------------------------------------------------------------------- /
 *
 * Application: PTZ Auto-Tracking
 * Author: Nobel Hsu, Joe Lin
 * Create date: 09/23/2011
 *
 * Description:
 *  This application is presented the PTZ Auto-Tracking.
 *  It include two ways of implementation:
 *  1. Step Version - The way of PTZ movement is step by step, not continually and smoothly.
 *
 * Usage:
 *  [Step Version]
 *  Just keep calling cvGetPtzGoTarget().
 *  This function return the target position for app to control PTZ to the position.
 *                        
 -------------------------------------------------------------------------- */
typedef struct _cvPIrisAnalzeValue
{
	CV_32S pts;
	CV_8U lum_value;//average graylevel
	CV_32U over_exp_blk_num;//the number of over-exposure blocks
	CV_8U max_region;//0~15	the most bright graylevel in the frame
	CV_8U img_match;//the image change or not, 1:match , 0:change
    CV_32U hist[256];
    CV_32S pix_num;
}cvPIrisAnalzeValue_t;

typedef struct _cvPIrisAnalzeParameter
{
     CV_8U win_h_s;//2 to the power
     CV_8U win_w_s;//2 to the power
     CV_32S var_th;//0~255
     CV_32S width;
     CV_32S height;	 
	 CV_8U enable_var_anal;//0 or 1
	 CV_8U sample_freq;
     CV_8U over_exp_range_l;//0~255
     CV_8U over_exp_range_h;//0~255
}cvPIrisAnalzeParameter_t;

//Joe Added
typedef struct _cvPTZTargetInfo
{
	cvImage_t *yuv422;
	cvImage_t *bin;
	cvRect_t rect;
}cvPtzTargetInfo_t;

typedef struct _cvMaskInfo
{
	CV_32S onoff;
	CV_32S start_x;
	CV_32S start_y;
	CV_32S width;
	CV_32S height;

}cvMaskInfo_t;

#define NUM_MASK	4
typedef struct _cvPtzGetTargetParas
{
	cvPtzTargetInfo_t info;
	cvMaskInfo_t mask[NUM_MASK];
	CV_32S manual_select;
	cvPoint_t manual_center;

}cvPtzGetTargetParas_t;

typedef struct _cvPtzTrackingParas
{
    // user setting
    CV_32S ZoomMode;			//0(off),1(auto)
    CV_32S MaxZoomRatio;       //SONY(10~200), HITACHI(10~220)
	cvPtzTargetInfo_t *info;

}cvPtzTrackingParas_t;


typedef struct _cvPtzTrackingAction
{
    // PTZ action
    CV_16U ZoomValue;	//unit:0.1x
    CV_8U XValue;		//0~100%
	CV_8U YValue;		//0~100%
	CV_8U Operation;	//bit0:Xdir;bit1:YDir;bit2:Spd  0:right 0:up 1?:fast

	CV_16U Xdir;
	CV_16U Xspd;

	CV_16U Ydir;
	CV_16U Yspd;

	cvRect_t TrackWin;

}cvPtzTrackingAction_t;

/* CT Haar classifier  TODO rename and port to cv? */
#define CT_HAAR_FEATURE_MAX  3
typedef struct CtSize
{
    int width;
    int height;
} CtSize;

typedef struct CtRect
{
	int x; 
	int y;
	int width;
	int height;
} CtRect;

typedef struct CtHaarFeature
{
    int tilted;
	struct
    {
        CtRect r;
        float weight;
		float ori_weight;
        int *p0, *p1, *p2, *p3;
    }
    rect[CT_HAAR_FEATURE_MAX];
} CtHaarFeature;


typedef struct CtHaarTreeNode
{
    CtHaarFeature feature;
    int two_rects;
    float threshold;
    float left;
    float right;
} CtHaarTreeNode;


typedef struct CtHaarClassifier
{
    int count;
    //CvHaarFeature* orig_feature;
    CtHaarTreeNode node;
    //float* alpha;
} CtHaarClassifier;


typedef struct CtHaarStageClassifier
{
    int  count;
    float threshold;
    CtHaarClassifier* classifier;
    int two_rects;

    struct CtHaarStageClassifier* next;
    struct CtHaarStageClassifier* child;
    struct CtHaarStageClassifier* parent;
} CtHaarStageClassifier;


typedef struct CtHaarClassifierCascade
{
    int  count;
    int  isStumpBased;
	CtSize orig_window_size;
    CtSize real_window_size;
	double scale;
    int  has_tilted_features;
    int  is_tree;
    double inv_window_area;
    CtHaarStageClassifier* stage_classifier;
    double *pq0, *pq1, *pq2, *pq3;
    int *p0, *p1, *p2, *p3;
} CtHaarClassifierCascade;

/*Cv Examinator structures*/
typedef struct CvStatus
{
	unsigned char State;
	unsigned char Trigger;
}CvStatus;

typedef struct CvTag
{
	unsigned char Type;
	int x;
	int y;
	int width;
	int height;
} CvTag;

typedef struct CvTracker
{
	int x;
	int y;
	int width;
	int height;
	unsigned int life;
	unsigned char check;
	unsigned char detected;
} CvTracker;

typedef struct CvExamData
{
	int ID;
	cvImage_t *img;
	CtSize imgSize;
	CtRect ScanROI;
	CvTag Tag[10];
	int DetectorSize[10];
	int TagNum;
} CvExamData;

typedef struct CvDetector
{
	CtHaarClassifierCascade Cascade;
	CtHaarStageClassifier CascadeStages[25];
	CtHaarClassifier CascadeClassifiers[500];
	cvSeq_t *Objects;
	cvSeq_t *Tracks;
	CtRect ScanROI;
	CvStatus Status;
	unsigned char Checked;
	unsigned char HitNum;
} CvDetector;

typedef struct CvExaminator
{
	CvExamData ExamData;
	CvDetector Detector[10];
	cvIntegralImg_t *Itlmg;
	//cvLine_t ScanLine;
	cvRect_t ScanBar;
} CvExaminator;
/*End of cv examinator*/

#define calc_sum(rect,offset) \
    ((rect).p0[offset] - (rect).p1[offset] - (rect).p2[offset] + (rect).p3[offset])

#define  CT_IMIN(a, b)  ((a) ^ (((a)^(b)) & (((a) < (b)) - 1)))

#define calc_sum(rect,offset) \
    ((rect).p0[offset] - (rect).p1[offset] - (rect).p2[offset] + (rect).p3[offset])

#define calc_sumf(rect,offset) \
    static_cast<float>((rect).p0[offset] - (rect).p1[offset] - (rect).p2[offset] + (rect).p3[offset])
/* CT Haarclassifier end */

// if no object be detected return 0, else return 1
CV_API(CV_VOID) cvPtzGetTargetInit(CV_32S width, CV_32S height);

CV_API(CV_VOID) cvPtzGetTargetReset();
CV_API(CV_32S) cvPtzGetTarget(const cvImage_t *pre_img, const cvImage_t *cur_yuv422, cvPtzGetTargetParas_t *para);

// reset tracking paras to find a new target, would NOT reset user setting
CV_API(CV_VOID) cvPtzTrackingInit(CV_32S width, CV_32S height);

// if no object be detected return 0, else return 1 by Vergil Tien.
CV_API(CV_32S) cvPtzTrackingTarget(const cvImage_t *pYUV422Img, cvPtzTrackingParas_t *paras, cvPtzTrackingAction_t *action);
/* End of PTZ Auto-Tracking */

CV_API(cvError_t) cvCheckhistory(cvSADTrackingParam_t **param);

CV_API(CV_8S) cvPIrisAnalyze(const cvImage_t *src, cvPIrisAnalzeValue_t* pIrisAnalyzeValue, cvPIrisAnalzeParameter_t* PIrisAnalyzeParameter);

enum
{
	CV_BGM_GMM = 1,
	CV_BGM_ISB,
};

CV_API(cvError_t) cvBackgroundModelingInit(CV_32U width, CV_32U height, CV_32U type);
CV_API(CV_8S) cvConfigPIrisAnalyze( cvPIrisAnalzeParameter_t* pIrisAnalyeParameter );

CV_API(cvError_t) cvBackgroundModeling(cvImage_t *curimg, cvImage_t *bkimg);
CV_API(CV_VOID) cvRleasePIrisAnalyze();

CV_API(cvError_t) cvBackgroundModelingReset();
CV_API(cvError_t) cvBackgroundModelingRelease();

/**Object Detection Function Headers**/
CV_API(cvSeq_t*) cvObjectDetection(cvImage_t* img, CtHaarClassifierCascade* cascade, double scaleFactor, int minNeighbors, int flags, cvSize_t minSize, cvSize_t maxSize);
CV_API(CV_VOID) cvObjectDetection1(cvImage_t* img, cvSeq_t* Objects, CtHaarClassifierCascade* cascade, double scaleFactor, int minNeighbors, int flags, cvSize_t minSize, cvSize_t maxSize);
CV_API(CV_VOID) cvObjectDetectionInit(CtHaarClassifierCascade* cascade, CtHaarStageClassifier *cascade_stages, CtHaarClassifier *cascade_classifiers, double *CascadeParaTable);
CV_API(cvIntegralImg_t*) cvIntegral_Light(cvImage_t *img);
CV_API(CV_VOID) cvIntegral_LightRelease(cvIntegralImg_t* Itlmg);
CV_API(CV_VOID) cvObjectDetection_Light(cvIntegralImg_t *Itlmg, cvSeq_t* Objects, CtHaarClassifierCascade* cascade, double scaleFactor, int minNeighbors, cvSize_t minSize, cvSize_t maxSize);

/**Examinator Function Headers**/
CV_API(CV_VOID) Examinator_Init_Buf(CV_8U *buf, CvExaminator *Examinator);
CV_API(CV_VOID) Examinator_Init(FILE *fp, CvExaminator *Examinator);
CV_API(CV_VOID) Examinator_Run(CvExaminator *Examinator);
CV_API(CV_VOID) Examinator_Release(CvExaminator *Examinator);
CV_API(CV_VOID) Examinator_Teach(CvExamData *Data);
CV_API(CV_VOID) ExampleExaminatorMaker();

#endif /* _CVAPP_H_ */

/* End of file. */
