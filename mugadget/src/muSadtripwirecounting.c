/* ------------------------------------------------------------------------- /
 *
 * Module: mu_sadtripwirecounting.c
 * Author: Joe Lin
 * Create date: 03/16/2010
 *
 * Description:
 *  This file is presented the SAD-based object counting and tripwire
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


MU_VOID crossproduction(HistoricalObject *po,  HistoryList *co, MU_8U *in, muLine_t line)
{

	MU_32S v1x,v1y,v2x,v2y,v3x,v3y;
	MU_32S t1x,t1y,t2x,t2y;
	MU_32S v1crossv2, v3crossv2;
	MU_32S o1, o2;

	t1x = line.start.x;
	t1y = line.start.y;
	t2x = line.end.x;
	t2y = line.end.y;

	v1x = po->centerx - t1x;
	v1y = po->centery - t1y;

	//printf("[MULIB] ho->centerx=%d centery=%d\n", po->centerx, po->centery);

	v2x = t2x - t1x;
	v2y = t2y - t1y;

	v3x = co->centerx - t1x;
	v3y = co->centery - t1y;

	//printf("[MULIB] hl->centerx = %d centery=%d\n", co->centerx, co->centery);

	//v1 cross v2
	v1crossv2 = (v1x*v2y) - (v2x*v1y);

	v3crossv2 = (v3x*v2y) - (v2x*v3y);

	//1 = in ; 2 = out;
	if( (t1x > t2x) || (t1y < t2y))
	{
		if(v3crossv2 < 0)
			*in = 1;
		else
			*in = 2;

		if((t1x > t2x) && (t1y > t2y))
		{
			if(v3crossv2 < 0)
				*in = 2;
			else
				*in = 1;
		}
	}
	else
	{
		if(v3crossv2 < 0)
			*in = 1;
		else
			*in = 2;
	}



	o1 = v1crossv2 * v3crossv2;

	v1x = t1x - co->centerx;
	v1y = t1y - co->centery;

	v2x = po->centerx - co->centerx;
	v2y = po->centery - co->centery;

	v3x = t2x - co->centerx;
	v3y = t2y - co->centery;

	v1crossv2 = (v1x*v2y) - (v2x*v1y);
	v3crossv2 = (v3x*v2y) - (v2x*v3y);

	o2 = v1crossv2 * v3crossv2;

	//printf("[MULIB] o1 = %d, o2 = %d\n", o1, o2);


	if(o1<0 && o2<0)
	{
		return;
	}
	else
	{
		*in=0;
		return;
	}

}


muError_t muTrackinglistDebug(muSeqBlock_t *first)
{
	HistoryList *hldata;
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

		//printf("label= %d nu= %d cx=%d cy=%d if=%d of=%d\n", hodata->label, hodata->numupdate, hodata->centerx, hodata->centery, hodata->crossflagin, hodata->crossflagout);

		hlseqhead = hodata->listhead;
		hlcurrent = hlseqhead->first;
		hlhead = hlcurrent;
		while(hlhead != NULL)
		{
			hlhead = hlhead->next;
			hldata = (HistoryList*)hlcurrent->data;

			printf("|\n");
			printf("frame= %d cx=%d cy=%d\n", hldata->frame, hldata->centerx, hldata->centery);

			hlcurrent = hlhead;
		}


		printf("#####\n\n");

		hocurrent = hohead;
	}

	return MU_ERR_SUCCESS;
}



//tripwire cross product&find direction
muError_t muSADTripwireCounting(muSeqBlock_t **first, muSADTripwireCounting_t *param)
{
	MU_8U in;
	MU_32S diff;
	MU_32S linenum;
	HistoryList *hldata;
	HistoricalObject *hodata;
	muSeqBlock_t *hohead, *hocurrent;
	muSeqBlock_t *hlhead, *hlcurrent;
	muSeq_t *hlseqhead;

	if( (*first) == NULL)
	{
		return MU_ERR_SUCCESS;
	}

	hohead = *first;

	hocurrent = hohead;

	linenum = param->linenum;

	while(hohead != NULL)
	{
		hohead = hohead->next;
		hodata = (HistoricalObject*)hocurrent->data;

		if(hodata->crossflagin[linenum]  == 1 && hodata->crossflagout[linenum]  == 1)
		{
			hocurrent = hohead;
			continue;
		}

		hlseqhead = hodata->listhead;
		hlcurrent = hlseqhead->first;
		hlhead = hlcurrent;

		diff = hodata->numupdate - param->dirsensitive;

		if(diff <= 0)
		{
			diff = 1;
		}


		while(hlhead != NULL)
		{

			hlhead = hlhead->next;

			hldata = (HistoryList*)hlcurrent->data;


			if((hldata->frame == diff && hodata->crossflagin[linenum] != 1) || (hldata->frame == diff && hodata->crossflagout[linenum] != 1) )
			{
				//cross production
				crossproduction(hodata,  hldata, &in, param->line);

				//printf("[MULIB] in=%d\n", in);

				if(param->way == MU_ONE_WAY)
				{
					if(in == 1)
					{
						if(hodata->crossflagin[linenum] != 1)
						{
							if(param->objectsize == 0)
							{
								param->incount++;
								hodata->crossflagin[linenum] = 1;
								hodata->crossflagout[linenum] = 0;
							}
							else
							{
								param->incount += ceil(hldata->area/(float)param->objectsize);
								hodata->crossflagin[linenum] = 1;
								hodata->crossflagout[linenum] = 0;
							}
						}

					}
					else if (in == 2)
					{
						if(hodata->crossflagout[linenum] != 1)
						{
							if(param->objectsize == 0)
							{
								param->outcount++;
								hodata->crossflagout[linenum] = 1;
								hodata->crossflagin[linenum] = 0;
							}
							else
							{
								param->outcount += ceil(hldata->area/(float)param->objectsize);
								hodata->crossflagout[linenum] = 1;
								hodata->crossflagin[linenum] = 0;
							}
						}
					}
				}
				else
				{
					if(in == 1)
					{
						if(hodata->crossflagin[linenum] != 1)
						{
							if(param->objectsize == 0)
							{
								param->incount++;
								hodata->crossflagin[linenum] = 1;
								hodata->crossflagout[linenum] = 0;
							}
							else
							{
								param->incount += ceil(hldata->area/(float)param->objectsize);
								hodata->crossflagin[linenum] = 1;
								hodata->crossflagout[linenum] = 0;
							}
						}

					}
					else if (in == 2)
					{
						if(hodata->crossflagout[linenum] != 1)
						{
							if(param->objectsize == 0)
							{
								param->incount++;
								hodata->crossflagout[linenum] = 1;
								hodata->crossflagin[linenum] = 0;
							}
							else
							{
								param->incount += ceil(hldata->area/(float)param->objectsize);
								hodata->crossflagout[linenum] = 1;
								hodata->crossflagin[linenum] = 0;
							}
						}
					}
				}

				break;
			}// if(hldata->frame == diff && hodata->crossflag != 1)
			hlcurrent = hlhead;
		}//while(hlhead != NULL)


		hocurrent = hohead;

	}//find direction end

	return MU_ERR_SUCCESS;

}
