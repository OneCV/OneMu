/* ------------------------------------------------------------------------- /
 *
 * Module: muThreshold.c
 * Author: Joe Lin
 *
 * Description:
 *    OtsuThresholding.
 *
 -------------------------------------------------------------------------- */
#include "muCore.h"

typedef struct _Link{

		struct _Link *add;
		int data;
}Link;

typedef struct _GrayScale{
		
		Link *firstptr;
		int count;
}GrayScale;

typedef struct _Otsuparameter{
		
		int WB;
		int WO;
		int MeanB;
		int MeanO;
}Otsuparameter;

/* Test on 400MHZ SOC which cost 0.8s for D1 resolution */
muError_t muOtsuThresholding(const muImage_t * src, muImage_t * dst)
{
	int idx = 0, jdx = 0;
	int hidx = 0, widx = 0;
	float BeVar = 0, TempVar = 0;
	int FixedBeVar = 0, FixedTempVar = 0;
	int Candidateflag = 0;// initially set the threshold = 0; 
	short thidx = 0;
	float tempdenominator = 0, tempdenominator2 = 0;
	int outidx = 0, Idxnum = 0;
	int ohidx = 0, owidx = 0;
	int iheight;
	int iwidth; 
	int piwidth;
	int totalsize;
	int **InData;
	int *In;
	int *OutData;
	int inidx = 0;
	unsigned char *in, *out; 

	GrayScale graynum[256] = {NULL, 0};

	Link *temp[256] ={NULL, 0}, *current, *buffer;

	Otsuparameter Thres[256] = {0, 0, 0, 0};
	
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	iwidth = src->width;

	iheight = src->height;

	in = src->imagedata;

	out = dst->imagedata;

	//piwidth = Image1->widthStep;

	totalsize = ( iheight * iwidth );

	/***memory allcoate of the input data***/

	InData = (int **) malloc( (iheight) * sizeof(void *) );

	In = (int *) malloc((iheight) * (iwidth) * sizeof(int ) );

	for( inidx = 0; inidx < (iheight); inidx++, In += (iwidth) )
		InData[ inidx ] = In;

	/**************************************/

	/***memory allcoate of the input data***/
	
	OutData = (int *) calloc( iheight * iwidth, sizeof(int) );

	/**************************************/

		/*!
 ****************************************************************************************************************
 * \brief
 *    construt the Histogram parameter (Statistical number). 
 *    
 *    in: image data InData[height][width]          out: graynum[0].count: the total number of ro,
 *                                                      graynum[1].count: the total number of r1,
 *                                                                        .
 *                                                                        .
 *                                                                        .
 *                                                      graynum[255].ocunt: the total number of r255
 *                                                      and the graynum[num].firstptr is the first node 
 *                                                      which could use the link visitation to search each positin
 *                                                      of gray scale num.
 *****************************************************************************************************************
*/
	for( idx = 0; idx < (iheight); idx++)
	{
		for(jdx = 0; jdx < (iwidth); jdx++)
			InData[ idx ][ jdx ] = (int) in[ idx * (iwidth) + jdx ];
	}

	for(hidx = 0; hidx < (iheight); hidx++)//record the data and location of each pixel in whole image
	{
		for(widx = 0; widx < (iwidth); widx++)
		{
			graynum[ InData[hidx][widx] ].count += 1;

			current = ( Link * ) malloc( sizeof(Link) );

			current->data = ( hidx * (iwidth) ) + widx;

			if( graynum[ InData[hidx][widx] ].firstptr == NULL )
				graynum[ InData[hidx][widx] ].firstptr = current;

			else
				temp[ InData[hidx][widx] ]->add = current;
			
			current->add = NULL;

			temp[ InData[hidx][widx] ] = current;
				
		}
		
	}

	free(InData[0]);

	free(InData);

	/*!
 ****************************************************************************************************************
 * \brief
 *    Find the threshold of the Otsu Algorithm.
 *    
 *    WB( T + 1) = WB( T ) + P( T + 1)    MeanB( T + 1 ) = [ MeanB( T ) WB( T ) + ( T + 1) P( T + 1) ] / WB( T + 1)
 *
 *    WO( T + 1) = WO( T ) + P( T + 1)    MeanO( T + 1 ) = [ MeanO( T ) WO( T ) + ( T + 1) P( T + 1) ] / WO( T + 1)
 *
 *    in: graynum[0].count                             out: Maximum between variance BeVar.
 *        graynum[1].count                                             
 *                .                                         The Cadidateflag = the chosen threshold.                
 *                .                                                        
 *                .                                                        
 *        graynum[255].count                                                                                           
 *****************************************************************************************************************
*/
	//set the initial value T = 0 of the otsu parameter.
	
	Thres[0].WB = graynum[0].count;

	Thres[0].WO = totalsize - graynum[0].count;

	Thres[0].MeanB = 0;

	for( idx = 1; idx < 256; idx++)
		Thres[0].MeanO +=( (graynum[idx].count) * idx );

	 /*** avoid the denominator being 0. ***/

	if( Thres[0].WB == 0)
		tempdenominator = 0.0000000001;
	else
		tempdenominator = (float)Thres[0].WB;

	if( Thres[0].WO == 0)
		tempdenominator2 = 0.0000000001;
	else
		tempdenominator2 = (float)Thres[0].WO;

	/***************************************/

	BeVar = ( ( (float)Thres[0].WO / tempdenominator ) * Thres[0].MeanB * Thres[0].MeanB )// calculate and set the maximum between variance. 
	    
	    - (2 * (float)Thres[0].MeanB * Thres[0].MeanO)
	         
		+  ( ( (float)Thres[0].WB / tempdenominator2 ) * Thres[0].MeanO * Thres[0].MeanO);

		for( thidx = 1; thidx < 256; thidx++)//perform in each threshold and find the correspoding threshold.
	{
		Thres[ thidx ].WB = Thres[ thidx -1 ].WB + graynum[ thidx ].count;

		Thres[ thidx ].MeanB = Thres[ thidx-1 ].MeanB + ( thidx * graynum[ thidx ].count);

		Thres[ thidx ].WO = Thres[ thidx -1 ].WO - graynum[ thidx ].count;

		Thres[ thidx ].MeanO = Thres[ thidx-1 ].MeanO - ( thidx * graynum[ thidx ].count);

		if( Thres[thidx].WB == 0)
			tempdenominator = 0.0000000001;

		else
			tempdenominator = (float)Thres[thidx].WB;

		if( Thres[thidx].WO == 0)
			tempdenominator2 = 0.0000000001;

		else
			tempdenominator2 = (float)Thres[thidx].WO;

		TempVar = ( ( (float)Thres[ thidx ].WO / tempdenominator ) * Thres[ thidx ].MeanB * Thres[ thidx ].MeanB ) 
		    
		    - (2 * (float)Thres[ thidx ].MeanB * Thres[ thidx ].MeanO)
		         
			+  ( ( (float)Thres[ thidx ].WB / tempdenominator2 ) * Thres[ thidx ].MeanO * Thres[ thidx ].MeanO);
				
		if( TempVar > BeVar)
		{
			BeVar = TempVar;

			Candidateflag = thidx;
		}
	}//end loop for

		
	/*!
 ****************************************************************************************************************
 * \brief
 *   Utilize the chosen threshold to get the binarization outputdata.
 *    
 *    in: Cadidateflag                out: binarition image.
 *                                                      
 *        graynum[0].firstptr                                                                
 *                 .                                                       
 *                 .                                                       
 *                 .                                     
 *        graynum[255].firstptr                                               
 *                                                      
 *                                                      
 *****************************************************************************************************************
*/
	while( outidx < 256)
	{
		while(graynum[ outidx ].firstptr != NULL)
		{
			buffer =  graynum[outidx].firstptr;

			Idxnum = graynum[ outidx ].firstptr->data;

			ohidx = (int)(Idxnum / iwidth);

			owidx = (Idxnum % iwidth);

			if( outidx <= Candidateflag )
				OutData[ ohidx * iwidth + owidx ] = 0;

			else
				OutData[ ohidx * iwidth + owidx ] = 255;

			out[ ohidx * iwidth + owidx ] = (unsigned char)OutData[ ohidx * iwidth + owidx ];

			graynum[outidx].firstptr = graynum[outidx].firstptr->add;

			free( buffer );//release each node.
		}
				
		++outidx;
	}

	free(OutData);

	return MU_ERR_SUCCESS;


}





/*===========================================================================================*/
/*   muThreshold                                                                             */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a thresholding on an input data, and the thresholded pixels are   */
/*   overwritten to the input buffer.                                                        */
/*   if src data is between with threshold1 and threshold2, then src data = 255, else        */
/*   src data = 0.                                                                           */      
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                          */
/*   MU_8U th1 --> threshold1                                                                */
/*   MU_8U th2 --> threshold2                                                                */
/*===========================================================================================*/

muError_t muThresholding(const muImage_t *src, muImage_t *dst,  muDoubleThreshold_t th)
{
	MU_8U *in, *out;
	MU_32S i;
	MU_32S width, height;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	width = src->width;
	height = src->height;
	in = src->imagedata;
	out = dst->imagedata;

	i = width*height;

	do
	{
		if((*in > th.min) && (*in <= th.max))
			*out = 255;
		else
			*out = 0;

		in++;
		out++;

	}while(--i);

	return MU_ERR_SUCCESS;
}

/* find iso data from input image */
static MU_8U findThresholdIsodata(const muImage_t *src)
{
	MU_8U th1 = 0,th2 = 128;
	MU_32S i, width, height;
	MU_32U th1_count = 0, th2_count = 0, th1_sum = 0, th2_sum = 0;
	MU_16U	histogram[256] = {0};

	width = src->width;
	height = src->height;
	
	i = width*height;

	do
	{
		histogram[src->imagedata[i]]++;	
	}while(--i);
	
	histogram[255] = 0;

	while( abs(th1-th2) >= 1)
	{
		th1_count = 0;
		th2_count = 0;
		th1_sum = 0;
		th2_sum = 0;
		for(i=0; i<256; i++)
		{
			if(i<th2)
			{
				th1_count += histogram[i];
				th1_sum += histogram[i]*i;
			}
			else
			{
				th2_count += histogram[i];
				th2_sum += histogram[i]*i;
			}
		}

		th1 = th2;
		if(th1_count != 0 && th2_count != 0)
		{
			th2 = ((th1_sum/(float)th1_count) + (th2_sum/(float)th2_count))/(float)2;
		}

	}

	return th2;
}

/*===========================================================================================*/
/*   muISOThreshold                                                                         */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a thresholding on an input data, and the thresholded pixels are   */
/*   overwritten to the input buffer.                                                        */
/*   if src data is between with threshold1 and threshold2, then src data = 255, else        */
/*   src data = 0.                                                                           */      
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   MU_8U th1 --> threshold1                                                                */
/*   MU_8U th2 --> threshold2                                                                */
/*===========================================================================================*/

muError_t muISOThresholding(const muImage_t *src, muImage_t *dst)
{
	muDoubleThreshold_t th;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	if(src == NULL)
	{
			return MU_ERR_NOT_SUPPORT; 
	}

	th.max = 255;
	
	th.min = findThresholdIsodata(src);

	if(muThresholding(src, dst, th))
	{
		return MU_ERR_NOT_SUPPORT;
	}

	return MU_ERR_SUCCESS;
}

static MU_8U findThresholdMean(const muImage_t *src)
{
	MU_32S i;
	MU_32S w, h;
	MU_32S sum, count;
	MU_8U *buf;
	
	sum = 0;
	count = 0;
	w = src->width;
	h = src->height;
	buf = src->imagedata;
	for(i=0; i<w*h; i++)
	{
		sum += buf[i];
		if(buf[i] > 0)
		{
			count++;
		}
	}

	return (MU_8U)(sum/(float)count);
}

/*===========================================================================================*/
/*   muMeanThreshold                                                                        */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine finds the mean of total pixels and add the offset to do the thresholding   */      
/*                                                                                           */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*   MU_8U offset --> offset                                                                 */
/*===========================================================================================*/
muError_t muMeanThresholding(const muImage_t *src, muImage_t *dst, MU_8U offset)
{
	muDoubleThreshold_t th;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	if(src == NULL)
	{
		return MU_ERR_NOT_SUPPORT; 
	}

	th.max = 255;

	th.min = findThresholdMean(src);
	
	th.min += offset;

	if(muThresholding(src, dst, th))
	{
		return MU_ERR_NOT_SUPPORT;
	}
	
	return MU_ERR_SUCCESS;

}





