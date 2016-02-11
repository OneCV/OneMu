/* ------------------------------------------------------------------------- /
 *
 * Module: mu_sadtripzone.c
 * Author: Nobel Hsu, Joe Lin
 * Create date: 03/16/2010
 *
 * Description:
 *  This file is presented the SAD-based tripzone
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


muError_t FindOverlapZone(HistoricalObject *B1, muSADTripZone_t *B2, MU_32S *overlapsize)
{	    
	MU_32S left, top, right, bottom;
	MU_32S b2maxx, b2maxy;



	B1->maxx = B1->minx+B1->width;
	B1->maxy = B1->miny+B1->height;


	b2maxx = B2->rect.x+B2->rect.width;
	b2maxy = B2->rect.y+B2->rect.height;


	left   = MU_MAX( B1->minx, B2->rect.x );
	top    = MU_MAX( B1->miny, B2->rect.y );
	right  = MU_MIN( B1->maxx, b2maxx );
	bottom = MU_MIN( B1->maxy, b2maxy );

	if( right >= left && bottom >= top )
		*overlapsize = (right-left+1)*(bottom-top+1);
	else
		*overlapsize = 0;

	return MU_ERR_SUCCESS;

}


muError_t muSADTripZone(muSeqBlock_t *first, muSADTripZone_t *param)
{
	MU_32S overlapsize;
	MU_32S zonenum;
	HistoricalObject *hodata;
	muSeqBlock_t *hohead, *hocurrent;
	muSeqBlock_t *hlhead, *hlcurrent;
	muSeq_t *hlseqhead;


	if(first == NULL)
	{
		return MU_ERR_SUCCESS;
	}

	hohead = first;
	hocurrent = hohead;

	while(hohead != NULL)
	{
		hohead = hohead->next;
		hodata = (HistoricalObject*)hocurrent->data;
		zonenum = param->zonenum;

		hlseqhead = hodata->listhead;
		hlcurrent = hlseqhead->first;
		hlhead = hlcurrent;

		if(hodata->numupdate >= param->updateth && hlhead != NULL)
		{
			FindOverlapZone(hodata, param, &overlapsize);
			if(overlapsize > 0)
			{
				param->alarm = 1;
			}
		}


		hocurrent = hohead;
	}


	return MU_ERR_SUCCESS;
}
