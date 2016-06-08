/* ------------------------------------------------------------------------- /
 *
 * Module: muBase.c
 * Author: Joe Lin
 *
 * Description:
 *    the basic functions
 *
 -------------------------------------------------------------------------- */

#include "muCore.h"

//TODO Bitmap function naming rule
/* load bmp header file */
typedef struct _bitmapFileHeader{
	MU_16U	bfType;
	MU_32U	bfSize;
	MU_16U	bfReserved1;
	MU_16U	bfReserved2;
	MU_32U	bfOffBits;
}bitmapFileHeader_t;

/* load bmp header file */
typedef struct _bitmapInfoHeader{
	MU_32U	biSize;
	MU_32S	biWidth;
	MU_32S	biHeight;
	unsigned short	biPlanes;
	unsigned short	biBitCount;
	unsigned int	biCompression;
	unsigned int	biSizeImage;
	long			biXPelsPerMeter;
	long			biYPelsPerMeter;
	unsigned int	biClrUsed;
	unsigned int	biClrImportant;
}bitmapInfoHeader_t;

/* load bmp header file */
typedef struct rgbQuad{
	unsigned char	rgbBlue;
	unsigned char	rgbGreen;
	unsigned char	rgbRed;
	unsigned char	rgbReserved;
}rgbQuad_t;


// Default value of gray palette
rgbQuad_t GrayPalette[256] = 
{ {0x00, 0x00, 0x00, 0x0}, {0x01, 0x01, 0x01, 0x0},
	{0x02, 0x02, 0x02, 0x0}, {0x03, 0x03, 0x03, 0x0},
	{0x04, 0x04, 0x04, 0x0}, {0x05, 0x05, 0x05, 0x0},
	{0x06, 0x06, 0x06, 0x0}, {0x07, 0x07, 0x07, 0x0},
	{0x08, 0x08, 0x08, 0x0}, {0x09, 0x09, 0x09, 0x0},
	{0x0a, 0x0a, 0x0a, 0x0}, {0x0b, 0x0b, 0x0b, 0x0},
	{0x0c, 0x0c, 0x0c, 0x0}, {0x0d, 0x0d, 0x0d, 0x0},
	{0x0e, 0x0e, 0x0e, 0x0}, {0x0f, 0x0f, 0x0f, 0x0},
	{0x10, 0x10, 0x10, 0x0}, {0x11, 0x11, 0x11, 0x0},
	{0x12, 0x12, 0x12, 0x0}, {0x13, 0x13, 0x13, 0x0},
	{0x14, 0x14, 0x14, 0x0}, {0x15, 0x15, 0x15, 0x0},
	{0x16, 0x16, 0x16, 0x0}, {0x17, 0x17, 0x17, 0x0},
	{0x18, 0x18, 0x18, 0x0}, {0x19, 0x19, 0x19, 0x0},
	{0x1a, 0x1a, 0x1a, 0x0}, {0x1b, 0x1b, 0x1b, 0x0},
	{0x1c, 0x1c, 0x1c, 0x0}, {0x1d, 0x1d, 0x1d, 0x0},
	{0x1e, 0x1e, 0x1e, 0x0}, {0x1f, 0x1f, 0x1f, 0x0},
	{0x20, 0x20, 0x20, 0x0}, {0x21, 0x21, 0x21, 0x0},
	{0x22, 0x22, 0x22, 0x0}, {0x23, 0x23, 0x23, 0x0},
	{0x24, 0x24, 0x24, 0x0}, {0x25, 0x25, 0x25, 0x0},
	{0x26, 0x26, 0x26, 0x0}, {0x27, 0x27, 0x27, 0x0},
	{0x28, 0x28, 0x28, 0x0}, {0x29, 0x29, 0x29, 0x0},
	{0x2a, 0x2a, 0x2a, 0x0}, {0x2b, 0x2b, 0x2b, 0x0},
	{0x2c, 0x2c, 0x2c, 0x0}, {0x2d, 0x2d, 0x2d, 0x0},
	{0x2e, 0x2e, 0x2e, 0x0}, {0x2f, 0x2f, 0x2f, 0x0},
	{0x30, 0x30, 0x30, 0x0}, {0x31, 0x31, 0x31, 0x0},
	{0x32, 0x32, 0x32, 0x0}, {0x33, 0x33, 0x33, 0x0},
	{0x34, 0x34, 0x34, 0x0}, {0x35, 0x35, 0x35, 0x0},
	{0x36, 0x36, 0x36, 0x0}, {0x37, 0x37, 0x37, 0x0},
	{0x38, 0x38, 0x38, 0x0}, {0x39, 0x39, 0x39, 0x0},
	{0x3a, 0x3a, 0x3a, 0x0}, {0x3b, 0x3b, 0x3b, 0x0},
	{0x3c, 0x3c, 0x3c, 0x0}, {0x3d, 0x3d, 0x3d, 0x0},
	{0x3e, 0x3e, 0x3e, 0x0}, {0x3f, 0x3f, 0x3f, 0x0},
	{0x40, 0x40, 0x40, 0x0}, {0x41, 0x41, 0x41, 0x0},
	{0x42, 0x42, 0x42, 0x0}, {0x43, 0x43, 0x43, 0x0},
	{0x44, 0x44, 0x44, 0x0}, {0x45, 0x45, 0x45, 0x0},
	{0x46, 0x46, 0x46, 0x0}, {0x47, 0x47, 0x47, 0x0},
	{0x48, 0x48, 0x48, 0x0}, {0x49, 0x49, 0x49, 0x0},
	{0x4a, 0x4a, 0x4a, 0x0}, {0x4b, 0x4b, 0x4b, 0x0},
	{0x4c, 0x4c, 0x4c, 0x0}, {0x4d, 0x4d, 0x4d, 0x0},
	{0x4e, 0x4e, 0x4e, 0x0}, {0x4f, 0x4f, 0x4f, 0x0},
	{0x50, 0x50, 0x50, 0x0}, {0x51, 0x51, 0x51, 0x0},
	{0x52, 0x52, 0x52, 0x0}, {0x53, 0x53, 0x53, 0x0},
	{0x54, 0x54, 0x54, 0x0}, {0x55, 0x55, 0x55, 0x0},
	{0x56, 0x56, 0x56, 0x0}, {0x57, 0x57, 0x57, 0x0},
	{0x58, 0x58, 0x58, 0x0}, {0x59, 0x59, 0x59, 0x0},
	{0x5a, 0x5a, 0x5a, 0x0}, {0x5b, 0x5b, 0x5b, 0x0},
	{0x5c, 0x5c, 0x5c, 0x0}, {0x5d, 0x5d, 0x5d, 0x0},
	{0x5e, 0x5e, 0x5e, 0x0}, {0x5f, 0x5f, 0x5f, 0x0},
	{0x60, 0x60, 0x60, 0x0}, {0x61, 0x61, 0x61, 0x0},
	{0x62, 0x62, 0x62, 0x0}, {0x63, 0x63, 0x63, 0x0},
	{0x64, 0x64, 0x64, 0x0}, {0x65, 0x65, 0x65, 0x0},
	{0x66, 0x66, 0x66, 0x0}, {0x67, 0x67, 0x67, 0x0},
	{0x68, 0x68, 0x68, 0x0}, {0x69, 0x69, 0x69, 0x0},
	{0x6a, 0x6a, 0x6a, 0x0}, {0x6b, 0x6b, 0x6b, 0x0},
	{0x6c, 0x6c, 0x6c, 0x0}, {0x6d, 0x6d, 0x6d, 0x0},
	{0x6e, 0x6e, 0x6e, 0x0}, {0x6f, 0x6f, 0x6f, 0x0},
	{0x70, 0x70, 0x70, 0x0}, {0x71, 0x71, 0x71, 0x0},
	{0x72, 0x72, 0x72, 0x0}, {0x73, 0x73, 0x73, 0x0},
	{0x74, 0x74, 0x74, 0x0}, {0x75, 0x75, 0x75, 0x0},
	{0x76, 0x76, 0x76, 0x0}, {0x77, 0x77, 0x77, 0x0},
	{0x78, 0x78, 0x78, 0x0}, {0x79, 0x79, 0x79, 0x0},
	{0x7a, 0x7a, 0x7a, 0x0}, {0x7b, 0x7b, 0x7b, 0x0},
	{0x7c, 0x7c, 0x7c, 0x0}, {0x7d, 0x7d, 0x7d, 0x0},
	{0x7e, 0x7e, 0x7e, 0x0}, {0x7f, 0x7f, 0x7f, 0x0},
	{0x80, 0x80, 0x80, 0x0}, {0x81, 0x81, 0x81, 0x0},
	{0x82, 0x82, 0x82, 0x0}, {0x83, 0x83, 0x83, 0x0},
	{0x84, 0x84, 0x84, 0x0}, {0x85, 0x85, 0x85, 0x0},
	{0x86, 0x86, 0x86, 0x0}, {0x87, 0x87, 0x87, 0x0},
	{0x88, 0x88, 0x88, 0x0}, {0x89, 0x89, 0x89, 0x0},
	{0x8a, 0x8a, 0x8a, 0x0}, {0x8b, 0x8b, 0x8b, 0x0},
	{0x8c, 0x8c, 0x8c, 0x0}, {0x8d, 0x8d, 0x8d, 0x0},
	{0x8e, 0x8e, 0x8e, 0x0}, {0x8f, 0x8f, 0x8f, 0x0},
	{0x90, 0x90, 0x90, 0x0}, {0x91, 0x91, 0x91, 0x0},
	{0x92, 0x92, 0x92, 0x0}, {0x93, 0x93, 0x93, 0x0},
	{0x94, 0x94, 0x94, 0x0}, {0x95, 0x95, 0x95, 0x0},
	{0x96, 0x96, 0x96, 0x0}, {0x97, 0x97, 0x97, 0x0},
	{0x98, 0x98, 0x98, 0x0}, {0x99, 0x99, 0x99, 0x0},
	{0x9a, 0x9a, 0x9a, 0x0}, {0x9b, 0x9b, 0x9b, 0x0},
	{0x9c, 0x9c, 0x9c, 0x0}, {0x9d, 0x9d, 0x9d, 0x0},
	{0x9e, 0x9e, 0x9e, 0x0}, {0x9f, 0x9f, 0x9f, 0x0},
	{0xa0, 0xa0, 0xa0, 0x0}, {0xa1, 0xa1, 0xa1, 0x0},
	{0xa2, 0xa2, 0xa2, 0x0}, {0xa3, 0xa3, 0xa3, 0x0},
	{0xa4, 0xa4, 0xa4, 0x0}, {0xa5, 0xa5, 0xa5, 0x0},
	{0xa6, 0xa6, 0xa6, 0x0}, {0xa7, 0xa7, 0xa7, 0x0},
	{0xa8, 0xa8, 0xa8, 0x0}, {0xa9, 0xa9, 0xa9, 0x0},
	{0xaa, 0xaa, 0xaa, 0x0}, {0xab, 0xab, 0xab, 0x0},
	{0xac, 0xac, 0xac, 0x0}, {0xad, 0xad, 0xad, 0x0},
	{0xae, 0xae, 0xae, 0x0}, {0xaf, 0xaf, 0xaf, 0x0},
	{0xb0, 0xb0, 0xb0, 0x0}, {0xb1, 0xb1, 0xb1, 0x0},
	{0xb2, 0xb2, 0xb2, 0x0}, {0xb3, 0xb3, 0xb3, 0x0},
	{0xb4, 0xb4, 0xb4, 0x0}, {0xb5, 0xb5, 0xb5, 0x0},
	{0xb6, 0xb6, 0xb6, 0x0}, {0xb7, 0xb7, 0xb7, 0x0},
	{0xb8, 0xb8, 0xb8, 0x0}, {0xb9, 0xb9, 0xb9, 0x0},
	{0xba, 0xba, 0xba, 0x0}, {0xbb, 0xbb, 0xbb, 0x0},
	{0xbc, 0xbc, 0xbc, 0x0}, {0xbd, 0xbd, 0xbd, 0x0},
	{0xbe, 0xbe, 0xbe, 0x0}, {0xbf, 0xbf, 0xbf, 0x0},
	{0xc0, 0xc0, 0xc0, 0x0}, {0xc1, 0xc1, 0xc1, 0x0},
	{0xc2, 0xc2, 0xc2, 0x0}, {0xc3, 0xc3, 0xc3, 0x0},
	{0xc4, 0xc4, 0xc4, 0x0}, {0xc5, 0xc5, 0xc5, 0x0},
	{0xc6, 0xc6, 0xc6, 0x0}, {0xc7, 0xc7, 0xc7, 0x0},
	{0xc8, 0xc8, 0xc8, 0x0}, {0xc9, 0xc9, 0xc9, 0x0},
	{0xca, 0xca, 0xca, 0x0}, {0xcb, 0xcb, 0xcb, 0x0},
	{0xcc, 0xcc, 0xcc, 0x0}, {0xcd, 0xcd, 0xcd, 0x0},
	{0xce, 0xce, 0xce, 0x0}, {0xcf, 0xcf, 0xcf, 0x0},
	{0xd0, 0xd0, 0xd0, 0x0}, {0xd1, 0xd1, 0xd1, 0x0},
	{0xd2, 0xd2, 0xd2, 0x0}, {0xd3, 0xd3, 0xd3, 0x0},
	{0xd4, 0xd4, 0xd4, 0x0}, {0xd5, 0xd5, 0xd5, 0x0},
	{0xd6, 0xd6, 0xd6, 0x0}, {0xd7, 0xd7, 0xd7, 0x0},
	{0xd8, 0xd8, 0xd8, 0x0}, {0xd9, 0xd9, 0xd9, 0x0},
	{0xda, 0xda, 0xda, 0x0}, {0xdb, 0xdb, 0xdb, 0x0},
	{0xdc, 0xdc, 0xdc, 0x0}, {0xdd, 0xdd, 0xdd, 0x0},
	{0xde, 0xde, 0xde, 0x0}, {0xdf, 0xdf, 0xdf, 0x0},
	{0xe0, 0xe0, 0xe0, 0x0}, {0xe1, 0xe1, 0xe1, 0x0},
	{0xe2, 0xe2, 0xe2, 0x0}, {0xe3, 0xe3, 0xe3, 0x0},
	{0xe4, 0xe4, 0xe4, 0x0}, {0xe5, 0xe5, 0xe5, 0x0},
	{0xe6, 0xe6, 0xe6, 0x0}, {0xe7, 0xe7, 0xe7, 0x0},
	{0xe8, 0xe8, 0xe8, 0x0}, {0xe9, 0xe9, 0xe9, 0x0},
	{0xea, 0xea, 0xea, 0x0}, {0xeb, 0xeb, 0xeb, 0x0},
	{0xec, 0xec, 0xec, 0x0}, {0xed, 0xed, 0xed, 0x0},
	{0xee, 0xee, 0xee, 0x0}, {0xef, 0xef, 0xef, 0x0},
	{0xf0, 0xf0, 0xf0, 0x0}, {0xf1, 0xf1, 0xf1, 0x0},
	{0xf2, 0xf2, 0xf2, 0x0}, {0xf3, 0xf3, 0xf3, 0x0},
	{0xf4, 0xf4, 0xf4, 0x0}, {0xf5, 0xf5, 0xf5, 0x0},
	{0xf6, 0xf6, 0xf6, 0x0}, {0xf7, 0xf7, 0xf7, 0x0},
	{0xf8, 0xf8, 0xf8, 0x0}, {0xf9, 0xf9, 0xf9, 0x0},
	{0xfa, 0xfa, 0xfa, 0x0}, {0xfb, 0xfb, 0xfb, 0x0},
	{0xfc, 0xfc, 0xfc, 0x0}, {0xfd, 0xfd, 0xfd, 0x0},
	{0xfe, 0xfe, 0xfe, 0x0}, {0xff, 0xff, 0xff, 0x0}
};


/****************************************************************************************\
 *          Array allocation, deallocation, initialization and access to elements         *
 \****************************************************************************************/
/* Allocates and initializes muImage_t header (not allocates data) */
muImage_t*  muCreateImageHeader( muSize_t size, MU_32S depth, MU_32S channels )
{
	muImage_t* img=0;

	img = (muImage_t*)malloc( sizeof( muImage_t ));
	if(img == NULL)
	{
		printf("muCreateImageHeader Failed!! buffer is NULL\n");
		return NULL;
	}

	img->channels   = channels;
	img->depth      = depth;
	img->dataorder  = MU_IMG_DATAORDER_PIXEL;
	img->origin     = MU_IMG_ORIGIN_TL;
	img->width      = size.width;
	img->height     = size.height;
	img->roi        = 0;
	img->imagedata  = 0;
	img->phyaddr	= 0;

	return img;
}


/* Allocates and initializes muImage_t header and data */
muImage_t* muCreateImage( muSize_t size, MU_32S depth, MU_32S channels )
{
	muImage_t* img = muCreateImageHeader(size, depth, channels);
	
	if(img == NULL)
	{
		printf("muCreateImage Failed!! buffer is NULL\n");
		return NULL;
	}

	img->imagedata = (MU_8U*)malloc((depth&0x0ff)*channels*size.width*size.height);

	return img;
}

/* Releases image header */
muError_t  muReleaseImageHeader( muImage_t** image )
{
	free( (*image) );

	return MU_ERR_SUCCESS;
}

/* Releases image header and data */
muError_t  muReleaseImage( muImage_t** image )
{

	free( (*image)->imagedata );

	free( (*image) );

	return MU_ERR_SUCCESS;
}

/* Returns width and height of image */
muSize_t muGetSize( const muImage_t* image )
{
	return muSize( image->width, image->height );
}

/* Clears all the image array elements (sets them to 0) */
muError_t muSetZero( muImage_t* image )
{
	memset( image->imagedata, 0, image->width*image->height*(image->depth&0x0ff)*image->channels);

	return MU_ERR_SUCCESS;
}

/****************************************************************************************\
 *          Dynamic Structure setting, create, insert, delete                             *
 \****************************************************************************************/

/* create a sequence with element size  */
muSeq_t* muCreateSeq(MU_32S elementsize)
{
	muSeq_t *seq;

	seq = (muSeq_t *)malloc(sizeof(muSeq_t));

	if(seq == NULL)
	{
		muDebugError(MU_ERR_NULL_POINTER);
	}

	seq->elem_size = elementsize;
	seq->first = NULL;
	seq->total = 0;

	return seq;
}


/* insert the sequence block to the last list */
muSeqBlock_t * muPushSeq(muSeq_t *seq, MU_VOID* element)
{

	muSeqBlock_t *sbprev, *sbcurrent, *sbhead;


	if(seq->first == NULL)
	{
		sbcurrent = (muSeqBlock_t *)malloc(sizeof(muSeqBlock_t));

		seq->first = sbcurrent;

		sbcurrent->next = NULL;
		sbcurrent->prev = NULL;

		sbcurrent->data = (MU_VOID *)malloc(seq->elem_size);
		printf("element size =%d\n", seq->elem_size);

		if(element != NULL)
			memcpy(sbcurrent->data, element, seq->elem_size);

	}
	else
	{	
		sbhead = seq->first;

		while(sbhead != NULL)
		{
			sbprev = sbhead;
			sbhead = sbhead->next;
		}

		sbcurrent = (muSeqBlock_t *)malloc(sizeof(muSeqBlock_t));

		sbcurrent->data = (MU_VOID *)malloc(seq->elem_size);

		if(element != NULL)
			memcpy(sbcurrent->data, element, seq->elem_size);	

		sbprev->next = sbcurrent;

		sbcurrent->next = NULL;
		sbcurrent->prev = NULL;
	}

	return sbcurrent;

}


/* insert the sequence block to the front */
muSeqBlock_t * muPushSeqFront(muSeq_t *seq, MU_VOID* element)
{

	muSeqBlock_t  *sbcurrent, *sbhead;


	if(seq->first == NULL)
	{
		sbcurrent = (muSeqBlock_t *)malloc(sizeof(muSeqBlock_t));

		seq->first = sbcurrent;

		sbcurrent->next = NULL;
		sbcurrent->prev = NULL;

		sbcurrent->data = (MU_VOID *)malloc(seq->elem_size);

		if(element != NULL)
			memcpy(sbcurrent->data, element, seq->elem_size);

	}
	else
	{	
		sbhead = seq->first;

		sbcurrent = (muSeqBlock_t *)malloc(sizeof(muSeqBlock_t));

		sbcurrent->data = (MU_VOID *)malloc(seq->elem_size);

		if(element != NULL)
			memcpy(sbcurrent->data, element, seq->elem_size);	

		seq->first = sbcurrent;
		sbcurrent->next = sbhead;
	}

	return sbcurrent;

}



/* clear the whole sequence */
MU_VOID muClearSeq(muSeq_t **seq)
{
	muSeqBlock_t *sbhead, *sbcurrent;

	if((*seq) == NULL)
	{
		return;
	}

	sbhead = (*seq)->first;
	sbcurrent = sbhead;

	while(sbhead != NULL)
	{
		sbhead = sbhead->next;

		free(sbcurrent->data);
		free(sbcurrent);
		sbcurrent = sbhead;
	}

	free((*seq));
	(*seq)=NULL;
}

/* Delete Nodde by index */
muError_t muRemoveIndexNode(muSeq_t **seq, MU_32S index)
{
	muSeqBlock_t *head, *prev, *current;
	MU_32S count=1, last=1;


	if(index<=0)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	//find the last index
	head = (*seq)->first;
	while(head != NULL)
	{
		last++;
		head = head->next;
	}

	head = (*seq)->first;
	current = head;

	while(head != NULL)
	{
		prev = head;
		head = head->next;
		if(index > last)
		{
			return MU_ERR_OUT_OF_MEMORY;
		}
		else if(index == 1)
		{
			(*seq)->first = head;
			free(current->data);
			free(current);
			return MU_ERR_SUCCESS;
		}
		else if(last == index)
		{
			prev->next = NULL;
			free(current->data);
			free(current);
			return MU_ERR_SUCCESS;
		}
		else if(count == index)
		{
			prev->next = head;
			free(current->data);
			free(current);
			return MU_ERR_SUCCESS;
		}

		count++;
		current = head;
	}


	return MU_ERR_SUCCESS;
}


/* Delete Nodde by address */
muError_t muRemoveAddressNode(muSeq_t **seq, muSeqBlock_t *ptr)
{

	muSeqBlock_t *head, *prev;

	if(ptr == NULL)
	{
		printf("muRemoveAddressNode = NULL\n");
		return MU_ERR_NULL_POINTER;
	}

	head = (*seq)->first;

	if(ptr == head)
	{
		(*seq)->first = head->next;	
	}
	else
	{
		prev = head;
		while(prev->next!=ptr)
			prev = prev->next;

		if(ptr->next == NULL)
		{
			prev->next = NULL;	
		}
		else
		{
			prev->next = ptr->next;

		}
	}

	free(ptr->data);
	free(ptr);

	return MU_ERR_SUCCESS;

}


/* delete the last sequence */
muError_t muSeqPop(muSeq_t **seq, MU_VOID *element)
{
	muSeqBlock_t *head, *prev, *current, *next, *last = NULL;

	head = (*seq)->first;
	prev = head;

	while(head != NULL)
	{
		last = head;
		current = head;

		//find the last but one node
		if(current->next!=NULL)
		{
			next = current->next;
			if(next->next == NULL);
			prev = current;
		}

		head = head->next;

	}

	if(element!=NULL)
	{
		memcpy(element, last->data, (*seq)->elem_size);
	}
	prev->next = NULL;

	if(last == NULL)
	{	
		muDebugError(MU_ERR_NULL_POINTER);
		return MU_ERR_NULL_POINTER;
	}

	free(last->data);
	free(last);


	return MU_ERR_SUCCESS;
}

/* get element by index */
MU_VOID* muGetSeqElement(muSeq_t **seq, MU_32S index)
{
	muSeqBlock_t *head, *prev, *current;
	MU_32S count=1, last=1;


	if(index<=0)
	{
		return NULL;
	}

	//find the last index
	head = (*seq)->first;
	while(head != NULL)
	{
		last++;
		head = head->next;
	}

	head = (*seq)->first;
	current = head;

	while(head != NULL)
	{
		prev = head;
		head = head->next;
		if(index > last)
		{
			return NULL;
		}

		if(count == index)
		{
			return current->data;
		}

		count++;
		current = head;
	}

	return NULL;

}


/****************************************************************************************\
 *          Load image                                                                    *
 \****************************************************************************************/
//----------------------------------------------------------------------------------------------------
// Read a bitmap, malloc memory space for data,
// and return data buffer pointer and some attributes of the bitmap.
// Intensity range of data buffer is 0 ~ 255.
//
//   *in_filename : Input bitmap file path for opening
//   *out_height  : Output attribute of the bitmap height
//   *out_width   : Output attribute of the bitmap width
//   *out_bitcount: Output attribute of the bitmap color depth
//                  8 means 8 bits, gray imaage
//                  24 means 24 bits, color image
//----------------------------------------------------------------------------------------------------
unsigned char *muReadBMP(const char *in_filename, int *out_height, int *out_width, int *out_bitcount)
{
	int i = 0, j = 0;
	int fix = 0;					// bmp width align 4 compliment
	int buf_size = 0;				// Size of data buffer
	int byte_per_pixel = -1;		// Byte per pixel
	int palette_number = 0;			// Palette number
	unsigned char *out_buf = NULL;	// Data buffer pointer
	unsigned char zero = '0';		// 補0用的變數
	FILE *infp = NULL;				// File pointer
	bitmapFileHeader_t fileheader;	// Bitmap file header
	bitmapInfoHeader_t infoheader;	// Bitmap file information header
	rgbQuad_t palette[256];			// Storge of palette


	// Open file to read and check the result
	infp = fopen(in_filename, "rb");
	if(infp == NULL)
	{
		printf("ReadBMP: File does not exist!\n");
		return NULL;
	}

	// Read bitmap file header
	// Beacuse memory alignment, sizeof(BITMAPFILEHEADER) = 16 not 14
	// So, we read data individually
	fread(&fileheader.bfType, sizeof(unsigned short), 1, infp);
	fread(&fileheader.bfSize, sizeof(unsigned int), 1, infp);
	fread(&fileheader.bfReserved1, sizeof(unsigned short), 1, infp);
	fread(&fileheader.bfReserved2, sizeof(unsigned short), 1, infp);
	fread(&fileheader.bfOffBits, sizeof(unsigned int), 1, infp);

	// Check it is BM or not
	// In little endian, BM becomes MB
	if(fileheader.bfType != 'MB')
	{
		printf("ReadBMP: Type is not 'BM'!\n");
		fclose(infp);
		return NULL;
	}

	// Read bitmap information head
	fread(&infoheader, sizeof(bitmapInfoHeader_t), 1, infp);

	// Check it is compression or not
	if(infoheader.biCompression != 0)
	{
		printf("ReadBMP: Only read no compression BMP!\n");
		fclose(infp);
		return NULL;
	}

	// Check its color type
	if(infoheader.biBitCount != 8 && infoheader.biBitCount != 24)
	{
		printf("ReadBMP: Only read 8 bits and 24 bits BMP!\n");
		fclose(infp);
		return NULL;
	}

	// Get some attributes for outputs
	*out_height = (int)(infoheader.biHeight);
	*out_width = (int)(infoheader.biWidth);
	*out_bitcount = (int)(infoheader.biBitCount);

	// Determine palette number and byte per pixel
	switch(*out_bitcount)
	{
		case 8:
			palette_number = 256;
			byte_per_pixel = 1;
			break;
		case 24:
			palette_number = 0;
			byte_per_pixel = 3;
			break;
	}


	// Skip palette part
	if(palette_number != 0)
		fread(palette, sizeof(rgbQuad_t), palette_number, infp);

	//offset to bmp data
	fseek(infp, fileheader.bfOffBits, SEEK_SET);

	// Malloc memory space for bitmap data
	buf_size = infoheader.biWidth * byte_per_pixel * infoheader.biHeight;
	out_buf = (unsigned char*)malloc(buf_size);
	if(out_buf == NULL)
	{
		printf("ReadBMP: Malloc memory space for BMP data failed!\n");
		fclose(infp);
		return NULL;
	}

	// Find fix byte number
	fix = (infoheader.biWidth * byte_per_pixel) % 4;
	if(fix != 0)
		fix = 4 - fix;

	// Read BMP data
	for(j=infoheader.biHeight-1; j>=0; j--)
	{
		for(i=0; i<infoheader.biWidth * byte_per_pixel; i++)
			fread(&out_buf[j * infoheader.biWidth * byte_per_pixel + i], sizeof(unsigned char), 1, infp);

		for(i=0; i<fix; i++)
			fread(&zero, sizeof(unsigned char), 1, infp);
	}

	fclose(infp);
	return out_buf;
}



//----------------------------------------------------------------------------------------------------
// Save a bitmap, and retrun 0 for save failed, 1 for success.
// Intensity range of data buffer is 0 ~ 255.
//   *in_filename : Input bitmap file path for saving
//   *in_buf      : Input data buffer pointer
//   in_height    : Input attribute of the bitmap height
//   in_width     : Input attribute of the bitmap width
//   in_bitcount  : Input attribute of the bitmap color depth
//                  1 means 1 bit, binary image, but we will save it as gray image
//                  8 means 8 bits, gray imaage
//                  24 means 24 bits, color image
//----------------------------------------------------------------------------------------------------
int saveBMP(const char *in_filename, unsigned char *in_buf, int in_height, int in_width, int in_bitcount)
{
	int i = 0, j = 0;
	int fix = 0;					// BMP width align 4 compliment
	int buf_size = 0;				// Size of data buffer
	int byte_per_pixel = -1;		// Byte per pixel
	int palette_number = 0;			// Palette number
	unsigned char zero = '0';		// zero buffer
	FILE *infp = NULL;				// File pointer
	bitmapFileHeader_t fileheader;	// Bitmap file header
	bitmapInfoHeader_t infoheader;	// Bitmap file information header

	// Check save bmp color type
	if(in_bitcount != 1 && in_bitcount != 8 && in_bitcount != 24)
	{
		printf("SaveBMP: Only save 1 bit, 8 bits and 24 bits BMP!\n");
		return 0;
	}

	// Check buffer exists or not
	if(in_buf == NULL)
	{
		printf("SaveBMP: Data buffer does not exist!\n");
		return 0;
	}

	// Open file to write and check result
	infp = fopen(in_filename, "wb");
	if(infp == NULL)
	{
		printf("SaveBMP: Open file pointer failed!\n");
		return 0;
	}

	// Determine palette number and byte per pixel
	switch(in_bitcount)
	{
		case 1:
		case 8:
			palette_number = 256;
			byte_per_pixel = 1;
			break;
		case 24:
			palette_number = 0;
			byte_per_pixel = 3;
			break;
	}

	// Find fix byte number
	fix = (in_width * byte_per_pixel) % 4;
	if(fix != 0)
		fix = 4 - fix;

	// Calculate buffer size for bitmap data
	buf_size = (in_width * byte_per_pixel + fix) * in_height;

	// Set bitmap file header
	fileheader.bfType = 'MB';
	fileheader.bfSize = 54 + (palette_number * sizeof(rgbQuad_t)) + buf_size;
	fileheader.bfReserved1 = 0;
	fileheader.bfReserved2 = 0;
	fileheader.bfOffBits = 54 + (palette_number * sizeof(rgbQuad_t));

	// Set bitmap information header
	infoheader.biSize = 40;
	infoheader.biWidth = in_width;
	infoheader.biHeight = in_height;
	infoheader.biPlanes = 1;
	if(in_bitcount == 1)
		infoheader.biBitCount = 8;
	else
		infoheader.biBitCount = in_bitcount;
	infoheader.biCompression = 0;
	infoheader.biSizeImage = buf_size;
	infoheader.biXPelsPerMeter = 0;
	infoheader.biYPelsPerMeter = 0;
	infoheader.biClrUsed = 0;
	infoheader.biClrImportant = 0;

	// Write bitmap file header
	// Beacuse memory alignment, sizeof(BITMAPFILEHEADER) = 16 not 14
	// So, we write data individually
	fwrite(&fileheader.bfType, sizeof(unsigned short), 1, infp);
	fwrite(&fileheader.bfSize, sizeof(unsigned int), 1, infp);
	fwrite(&fileheader.bfReserved1, sizeof(unsigned short), 1, infp);
	fwrite(&fileheader.bfReserved2, sizeof(unsigned short), 1, infp);
	fwrite(&fileheader.bfOffBits, sizeof(unsigned int), 1, infp);

	// Write bitmap information header
	fwrite(&infoheader, sizeof(bitmapInfoHeader_t), 1, infp);

	// Write palette
	if(in_bitcount == 1 || in_bitcount == 8)
		fwrite(GrayPalette, sizeof(rgbQuad_t), 256, infp);

	// Write data
	for(j=in_height-1; j>=0; j--)
	{
		for(i=0; i<in_width * byte_per_pixel; i++)
			fwrite(&in_buf[j * in_width * byte_per_pixel + i], sizeof(unsigned char), 1, infp);

		for(i=0; i<fix; i++)
			fwrite(&zero, sizeof(unsigned char), 1, infp);
	}

	fclose(infp);
	return 1;
}


/* Load BMP image */
muImage_t * muLoadBMP(const char *filename)
{
	MU_8U channels = 3;
	MU_32S width, height, bitcount;
	MU_8U *buffer;
	muSize_t size;
	muImage_t *image;

	buffer = muReadBMP(filename, &height, &width, &bitcount);

	size.height = height;
	size.width = width;


	switch(bitcount)
	{
		case 8:
			channels = 1;
			break;
		case 24:
			channels = 3;
			break;
	}

	image = muCreateImage(size, MU_IMG_DEPTH_8U, channels);

	memcpy(image->imagedata, buffer, width*height*channels);

	return image;
}


/* Save raw data to BMP formant */
muError_t muSaveBMP(const char *filename, muImage_t *image)
{
	MU_32S width, height, bitcount;

	width = image->width;
	height = image->height;

	switch(image->channels)
	{
		case 1:
			bitcount = 8;
			break;
		case 3:
			bitcount = 24;
			break;
	}

	if(!saveBMP(filename, image->imagedata, height, width, bitcount))
	{
		printf("error\n");
		return MU_ERR_INVALID_PARAMETER;
	}

	return MU_ERR_SUCCESS;
}

/****************************************************************************************\
 *          get sub image to dst (YUV422)                                                         *
 \****************************************************************************************/
muError_t muGetSubYUV422Image(const muImage_t *src, muImage_t *dst, const muRect_t rect)
{
	MU_32S a,b,x,y;
	MU_32S width, height;
	MU_32S offsetY, offsetU;
	MU_32S offsetSubY, offsetSubU;
	MU_8U *in, *out;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}
	if(src->channels != 3 || dst->channels != 3)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	if((rect.width %2 != 0) || (rect.height%2 != 0))
	{
		muDebugError(MU_ERR_NOT_SUPPORT);
		return MU_ERR_NOT_SUPPORT;
	}

	width = src->width;
	height = src->height;
	in = src->imagedata;
	out = dst->imagedata;

	offsetY = width * height;
	offsetU = width * height + (width*height/2);

	offsetSubY = dst->width * dst->height;
	offsetSubU = dst->width * dst->height + (dst->width*dst->height/2);

	for(a=0,x=rect.x;a<rect.width;x++,a++)
	{
		for(b=0,y=rect.y;b<rect.height;y++,b++)
		{
			out[(x-rect.x)+rect.width*(y-rect.y)] = in[x+width*y];
		}
	}
	//Get UV 
	for(a=0,x=(rect.x/2);a<(rect.width/2);x++,a++)
	{
		for(b=0, y=rect.y; b<rect.height; y++, b++)
		{
			out[offsetSubY + (x-(rect.x/2))+(rect.width/2)*(y-rect.y)] = in[offsetY + x+(width/2)*(y)];
			out[offsetSubU + (x-(rect.x/2))+(rect.width/2)*(y-rect.y)] = in[offsetU + x+(width/2)*(y)];
		}
	}


	return MU_ERR_SUCCESS;
}

/****************************************************************************************\
 *          get sub image to dst                                                          *
 \****************************************************************************************/
muError_t muGetSubImage(const muImage_t *src, muImage_t *dst, const muRect_t rect)
{
	MU_32S a,b,x,y;
	MU_32S width;
	MU_8U *in, *out;
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
	in = src->imagedata;
	out = dst->imagedata;


	for(a=0,x=rect.x;a<rect.width;x++,a++)
	{
		for(b=0,y=rect.y;b<rect.height;y++,b++)
		{
			out[(x-rect.x)+rect.width*(y-rect.y)] = in[x+width*y];
		}
	}


	return MU_ERR_SUCCESS;
}

/****************************************************************************************\
 *          Remove the sub image from src                                                 *
 \****************************************************************************************/
muError_t muRemoveSubImage(muImage_t *src, const muRect_t rect, const MU_32S replaceVal)
{
	MU_32S a,b,x,y;
	MU_32S width;
	MU_8U *in, *out;
	muError_t ret;


	if(src->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}

	width = src->width;
	in = src->imagedata;

	for(a=0,x=rect.x;a<rect.width;x++,a++)
	{
		for(b=0,y=rect.y;b<rect.height;y++,b++)
		{
			in[x+width*y] = replaceVal;
		}
	}

	return MU_ERR_SUCCESS;
}
/* Return the particular element of image (idx0:height, idx1:width, idx2:channel) */
/* The functions return 0 if the requested node does not exist */
MU_32S muGet2D(const muImage_t *image, MU_32S idx0, MU_32S idx1)
{
	MU_32S height = image->height;
	MU_32S width = image->width;
	MU_32S channels = image->channels;

	if( channels!=1 || height<=idx0 || width<=idx1 )
	{
		return 0;
	}
	else
	{
		return image->imagedata[idx0*width+idx1];
	}
}

MU_32S muGet3D(const muImage_t *image, MU_32S idx0, MU_32S idx1, MU_32S idx2)
{
	MU_32S height = image->height;
	MU_32S width = image->width;
	MU_32S channels = image->channels;

	if( channels<=idx2 || height<=idx0 || width<=idx1 )
	{
		return 0;
	}
	else
	{
		return image->imagedata[(idx0*width+idx1)*channels+idx2];
	}
}


/************************************************************************************************\
 *          Checking function																	*
 *          Author Joe																			*
 *			amount = number of input, must be an even											*
 *			For Example: muCheckDepth(8, a, DEPTH_8U, b, DEPTH_16U, c, DEPTH_32U, d, DEPTH_8U) *
 *																								*
 \**********************************************************************************************/
/* check the input parameter with image depth */
muError_t muCheckDepth(MU_32S amount, ...)
{
	MU_32S i;
	MU_32S temp;
	MU_32S count, depth;
	muImage_t *buf = NULL;
	va_list vl;
	
	if((amount <= 0) || (amount % 2 != 0))
	{
		printf("[MU_LIB] \"muCheckDepth\" parametaer amount must be an even and larger than zero\n");
		return MU_ERR_INVALID_PARAMETER;
	}

	va_start(vl, amount);

	for(i=0, count = 1; i<amount; i++, count++)
	{
		if(count % 2 == 0)
		{
			temp = va_arg(vl, int);
			depth = temp;

			if(buf == NULL)
			{
				printf("[MU_LIB]\"muCheckDepth\" buffer = NULL\n");
				return MU_ERR_NULL_POINTER;
			}

			if((buf->depth & 0x00F) != (depth & 0x00F))
			{
				printf("[MU_LIB] \"muCheckDepth\" muimage->depth %x != depth %x\n", buf->depth, depth);
				return MU_ERR_INVALID_PARAMETER;
			}
		}
		else
		{
			buf = (muImage_t *)va_arg(vl, muImage_t *);
		}
	}

	va_end(vl);

	return MU_ERR_SUCCESS; 
}

/****************************************************************************************\
 *          debug function                                                                *
 \****************************************************************************************/
/* Debug function for error code displaying */
MU_VOID muDebugError(muError_t errorcode)
{
	switch(errorcode)
	{
		case MU_ERR_INVALID_PARAMETER : 
			printf("Invalid parameter\n");
			break;
		case MU_ERR_NULL_POINTER :
			printf("Null pointer or pointer error\n");
			break;
		case MU_ERR_OUT_OF_MEMORY :
			printf("Memory leak or out of memory\n");
			break;
		case MU_ERR_NOT_SUPPORT :
			printf("No support\n");
			break;
		case MU_ERR_UNKNOWN :
			printf("Unknow error\n");
			break;
		default :
			printf("Error code is no defined\n");
			break;

	}

}
