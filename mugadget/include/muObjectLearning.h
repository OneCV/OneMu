#ifndef _MUOBJECTLEARNING_H_
#define _MUOBJECTLEARNING_H_

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include "muCore.h"

//*************from obdection************************** 
#define CT_HAAR_FEATURE_MAX  3
#define features_num 1000

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

typedef struct CvHaarRect
{
    int x;
    int y;
    int width;
    int height;
    float ori_weight;
} CvHaarRect;

typedef struct CtHaarFeature
{
    int tilted;
    struct
    {
        CtRect r; //*
        float weight;
        float ori_weight; //*
        int *p0, *p1, *p2, *p3;
    }
    rect[CT_HAAR_FEATURE_MAX];
} CtHaarFeature;

typedef struct CtHaarTreeNode
{
    CtHaarFeature feature;
    int two_rects;
    float threshold; //*
    float left; //*
    float right; //*
} CtHaarTreeNode;


typedef struct CtHaarClassifier
{
    int count;
    //CvHaarFeature* orig_feature;
    CtHaarTreeNode* node;
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

//Structure for Learning weak classifiers
typedef struct CvLearningHaarClassifier
{
    CtHaarFeature feature;
    unsigned char  rect_num;
    unsigned char polar;
    float threshold; //*
    float left; //*
    float right; //*
    //Learning PN
    unsigned int remainPos;
    unsigned int remainNeg;
}CvLearningHaarClassifier;

//Structure for Learning weak classifier pool
typedef struct CvLearningModel
{
    int  count;
    CtSize orig_window_size;
    double inv_window_area;
    CvLearningHaarClassifier pool[features_num];
    double *pq0, *pq1, *pq2, *pq3;
    int *p0, *p1, *p2, *p3;
} CvLearningModel;


////////////////////////////////////////
typedef struct randomhaarfeature
{
    int poolindex, rect_num, tilted, correctlabel;
    CvHaarRect r[3];
    float ori_weight;
    float threshold; //*
    float left; //*
    float right; //*
}randomhaarfeature;

/** Init functions of boosting detector **/
MU_API(MU_VOID) muObjectLearning_Init(muImage_t *img, CtRect *box, muImage_t *ultraNeg);

#endif /*_MUOBJECTLEARNING_H_*/