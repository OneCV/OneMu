/* ------------------------------------------------------------------------- /
 *
 * Module: muMotion.c
 * Author: Joe Lin
 * Create date: 10/12/2012
 *
 * Description:
 *  This file is presented the motion detection of image processing 
 *
 -------------------------------------------------------------------------- */

/* MU include files */
#include "muCore.h"

/*===========================================================================================*/
/*   muLKOpticalFlow->muTransVector2Angle->muGetVecotrImage                               */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This processing could detect the vectorimage between two successive images by           */
/*   Locus-Canade Optical flow method.                                                       */
/*																							 */
/*   NOTE                                                                                    */
/*   1.the three function should applicate sequentially.                                     */
/*   2.the size of *vector_x, *vector_y, *lost_table , *angle_table                          */ 
/*	   should allocate the same size with input image.        								 */                    
/*	                                                                                         */ 
/*   USAGE                                                                                   */
/*	 muLKOpticalFlow																		 */
/*   in:													out:							 */
/*   muImage_t *image_i --> pre-image					    MU_32S *vector_x			     */
/*   muImage_t *image_j --> current image				    MU_32S *vector_y                 */
/*															MU_32S *lost_table				 */
/*   muTransVector2Angle																	 */
/*	 in:													out:							 */
/*   MU_32S *vector_x-> value from muLKOpticalFlow		    MU_32S *angle_table				 */
/*   MU_32S *vector_y->	value from muLKOpticalFlow											 */
/*	 MU_32S *lost_table-> value from muLKOpticalFlow										 */
/*																							 */
/*	 muGetVecotrImage																		 */
/*	 in:						                            out:							 */
/*	 MU_32S *angle_map-> value from muTransVector2Angle    muImage_t *dst->vector image      */
/*   muImage_t *src->current image                                                            */
/*===========================================================================================*/

muError_t muLKOpticalFlow(muImage_t *image_i, muImage_t *image_j, MU_32S *vector_x, MU_32S *vector_y, MU_32S *lost_table)
{
	int image_h, image_w;
	int i,j,idx,jdx;
	int hori_left, hori_right, diff_x;
	int verti_up, verti_down, diff_y;
	MU_8S *i_x;
	MU_8S *i_y;

	//LK Optical flow
	int iteration;
	double *etha_table;
	int matrix_g[2][2]={0}, b_k[2][1]={0};
	int m,n;
	int delta_i,pixeli, pixelj;
	int det_g;
	float inverse_g[2][2]={0.f};
	float etha_x,etha_y;
	double norm_etha;
	int round_etha_x, round_etha_y;
	muImage_t *s8_i_x;
	muImage_t *s8_i_y;

	if(image_i->channels != 1 || image_j->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	image_h = image_i->height;
	image_w = image_i->width;

	etha_table = (double *) malloc(image_h*image_w*sizeof(double));
	for(i=0;i<(image_w*image_h);i++)
		etha_table[i]=100.f;

	/*calculate the central difference image*/
	s8_i_x = muCreateImage(muSize(image_w,image_h),MU_IMG_DEPTH_8S,1);
	s8_i_y = muCreateImage(muSize(image_w,image_h),MU_IMG_DEPTH_8S,1);
	muSetZero(s8_i_x);
	muSetZero(s8_i_y);

	i_x = (MU_8S *) s8_i_x->imagedata;
	i_y = (MU_8S *) s8_i_y->imagedata;

	for(i=0;i<image_h;i++)//calculate I_x
		for(j=0;j<image_w;j++)
		{
			if(j-1<0)
				hori_left = image_i->imagedata[i*image_w+j];
			else
				hori_left = image_i->imagedata[i*image_w+(j-1)];

			if(j+1>=image_w)
				hori_right = image_i->imagedata[i*image_w+j];
			else
				hori_right = image_i->imagedata[i*image_w+(j+1)];

			diff_x = (int)((hori_left-hori_right)/2);

			i_x[i*image_w+j] = diff_x;
		}

	for(i=0;i<image_h;i++)//calculate I_y
		for(j=0;j<image_w;j++)
		{
			if(i-1<0)
				verti_up = image_i->imagedata[i*image_w+j];
			else
				verti_up = image_i->imagedata[(i-1)*image_w+j];

			if(i+1>=image_h)
				verti_down = image_i->imagedata[i*image_w+j];
			else
				verti_down = image_i->imagedata[(i+1)*image_w+j];

			diff_y = (int)((verti_down-verti_up)/2);

			i_y[i*image_w+j] = diff_y;
		}

	/*Optical Flow of Locus Canade with Newton Raphson iteration*/
	for(iteration=0;iteration<4;iteration++)
	{//7
		for(i=0;i<image_h;i++)
			for(j=0;j<image_w;j++)
			{//6
				if((lost_table[i*image_w+j]!=0)
					||(etha_table[i*image_w+j]<0.0009f))
					continue;
				else
				{//5
					if((i+vector_y[i*image_w+j]<0)||(i+vector_y[i*image_w+j]>=image_h)
						||(j+vector_x[i*image_w+j]<0)||(j+vector_x[i*image_w+j]>=image_w))
					{
						lost_table[i*image_w+j]=1;
						continue;
					}

					else
					{//4
						for(idx=0;idx<2;idx++)//initial matrix
						{
							b_k[idx][0]=0;
							for(jdx=0;jdx<2;jdx++)
								matrix_g[idx][jdx]=0;
						}

						for(m=-1;m<=1;m++)
							for(n=-1;n<=1;n++)
							{//3
								if((i+m<0)||(i+m>=image_h)
									||(j+n<0)||(j+n>=image_w))
									continue;
								else
								{//2
									if((i+vector_y[i*image_w+j]+m<0)||(i+vector_y[i*image_w+j]+m>=image_h)
										||(j+vector_x[i*image_w+j]+n<0)||(j+vector_x[i*image_w+j]+n>=image_w))
										continue;
									else
									{//1
										matrix_g[0][0] += (i_x[(i+m)*image_w+(j+n)]*i_x[(i+m)*image_w+(j+n)]);

										matrix_g[0][1] += (i_x[(i+m)*image_w+(j+n)]*i_y[(i+m)*image_w+(j+n)]);

										matrix_g[1][0] = matrix_g[0][1];

										matrix_g[1][1] += (i_y[(i+m)*image_w+(j+n)]*i_y[(i+m)*image_w+(j+n)]);

										pixeli = image_i->imagedata[(i+m)*image_w+(j+n)];
										pixelj = image_j->imagedata[(i+vector_y[i*image_w+j]+m)*image_w+(j+vector_x[i*image_w+j]+n)];
										delta_i = pixeli-pixelj;

										b_k[0][0] += delta_i*i_x[(i+m)*image_w+(j+n)];
										b_k[1][0] += delta_i*i_y[(i+m)*image_w+(j+n)];

									}//1 end if((i+vector_y[i*image_w+j]+m<0)||(i+vector_y[i*image_w+j]+m>=image_h)...
									 
								}//2 end else if((i+m<0)||(i+m>=image_h)||(j+n<0)||(j+n>=image_w))

							}//3end for m,n

							det_g = (matrix_g[0][0]*matrix_g[1][1])-(matrix_g[1][0]*matrix_g[0][1]);

							if(!det_g)
							{
								lost_table[i*image_w+j]=2;
								continue;
							}
							else
							{
								inverse_g[0][0] = matrix_g[1][1]/(float)det_g;
								inverse_g[1][1] = matrix_g[0][0]/(float)det_g;
								inverse_g[0][1] = (-1*matrix_g[0][1])/(float)det_g;
								inverse_g[1][0] = (-1*matrix_g[1][0])/(float)det_g;

								etha_x = (inverse_g[0][0]*b_k[0][0])+(inverse_g[0][1]*b_k[1][0]);
								etha_y = (inverse_g[1][0]*b_k[0][0])+(inverse_g[1][1]*b_k[1][0]);

								norm_etha = (etha_x*etha_x)+(etha_y*etha_y);
								etha_table[i*image_w+j] = norm_etha;

								if(etha_x>0.0f)
									round_etha_x = (int)(etha_x+0.5f);
								else if(etha_x<0.0f)
									round_etha_x = (int)(etha_x-0.5f);
								else
									round_etha_x = 0;

								if(etha_y>0.0f)
									round_etha_y = (int)(etha_y+0.5f);
								else if(etha_y<0.0f)
									round_etha_y = (int)(etha_y-0.5f);
								else
									round_etha_y = 0;

								vector_x[i*image_w+j] += round_etha_x;
								vector_y[i*image_w+j] += round_etha_y;
							}
					
					}//4end if((i+vector_y[i*image_w+j]<0)||(i+vector_y[i*image_w+j]>=image_h)||(j+vector_x[i*image_w+j]<0)||(j+vector_x[i*image_w+j]>=image_w))

				}//5end else if((lost_table[i*image_w+j]!=0)||(etha_table[i*image_w+j]<0.03)) 5

			}//6end for process whole image one time 6
	}//7end for iteration 7

	muReleaseImage(&s8_i_x);
	muReleaseImage(&s8_i_y);

	free(etha_table);

	return MU_ERR_SUCCESS;
}


muError_t muTransVector2Angle(muImage_t *cur_frame, MU_32S *vector_x, MU_32S *vector_y, MU_32S *lost_table, MU_32S *angle_table)
{
	int i,j;
	int image_h, image_w;
	double dir_y, dir_x;
	int angle;

	image_h = cur_frame->height;
	image_w = cur_frame->width;

	if(cur_frame->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}
	
	for(i=0;i<image_h;i++)
		for(j=0;j<image_w;j++)
		{//1
			if(lost_table[i*image_w+j]!=0)
				angle_table[i*image_w+j]=-2;
			else
			{//2
				if((vector_x[i*image_w+j]==0)&&(vector_y[i*image_w+j]==0))
					angle_table[i*image_w+j] = -1;
				else
				{//3
					dir_y = (double)(-1*vector_y[i*image_w+j]);
					dir_x = (double)(vector_x[i*image_w+j]);

					angle = (int)(atan2(dir_y,dir_x)*180/MU_PI);

					if((angle>=23)&&(angle<67))
						angle=45;
					else if((angle>=67)&&(angle<113))
						angle=90;
					else if((angle>=113)&&(angle<158))
						angle=135;
					else if((angle>=158)&&(angle<203))
						angle=180;
					else if((angle>=203)&&(angle<248))
						angle=225;
					else if((angle>=248)&&(angle<293))
						angle=270;
					else if((angle>=293)&&(angle<338))
						angle=315;
					else
						angle=0;

					angle_table[i*image_w+j]=angle;

				}//3
			}//2
		}//1

	return MU_ERR_SUCCESS;
}

muError_t muGetVectorImage(MU_32S *angle_map, muImage_t *src, muImage_t *dst)
{
	int image_h,image_w;
	int i,j;
	int vector;

	image_w = src->width;
	image_h = src->height;

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	for(i=0;i<image_h;i++)
		for(j=0;j<image_w;j++)
		{
			vector = angle_map[i*image_w+j];

			if(vector==0)
				dst->imagedata[i*image_w+j] = 64;
			else if(vector == 45)
				dst->imagedata[i*image_w+j] = 96;
			else if(vector==90)
				dst->imagedata[i*image_w+j] = 128;
			else if(vector==135)
				dst->imagedata[i*image_w+j] = 161;
			else if(vector==180)
				dst->imagedata[i*image_w+j] = 192;
			else if(vector==225)
				dst->imagedata[i*image_w+j] = 224;
			else if(vector==270)
				dst->imagedata[i*image_w+j] = 255;
			else if(vector==315)
				dst->imagedata[i*image_w+j] = 32;
			else
				dst->imagedata[i*image_w+j] = 0;
		}

	return MU_ERR_SUCCESS;
}

