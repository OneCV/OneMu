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
 * Module: muGadget.h
 * Author: Joe Lin, Chao-Ting Hong
 *
 * Description:
 *    define the video analytics functions
 *
 -------------------------------------------------------------------------- */

#ifndef _MUGADGET_H_
#define _MUGADGET_H_

#include "muCore.h"

/* muDetectCamTampering()
*
* Detect whether the input image is covered or lost focus.
*
* Parameters
* src:            the source image frame, it must be 1 channel (gray-level) image.
* flags:          the situations want to detect (MU_CAM_LOSTFOCUS/ MU_CAM_OCCLUSION)
* sensitivity:    from 1 to 5
* return value:   detection result (MU_CAM_LOSTFOCUS/ MU_CAM_OCCLUSION)
*
*/

#define MU_CAM_NORMAL      0x00
#define MU_CAM_LOSTFOCUS   0x01
#define MU_CAM_OCCLUSION   0x02

MU_API(MU_32S) muDetectCamTampering( const muImage_t* src, MU_32S flags, MU_32S sensitivity);
/* end of muDetectCamTampering */

/* the direction of muSADObjectCounting()*/
#define MU_ONE_WAY 0x00
#define MU_TWO_WAY 0x01

typedef enum
{
	MU_TRIPWIRE=1,
	MU_HEADCOUNTING=2,
}muTrackingAppMode_t;


typedef struct _muSADTrackingParam
{
	MU_32S sadsensitive; // SAD threshold
	MU_32U label; // the number of label for history list
	MU_32S timeth; // the threshold of time to delete
	muSeq_t *historyhead; // the pointer buffer of history header
	muSeqBlock_t *historylast; // the pointer buffer of history last address
	muDoubleThreshold_t th; // the basic object size, if objectsize bigger than th.max  or small than th.min will discard
	muTrackingAppMode_t mode; // MU_TRIPWIRE=1; MU_HEADCOUNTING=2;

}muSADTrackingParam_t;


typedef struct _muSADTripwireCounting
{
	MU_32S way;  //one-way or two-way
	MU_32S incount, outcount; // object counting output
	MU_32S objectsize; // pre-define object size
	MU_32S dirsensitive; // the threshold of sensitive for direction
	MU_32S linenum;
	muLine_t line; // pre-define the axis of tripwire

}muSADTripwireCounting_t;


typedef struct _muSADTripZone
{
	muRect_t rect; //pre-define rectangle position 
	MU_32S alarm, updateth;  //output alarm and the threshold of update times
	MU_32S zonenum;

}muSADTripZone_t;


typedef struct _muSADApp
{
	muBool_t tripwire;
	muBool_t obcounting;
	muBool_t tripzone;
	muResolution_t mode;
	MU_8U *saddata;
	muSize_t sadsize;
}muSADApp_t;


/* ------------------------------------------------------------------------- /
 *
 * Application: SAD-Based object tracking 
 * Author: Joe Lin
 * Create date: 03/04/2010
 *
 * Description:
 *  This application is presented the SAD-Based tracking.
 *  The flow chart is shown as follow
 *
 *  [SAD RAW data in]->[muContraststretching]->[SADFindThreshold]->
 *
 *  [muThresholding]->[CLOSING(D->E)]->[OPENING(E->D)]->[mu4ConnectedComponent8u]->
 *
 *  [muFindBoundingBox]->[ObjectCounting]->[counting output]
 *
 * Usage:
 *  muSADTracking(muImage_t *src, // input image
 *                       muImage_t *dst,  // output image
 *                       muTrackingParam param, // parameters of object counting
 *
 *  The more detail of parameter setting and technical please see the MULIB user guide.
 *
 -------------------------------------------------------------------------- */
MU_API(muError_t) muSADTracking(muImage_t *src, muImage_t *dst, muSADTrackingParam_t *param);



/* ------------------------------------------------------------------------- /
 *
 * Application: SAD-Based tripwire and object counting 
 * Author: Joe Lin
 * Create date: 03/16/2010
 *
 * Description:
 *  This application is presented the SAD-Based object counting.
 *  Please note that this function must work after muSADTracking. 
 *
 * Usage:
 *  muSADTripwireCounting(muSeqBlock *first, // please feed the first sequence block address of history header list  
 *                         muTripwireCounting *param); // the parameter setting of tripwire and objectcounting
 *                        
 *                       
 *  The more detail of parameter setting and technical please see the MULIB user guide.
 *
 -------------------------------------------------------------------------- */
MU_API(muError_t) muSADTripwireCounting(muSeqBlock_t **first, muSADTripwireCounting_t *param);


/* ------------------------------------------------------------------------- /
 *
 * Application: SAD-Based tripzone 
 * Author: Joe Lin
 * Create date: 03/16/2010
 *
 * Description:
 *  This application is presented the SAD-Based tripzone.
 *  Please note that this function must work after muSADTracking. 
 *
 * Usage:
 *  muSADTripZone(muSeqBlock *first, // please feed the first sequence block address of history header list  
 *                         muTripwireZone *param); // the parameter setting of tripzone
 *                        
 *                       
 *  The more detail of parameter setting and technical please see the MULIB user guide.
 *
 -------------------------------------------------------------------------- */
MU_API(muError_t) muSADTripZone(muSeqBlock_t *first, muSADTripZone_t *param);

// This function must call after finish muSADTracking
MU_API(MU_VOID) muSADDeleteObject(muSeq_t *);
/* end of muSADtracking Based algorithm */


/* ------------------------------------------------------------------------- /
 *
 * Application: PTZ Auto-Tracking
 * Author: Joe Lin
 * Create date: 09/23/2011
 *
 * Description:
 *  This application is presented the PTZ Auto-Tracking.
 *  It include two ways of implementation:
 *  1. Step Version - The way of PTZ movement is step by step, not continually and smoothly.
 *
 * Usage:
 *  [Step Version]
 *  Just keep calling muGetPtzGoTarget().
 *  This function return the target position for app to control PTZ to the position.
 *                        
 -------------------------------------------------------------------------- */
typedef struct _muPIrisAnalzeValue
{
	MU_32S pts;
	MU_8U lum_value;//average graylevel
	MU_32U over_exp_blk_num;//the number of over-exposure blocks
	MU_8U max_region;//0~15	the most bright graylevel in the frame
	MU_8U img_match;//the image change or not, 1:match , 0:change
    MU_32U hist[256];
    MU_32S pix_num;
}muPIrisAnalzeValue_t;

typedef struct _muPIrisAnalzeParameter
{
     MU_8U win_h_s;//2 to the power
     MU_8U win_w_s;//2 to the power
     MU_32S var_th;//0~255
     MU_32S width;
     MU_32S height;	 
	 MU_8U enable_var_anal;//0 or 1
	 MU_8U sample_freq;
     MU_8U over_exp_range_l;//0~255
     MU_8U over_exp_range_h;//0~255
}muPIrisAnalzeParameter_t;

//Joe Added
typedef struct _muPTZTargetInfo
{
	muImage_t *yuv422;
	muImage_t *bin;
	muRect_t rect;
}muPtzTargetInfo_t;

typedef struct _muMaskInfo
{
	MU_32S onoff;
	MU_32S start_x;
	MU_32S start_y;
	MU_32S width;
	MU_32S height;

}muMaskInfo_t;

#define NUM_MASK	4
typedef struct _muPtzGetTargetParas
{
	muPtzTargetInfo_t info;
	muMaskInfo_t mask[NUM_MASK];
	MU_32S manual_select;
	muPoint_t manual_center;

}muPtzGetTargetParas_t;

typedef struct _muPtzTrackingParas
{
    // user setting
    MU_32S ZoomMode;			//0(off),1(auto)
    MU_32S MaxZoomRatio;       //SONY(10~200), HITACHI(10~220)
	muPtzTargetInfo_t *info;

}muPtzTrackingParas_t;


typedef struct _muPtzTrackingAction
{
    // PTZ action
    MU_16U ZoomValue;	//unit:0.1x
    MU_8U XValue;		//0~100%
	MU_8U YValue;		//0~100%
	MU_8U Operation;	//bit0:Xdir;bit1:YDir;bit2:Spd  0:right 0:up 1?:fast

	MU_16U Xdir;
	MU_16U Xspd;

	MU_16U Ydir;
	MU_16U Yspd;

	muRect_t TrackWin;

}muPtzTrackingAction_t;

/* Mu Simple Haar Cascade classifier */
#define MU_HAAR_FEATURE_MAX  3

typedef struct MuHaarFeature
{
    int tilted;
	struct
    {
        muRect_t r;
        float weight;
		float ori_weight;
        int *p0, *p1, *p2, *p3;
    }
    rect[MU_HAAR_FEATURE_MAX];
} MuHaarFeature;


typedef struct MuHaarTreeNode
{
    MuHaarFeature feature;
    int two_rects;
    float threshold;
    float left;
    float right;
} MuHaarTreeNode;


typedef struct MuHaarClassifier
{
    MU_32S count;
    MuHaarTreeNode node;
} MuHaarClassifier;

typedef struct MuHaarStageClassifier
{
    int  count;
    float threshold;
    MuHaarClassifier* classifier;
    int two_rects;

    struct MuHaarStageClassifier* next;
    struct MuHaarStageClassifier* child;
    struct MuHaarStageClassifier* parent;
} MuHaarStageClassifier;


typedef struct MuSimpleDetector
{
    int  count;
    int  isStumpBased;
	muSize_t orig_window_size;
    muSize_t real_window_size;
	double scale;
    int  has_tilted_features;
    int  is_tree;
    double inv_window_area;
    MuHaarStageClassifier* stage_classifier;
    double *pq0, *pq1, *pq2, *pq3;
    int *p0, *p1, *p2, *p3;
} MuSimpleDetector;

/*Mu Examinator structures*/
typedef struct MuStatus
{
	MU_8U State;
	MU_8U Trigger;
}MuStatus;

typedef struct MuTag
{
	MU_32S Type;
	MU_32S x;
	MU_32S y;
	MU_32S width;
	MU_32S height;
} MuTag;

typedef struct MuTracker
{
	MU_32S x;
	MU_32S y;
	MU_32S width;
	MU_32S height;
	MU_32U life;
	MU_8U check;
	MU_8U detected;
} MuTracker;

typedef struct MuExamData
{
	MU_32S ID;
	muSize_t imgSize;
	muRect_t ScanROI;
	MuTag Tag[10];
	MU_32S DetectorSize[10];
	MU_32S TagNum;
} MuExamData;

typedef struct MuDetector
{
	MuSimpleDetector Cascade;
	MuHaarStageClassifier CascadeStages[25];
	MuHaarClassifier CascadeClassifiers[500];
	muSeq_t *Objects;
	muSeq_t *Tracks;
	muRect_t ScanROI;
	MuStatus Status;
	MU_8U Checked;
	MU_8U HitNum;
} MuDetector;

typedef struct MuExaminator
{
	MuExamData ExamData;
	MuDetector Detector[10];
	muIntegralImg_t *Itlmg;
	muRect_t ScanBar;
} MuExaminator;
/*End of mu examinator*/

/* Mu Boost Learning structure start */

#define features_num 2000

typedef struct MuHaarRect
{
    MU_32S x;
    MU_32S y;
    MU_32S width;
    MU_32S height;
    MU_32F weight;
} MuHaarRect;

//Structure for Learning weak classifiers
typedef struct MuLearningHaarClassifier
{
    MuHaarFeature feature;
    MU_8U  rect_num;
    MU_8U polar;
    MU_32F threshold;
    MU_32F left;
    MU_32F right;
    //Learning P,N
    MU_32U remainPos;
    MU_32U remainNeg;
}MuLearningHaarClassifier;

//Structure for Learning weak classifier pool
typedef struct MuLearningModel
{
    MU_32S  count;
    muSize_t orig_window_size;
    MU_64F inv_window_area;
    MuLearningHaarClassifier pool[features_num];
    MU_64F *pq0, *pq1, *pq2, *pq3;
    MU_32S *p0, *p1, *p2, *p3;
} MuLearningModel;

//Random Haar
typedef struct MuRandHaarFeature
{
    MU_32S rect_num;
    MuHaarRect rect[3];
}MuRandHaarFeature;

/* Mu Boost Learning Structure End */

#define calc_sum(rect,offset) \
    ((rect).p0[offset] - (rect).p1[offset] - (rect).p2[offset] + (rect).p3[offset])

#define calc_sumf(rect,offset) \
    static_cast<MU_32F>((rect).p0[offset] - (rect).p1[offset] - (rect).p2[offset] + (rect).p3[offset])

// if no object be detected return 0, else return 1
MU_API(MU_VOID) muPtzGetTargetInit(MU_32S width, MU_32S height);

MU_API(MU_VOID) muPtzGetTargetReset();
MU_API(MU_32S) muPtzGetTarget(const muImage_t *pre_img, const muImage_t *cur_yuv422, muPtzGetTargetParas_t *para);

// reset tracking paras to find a new target, would NOT reset user setting
MU_API(MU_VOID) muPtzTrackingInit(MU_32S width, MU_32S height);

// if no object be detected return 0 else 1
MU_API(MU_32S) muPtzTrackingTarget(const muImage_t *pYUV422Img, muPtzTrackingParas_t *paras, muPtzTrackingAction_t *action);
/* End of PTZ Auto-Tracking */

MU_API(muError_t) muCheckhistory(muSADTrackingParam_t **param);

MU_API(MU_8S) muPIrisAnalyze(const muImage_t *src, muPIrisAnalzeValue_t* pIrisAnalyzeValue, muPIrisAnalzeParameter_t* PIrisAnalyzeParameter);

enum
{
	MU_BGM_GMM = 1,
	MU_BGM_ISB,
};

MU_API(muError_t) muBackgroundModelingInit(MU_32U width, MU_32U height, MU_32U type);
MU_API(MU_8S) muConfigPIrisAnalyze( muPIrisAnalzeParameter_t* pIrisAnalyeParameter );

MU_API(muError_t) muBackgroundModeling(muImage_t *curimg, muImage_t *bkimg);
MU_API(MU_VOID) muReleasePIrisAnalyze();

MU_API(muError_t) muBackgroundModelingReset();
MU_API(muError_t) muBackgroundModelingRelease();

/**Object Detection Function Headers**/
MU_API(MU_VOID) muCalcIntegralImage( const MU_8U* src, MU_32S* sum, MU_64F* sqsum, muSize_t size);
MU_API(MuSimpleDetector*) muLoadSimpleDetector(const char* filename);
MU_API(MU_VOID) muReleaseSimpleDetector(MuSimpleDetector* Detector);
MU_API(MU_VOID) muObjectDetectionInit(MuSimpleDetector* Detector, MuHaarStageClassifier *cascade_stages, MuHaarClassifier *cascade_classifiers, double *CascadeParaTable);

/*Classic Object Detection Function*/
MU_API(muSeq_t*) muObjectDetection(muImage_t* img, MuSimpleDetector* Detector, double scaleFactor, muSize_t minSize, muSize_t maxSize);

/*Lightened Object Detection functions*/
MU_API(muIntegralImg_t*) muIntegral_Light(muImage_t *img);
MU_API(MU_VOID) muIntegral_LightRelease(muIntegralImg_t* Itlmg);
MU_API(MU_VOID) muObjectDetection_Light(muIntegralImg_t *Itlmg, muRect_t ScanROI, muSeq_t* Objects, MuSimpleDetector* Detector, double scaleFactor, muSize_t minSize, muSize_t maxSize);
MU_API(MU_VOID) muObjectDetection_SuperLight(muIntegralImg_t *Itlmg, muRect_t ScanROI, muSeq_t* Objects, MuSimpleDetector* Detector, muSize_t winSize);
MU_API(MU_VOID) muMergeRectangles(muSeq_t *Rectangles, int MergeObjDistTH, int HitNum);

/*Boost Learning function*/
MU_API(MU_VOID) muObjectLearning_Init(muImage_t *img, muRect_t *box, muImage_t *ultraNeg);

/**Examinator Function Headers**/
MU_API(MU_VOID) Examinator_Init_Buf(MU_8U *buf, MuExaminator *Examinator);
MU_API(MU_VOID) Examinator_Init(FILE *fp, MuExaminator *Examinator);
MU_API(MU_VOID) Examinator_Run(muImage_t *src, MuExaminator *Examinator);
MU_API(MU_VOID) Examinator_Release(MuExaminator *Examinator);
MU_API(MU_VOID) Examinator_Teach(MuExamData *Data);
MU_API(MU_VOID) ExampleExaminatorMaker();

#endif /* _MUGADGET_H_ */

/* End of file. */
