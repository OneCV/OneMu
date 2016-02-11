
#define MAX_ZONE    5
#define MAX_LINE    5

typedef struct _HistoryList
{
	MU_16U width, height;
	MU_32S minx, miny, maxx, maxy;
	MU_32S frame, area;
	MU_32U centerx, centery;

}HistoryList;


typedef struct _HistoricalObject
{
    MU_8U  crossflagin[MAX_LINE], crossflagout[MAX_LINE];
	MU_8U  tripzoneflag[MAX_ZONE];
    MU_32S label, updateflag;
	struct timeb update;
	MU_32S numupdate;
	MU_32S minx, miny, maxx, maxy;
	MU_32S numofobject;
	MU_32U centerx, centery;
    MU_16U width, height;
	muSeq_t *listhead;

}HistoricalObject;

