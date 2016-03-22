/* ------------------------------------------------------------------------- /
 *
 * Module: muExaminator.c
 * Author: Chao-Ting Hong
 * Create date: 01/03/2015
 *
 * Description:
 * This file is presented object detector with haar-cascade
 *  
 -------------------------------------------------------------------------- */
#include <stdio.h>
#include "muGadget.h"

/**Function**/
/**Merge Function**/
/*MergeObjDistTH: OverlapTH - 2 means 1/2, 3 means 1/3*/
/*HitNum: TH for number of merged blocks*/
void muMergeRectangles(muSeq_t *Rectangles, int MergeObjDistTH, int HitNum)
{
    int i, j, MergedNum;
    int CrossArea, AreaMinX, AreaMaxX, AreaMinY, AreaMaxY;
    int RecMaxX1, RecMaxX2, RecMinX1, RecMinX2;
    int RecMaxY1, RecMaxY2, RecMinY1, RecMinY2;
    long X, Y, Wid, Hei;
    muSeqBlock_t *current1, *current2, *tmpBlock=NULL;
    muRect_t *rectp1, *rectp2;

    if(Rectangles != NULL)
	{
		current1 = Rectangles->first;
		while(current1 != NULL)
		{
			rectp1 = (muRect_t *)current1->data;
			X = RecMinX1 = rectp1->x;
			Y = RecMinY1 = rectp1->y;
			Wid = rectp1->width;
			Hei = rectp1->height;
			RecMaxX1 = RecMinX1 + Wid;
			RecMaxY1 = RecMinY1 + Hei;
			MergedNum = 1;
			current2 = current1->next;
			while(current2 != NULL)
			{
				rectp2 = (muRect_t *)current2->data;
				RecMinX2 = rectp2->x;
				RecMinY2 = rectp2->y;
				RecMaxX2 = RecMinX2 + rectp2->width;
				RecMaxY2 = RecMinY2 + rectp2->height;
				tmpBlock=NULL;
				//Sum of x, y, w, h; if overlapped
				if(!(RecMaxX2<=RecMinX1 || RecMinX2>=RecMaxX1 ||
                 RecMaxY2<=RecMinY1 || RecMinY2>=RecMaxY1 ))
	            {
	                AreaMinX = RecMinX2 > RecMinX1 ? RecMinX2 : RecMinX1;
	                AreaMaxX = RecMaxX2 < RecMaxX1 ? RecMaxX2 : RecMaxX1;
	                AreaMinY = RecMinY2 > RecMinY1 ? RecMinY2 : RecMinY1;
	                AreaMaxY = RecMaxY2 < RecMaxY1 ? RecMaxY2 : RecMaxY1;
	                CrossArea = (AreaMaxX - AreaMinX)*(AreaMaxY - AreaMinY);
	                if(CrossArea!=0)
	                if( rectp1->width*rectp1->height<MergeObjDistTH*CrossArea || rectp2->width*rectp2->height<MergeObjDistTH*CrossArea)
	                {
	                    /*rectp1->x = (rectp1->x + rectp2->x)/2;
	                    rectp1->y = (rectp1->y + rectp2->y)/2;
	                    rectp1->width = (rectp1->width + rectp2->width)/2;
	                    rectp1->height = (rectp1->height + rectp2->height)/2;*/
	                    X+=rectp2->x;
	                    Y+=rectp2->y;
	                    Wid+=rectp2->width;
	                    Hei+=rectp2->height;
	                    MergedNum++;
	                    tmpBlock = current2;
	                }
	            }
				current2 = current2->next;
				if(tmpBlock!=NULL)
					muRemoveAddressNode(&Rectangles, tmpBlock);
			}

			tmpBlock = current1;
			current1 = current1->next;
			//save mean of rect to current1 data
			if(MergedNum > HitNum)
			{
				rectp1->x = X/MergedNum;
				rectp1->y = Y/MergedNum;
				rectp1->width = Wid/MergedNum;
				rectp1->height = Hei/MergedNum;
			}
			else
				muRemoveAddressNode(&Rectangles, tmpBlock);
		}
	}
}

/*Tracking Objects' Rectangles*/
void muTrackRectangles(muSeq_t *Objects, muSeq_t *Trackers)
{
	int i, j, index;
	int InheritFlag;
	int CrossArea, X1, X2, Y1, Y2;
	muSeqBlock_t *current1, *current2, *tmpBlock=NULL;
    muRect_t *rectp;
    MuTracker *tracp, tractmp;

	if(Trackers != NULL)
	{
		//Reset trackers' detectflag
		current1 = Trackers->first;
		while(current1 != NULL)
		{
			//rectp1 = (muRect_t *)current1->data;
			tracp = (MuTracker *)current1->data;
			tracp->detected = 0;
			current1 = current1->next;
		}
		
		if(Objects != NULL)
		{
			//For detected objects
			current1 = Objects->first;
			while(current1!=NULL)
			{
			    rectp = (muRect_t *)current1->data;
			    InheritFlag = 0;
			    
			    //For trackers
			    current2 = Trackers->first;
			    while(current2!=NULL)
			    {
			    	tracp = (MuTracker *)current2->data;
			    	if(tracp->detected == 0 
	                   && abs(tracp->y - rectp->y) < tracp->height/2
	                   && abs(tracp->x - rectp->x) < tracp->width/2)
			    	{
			    		tracp->x = rectp->x;
			    		tracp->y = rectp->y;
			    	    tracp->width = rectp->width;
			    	    tracp->height = rectp->height;
			    	    tracp->detected = 1;
			    	    if(tracp->life < 3) tracp->life++;
			    	    if(tracp->check < 5) tracp->check++;
			    	    InheritFlag = 1;
			    	}
			    	current2 = current2->next;
			    }

			    //If no tracker tracks this object, new a tracker
			    if(InheritFlag==0)
			    {
			    	tractmp.x = rectp->x;
			    	tractmp.y = rectp->y;
			    	tractmp.width = rectp->width;
			    	tractmp.height = rectp->height;
			    	tractmp.detected = 1;
			    	tractmp.check = 1;
			    	tractmp.life = 1;
			    	muPushSeq(Trackers, (MU_VOID *)&tractmp);
			    }

			 	current1 = current1->next;
			}
		}

		//For non-detected trackers, life--; if life<=0, delete;
		current1 = Trackers->first;
		while(current1 != NULL)
		{
			//rectp1 = (muRect_t *)current1->data;
			tracp = (MuTracker *)current1->data;
			if(tracp->detected==0)
			{
				tracp->life--;
				tmpBlock = current1;
			}
			
			current1 = current1->next;
			if(tracp->life<=0)
				muRemoveAddressNode(&Trackers, tmpBlock);
		}
	}
}

void ExampleExaminatorMaker()
{
	double CascadeParaTable0[64] = {40, 25, 2, 1, 1, 3, 9, 3, 6, 16, -1, 9, 3, 3, 8, 2, 12, 11, 3, 8, 2, 0, 0.047854, -1.000000, 0.994061, 0.994061, -1, -1, 2, 1, 2, 4, 0, 28, 20, -1, 4, 5, 28, 10, 2, 0, 0.252720, -1.000000, 0.996030, 1, 2, 16, 10, 8, 8, -1, 16, 14, 8, 4, 2, 0, -0.042733, 0.992085, -0.999971, 1.988115, 0, -1};
	double CascadeParaTable1[68] = {32, 16, 3, 1, 1, 2, 4, 7, 21, 7, -1, 11, 7, 7, 7, 3, 0, 0.272483, -1.000000, 0.986163, 0.986163, -1, -1, 1, 1, 2, 2, 0, 10, 8, -1, 2, 2, 10, 4, 2, 0, 0.079174, -1.000000, 0.992075, 0.992075, 0, -1, 1, 1, 3, 13, 10, 6, 6, -1, 13, 10, 3, 3, 2, 16, 13, 3, 3, 2, 0, -0.018457, 1.000000, -0.999924, 1.000000, 1, -1};
	double CascadeParaTable2[91] = {10, 15, 2, 1, 1, 2, 2, 0, 5, 2, -1, 2, 1, 5, 1, 2, 0, 0.136821, -1.000000, 0.991413, 0.991413, -1, -1, 4, 1, 2, 0, 0, 6, 15, -1, 2, 0, 2, 15, 3, 0, 0.826638, -1.000000, 0.998678, 1, 2, 7, 13, 2, 2, -1, 7, 14, 2, 1, 2, 0, -0.003292, 1.000000, -0.942793, 1, 2, 0, 10, 4, 3, -1, 2, 10, 2, 3, 2, 0, 0.158385, -1.000000, 0.994891, 1, 2, 7, 11, 2, 4, -1, 7, 12, 2, 2, 2, 0, -0.001889, 1.000000, -0.963244, 2.030325, 0, -1};
	FILE *ptr_myfile;
	MuExamData ExamData;

	ExamData.ID = 14129;
	ExamData.ScanROI.x = 100;
	ExamData.ScanROI.y = 50;
	ExamData.ScanROI.width = 400;
	ExamData.ScanROI.height = 300;
	ExamData.TagNum = 2;
	ExamData.DetectorSize[0] = 64;
	ExamData.DetectorSize[1] = 68;
	ExamData.DetectorSize[2] = 91;

	//Set Tag Info
	ExamData.Tag[0].x = 250;
	ExamData.Tag[0].y = 40;
	ExamData.Tag[0].width = 95; //40 25
	ExamData.Tag[0].height = 60;

	ExamData.Tag[1].x = 350;
	ExamData.Tag[1].y = 270;
	ExamData.Tag[1].width = 60; //32 16
	ExamData.Tag[1].height = 30;

	ptr_myfile=fopen("Examinator.dk","wb");
	if (!ptr_myfile)
	{
		printf("Unable to open file!");
		return;
	}

	//Write Header
	fwrite(&ExamData, sizeof(struct MuExamData), 1, ptr_myfile);

	//Write Tables
	fwrite(CascadeParaTable0, sizeof(double)*ExamData.DetectorSize[0], 1, ptr_myfile);
	fwrite(CascadeParaTable1, sizeof(double)*ExamData.DetectorSize[1], 1, ptr_myfile);
	fwrite(CascadeParaTable2, sizeof(double)*ExamData.DetectorSize[0], 1, ptr_myfile);
	
	fclose(ptr_myfile);
}

void Examinator_Init_Buf(MU_8U *buf, MuExaminator *Examinator)
{
	MuExamData ExamData;
	double CascadeParaTable[500];
	MU_8U *buftmp = buf;
	int i;

	//Read ExamData (FileID, Tags Info, Number of Tables)
	memcpy(&(Examinator->ExamData),buftmp,sizeof(struct MuExamData));
	buftmp+=sizeof(struct MuExamData);

    //Load Tables & init detector
    
	for(i=0; i<Examinator->ExamData.TagNum; i++)
	{
		memcpy(CascadeParaTable,buftmp,sizeof(double)*(Examinator->ExamData.DetectorSize[i]));
        muObjectDetectionInit( &(Examinator->Detector[i].Cascade), Examinator->Detector[i].CascadeStages, Examinator->Detector[i].CascadeClassifiers, CascadeParaTable);
        Examinator->Detector[i].Objects=NULL;
        Examinator->Detector[i].Status.State = 0;
        Examinator->Detector[i].Status.Trigger = 0;
        buftmp+=sizeof(double)*(Examinator->ExamData.DetectorSize[i]);
	}

    //Init tracker
    for(i=0; i<Examinator->ExamData.TagNum; i++)
        Examinator->Detector[i].Tracks = muCreateSeq(sizeof(MuTracker));

    //Set scan bar (default: in the middle of scream)
    Examinator->ScanBar = muRect(Examinator->ExamData.Tag[0].x+Examinator->ExamData.Tag[0].width/2-5, 0, 10, 480);

    //Scale

    //Scan Range ROI

    //Advance - Integral Images' Setting
    
}

void Examinator_Init(FILE *fp, MuExaminator *Examinator)
{
	FILE *ptr_myfile;
	MuExamData ExamData;
	double CascadeParaTable[500];
	int i;

	ptr_myfile=fopen("Examinator.dk","rb");
	if (!ptr_myfile)
	{
		printf("Unable to open file!");
		return;
	}

	//Read ExamData (FileID, Tags Info, Number of Tables)
	fread(&(Examinator->ExamData),sizeof(struct MuExamData),1,ptr_myfile);

	printf("tagNum = %d\n", Examinator->ExamData.TagNum);

    //Load Tables & init detector
	for(i=0; i<Examinator->ExamData.TagNum; i++)
	{
		fread(CascadeParaTable,sizeof(double)*(Examinator->ExamData.DetectorSize[i]),1,ptr_myfile);
        muObjectDetectionInit( &(Examinator->Detector[i].Cascade), Examinator->Detector[i].CascadeStages, Examinator->Detector[i].CascadeClassifiers, CascadeParaTable);
        Examinator->Detector[i].Objects=NULL;
        Examinator->Detector[i].Status.State = 0;
        Examinator->Detector[i].Status.Trigger = 0;
	}
    fclose(ptr_myfile);

    //Init tracker
    for(i=0; i<Examinator->ExamData.TagNum; i++)
        Examinator->Detector[i].Tracks = muCreateSeq(sizeof(MuTracker));

    //Set scan bar (default: in the middle of scream)
    Examinator->ScanBar = muRect(Examinator->ExamData.Tag[0].x+Examinator->ExamData.Tag[0].width/2-5, 0, 10, 480);
    
    //Scale

    //Scan Range ROI

    //Advance - Integral Images' Setting
    
}

void Examinator_Run(muImage_t *src, MuExaminator *Examinator)
{
	//For Run detectors
	muSize_t min, max;
	muSize_t imgSize;

	//For Check Mark
	muSeq_t *Trackers;
	MuTracker *tracp;
	muSeqBlock_t *current;

	//For Scan
	unsigned char scanflag;
	int i; //For fors

	//Create and calculate integral img
	Examinator->Itlmg = muIntegral_Light(src);
	
	//Run cascase detectors
	for(i=0;i<Examinator->ExamData.TagNum;i++)
	{
		//Detector para
		min.width = Examinator->ExamData.Tag[i].width;
		min.height = Examinator->ExamData.Tag[i].height;
		max.width = (double)min.width*1.1;
		max.height = (double)min.height*1.1;
		
		//Initialize object sequences
		if(Examinator->Detector[i].Objects!=NULL)
		{
		    muClearSeq(&(Examinator->Detector[i].Objects));
		    Examinator->Detector[i].Objects=NULL;
		}
		Examinator->Detector[i].Objects = muCreateSeq(sizeof(CtRect));

		//muObjectDetection_Light
		//muObjectDetection_Light(Examinator->Itlmg, Examinator->ExamData.ScanROI, Examinator->Detector[i].Objects, &(Examinator->Detector[i].Cascade), 1.1, min, max);
		muObjectDetection_SuperLight(Examinator->Itlmg, Examinator->ExamData.ScanROI, Examinator->Detector[i].Objects, &(Examinator->Detector[i].Cascade), min);
		//Merge and Track detection results
		muMergeRectangles(Examinator->Detector[i].Objects, 2, 2);
		muTrackRectangles(Examinator->Detector[i].Objects, Examinator->Detector[i].Tracks);
	}
	//Release integral img
	muIntegral_LightRelease(Examinator->Itlmg);

	//Check Mark status with scan line//
	scanflag = 0;
	Examinator->Detector[0].Status.Trigger = 0;
	Trackers = Examinator->Detector[0].Tracks;
	//Check if Mark getin scan line
	if(Trackers != NULL)
	{
		//Reset trackers' detectflag
		current = Trackers->first;
		while(current != NULL)
		{
			tracp = (MuTracker *)current->data;
			//tracp->detected tracp->check;

			//if stable
			if(tracp->check>1)
			{
				//check in line for default vertical scan bar
				if(abs(tracp->y+tracp->height/2-Examinator->ScanBar.y-Examinator->ScanBar.height/2) < (tracp->height + Examinator->ScanBar.height)/2
	               && abs(tracp->x+tracp->width/2-Examinator->ScanBar.x-Examinator->ScanBar.width/2) < (tracp->width + Examinator->ScanBar.width)/2)
				{
					if(Examinator->Detector[0].Status.State == 0)
					{
					    Examinator->Detector[0].Status.State = 1;
					    Examinator->Detector[0].Status.Trigger = 1;
					}
					scanflag = 1;
				}
			}
			current = current->next;
		}
	}
	//Check if Mark leave scan line
	if(scanflag==0)
	{
		if(Examinator->Detector[0].Status.State == 1)
		{
		    Examinator->Detector[0].Status.State = 0;
		    Examinator->Detector[0].Status.Trigger = 1;
		}
	}

	//Init tag check list, when mark get into scanline//
	if(Examinator->Detector[0].Status.State == 1 && 
		Examinator->Detector[0].Status.Trigger == 1)
	{
		for(i=0;i<Examinator->ExamData.TagNum;i++)
		{
			Examinator->Detector[i].Checked=0;
		    Examinator->Detector[i].HitNum=0;
		}
	}

	//Update Check List. Checked up, if detected and tracked three times.
	//If detected + tracked -> HitNum++. If HitNum > 2 -> Checked = True.
	if(Examinator->Detector[0].Status.State == 1 )
	{
		for(i=0;i<Examinator->ExamData.TagNum;i++)
	    {
			scanflag = 0;
			Trackers = Examinator->Detector[i].Tracks;
			if(Trackers != NULL)
			{
				//Reset trackers' detectflag
				current = Trackers->first;
				while(current != NULL)
				{
					//rectp1 = (muRect_t *)current->data;
					tracp = (MuTracker *)current->data;

					//if stable & detected
					if(tracp->check>1 && tracp->detected==1)
						scanflag = 1;

					current = current->next;
				}
			}
			if(scanflag==1)		
		        Examinator->Detector[i].HitNum++;
	    }
	}

	//Send check result, when mark leave scanline
	if(Examinator->Detector[0].Status.State == 0 && 
		Examinator->Detector[0].Status.Trigger == 1)
	{
		for(i=0;i<Examinator->ExamData.TagNum;i++)
			if(Examinator->Detector[i].HitNum>2)
				Examinator->Detector[i].Checked = 1;
	}

	//Advance - Integral Image Calculation

	//Advance - Detection with calculated integral images

}

void Examinator_Release(MuExaminator *Examinator)
{
	int i;
	for(i=0; i<Examinator->ExamData.TagNum; i++)
	{
        muClearSeq(&Examinator->Detector[i].Tracks);
		if(Examinator->Detector[i].Objects!=NULL)
		{
		    muClearSeq(&(Examinator->Detector[i].Objects));
		    Examinator->Detector[i].Objects=NULL;
		}
    }
}

void Examinator_Teach(MuExamData *Data)
{

}
