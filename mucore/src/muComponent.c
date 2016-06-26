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
 * Module: muComponent.c
 * Author: Joe Lin
 *
 * Description:
 *  This file is presented some of image segmentation and connected components
 *  algorithms.
 *
 -------------------------------------------------------------------------- */

/* MU include files */
#include "muCore.h"


/* the find algorithm of tree structre */
MU_INLINE MU_8U find(MU_32S x, MU_8U *p)   
{   if(x==p[x])
	x=p[x];
	else
		x=p[x]=find(p[x],p);

	return x;

}


/*===========================================================================================*/
/*   mu4ConnectedComponent8u                                                                */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a two-pass four-connected component algorithm.                    */
/*   The output label will overwrite to the input buffer.                                    */ 
/*                                                                                           */   
/*   NOTE                                                                                    */
/*   This routine only supports less than 252 components.                                    */
/*	 253,254 is reserved																	 */
/*   please look out the label quantity                                                      */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   MU_8U *numlabel --> return number of label of processed image                           */
/*                                                                                           */
/*===========================================================================================*/
muError_t mu4ConnectedComponent8u(muImage_t * src, muImage_t * dst, MU_8U *numlabel)
{
	MU_8U *in, *out, *tempbuffer=NULL;
	MU_8U label=1;
	MU_16U labelcount=1;
	MU_32S i;
	MU_32S x,y;
	MU_32S width, height;
	MU_32S center, left, up;
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

	width = src->width;
	height = src->height;
	in = src->imagedata;
	out = dst->imagedata;

	if(in != out)
		memcpy(out, in, width*height*(src->depth&0x00ff));

	tempbuffer = (MU_8U *)malloc(255*sizeof(MU_8U));
	memset(tempbuffer, 0, 255);


	for(y=0; y<(height-1); y++)
		for(x=0; x<(width-1); x++)
		{

			center = x+1+width*(y+1);
			left = x+width*(y+1);   
			up = x+1+width*y;


			if(out[center]==255)
			{

				if((out[left]== 0)&&(out[up] == 0))
				{             
					out[center] = label;
					tempbuffer[label] = label;
					label++;               
				}
				else if((out[left]== 0)&&(out[up] == 255))
				{
					out[center] = label;
					out[up] = label;
					tempbuffer[label] = label;
					label++; 	   
				}
				else if((out[left]== 255)&&(out[up] == 0))
				{
					out[center] = label;
					out[left] = label;
					tempbuffer[label] = label;
					label++; 

				}
				else if((out[left]== 255)&&(out[up] == 255))
				{
					out[center] = label;
					out[left] = label;
					out[up] = label;
					tempbuffer[label] = label;
					label++; 

				}
				else
				{

					if(out[left] == 0)
					{
						out[center] = out[up];
					}
					else if(out[up] == 0)
					{
						out[center] = out[left];
					}
					else if(out[up] == 255)
					{
						out[center] = out[left];  
					}
					else if(out[left] == 255)
					{
						out[center] = out[up];
					}
					else if(out[up] == out[left])
					{
						out[center] = out[up];	   
					}
					else // foutd small label
					{        
						if(out[left] < out[up])
						{ 
							//union
							tempbuffer[out[left]] = find(out[left],tempbuffer);
							out[center] = tempbuffer[out[left]];
							tempbuffer[out[up]] = tempbuffer[out[left]];

						}
						else if(out[up] < out[left])
						{
							//union
							tempbuffer[out[up]] = find(out[up],tempbuffer);
							out[center] = tempbuffer[out[up]];
							tempbuffer[out[left]] = tempbuffer[out[up]];

						}

					}

				}

			} //if


		}//for


	//re-labeloutg
	for(i=1;i<=label;i++)
	{
		if(tempbuffer[i]==i)
		{
			tempbuffer[i] = labelcount;
			labelcount++;
		}
		else
		{
			tempbuffer[i] = tempbuffer[tempbuffer[i]];
		}

	}


	//foutd
	for(i=0;i<width*height;i++)
	{
		if(out[i]!=0)
			out[i] = tempbuffer[out[i]];

	}       

	free(tempbuffer);

	if(labelcount > 253)
	{
		printf("[mu4ConnectedComponent8u] label quantity cannot larger than 253\n");
	}

	*numlabel = (MU_8U)labelcount;

	return MU_ERR_SUCCESS;              
}


/*===========================================================================================*/
/*   muFindBoundingBox                                                                      */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a two-pass four-connected component algorithm.                    */
/*   The output label will overwrite to the input buffer.                                    */ 
/*                                                                                           */   
/*   NOTE                                                                                    */
/*   This routine only supports less than 255 components.                                    */
/*   please look out the label quantity                                                      */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *input --> input image                                                         */
/*   MU_8U numlabel --> number of label of processed image                                   */
/*   muDoubleThreshold_t --> limitation object size,  th.min << size acception << th.max      */
/*                                                                                           */
/*===========================================================================================*/

muSeq_t * muFindBoundingBox(const muImage_t * image, MU_8U numlabel,  muDoubleThreshold_t th)
{
	MU_8U *labelimg;
	MU_32S i, width, height;
	MU_32S x, y;
	MU_32S area;
	MU_32S minx, miny, maxx, maxy;
	MU_16U objwidth, objheight;
	muBoundingBox_t bp;
	muSeqBlock_t *sbcurrent;
	muSeq_t *sqhead;
	muError_t ret;
	sqhead = NULL;

	ret = muCheckDepth(2, image, MU_IMG_DEPTH_8U);
	if(ret)
	{
		muDebugError(ret); 
		return NULL;
	}

	if(image->channels != 1)
	{
		muDebugError(MU_ERR_NOT_SUPPORT); 
		return NULL;
	}

	labelimg = image->imagedata;
	width = image->width;
	height = image->height;

	// i=1 because the lable image is start at 1
	for(i=1; i<numlabel; i++)
	{
		minx = width;
		miny = height;
		maxx = 0;
		maxy = 0;
		area = 0;

		for(y=0; y<height; y++)
			for(x=0; x<width; x++)
			{
				if(labelimg[x+width*y] == i)
				{
					if(x < minx)
						minx = x;
					else if( x > maxx)
						maxx = x;

					if(y < miny)
						miny = y;
					else if( y > maxy)
						maxy = y;

					area++;  
				}
			}

		objwidth = abs(maxx - minx)+1;
		objheight = abs(maxy - miny)+1;




		//linked list components information 
		if(area >= th.min && area <= th.max)
		{
			bp.minx = minx;
			bp.maxx = maxx;
			bp.miny = miny;
			bp.maxy = maxy;

			bp.width = objwidth;
			bp.height = objheight;
			bp.area = area;
			bp.overlap = 0;
			bp.label = i;


			if(sqhead == NULL)
			{
				sqhead = muCreateSeq(sizeof(muBoundingBox_t));			
			}

			sbcurrent = muPushSeq(sqhead, &bp);


		}


	}//for


	return sqhead;
}


/*===========================================================================================*/
/*   muFindOverlapSize                                                                      */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs the checking overlap size between components                      */
/*																							 */
/*   USAGE                                                                                   */
/*																							 */
/*   muBounding Box1, Box2                                                                  */
/*                                                                                           */
/*===========================================================================================*/

muError_t muFindOverlapSize(muBoundingBox_t *B1, muBoundingBox_t *B2, MU_32S* overlapsize)
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

static MU_VOID muFillSpecificNumberByCol(muImage_t *label_img, MU_32S y_start, MU_32S y_end, MU_32S x_column, MU_8U background_number)
{
	MU_32S width, height;
	MU_32S j;
	MU_8U *limg;

	width = label_img->width;
	height = label_img->height;

	limg = (MU_8U *)label_img->imagedata;

	for(j=y_start; j<y_end; j++)
	{
		if(limg[x_column+width*j] == background_number)
		{
			limg[x_column+width*j]++;
		}
		else
		{
			limg[x_column+width*j] = background_number;
		}
	}

	return;
}


static MU_VOID muFillSpecificNumberByRow(muImage_t *label_img, MU_32S x_start, MU_32S x_end, MU_32S y_row, MU_8U background_number)
{
	MU_32S width, height;
	MU_32S i;
	MU_8U *limg;

	width = label_img->width;
	height = label_img->height;

	limg = (MU_8U *)label_img->imagedata;

	for(i=x_start; i<x_end; i++)
	{
		limg[i+width*y_row] = background_number;	
	}

	return;
}

//have a bug TODO
static MU_VOID muRelabelCompensation(muImage_t *label_img, muImage_t *binary_img, muBoundingBox_t *box, MU_32S background_number)
{
	MU_32S width, height;
	MU_32S box_width, box_height;
	MU_32S box_x, box_y;
	MU_32S i,j,x,y;
	MU_32S bn = background_number;
	MU_32S fn;
	MU_32S area_count = 0;
	MU_8U label;
	MU_8U *r1c1, *r1c2, *r2c1, *r2c2;
	MU_8U *limg, *bimg;
	MU_8U *pixel;
	
	width =  label_img->width;
	height = label_img->height;

	box_width = box->width;
	box_height = box->height;
	box_x = box->minx;
	box_y = box->miny;
	label = box->label;
	fn = bn+1;

	limg = (MU_8U *)label_img->imagedata;


	// top -> bottom
	for(y=0, j=box_y; y<(box_height-1); j++, y++)
		for(x=0, i=box_x; x<(box_width-1); i++, x++)
		{
			r1c1 = limg+i+width*j;
			r1c2 = limg+i+1+width*j;
			r2c1 = limg+i+width*(j+1);
			r2c2 = limg+i+1+width*(j+1);
			
			if((*r1c1) == bn)
			{
				*r1c2 = *r1c2 == fn ? bn : *r1c2;
				*r2c1 = *r2c1 == fn ? bn : *r2c1;
				*r2c2 = *r2c2 == fn ? bn : *r2c2;
			}	
			else if((*r1c2) == bn)
			{
				*r1c1 = *r1c1 == fn ? bn : *r1c1;
				*r2c1 = *r2c1 == fn ? bn : *r2c1;
				*r2c2 = *r2c2 == fn ? bn : *r2c2;
			}
			else if((*r2c1) == bn)
			{
				*r1c1 = *r1c1 == fn ? bn : *r1c1;
				*r1c2 = *r1c2 == fn ? bn : *r1c2;
				*r2c2 = *r2c2 == fn ? bn : *r2c2;
			}
			else if((*r2c2) == bn)
			{
				*r1c1 = *r1c1 == fn ? bn : *r1c1;
				*r1c2 = *r1c2 == fn ? bn : *r1c2;
				*r2c1 = *r2c1 == fn ? bn : *r2c1;
			}

		}


	box_y = box->maxy;
	box_x = box->maxx;

	//bottom -> top
	for(y=0, j=box_y; y<(box_height-1); j--, y++)
		for(x=0, i=box_x; x<(box_width-1); i--, x++)
		{
			r2c2 = limg+i+width*j;
			r2c1 = limg+i-1+width*j;
			r1c2 = limg+i+width*(j-1);
			r1c1 = limg+i-1+width*(j-1);
			
			if((*r1c1) == bn)
			{
				*r1c2 = *r1c2 == fn ? bn : *r1c2;
				*r2c1 = *r2c1 == fn ? bn : *r2c1;
				*r2c2 = *r2c2 == fn ? bn : *r2c2;
			}	
			else if((*r1c2) == bn)
			{
				*r1c1 = *r1c1 == fn ? bn : *r1c1;
				*r2c1 = *r2c1 == fn ? bn : *r2c1;
				*r2c2 = *r2c2 == fn ? bn : *r2c2;
			}
			else if((*r2c1) == bn)
			{
				*r1c1 = *r1c1 == fn ? bn : *r1c1;
				*r1c2 = *r1c2 == fn ? bn : *r1c2;
				*r2c2 = *r2c2 == fn ? bn : *r2c2;
			}
			else if((*r2c2) == bn)
			{
				*r1c1 = *r1c1 == fn ? bn : *r1c1;
				*r1c2 = *r1c2 == fn ? bn : *r1c2;
				*r2c1 = *r2c1 == fn ? bn : *r2c1;
			}

		}

	// compensation
	bimg = (MU_8U *)binary_img->imagedata;
	box_x = box->minx;
	box_y = box->miny;

	for(y=0, j=box_y; y<box_height; j++, y++)
		for(x=0, i=box_x; x<box_width; i++, x++)
		{
			pixel = limg+i+width*j;
			if((*pixel) == fn)
			{
				bimg[i+width*j] = 255;
				area_count++;
			}
			else if((*pixel) == bn)
			{
				*pixel = 0;
			}
		}

	box->area += area_count;

	return;
}

/*===========================================================================================*/
/*   muHoleFillingByLabelImage                                                               */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*	 Label image based hole filling, first, scan row image and then scan column image to     */
/*	 find the real foreground image, and compensation.                                       */
/*																							 */
/*   USAGE                                                                                   */
/*	 muImage_t *label_img --> after connected component image  								 */
/*   muImage_t *binary_img --> after thresholding image                                      */
/*	 muBoundingBox_t *box --> after boundary image (linked list box)                          */
/*																							 */
/*   NOTE																					 */
/*   please check your connected is 4-c or 8-c connected. for debugging                      */
/*===========================================================================================*/
#define MUHOLEFILLING_DEBUG	0
muError_t muHoleFillingByLabelImage(muImage_t *label_img, muImage_t *binary_img, muBoundingBox_t *box)
{
	MU_32S width, height;
	MU_32S box_width, box_height;
	MU_32S box_x, box_y;
	MU_32S i,j;
	MU_32S x,y;
	MU_32S x_start, x_end;
	MU_32S y_start, y_end;
	MU_8U *limg;
	MU_8U pre_label;
	MU_8U label;
	MU_8U hole_flag; 
	MU_8U bn = 253;
	MU_8U *pixel;

	//Test Debug Buffer;
	MU_8U fill_flag = 0;

	if(label_img == NULL || binary_img == NULL || box == NULL)
	{
		return MU_ERR_NULL_POINTER;
	}

	width = label_img->width;
	height = label_img->height;
	
	box_width = box->width;
	box_height = box->height;
	box_x = box->minx;
	box_y = box->miny;

	limg = (MU_8U *)label_img->imagedata;
	
	//row scan
	for(y=0, j=box_y; y<box_height; j++, y++)
	{
		pre_label = 0;
		hole_flag = 0;
		for(x=0, i=box_x; x<box_width; i++, x++)
		{
			pixel = limg+i+j*width;
			if((*pixel))
			{
				label = *pixel;

				if(!pre_label)
				{
					if(label == box->label)
					{
						pre_label = label;
						x_start = i;
					}
					//printf("input %d label = %d\n",pre_label, box->label);
				}
				else
				{
					if(hole_flag &&(pre_label == label))
					{
						x_end = i;
						//printf("fill row x=%d --> x=%d y =%d\n", x_start, x_end, j);
						muFillSpecificNumberByRow(label_img, x_start, x_end, j, bn);
						hole_flag = 0;
						fill_flag = 1;
					}
				}
			}
			else
			{
				if(pre_label)
				{
					if(!hole_flag)
					{
						x_start = i;
						hole_flag = 1;

					}
				}
			}

		}
	}
	
	hole_flag = 0;
	pre_label = 0;
	fill_flag = 0;

	// column scan
	for(x=0, i=box_x; x<box_width; i++, x++)
	{
		pre_label = 0;
		hole_flag = 0;
		for(y=0, j=box_y; y<box_height; j++, y++)
		{
			pixel = limg+i+j*width;

			if((*pixel) && ((*pixel) != bn))
			{
				label = (*pixel);

				if(!pre_label)
				{
					if(label == box->label)
					{
						pre_label = label;
						y_start = j;
					}
					//printf("input %d label = %d\n",pre_label, box->label);
				}
				else
				{
					if(hole_flag &&(pre_label == label))
					{
						y_end = j;
						//printf("fill column x=%d --> x=%d y =%d\n", y_start, y_end, i);
						muFillSpecificNumberByCol(label_img, y_start, y_end, i, bn);
						hole_flag = 0;
						fill_flag = 1;
					}
				}
			}
			else
			{
				if(pre_label)
				{
					if(!hole_flag)
					{
						y_start = j;
						hole_flag = 1;

					}
				}
			}

		}
	}


	// re-labeling and compensation
	muRelabelCompensation(label_img, binary_img, box, bn);

#if MUHOLEFILLING_DEBUG
	if(fill_flag)
	{
		for(y=0, j=box_y; y<box_height; j++, y++)
		{
			for(x=0, i=box_x; x<box_width; i++,x++)
			{
				printf("%3d", limg[i+width*j]);
			}
			printf("\n");
		}

	}
#endif

	return MU_ERR_SUCCESS;

}

/*===========================================================================================*/
/*   muFindGravityCenter                                                                    */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs the checking overlap size between components                      */
/*																							 */
/*   USAGE                                                                                   */
/*																							 */
/*   muBounding Box1, Box2                                                                  */
/*                                                                                           */
/*===========================================================================================*/
#define	DEBUG_FIND_GRAVITY_CENTER 0
muPoint_t muFindGravityCenter(muImage_t *binary_img)
{
	MU_32S width, height;
	MU_32S x,y;
	MU_8U *bimg;
	muPoint_t point;
	MU_32S count = 0;
	MU_32S x_sum = 0, y_sum = 0;

	width = binary_img->width;
	height = binary_img->height;
	
	bimg = (MU_8U *)binary_img->imagedata;

	for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			if(bimg[x+width*y] == 255)
			{
				x_sum+=x;
				y_sum+=y;
				count++;
			}
		}
	
	point.x = x_sum/count;
	point.y = y_sum/count;

#if DEBUG_FIND_GRAVITY_CENTER
	bimg[point.x+width*point.y] = 128;
#endif
	return point;

}

/*===========================================================================================*/
/*   muIntegralImage                                                                        */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*                                                                                           */
/*   NOTE                                                                                    */
/*   This routine would be modified the original data.                                       */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*===========================================================================================*/
muError_t muIntegralImage(const muImage_t *src, muImage_t *ii)
{
	int x, y;
	MU_32U *buf;
	MU_32S width, height;
	MU_8U *in;
	MU_32U *out;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, ii, MU_IMG_DEPTH_32U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || ii->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	width = ii->width;
	height = ii->height;

	buf = (MU_32U *)calloc(width*height, sizeof(MU_32U));

	in = src->imagedata;
	out = (MU_32U *)ii->imagedata;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			if((y-1) == -1)
			{
				buf[x+width*y] = in[x+width*y];
			}
			else
			{
				buf[x+width*y] = in[x+width*y] + buf[x+width*(y-1)];
			}

			if((x-1) == -1)
			{
				out[x+width*y] = buf[x+width*y];
			}
			else
			{
				out[x+width*y] = buf[x+width*y] + out[(x-1)+width*y];
			}
		}
	}

	free(buf);

	return MU_ERR_SUCCESS;
}
