/* ------------------------------------------------------------------------- /
 *
 * Module: muCamtamper.c
 * Author: Joe Lin
 *
 * Description:
 *    Camera Tampering Detection Function
 *
 -------------------------------------------------------------------------- */

#include "muGadget.h"

MU_32S muDetectCamTampering( const muImage_t* src, MU_32S flags, MU_32S sensitivity )
{
	// parameter
	int GradTH, HiGradNumTH;
	int HiHistTH, LoHistTH;
	int HiHistNumTH = 1, LoHistNumTH = 22;
	int OcclBlockTH, LFocBlockTH;
	int sub_block_w;
	int sub_block_h;
	// main method
	int i, j, k, l;
	int Situation;

	// for gradient magnitude
	muImage_t *pGradImg;
	int HiGradCount[9] = {0};

	// for histogram
	// histogram contain only 32(256/8) fields
	int Empty, Peak;
	int HistogramData[9][32]={0};
	int GrayLevelColor;

	// represent the situation of each field
	int BlkSituation[9]={0};
	int LFocBlkNum = 0;
	int OcclBlkNum = 0;

	// mu threshold
	muDoubleThreshold_t dth = {0, 255};

	// image
	muImage_t *pSmallImg;  
	muImage_t *pSubSrcImg;
	muImage_t *pSubGradImg;
	muRect_t ROI;

    sub_block_w=src->width/3;
    sub_block_h=src->height/3;
    
    
	// set initial parameter
	switch (sensitivity)//5:low
	{
		case 1:

			GradTH = 160;
			HiGradNumTH = 36;
			HiHistTH = sub_block_w*sub_block_h*3/4;
			LoHistTH = 2;
			OcclBlockTH = 7;
			LFocBlockTH = 8;

			break;
		case 2:

			GradTH = 180;
			HiGradNumTH = 42;
			HiHistTH = sub_block_w*sub_block_h*3/5;
			LoHistTH = 3;
			OcclBlockTH = 6;
			LFocBlockTH = 7;

			break;

		case 3:

			GradTH = 200;
			HiGradNumTH = 48;
			HiHistTH = sub_block_w*sub_block_h/2;
			LoHistTH = 4;
			OcclBlockTH = 5;
			LFocBlockTH = 7;

			break;

		case 4:

			GradTH = 220;
			HiGradNumTH = 60;
			HiHistTH = sub_block_w*sub_block_h*9/20;
			LoHistTH = 5;
			OcclBlockTH = 4;
			LFocBlockTH = 6;

			break;

		case 5:

			GradTH = 240;
			HiGradNumTH = 78;
			HiHistTH = sub_block_w*sub_block_h*2/5;
			LoHistTH = 6;
			OcclBlockTH = 3;
			LFocBlockTH = 6;

			break;

		default:
			return 0;
	}

	// check flag
	if(!(flags&(MU_CAM_LOSTFOCUS|MU_CAM_OCCLUSION)))
	{
		return 0;
	}

	// down scale image
	if( src->width >= 640 )
	{
		if(src->height >= 480) // D1/ 4CIF/ VGA
		{
			pSmallImg = muCreateImage( muSize(src->width/4, src->height/4), MU_IMG_DEPTH_8U, 1 );
			muDownScale(src, pSmallImg, 4, 4);
		}
		else // 2CIF
		{
			pSmallImg = muCreateImage( muSize(src->width/4, src->height/2), MU_IMG_DEPTH_8U, 1 );
			muDownScale(src, pSmallImg, 2, 4);
		}
	}
	else if( src->width >= 320 ) // CIF/ QVGA
	{
		pSmallImg = muCreateImage( muSize(src->width/2, src->height/2), MU_IMG_DEPTH_8U, 1 );
		muDownScale(src, pSmallImg, 2, 2);
	}
	else // QCIF
	{
		pSmallImg = src;
	}

	// create image
	pGradImg = muCreateImage( muGetSize(pSmallImg), MU_IMG_DEPTH_8U, 1 );
	pSubSrcImg  = muCreateImage( muSize(sub_block_w, sub_block_h), MU_IMG_DEPTH_8U, 1 );
	pSubGradImg = muCreateImage( muSize(sub_block_w, sub_block_h), MU_IMG_DEPTH_8U, 1 );

	// calculate gradient magnitude (Laplace filter)
	if( flags & MU_CAM_LOSTFOCUS )
	{
		muLaplace( pSmallImg, pGradImg, 2 );
		dth.min = GradTH;
		muThresholding( pGradImg, pGradImg, dth );
	}


	// image saparate into 9 parts, each size 40*30
	for( i=0; i<3; i++)
	{
		for( j=0; j<3; j++)
		{
			ROI=muRect(i*sub_block_w, j*sub_block_h, sub_block_w, sub_block_h);

			muGetSubImage(pSmallImg, pSubSrcImg, ROI);
			muGetSubImage(pGradImg, pSubGradImg, ROI);

			for( k=0;k<sub_block_h;k++)
			{
				for( l=0;l<sub_block_w;l++)
				{
					if( flags & MU_CAM_LOSTFOCUS )
					{ 
						// count high gradient number
						if( (muGet2D(pSubGradImg, k, l) == 255) )
							HiGradCount[j*3+i]++;
					}



					// count graylevel histogram
					if( flags & MU_CAM_OCCLUSION )
					{
						GrayLevelColor=(int)(muGet2D(pSubSrcImg, k, l)>>3);
						HistogramData[j*3+i][GrayLevelColor]++;
					}
				}
			}
		}
	}

	// release image
	muReleaseImage( &pGradImg );
	muReleaseImage( &pSubSrcImg );
	muReleaseImage( &pSubGradImg );

	if(src != pSmallImg)
	{
		//printf("src!= pSmallImg\n");
		muReleaseImage( &pSmallImg );
	}

	if( flags & MU_CAM_LOSTFOCUS )
	{
		for(i=0; i<9; i++)
		{
			BlkSituation[i] += MU_CAM_LOSTFOCUS;
			LFocBlkNum++;
		}

	}

	if( flags & MU_CAM_OCCLUSION )
	{
		for(i=0; i<9; i++)
		{
			Empty = 0;
			Peak = 0;
			for(j=0; j<32; j++)
			{
				if( HistogramData[i][j] < LoHistTH )
					Empty++;
				if( HistogramData[i][j] > HiHistTH )
					Peak++;
			}
			if(Empty>=LoHistNumTH || Peak>=HiHistNumTH)
			{
				BlkSituation[i] += MU_CAM_OCCLUSION;
				OcclBlkNum++;
			}
		}

	}

	// classify the tampering situation
	Situation = MU_CAM_NORMAL;

	if( LFocBlkNum >= LFocBlockTH )
		Situation += MU_CAM_LOSTFOCUS;
	if( OcclBlkNum >= OcclBlockTH )
		Situation += MU_CAM_OCCLUSION;

	return Situation;
}
