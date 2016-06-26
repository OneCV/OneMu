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
 * Module: muObjectLearning.c
 * Author: Chao-Ting Hong, Yutong 
 *
 * Description:
 * This file is presented object detector with haar-cascade
 *  
 -------------------------------------------------------------------------- */

#include "muGadget.h"

#define FLOAT_MAX 3.4e38f
#define FLOAT_MIN 1.2e-38f
#define MAX_NEG 200

muSeq_t *selectedIndex = NULL; //Selected feature Index
muSeq_t *poolIndex = NULL; //Remained Feature pool Index

double HaarValueNegMean[features_num];
double HaarValuePosMean[features_num];
double HaarValuePos[50][features_num];
double HaarValueNeg[MAX_NEG][features_num];

void muRandomFeatureGen(MuRandHaarFeature *RandomFeature, muRect_t *box)
{
    int mode;
    int width=box->width-1;
    int height=box->height-1;
    int DivWidth;
    int DivHeight;
    
    mode=(rand() % 7);

    switch(mode)
    {
        //Vertical 01 Haar
        case 0:
            RandomFeature->rect_num=2;
            DivWidth = (width-4)/2+1;
            RandomFeature->rect[0].width = (rand()%DivWidth)*2 + 4;
            RandomFeature->rect[0].height = rand()%(height-1) + 2;
            RandomFeature->rect[0].x = rand()%(box->width-RandomFeature->rect[0].width);
            RandomFeature->rect[0].y = rand()%(box->height-RandomFeature->rect[0].height);
            RandomFeature->rect[0].weight=-1;

            RandomFeature->rect[1].width=RandomFeature->rect[0].width/2;
            RandomFeature->rect[1].height=RandomFeature->rect[0].height;
            RandomFeature->rect[1].x=RandomFeature->rect[0].x+RandomFeature->rect[1].width;
            RandomFeature->rect[1].y=RandomFeature->rect[0].y;
            RandomFeature->rect[1].weight=2;

            RandomFeature->rect[2].width=0;
            RandomFeature->rect[2].height=0;
            RandomFeature->rect[2].x=0;
            RandomFeature->rect[2].y=0;
            RandomFeature->rect[2].weight=0;
            break;

        //Horizantal 01' Haar
        case 1:
            RandomFeature->rect_num=2;
            RandomFeature->rect[0].width = rand()%(width-1) + 2;
            DivHeight=(height-4)/2+1;
            RandomFeature->rect[0].height = (rand()%DivHeight)*2 + 4;
            RandomFeature->rect[0].x = rand()%(box->width-RandomFeature->rect[0].width);
            RandomFeature->rect[0].y = rand()%(box->height-RandomFeature->rect[0].height);
            RandomFeature->rect[0].weight=-1;

            RandomFeature->rect[1].width=RandomFeature->rect[0].width;
            RandomFeature->rect[1].height=RandomFeature->rect[0].height/2;
            RandomFeature->rect[1].x=RandomFeature->rect[0].x;
            RandomFeature->rect[1].y=RandomFeature->rect[0].y+RandomFeature->rect[1].height;
            RandomFeature->rect[1].weight=2;

            RandomFeature->rect[2].width=0;
            RandomFeature->rect[2].height=0;
            RandomFeature->rect[2].x=0;
            RandomFeature->rect[2].y=0;
            RandomFeature->rect[2].weight=0;
            break;
        //Vertical 010 Haar 3:1
        case 2:
            RandomFeature->rect_num=2;
            DivWidth = (width-6)/3+1;
            RandomFeature->rect[0].width = (rand()%DivWidth)*3 + 6;
            RandomFeature->rect[0].height = rand()%(height-1) + 2;
            RandomFeature->rect[0].x = rand()%(box->width-RandomFeature->rect[0].width);
            RandomFeature->rect[0].y = rand()%(box->height-RandomFeature->rect[0].height);
            RandomFeature->rect[0].weight=-1;

            RandomFeature->rect[1].width=RandomFeature->rect[0].width/3;
            RandomFeature->rect[1].height=RandomFeature->rect[0].height;
            RandomFeature->rect[1].x=RandomFeature->rect[0].x+RandomFeature->rect[1].width;
            RandomFeature->rect[1].y=RandomFeature->rect[0].y;
            RandomFeature->rect[1].weight=3;

            RandomFeature->rect[2].width=0;
            RandomFeature->rect[2].height=0;
            RandomFeature->rect[2].x=0;
            RandomFeature->rect[2].y=0;
            RandomFeature->rect[2].weight=0;
            break;
        //Horizantal 010' Haar 3:1
        case 3:
            RandomFeature->rect_num=2;
            RandomFeature->rect[0].width = rand()%(width-1) + 2;
            DivHeight=(height-6)/3+1;
            RandomFeature->rect[0].height = (rand()%DivHeight)*3 + 6;
            RandomFeature->rect[0].x = rand()%(box->width-RandomFeature->rect[0].width);
            RandomFeature->rect[0].y = rand()%(box->height-RandomFeature->rect[0].height);
            RandomFeature->rect[0].weight=-1;

            RandomFeature->rect[1].width=RandomFeature->rect[0].width;
            RandomFeature->rect[1].height=RandomFeature->rect[0].height/3;
            RandomFeature->rect[1].x=RandomFeature->rect[0].x;
            RandomFeature->rect[1].y=RandomFeature->rect[0].y+RandomFeature->rect[1].height;
            RandomFeature->rect[1].weight=3;

            RandomFeature->rect[2].width=0;
            RandomFeature->rect[2].height=0;
            RandomFeature->rect[2].x=0;
            RandomFeature->rect[2].y=0;
            RandomFeature->rect[2].weight=0;
            break;
        //Vertical 010 Haar 2:1
        case 4:
            RandomFeature->rect_num=2;
            DivWidth = (width-8)/4+1;
            RandomFeature->rect[0].width = (rand()%DivWidth)*4 + 8;
            RandomFeature->rect[0].height = rand()%(height-1) + 2;
            RandomFeature->rect[0].x = rand()%(box->width-RandomFeature->rect[0].width);
            RandomFeature->rect[0].y = rand()%(box->height-RandomFeature->rect[0].height);
            RandomFeature->rect[0].weight=-1;

            RandomFeature->rect[1].width=RandomFeature->rect[0].width/2;
            RandomFeature->rect[1].height=RandomFeature->rect[0].height;
            RandomFeature->rect[1].x=RandomFeature->rect[0].x+RandomFeature->rect[0].width/4;
            RandomFeature->rect[1].y=RandomFeature->rect[0].y;
            RandomFeature->rect[1].weight=2;

            RandomFeature->rect[2].width=0;
            RandomFeature->rect[2].height=0;
            RandomFeature->rect[2].x=0;
            RandomFeature->rect[2].y=0;
            RandomFeature->rect[2].weight=0;
            break;
        //Horizantal 010' Haar 2:1
        case 5:
            RandomFeature->rect_num=2;
            RandomFeature->rect[0].width = rand()%(width-1) + 2;
            DivHeight=(height-8)/4+1;
            RandomFeature->rect[0].height = (rand()%DivHeight)*4 + 8;
            RandomFeature->rect[0].x = rand()%(box->width-RandomFeature->rect[0].width);
            RandomFeature->rect[0].y = rand()%(box->height-RandomFeature->rect[0].height);
            RandomFeature->rect[0].weight=-1;

            RandomFeature->rect[1].width=RandomFeature->rect[0].width;
            RandomFeature->rect[1].height=RandomFeature->rect[0].height/2;
            RandomFeature->rect[1].x=RandomFeature->rect[0].x;
            RandomFeature->rect[1].y=RandomFeature->rect[0].y+RandomFeature->rect[0].height/4;
            RandomFeature->rect[1].weight=2;

            RandomFeature->rect[2].width=0;
            RandomFeature->rect[2].height=0;
            RandomFeature->rect[2].x=0;
            RandomFeature->rect[2].y=0;
            RandomFeature->rect[2].weight=0;
            break;
        // 10 Haar 4:1:1
        // 01
        case 6:
            RandomFeature->rect_num=3;
            DivWidth=(width-4)/2+1;
            RandomFeature->rect[0].width = (rand()%DivWidth)*2 + 4;
            DivHeight=(height-4)/2+1;
            RandomFeature->rect[0].height = rand()%(DivHeight)*2 + 4;
            RandomFeature->rect[0].x = rand()%(box->width-RandomFeature->rect[0].width);
            RandomFeature->rect[0].y = rand()%(box->height-RandomFeature->rect[0].height);
            RandomFeature->rect[0].weight=-1;

            RandomFeature->rect[1].width=RandomFeature->rect[0].width/2;
            RandomFeature->rect[1].height=RandomFeature->rect[0].height/2;
            RandomFeature->rect[1].x=RandomFeature->rect[0].x;
            RandomFeature->rect[1].y=RandomFeature->rect[0].y;
            RandomFeature->rect[1].weight=2;

            RandomFeature->rect[2].width=RandomFeature->rect[0].width/2;
            RandomFeature->rect[2].height=RandomFeature->rect[0].height/2;
            RandomFeature->rect[2].x=RandomFeature->rect[0].x+RandomFeature->rect[0].width/2;
            RandomFeature->rect[2].y=RandomFeature->rect[0].y+RandomFeature->rect[0].height/2;
            RandomFeature->rect[2].weight=2;
            break;
    }// end switch(mode)
}

void randomfeaturepool(MuLearningModel* LearningModel, muRect_t *box)
{
    int num = features_num, i, j, temp;

    srand(time(NULL));

    LearningModel->orig_window_size.width = box->width;
    LearningModel->orig_window_size.height = box->height;

    LearningModel->count = features_num;
    for(i=0; i<features_num; i++)
    {
        MuRandHaarFeature RandomFeature;

        muRandomFeatureGen(&RandomFeature, box);
        LearningModel->pool[i].rect_num = RandomFeature.rect_num;

        for(j=0; j < LearningModel->pool[i].rect_num; j++)
        {
            LearningModel->pool[i].feature.rect[j].r.x = RandomFeature.rect[j].x;
            LearningModel->pool[i].feature.rect[j].r.y = RandomFeature.rect[j].y;
            LearningModel->pool[i].feature.rect[j].r.width = RandomFeature.rect[j].width;
            LearningModel->pool[i].feature.rect[j].r.height = RandomFeature.rect[j].height;
            LearningModel->pool[i].feature.rect[j].ori_weight = RandomFeature.rect[j].weight;
        }

        LearningModel->pool[i].threshold = 0;
        LearningModel->pool[i].left = 0;
        LearningModel->pool[i].right = 0;
        LearningModel->pool[i].remainPos = 0;
        LearningModel->pool[i].remainNeg = 0;
    }
}

//Set haar pointers to integral image
//IN: &LearningModel, &sum, &sqsum, sumSize; OUT: &LearningModel
void setLearningImage(MuLearningModel *LearningModel, int *sum, double *sqsum, muSize_t sumSize)
{
    int i, j;
    double weight_scale;
    muRect_t equRect;
    
    //Set scan window area and pointers
    equRect.x = equRect.y = 1;
    equRect.width = LearningModel->orig_window_size.width-2;
    equRect.height = LearningModel->orig_window_size.height-2;
    weight_scale = 1./(equRect.width*equRect.height);
    LearningModel->inv_window_area = weight_scale;

    LearningModel->p0 = sum + sumSize.width*equRect.y + equRect.x;
    LearningModel->p1 = sum + sumSize.width*equRect.y + equRect.x + equRect.width;
    LearningModel->p2 = sum + sumSize.width*(equRect.y + equRect.height) + equRect.x;
    LearningModel->p3 = sum + sumSize.width*(equRect.y + equRect.height) + equRect.x + equRect.width;

    LearningModel->pq0 = sqsum + sumSize.width*equRect.y + equRect.x;
    LearningModel->pq1 = sqsum + sumSize.width*equRect.y + equRect.x + equRect.width;
    LearningModel->pq2 = sqsum + sumSize.width*(equRect.y + equRect.height) + equRect.x;
    LearningModel->pq3 = sqsum + sumSize.width*(equRect.y + equRect.height) + equRect.x + equRect.width;
    
    //Set each haar's pointers
    for(i=0; i<features_num; i++)
    {
        muRect_t tr; //tmp rect

        double sum0 = 0, area0 = 0;

        MuHaarFeature *feature = &LearningModel->pool[i].feature;
       
        for( j = 0; j < LearningModel->pool[i].rect_num; j++ )
        {
            tr = feature->rect[j].r; //feature's r to r
            feature->rect[j].p0 = sum + sumSize.width*tr.y + tr.x;
            feature->rect[j].p1 = sum + sumSize.width*tr.y + tr.x + tr.width;
            feature->rect[j].p2 = sum + sumSize.width*(tr.y + tr.height) + tr.x;
            feature->rect[j].p3 = sum + sumSize.width*(tr.y + tr.height) + tr.x + tr.width;

            feature->rect[j].weight = (float)(feature->rect[j].ori_weight);
            if( j == 0 )
                area0 = tr.width * tr.height;
            else
                sum0 += feature->rect[j].weight * tr.width * tr.height;
        }
        feature->rect[0].weight = (float)(-sum0/area0);
    }
}

void muObjectLearning_Init(muImage_t *img, muRect_t *box, muImage_t *ultraNeg)
{
    int  i, k, j, n, ix, iy, rm;
    int TotalPosNum, TotalNegNum;
    int NegPicActivated = 0;
    int Alpha = 5; //Threshold's pos weight
    int *sum1 = NULL;
    double *sqsum1 = NULL;
    unsigned char *NewFrame1 = NULL;
    int *sum = NULL;
    double *sqsum = NULL;
    unsigned char *NewFrame = NULL;
    int PosIn;
    int PosRange;
    int EndX;
    int EndY;
    int PosSartX;
    int PosSartY;
    int PosEndX;
    int PosEndY;
    double step;
    float minError;
    int featureindex;
    int EraseFeatureIndex;
    FILE *fp = NULL;
    MuLearningModel *LearningModel;
    //Pre-prepared neg image
    muSize_t FrameSize1 = {ultraNeg->width, ultraNeg->height};
    muSize_t sumSize1 = {(FrameSize1.width+1), (FrameSize1.height+1)};
    muSize_t FrameSize = {img->width, img->height};
    muSize_t sumSize = {(FrameSize.width+1), (FrameSize.height+1)};

    //MuLearningModel Model1;
    LearningModel = (MuLearningModel *)malloc(sizeof(MuLearningModel));

    //=======================
    //generate feature pool
    //=======================
    randomfeaturepool(LearningModel, box);
    printf("pool OK \n"); //flag

    selectedIndex = muCreateSeq(sizeof(unsigned int));
    poolIndex = muCreateSeq(sizeof(unsigned int));
    
    
    /*Intergal Image of Negtive Picture*/
    sum1 = (int*)malloc(sizeof(int)*(sumSize1.height*sumSize1.width));
    sqsum1 = (double*)malloc(sizeof(double)*(sumSize1.height*sumSize1.width));
    //Mat(ultraNeg) to C array(NewFrame1)
    NewFrame1 = (unsigned char*)malloc(sizeof(unsigned char)*(FrameSize1.width*FrameSize1.height));

    for(i=0;i<FrameSize1.width*FrameSize1.height;i++)
        NewFrame1[i]=ultraNeg->imagedata[i];

    muCalcIntegralImage(NewFrame1, sum1, sqsum1, FrameSize1); //set grayImg to integral img
    free (NewFrame1);

    //** Haar Values Calculation **//
    ///Integral Image Calculation///
    sum = (int*)malloc(sizeof(int)*(sumSize.height*sumSize.width));
    sqsum = (double*)malloc(sizeof(double)*(sumSize.height*sumSize.width));
    
    //Mat(img) to C array(NewFrame)
    NewFrame = (unsigned char*)malloc(sizeof(unsigned char)*(FrameSize.width*FrameSize.height));

    for(i=0;i<FrameSize.width*FrameSize.height;i++)
        NewFrame[i]=img->imagedata[i];

    muCalcIntegralImage(NewFrame, sum, sqsum, FrameSize);
    free (NewFrame);

    //Set haar pointers to integral image
    setLearningImage(LearningModel, sum, sqsum, sumSize);

    /// Pos(GoodBoxs) x featre values ///
    memset(HaarValuePosMean,0,features_num*sizeof(double));

    PosIn=3; TotalPosNum=0;
    for(iy=box->y-PosIn; iy<=box->y+PosIn; iy++)
        for(ix=box->x-PosIn; ix<=box->x+PosIn; ix++)
        {
            //Run for goodbox->x .y
            int p_offset, pq_offset;
            double mean, variance_norm_factor=-1;

            if(ix<0 || ix>img->width-box->width || iy<0 || iy>img->height-box->height)
                continue;

            pq_offset = p_offset = iy * (sumSize.width) + ix; //offset of sum
            mean = calc_sum(*LearningModel,p_offset)*LearningModel->inv_window_area; //
        
            variance_norm_factor = LearningModel->pq0[pq_offset] - LearningModel->pq1[pq_offset] -
                                   LearningModel->pq2[pq_offset] + LearningModel->pq3[pq_offset];
            variance_norm_factor = variance_norm_factor*LearningModel->inv_window_area - mean*mean; //Mean(Square sum) - mean(sum) square
            
            if( variance_norm_factor >= 0. )
                variance_norm_factor = sqrt(variance_norm_factor);
            else
                variance_norm_factor = 1.;
            
            for( j = 0; j < LearningModel->count; j++ )
            {
                double HaarValue;
                MuHaarFeature *feature = &LearningModel->pool[j].feature;
                HaarValue = calc_sum(feature->rect[0],p_offset) * feature->rect[0].weight;
                HaarValue += calc_sum(feature->rect[1],p_offset) * feature->rect[1].weight;
                
                if(LearningModel->pool[j].rect_num == 3)
                    HaarValue += calc_sum(feature->rect[2],p_offset) * feature->rect[2].weight;

                HaarValue = (HaarValue*LearningModel->inv_window_area)/variance_norm_factor;
                HaarValuePos[TotalPosNum][j]=HaarValue;
                HaarValuePosMean[j]+=HaarValue;
                //HaarValuePosSqureSum[j]+=HaarValue*HaarValue;
            }
            TotalPosNum++;
        }

    /// Neg(BadBoxs) x featre values & Set neg indexes///
    PosRange=MU_MIN(box->width,box->height)*0.1;
    EndX=img->width-box->width;
    EndY=img->height-box->height;
    PosSartX = box->x-PosRange;
    PosSartY = box->y-PosRange;
    PosEndX = box->x+PosRange;
    PosEndY = box->y+PosRange;
    step = sqrt((double)EndX*(double)EndY/(double)MAX_NEG);
    step = muRound(step);
    // >= second round: step = 1;
    TotalNegNum = 0;
    for(iy=0; iy<EndY; iy+=step)
        for(ix=0; ix<EndX; ix+=step)
        {
            int p_offset, pq_offset;
            double mean, variance_norm_factor=-1;
            
            if(iy>PosSartY && iy<PosEndY && ix>PosSartX && ix<PosEndX)
                continue;
            if(TotalNegNum>=MAX_NEG)
                break;

            //Run for badbox->x .y

            pq_offset = p_offset = iy*sumSize.width+ix; //offset of sum

            mean = calc_sum(*LearningModel,p_offset)*LearningModel->inv_window_area; //
        
            variance_norm_factor = LearningModel->pq0[pq_offset] - LearningModel->pq1[pq_offset] -
                                   LearningModel->pq2[pq_offset] + LearningModel->pq3[pq_offset];
            variance_norm_factor = variance_norm_factor*LearningModel->inv_window_area - mean*mean; //Mean(Square sum) - mean(sum) square
            
            if( variance_norm_factor >= 0. )
                variance_norm_factor = sqrt(variance_norm_factor);
            else
                variance_norm_factor = 1.;
            
            // >= Second round: check detection result for selected feature
            for( j = 0; j < LearningModel->count; j++ )
            {
                double HaarValue;

                MuHaarFeature *feature = &LearningModel->pool[j].feature;
                HaarValue = calc_sum(feature->rect[0],p_offset) * feature->rect[0].weight;
                HaarValue += calc_sum(feature->rect[1],p_offset) * feature->rect[1].weight;
                
                if(LearningModel->pool[j].rect_num == 3)
                    HaarValue += calc_sum(feature->rect[2],p_offset) * feature->rect[2].weight;

                HaarValue = (HaarValue*LearningModel->inv_window_area)/variance_norm_factor;
                HaarValueNeg[TotalNegNum][j]=HaarValue;
            }

            TotalNegNum++;
        }// End ix
        //End iy

    /// PosHaarMean & Indexes ///
    for(j=0; j<features_num; j++)
    {
        HaarValuePosMean[j]=HaarValuePosMean[j]/TotalPosNum;
        //HaarValuePosSqureSum[j] = HaarValuePosSqureSum[j]/(PosBoxes.size()) - HaarValuePosMean[j]*HaarValuePosMean[j];
        //HaarValuePosSqureSum[j] = sqrt(HaarValuePosSqureSum[j]);
        muPushSeq(poolIndex, (MU_VOID *)&j);
    }

    //Do While Loop
    do{
        //Select a weak-classifier
        //check for remained pos >= PosBoxes.size() and mini negremain
        minError = FLOAT_MAX;
        featureindex=0;
        EraseFeatureIndex=-1;

        memset(HaarValueNegMean,0,features_num*sizeof(double));
        for(i=0; i<features_num; i++)
        {
            //threshold
            double TmpThreshold;
            
            //Calculate Neg Mean, threshold, polar
            for(j=0; j<TotalNegNum; j++)
                HaarValueNegMean[i] += HaarValueNeg[j][i];

            //neg mean
            HaarValueNegMean[i]=HaarValueNegMean[i]/TotalNegNum;
            
            LearningModel->pool[i].threshold = ((10-Alpha)*HaarValueNegMean[i] + Alpha*HaarValuePosMean[i])/10;

            //polar
            if(HaarValuePosMean[i]>HaarValueNegMean[i])
                LearningModel->pool[i].polar = 1;
            else
                LearningModel->pool[i].polar = -1;

            //Count remained neg (false potive) number for each weak classifiers
            //reset remainNeg
            LearningModel->pool[i].remainNeg = 0;

            for(j=0; j<TotalNegNum; j++)
            {
                if(LearningModel->pool[i].polar==1)
                {
                    if(HaarValueNeg[j][i] > LearningModel->pool[i].threshold)
                        LearningModel->pool[i].remainNeg++;
                }
                else
                {
                    if(HaarValueNeg[j][i] < LearningModel->pool[i].threshold)
                        LearningModel->pool[i].remainNeg++;
                }
            }

            //Count true positive number
            //reset remainPos
            LearningModel->pool[i].remainPos = 0;
            for(j=0; j<TotalPosNum; j++)
            {
                if(LearningModel->pool[i].polar==1)
                {
                    if(HaarValuePos[j][i] > LearningModel->pool[i].threshold)
                        LearningModel->pool[i].remainPos++;
                }
                else
                {
                    if(HaarValuePos[j][i] < LearningModel->pool[i].threshold)
                        LearningModel->pool[i].remainPos++;
                }
            }
        }// end for each feauture

        
        //for(i=0; i<poolIndex.size(); i++)
        for(i=0; i<poolIndex->total; i++)
        {
            //n = poolIndex[i];
            n = (*(int *)muGetSeqElement(&poolIndex, i+1));
            if(minError > LearningModel->pool[n].remainNeg && 
               LearningModel->pool[n].remainPos >= TotalPosNum)
            {
                minError = LearningModel->pool[n].remainNeg;
                featureindex=n;
                EraseFeatureIndex=i;
            }
        }
        
        if(EraseFeatureIndex!=-1)
        {
            //record selected index
            //selectedIndex.push_back(featureindex);
            muPushSeq(selectedIndex, &featureindex);
            //Remove seleted feature index from poolindex
            //poolIndex.erase(poolIndex.begin() + EraseFeatureIndex);
            muRemoveIndexNode(&poolIndex, EraseFeatureIndex+1);
        }
        else
        {
            if(Alpha > 0)
                Alpha--;
            continue;
        }

        //Renew NegSamples and thier haar value
        TotalNegNum = 0;
        step = 1;
        if(NegPicActivated==0){
        for(iy=0; iy<EndY; iy+=step)
        for(ix=0; ix<EndX; ix+=step)
        {
            //Run for badbox->x .y
            int p_offset, pq_offset;
            double mean, variance_norm_factor=-1;
            // >= Second round: check detection result for selected feature
            int CurrResult=0;

            if(iy>PosSartY && iy<PosEndY && ix>PosSartX && ix<PosEndX)
                continue;
            if(TotalNegNum>=MAX_NEG)
                break;

            pq_offset = p_offset = iy*sumSize.width+ix; //offset of sum

            mean = calc_sum(*LearningModel,p_offset)*LearningModel->inv_window_area; //
        
            variance_norm_factor = LearningModel->pq0[pq_offset] - LearningModel->pq1[pq_offset] -
                                   LearningModel->pq2[pq_offset] + LearningModel->pq3[pq_offset];
            variance_norm_factor = variance_norm_factor*LearningModel->inv_window_area - mean*mean; //Mean(Square sum) - mean(sum) square
            
            if( variance_norm_factor >= 0. )
                variance_norm_factor = sqrt(variance_norm_factor);
            else
                variance_norm_factor = 1.;
            
            
            //for (j=0; j<selectedIndex.size(); j++)
            for (j=0; j<selectedIndex->total; j++)
            {   
                double HaarValue;
                MuHaarFeature *feature;
                int pos_j = (*(int*)muGetSeqElement(&selectedIndex, j+1));

                feature = &LearningModel->pool[pos_j].feature;
                HaarValue = calc_sum(feature->rect[0],p_offset) * feature->rect[0].weight;
                HaarValue += calc_sum(feature->rect[1],p_offset) * feature->rect[1].weight;

                if(LearningModel->pool[pos_j].rect_num == 3)
                    HaarValue += calc_sum(feature->rect[2],p_offset) * feature->rect[2].weight;

                HaarValue = (HaarValue*LearningModel->inv_window_area)/variance_norm_factor;

                if(LearningModel->pool[pos_j].polar==1)
                {
                    if(HaarValue <= LearningModel->pool[pos_j].threshold)
                        CurrResult=1;
                }
                else
                {
                    if(HaarValue >= LearningModel->pool[pos_j].threshold)
                        CurrResult=1;
                }
            }
            if(CurrResult==1)
                continue;

            for( j = 0; j < LearningModel->count; j++ )
            {
                double HaarValue;
                MuHaarFeature *feature = &LearningModel->pool[j].feature;
                HaarValue = calc_sum(feature->rect[0],p_offset) * feature->rect[0].weight;
                HaarValue += calc_sum(feature->rect[1],p_offset) * feature->rect[1].weight;
                
                if(LearningModel->pool[j].rect_num == 3)
                    HaarValue += calc_sum(feature->rect[2],p_offset) * feature->rect[2].weight;

                HaarValue = (HaarValue*LearningModel->inv_window_area)/variance_norm_factor;
                HaarValueNeg[TotalNegNum][j]=HaarValue;
                //HaarValueNegMean[j]+=HaarValue;
            }
            TotalNegNum++;
        }// End ix //End iy
        }// End if NegPic Activated

        if(TotalNegNum <=0 && NegPicActivated==0 )
        {
            setLearningImage(LearningModel, sum1, sqsum1, sumSize1);
            NegPicActivated = 1;
        }

        if( NegPicActivated==1 )
        {
            //setLearningImage(LearningModel, sum1, sqsum1, sumSize1);
            for(iy=0; iy<EndY; iy++)
            for(ix=0; ix<EndX; ix++)
            {
                //Run for badbox->x .y
                int p_offset, pq_offset;
                double mean, variance_norm_factor=-1;
                // >= Second round: check detection result for selected feature
                int CurrResult=0;
                //if(iy>PosSartY && iy<PosEndY && ix>PosSartX && ix<PosEndX)
                //    continue;
                if(TotalNegNum>=MAX_NEG)
                    break;

                pq_offset = p_offset = iy*sumSize.width+ix; //offset of sum

                mean = calc_sum(*LearningModel,p_offset)*LearningModel->inv_window_area; //
            
                variance_norm_factor = LearningModel->pq0[pq_offset] - LearningModel->pq1[pq_offset] -
                                       LearningModel->pq2[pq_offset] + LearningModel->pq3[pq_offset];
                variance_norm_factor = variance_norm_factor*LearningModel->inv_window_area - mean*mean; //Mean(Square sum) - mean(sum) square
                
                if( variance_norm_factor >= 0. )
                    variance_norm_factor = sqrt(variance_norm_factor);
                else
                    variance_norm_factor = 1.;
                
                
                //for (j=0; j<selectedIndex.size(); j++)
                for (j=0; j<selectedIndex->total; j++)
                {
                    double HaarValue;
                    int pos_j = (*(int*)muGetSeqElement(&selectedIndex, j+1));
                    MuHaarFeature *feature = &LearningModel->pool[pos_j].feature;
                    HaarValue = calc_sum(feature->rect[0],p_offset) * feature->rect[0].weight;
                    HaarValue += calc_sum(feature->rect[1],p_offset) * feature->rect[1].weight;

                    if(LearningModel->pool[pos_j].rect_num == 3)
                        HaarValue += calc_sum(feature->rect[2],p_offset) * feature->rect[2].weight;

                    HaarValue = (HaarValue*LearningModel->inv_window_area)/variance_norm_factor;

                    if(LearningModel->pool[pos_j].polar==1)
                    {
                        if(HaarValue <= LearningModel->pool[pos_j].threshold)
                            CurrResult=1;
                    }
                    else
                    {
                        if(HaarValue >= LearningModel->pool[pos_j].threshold)
                            CurrResult=1;
                    }
                }
                if(CurrResult==1)
                    continue;

                for( j = 0; j < LearningModel->count; j++ )
                {
                    double HaarValue;
                    MuHaarFeature *feature = &LearningModel->pool[j].feature;
                    HaarValue = calc_sum(feature->rect[0],p_offset) * feature->rect[0].weight;
                    HaarValue += calc_sum(feature->rect[1],p_offset) * feature->rect[1].weight;
                    
                    if(LearningModel->pool[j].rect_num == 3)
                        HaarValue += calc_sum(feature->rect[2],p_offset) * feature->rect[2].weight;

                    HaarValue = (HaarValue*LearningModel->inv_window_area)/variance_norm_factor;
                    HaarValueNeg[TotalNegNum][j]=HaarValue;
                }
                TotalNegNum++;
            }
        }
    } while(TotalNegNum>10 && selectedIndex->total<50);//while(TotalNegNum>0 && selectedIndex.size()<50);

    //write selected features into txt
    fp = fopen("MuDetector.txt", "w");
    if (fp== NULL) {
        printf("Error in opening a file..");
    }
    
    fprintf(fp,"%d %d\n",box->width,box->height);
    fprintf(fp,"%d\n",selectedIndex->total);

    for (i = 0;i<selectedIndex->total;i++)
    {
        int pos_i = (*(int*)muGetSeqElement(&selectedIndex, i+1));

        fprintf(fp,"%d\n",1);
        fprintf(fp,"%d\n",1);
        fprintf(fp,"%d\n",LearningModel->pool[pos_i].rect_num);
        for (rm = 0; rm <LearningModel->pool[pos_i].rect_num ;rm++ )
        {
            MuHaarFeature *feature = &(LearningModel->pool[pos_i].feature);
            fprintf(fp,"%d %d %d %d %f\n",feature->rect[rm].r.x, feature->rect[rm].r.y,feature->rect[rm].r.width,feature->rect[rm].r.height,feature->rect[rm].ori_weight);
        }
     
        fprintf(fp,"%d\n",0);
        //w-classifier's th
        fprintf(fp,"%lf\n",LearningModel->pool[pos_i].threshold);
        //left and right value
        if(LearningModel->pool[pos_i].polar==1)
        {
            fprintf(fp,"%lf\n",(float)0);
            fprintf(fp,"%lf\n",(float)1);
        }
        else
        {
            fprintf(fp,"%lf\n",(float)1);
            fprintf(fp,"%lf\n",(float)0);
        }

        //fprintf(fp,"%lf\n",(float)(selectedIndex.size()-0.5));
        fprintf(fp,"%lf\n",0.5);
        fprintf(fp,"-1\n");
        fprintf(fp,"-1\n");
    }// end for selectedIndex (selected weak-classifiers)
    
    fclose(fp);

    free (sum);
    free (sqsum);
    free(LearningModel);
    muClearSeq(&selectedIndex);
    muClearSeq(&poolIndex);
}


