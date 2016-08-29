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


enum
{
	MU_BGM_GMM = 1,
	MU_BGM_ISB,
};

MU_API(muError_t) muBackgroundModelingInit(MU_32U width, MU_32U height, MU_32U type);

MU_API(muError_t) muBackgroundModeling(muImage_t *curimg, muImage_t *bkimg);

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
