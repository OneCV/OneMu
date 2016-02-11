/* ------------------------------------------------------------------------- /
 *
 * Module: mu_sadtrcking.c
 * Author: Nobel Hsu, Joe Lin
 * Create date: 02/25/2010
 *
 * Description:
 *  This file is presented the SAD-based object tracking
 *
 *  
 -------------------------------------------------------------------------- */

/* System include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/timeb.h>


/* MU include files */
#include "muGadget.h"
/* must declare after muGadget.h */
#include "sadtracking.h"



static muError_t SADFindThreshold(muImage_t * image, MU_8U offset, MU_8U *th)
{
    MU_16U data[256]={0}; 
	MU_8U *ptr;
	MU_32S i,j; 
	MU_32S diff;
    MU_32S imax = 0, max_level=0, max_level2=0, numpixel=0;
	MU_32F frate;
 
	if(image->depth != MU_IMG_DEPTH_8U) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}

	ptr = image->imagedata;


	i = image->width*image->height-1;
	do
	{
	data[*(ptr+i)]++;
	}while(i--);

	j = 255;
	do
	{
		if(data[j] > imax)
		{
		
			imax = data[j];
			max_level = j;
		}
		numpixel += data[j];
	}while(--j);


	//rank2, because it is depended on thresholding, rank2 must larger than rank1 
	imax = 0;
	for(j=max_level+1; j<255; j++)
	{	
	
		if(data[j] > imax)
		{
		
			imax = data[j];
			max_level2 = j;
		}

	}	

	if(numpixel == 0)
	{
		*th = 255;
	}
	
	diff = abs(max_level-max_level2);
	frate = (data[max_level]/(float)data[max_level2]);

	if(frate <= 2.5f && diff < 5)
	{
	*th = (max_level2+offset);
	}
	else
	{
	*th = (max_level+offset);	
	}
	
	return MU_ERR_SUCCESS;

}


MU_VOID muSADDeleteObject(muSeq_t *headseq)
{
	HistoricalObject *hodata;
	muSeqBlock_t *hohead, *hocurrent;
	muSeq_t *hlseq;

    if(headseq == NULL)
	return;

	hohead = headseq->first;
	hocurrent = hohead;
	
	while(hohead != NULL)
	{
		hohead = hohead->next;
		hodata = (HistoricalObject *)hocurrent->data;
		hlseq = hodata->listhead;		
		muClearSeq(&hlseq);
		hocurrent = hohead;
	}
	
	muClearSeq(&headseq);
	
	return;
}



static muError_t FindOverlapSize(HistoricalObject *B1, muBoundingBox_t *B2, MU_32S* overlapsize)
{	    
    MU_32S left, top, right, bottom;

	
	B1->maxx = B1->minx+B1->width;
	B1->maxy = B1->miny+B1->height;
	
	B2->maxx = B2->minx+B2->width;
	B2->maxy = B2->miny+B2->height;


    left   = MU_MAX( B1->minx, B2->minx );
    top    = MU_MAX( B1->miny, B2->miny );
    right  = MU_MIN( B1->maxx, B2->maxx );
    bottom = MU_MIN( B1->maxy, B2->maxy );

    if( right >= left && bottom >= top )
       *overlapsize = (right-left+1)*(bottom-top+1);
    else
       *overlapsize = 0;

    return MU_ERR_SUCCESS;
	
}

muError_t muCheckhistory(muSADTrackingParam_t **param)
{
	struct timeb nt;
	MU_32S diff;
	MU_32S flag = 1;
	time_t timetemp;
	HistoricalObject *hodata;
	muSeqBlock_t *hohead, *hocurrent;
	muSeq_t *hoseqhead, *hlseqhead;
	muSeqBlock_t *first, *next, *prev;
	//delete the no update object, delete the object when the differential time larger than 2
	if((*param)->historyhead == NULL || (*param)->historyhead->first == NULL)
	return MU_ERR_SUCCESS;


	hohead = (*param)->historyhead->first;
	hocurrent = hohead;
	
	while(hohead!=NULL)
	{
		hohead = hohead->next;
		hodata = (HistoricalObject *)hocurrent->data;
		hoseqhead = (*param)->historyhead;
			
		ftime(&nt);
		timetemp = hodata->update.time;			
		diff= (nt.time - timetemp);

	
        //printf("[VA_LIB]label=%d nt.time= %d diff=%d timeth=%d update time=%d\n", hodata->label, nt.time, diff, (*param)->timeth, hodata->update.time);

		if(diff >= (*param)->timeth)
		{

			hlseqhead = hodata->listhead;	
            //printf("[VA_LIB]delete list of seq\n");
			muClearSeq(&hlseqhead);

			if(hocurrent == (*param)->historyhead->first)
			{
				first = (*param)->historyhead->first;
				next = first->next;
				if(next == NULL && flag == 1)
				{
					//printf("next = NULL\n");
					//printf("[VA_LIB]delete whole buffer\n");
					muClearSeq(&hoseqhead);
					(*param)->historyhead->first = NULL;
					(*param)->historyhead = NULL;
 					(*param)->historylast = NULL;
					(*param)->label = 1;
					flag = 0;
				}
				else
				{
					(*param)->historyhead->first = next;
				}
				
								
			}
			else
			{
				prev = (*param)->historyhead->first;
				while(prev->next!=hocurrent)
				{
					prev = prev->next;
				}

				if(hocurrent->next == NULL)
				{
					(*param)->historylast = prev;
					prev->next = NULL;
				
				}
				else
				{
					prev->next = hocurrent->next;
			
				}
				
			}

			if(flag)
			{
			    //printf("[VA_LIB]delete hodata and hocurrent\n");
				free(hodata);
				free(hocurrent);
			}
		}

		hocurrent = hohead;
	
	}

	return MU_ERR_SUCCESS;

}

static muError_t trackinghistory( muSeq_t **seq, muSADTrackingParam_t **param)
{
	MU_32S overlapsize;
	MU_32S minx, miny, width, height;
	MU_32S i, j;
	struct timeb nt;

	muBoundingBox_t *bbdata;
	
	HistoryList *hldata;

	HistoricalObject *hodata;

	muSeqBlock_t *bbhead, *bbcurrent, *bbtemp;
	muSeqBlock_t *hoprev, *hohead, *hocurrent;
	muSeqBlock_t *hlprev, *hlhead, *hlcurrent;
	muSeq_t *hoseqhead, *hlseqhead;
	
	
	if((*seq) == NULL)
	{
		return MU_ERR_SUCCESS;
	}

	bbhead = (*seq)->first;


	if(((*param)->historyhead == NULL) && (bbhead != NULL))
	{

		hoseqhead = muCreateSeq(sizeof(HistoricalObject));

		bbcurrent = bbhead;


		while(bbhead != NULL)
		{
		
		bbhead = bbhead->next;
		
		hocurrent = muPushSeq(hoseqhead, NULL);

		hlseqhead = muCreateSeq(sizeof(HistoryList));
		hlcurrent = muPushSeq(hlseqhead, NULL);
	
	
		if((*param)->historyhead == NULL)
		{
			(*param)->historyhead = hoseqhead;
		}
		else
		{
			hoprev->next = hocurrent; 
		}

		hodata = (HistoricalObject *)hocurrent->data;
		hldata = (HistoryList *)hlcurrent->data;
		bbdata = (muBoundingBox_t *)bbcurrent->data;

		hodata->listhead = hlseqhead;
		hocurrent->next = NULL;
		hlcurrent->next = NULL;
		hodata->label = (*param)->label;
		ftime(&nt);
		hodata->numupdate = 1;
		hodata->updateflag = 0;

		for(j=0; j<MAX_ZONE; j++)
		{
		    hodata->tripzoneflag[j] = 0;
		}

		hldata->frame = hodata->numupdate;
		hodata->update.time = nt.time;
		hodata->update.millitm = nt.millitm;

		for(i=0; i<MAX_LINE; i++)
		{
		    hodata->crossflagin[i] = 0;
		    hodata->crossflagout[i] = 0;
		}

		hodata->minx = minx = hldata->minx = bbdata->minx;
		hodata->miny = miny = hldata->miny = bbdata->miny;
    	hodata->width = width = hldata->width = bbdata->width;
		hodata->height = height = hldata->height = bbdata->height;
		hodata->centerx = hldata->centerx = (minx+minx+width)>>1;
		hodata->centery = hldata->centery = (miny+miny+height)>>1;
		hldata->area = bbdata->area;
					
		bbcurrent = bbhead;
		(*param)->label++;
		hoprev = hocurrent;
		(*param)->historylast = hocurrent;

	

		}	
	
	}
	else if(((*param)->historyhead != NULL) && (bbhead != NULL))//compare each object which is the same with previous object.
	{
		bbtemp = bbhead;
		hohead = (*param)->historyhead->first;
		hocurrent = hohead;

		while(hohead != NULL)
		{
			hohead = hohead->next;
			hodata = (HistoricalObject *)hocurrent->data;
	
			bbhead = bbtemp;
			bbcurrent = bbhead;


			while(bbhead != NULL)
			{
				bbhead = bbhead->next;	

				bbdata = (muBoundingBox_t *)bbcurrent->data;
				

				if(bbdata->overlap==0)
				{
					
					FindOverlapSize(hodata, bbdata, &overlapsize);
										
					//overlap && no upadate in this loop
					if(overlapsize >0 && hodata->updateflag == 0)
					{

						hlseqhead = hodata->listhead;
						hlhead = hlseqhead->first;
					

					//find the latest address
						while(hlhead!=NULL)
						{
						hlprev = hlhead;
						hlhead = hlhead->next;
						
						}
				
					hlcurrent = muPushSeq(hlseqhead, NULL);

					hldata = (HistoryList *)hlcurrent->data;
					
					//printf("bbcurrent %d %d \n",  BBcurrent->minX, BBcurrent->minY);

					minx = hodata->minx = hldata->minx = bbdata->minx;
					miny = hodata->miny = hldata->miny = bbdata->miny;
					width = hodata->width = hldata->width = bbdata->width;
					height = hodata->height = hldata->height = bbdata->height;
					hodata->centerx = hldata->centerx = (minx+minx+width)>>1;
					hodata->centery = hldata->centery = (miny+miny+height)>>1;
					hldata->area = bbdata->area;
					hodata->numupdate++;
					hodata->updateflag = 1;
					hldata->frame = hodata->numupdate;
					ftime(&nt);
					hodata->update.time = nt.time;
					hodata->update.millitm = nt.millitm;
					hlcurrent->next = NULL;
					bbdata->label = hodata->label;
					
					hlprev->next = hlcurrent;

					bbdata->overlap = overlapsize;
					break;
					}

				}

				bbcurrent = bbhead;
			}
			hodata->updateflag = 0;
			hocurrent = hohead;


		}//while


		//non-overlap must create a new HO buffer
		bbhead = bbtemp;
		bbcurrent = bbhead;
		while(bbhead != NULL)
		{
			bbhead = bbhead->next;
			bbdata = (muBoundingBox_t *)bbcurrent->data;


			if(bbdata->overlap == 0)
			{
				hoseqhead = (*param)->historyhead;

				hocurrent = muPushSeq(hoseqhead, NULL);
				hodata = (HistoricalObject *)hocurrent->data;


				hlseqhead = muCreateSeq(sizeof(HistoryList));
				hlcurrent = muPushSeq(hlseqhead, NULL);
				hldata = (HistoryList *)hlcurrent->data;
				
				(*param)->historylast->next = hocurrent;

				hodata->listhead = hlseqhead;
				hocurrent->next = NULL;	
				hlcurrent->next = NULL;
				hodata->label = (*param)->label;
				hodata->numupdate=1;
				hodata->updateflag = 0;

				for(i=0; i<MAX_LINE; i++)
				{
				    hodata->crossflagin[i] = 0;
				    hodata->crossflagout[i] = 0;
				}

				for(j=0; i<MAX_ZONE; j++)
				{
				    hodata->tripzoneflag[j] = 0;
				}

				hldata->frame = hodata->numupdate;
				ftime(&nt);
				hodata->update.time = nt.time;
				hodata->update.millitm = nt.millitm;
	

				hodata->minx = minx = hldata->minx = bbdata->minx;
				hodata->miny = miny = hldata->miny = bbdata->miny;
    			hodata->width = width = hldata->width = bbdata->width;
				hodata->height = height = hldata->height = bbdata->height;
				hodata->centerx = hldata->centerx = (minx+minx+width)>>1;
				hodata->centery = hldata->centery = (miny+miny+height)>>1;
				hldata->area = bbdata->area;
				(*param)->label++;

				(*param)->historylast = hocurrent;
			}
			bbcurrent = bbhead;

		}

	}//else if

	return MU_ERR_SUCCESS;

}

/*

muError_t muTrackDebug(muSeq_t *seq)
{
	muSeqBlock_t *bbhead, *bbcurrent;
	MU_32S i,j;
	muBoundingBox_t *bbdata;


	if(seq == NULL)
	{
		return MU_ERR_SUCCESS;
	}

	bbhead = seq->first;

	bbcurrent  = bbhead;

	
	while(bbhead != NULL)
	{
		bbhead = bbhead->next;
		bbdata = (muBoundingBox_t *)bbcurrent->data;
		
		for(i=bbdata->minx; i<bbdata->maxx+1; i++)
			for(j=bbdata->miny; j<bbdata->maxy+1; j++)
			{
				 //thresholding
			}
			
		bbcurrent = bbhead;
	}

	return MU_ERR_SUCCESS;

}
*/


muError_t muSADTracking( muImage_t *src, muImage_t *dst, muSADTrackingParam_t *param)
{
	MU_8U th, numlabel;
	muSeq_t *seqhead;
	muError_t ret;
	muDoubleThreshold_t dth;
	muImage_t *psrc, *pdst;


	if((src->depth != MU_IMG_DEPTH_8U) && (dst->depth != MU_IMG_DEPTH_8U)) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}


	if((param->sadsensitive < 0) || (param->th.min < 0) || (param->th.max < 0))
	{
		return MU_ERR_INVALID_PARAMETER;
	}


    psrc = src;
	pdst = dst;

#if 0
	ret = muContraststretching(src, dst, 255);
	if(ret != MU_ERR_SUCCESS)
	{
		muDebugError(ret);
	}
#endif
    
  
	ret = SADFindThreshold(psrc, param->sadsensitive, &th);
	if(ret != MU_ERR_SUCCESS)
	{
		muDebugError(ret);
	}
	
	dth.min = th;
	dth.max = 255;

	ret = muThresholding(psrc, pdst, dth);
	if(ret != MU_ERR_SUCCESS)
	{
		muDebugError(ret);
	}

	
	ret = muSetZero(psrc);
	if(ret != MU_ERR_SUCCESS)
	{
		muDebugError(ret);
	}

	switch (param->mode)
	{
	   case MU_HEADCOUNTING:
	   //closing55
	   ret = muDilate55(pdst, psrc);
	   if(ret != MU_ERR_SUCCESS)
	   {
		muDebugError(ret);
	   }

	   ret = muSetZero(pdst);
	   if(ret != MU_ERR_SUCCESS)
	   {
		muDebugError(ret);
	   }

	   ret = muErode55(psrc, pdst);
	   if(ret != MU_ERR_SUCCESS)
	   {
		muDebugError(ret);
	   }

	   ret = muSetZero(psrc);
	   if(ret != MU_ERR_SUCCESS)
	   {
		muDebugError(ret);
	   }
	   break;

	   case MU_TRIPWIRE:
	   //closing33
	   ret = muDilate33(pdst, psrc);
	   if(ret != MU_ERR_SUCCESS)
	   {
		muDebugError(ret);
	   }

	   ret = muSetZero(pdst);
	   if(ret != MU_ERR_SUCCESS)
	   {
		muDebugError(ret);
	   }

	   ret = muErode33(psrc, pdst);
	   if(ret != MU_ERR_SUCCESS)
	   {
		muDebugError(ret);
	   }

	   ret = muSetZero(psrc);
	   if(ret != MU_ERR_SUCCESS)
	   {
		muDebugError(ret);
	   }
	   break;
	}


	//opening
	ret = muErode33(pdst, psrc);
	if(ret != MU_ERR_SUCCESS)
	{
		muDebugError(ret);
	}

    ret = muSetZero(pdst);
	if(ret != MU_ERR_SUCCESS)
	{
		muDebugError(ret);
	}

	ret = muDilate33(psrc, pdst);
	if(ret != MU_ERR_SUCCESS)
	{
		muDebugError(ret);
	}

	ret = muSetZero(psrc);
	if(ret != MU_ERR_SUCCESS)
	{
		muDebugError(ret);
	}

	ret = mu4ConnectedComponent8u(pdst, psrc, &numlabel);
	if(ret != MU_ERR_SUCCESS)
	{
		muDebugError(ret);
	}


	seqhead = muFindBoundingBox(psrc, numlabel, param->th);


	ret = trackinghistory( &seqhead, &param);
	if(ret != MU_ERR_SUCCESS)
	{
		muDebugError(ret);
	}

#if 1
    //delete no update objects
    ret = muCheckhistory(&param);
	if(ret != MU_ERR_SUCCESS)
	{
	    muDebugError(ret);
	}
#endif

	muClearSeq(&seqhead);
	return MU_ERR_SUCCESS;
}
