/* ------------------------------------------------------------------------- /
 *
 * Module: muFilter.c
 * Author: Joe Lin
 *
 * Description:
 *    Filters
 *
 -------------------------------------------------------------------------- */

#include "muCore.h"

/* Convolves the image with the 3*3 kernel and
   border effect will be handle by specified type (MU_BORDER_*) */
muError_t muFilter33( const muImage_t* src, muImage_t* dst, const MU_8S kernel[], const MU_8U norm)
{
	MU_32S i, j, temp;
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

	
	for( i=1; i<(src->height-1); i++ )
	{
		for( j=1; j<(src->width-1); j++ )
		{
			temp = 0;
			temp += (src->imagedata[(i-1)*src->width+(j-1)]*kernel[0]);
			temp += (src->imagedata[(i-1)*src->width+(j+0)]*kernel[1]);
			temp += (src->imagedata[(i-1)*src->width+(j+1)]*kernel[2]);
			temp += (src->imagedata[(i+0)*src->width+(j-1)]*kernel[3]);
			temp += (src->imagedata[(i+0)*src->width+(j+0)]*kernel[4]);
			temp += (src->imagedata[(i+0)*src->width+(j+1)]*kernel[5]);
			temp += (src->imagedata[(i+1)*src->width+(j-1)]*kernel[6]);
			temp += (src->imagedata[(i+1)*src->width+(j+0)]*kernel[7]);
			temp += (src->imagedata[(i+1)*src->width+(j+1)]*kernel[8]);

			dst->imagedata[i*src->width+j] = temp>>norm;
		}
	}

	return MU_ERR_SUCCESS;
}


muError_t muMedian33(const muImage_t *src, muImage_t *dst)
{
	MU_8U temp, flag;
	MU_32S i,j;
	MU_32S x,y;
	MU_32S width, height;
	muError_t ret;
	MU_8U *in, *out;
	MU_8U data[9];

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

	for(j=0; j<(height-2); j++)
		for(i=0; i<(width-2); i++)
		{
			data[0] = in[i+width*j];
			data[1] = in[i+1+width*j];
			data[2] = in[i+2+width*j];

			data[3] = in[i+width*(j+1)];
			data[4] = in[i+1+width*(j+1)];
			data[5] = in[i+2+width*(j+1)];

			data[6] = in[i+width*(j+2)];
			data[7] = in[i+1+width*(j+2)];
			data[8] = in[i+2+width*(j+2)];

			for(x=0; x<9-1; x++)
			{
				flag = 0;
				for(y=0; y<9-x-1; y++)
				{
					if(data[y] > data[y+1])
					{
						flag = 1;
						temp = data[y];
						data[y] = data[y+1];
						data[y+1] = temp;
					}
				}

				if(!flag)
				break;
			}
			
			out[i+1+width*(j+1)] = data[4];
		}

	return MU_ERR_SUCCESS;
}

static MU_8U search_median_value(MU_8U Numarry[])
{
	// bitonic sorting.	
	MU_8U med; 
	MU_32S i=0, temp;
	MU_32S j=0;
	MU_32S k=0, l=0; 
	MU_32S m=0;

	while(i<4)
	{
		if(i%2==0)
		{
			if( Numarry[i*2] > Numarry[i*2+1])
			{
				temp = Numarry[i*2];
				Numarry[i * 2] = Numarry[i * 2 +1];
				Numarry[i * 2 +1] = temp;
			}
		}
		else
		{
			if( Numarry[ i * 2] < Numarry[i * 2 + 1])
			{
				temp = Numarry[ i * 2];
				Numarry[i * 2] = Numarry[i * 2 +1];
				Numarry[i * 2 +1] = temp;
			}
		}
		++i;
	}

	i=0;
	while( i < 2)
	{
		while( j < 2)
		{
			if( i % 2 == 0)
			{
				if( Numarry[ i * 4 + j] >  Numarry[ i * 4 + j + 2])
				{
					temp = Numarry[ i * 4 + j];
					Numarry[ i * 4 + j] = Numarry[ i * 4 + j + 2];
					Numarry[ i * 4 + j + 2] = temp;
				}
			}
			else
			{
				if( Numarry[ i * 4 + j] <  Numarry[ i * 4 + j + 2])
				{
					temp = Numarry[ i * 4 + j];
					Numarry[ i * 4 + j] = Numarry[ i * 4 + j + 2];
					Numarry[ i * 4 + j + 2] = temp;
				}
			}// end else
			++j;
		}// end while loop j.
		j = 0; // clesr value j. 
		++i;
	}

	i = 0, j = 0;

	while(j < 4)
	{
		if(j < 2 )
		{
			if( Numarry[ j * 2 ] > Numarry[ j * 2 + 1] )
			{
				temp = Numarry[ j * 2 ];
				Numarry[ j * 2 ] = Numarry[ j * 2 + 1];
				Numarry[ j * 2 + 1] = temp;
			}
		}
		else
		{
			if( Numarry[ j * 2 ] < Numarry[ j * 2 + 1] )
			{
				temp = Numarry[ j * 2 ];\
				Numarry[ j * 2 ] = Numarry[ j * 2 + 1];
				Numarry[ j * 2 + 1] = temp;
			}
		}
		++j;
	}
	j = 0;
	while( j < 4)
	{
		if( Numarry[ j ] > Numarry[ j + 4])
		{
			temp = Numarry[ j ];
			Numarry[ j ] = Numarry[ j + 4];
			Numarry[ j + 4] = temp;
		}
		++j;
	}// get the bitonic sequence that is dived into min and max group.

	while( k < 2)
	{
		while( l < 2)
		{
			if( Numarry[ k * 4 + l] > Numarry[ k * 4 + l + 2])
			{
				temp = Numarry[ k * 4 + l];
				Numarry[ k * 4 + l] = Numarry[ k * 4 + l + 2];
				Numarry[ k * 4 + l + 2] = temp;
			}
			++l;
		}
		l = 0;
		++k;
	}

	k = 0, l = 0;

	while( m < 4)
	{
		if( Numarry[ m * 2] > Numarry[ m * 2 + 1])
		{
			temp = Numarry[ m * 2];
			Numarry[ m * 2 ] = Numarry[ m * 2 + 1];
			Numarry[ m * 2 + 1] = temp;
		}
		++m;
	}

	//decide the median value.
	if( Numarry[8] <= Numarry[3])
		med = Numarry[3];
	else if( Numarry[8] >= Numarry[4])
		med = Numarry[4];
	else
		med = Numarry[8];

	return med;
}

/*  Fast Median Filter by biotonic search */
muError_t muFastMedian33(muImage_t * src, muImage_t * dst)
{
	MU_32S i,j,ret;
	MU_32S width, height;
	MU_8U data[9];
	MU_8U *in, *out;
	MU_8U median;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}
	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	in = src->imagedata;
	out = dst->imagedata;
	width = src->width;
	height = src->height;

	for(j=0; j<(height-2); j++)
		for(i=0; i<(width-2); i++)
		{
			data[0] = in[i+width*j];
			data[1] = in[i+1+width*j];
			data[2] = in[i+2+width*j];

			data[3] = in[i+width*(j+1)];
			data[4] = in[i+1+width*(j+1)];
			data[5] = in[i+2+width*(j+1)];

			data[6] = in[i+width*(j+2)];
			data[7] = in[i+1+width*(j+2)];
			data[8] = in[i+2+width*(j+2)];
		
			median = search_median_value(data);//get the median value.
			
			out[i+1+width*(j+1)] = median;
		}

	return MU_ERR_SUCCESS;
}
