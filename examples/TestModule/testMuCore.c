#include "testModule.h"



static int fileExist(char *fileName)
{
	FILE *fp;
	fp = fopen(fileName, "r");
	if(fp == NULL)
	{
		logInfo("%s file doesn't exist\n", fileName);
		return -1;
	}
	return 0;
}


int testRGB2HSV(char *rgbFile)
{
	muImage_t  *rgb = NULL;
	muImage_t  *hsv = NULL;
	muImage_t *testPattern = NULL;
	MU_32S ret;
	muSize_t size;
	MU_32S i, j;
	MU_16U *temp;
	MU_8U *bmp;
	MU_8U *pat;
	MU_16U cor[48];

	ret = fileExist(rgbFile);
	if(ret)
	{
		return 1;
	}

	//test case 1: load bmp and transfer to hsv;
	rgb = muLoadBMP(rgbFile);
	size.width = rgb->width;
	size.height = rgb->height;

	hsv = muCreateImage(size, MU_IMG_DEPTH_16U, 3);

	printf("test case 1:\n");

	muRGB2HSV(rgb, hsv);

	temp = (MU_16U *)hsv->imagedata;
	bmp = (MU_8U *)rgb->imagedata;

	if(rgb)
		muReleaseImage(&rgb);
	if(hsv)
		muReleaseImage(&hsv);

	//test case 2: test pattern;
	
	size.width = 1;
	size.height = 16;
	testPattern = muCreateImage(size, MU_IMG_DEPTH_8U, 3);
	hsv = muCreateImage(size, MU_IMG_DEPTH_16U, 3);

	pat = (MU_8U *)testPattern->imagedata;
	//b            g              r
	pat[0] = 0;    pat[1] = 0;    pat[2] = 0;
	pat[3] = 255;  pat[4] = 255;  pat[5] = 255;
	pat[6] = 0;    pat[7] = 0;    pat[8] = 255;
	pat[9] = 0;    pat[10] = 255; pat[11] = 0;
	pat[12] = 255; pat[13] = 0;   pat[14] = 0;
	pat[15] = 0;   pat[16] = 255; pat[17] = 255;
	pat[18] = 255; pat[19] = 255; pat[20] = 0;
	pat[21] = 255; pat[22] = 0;   pat[23] = 255;
	pat[24] = 192; pat[25] = 192; pat[26] = 192;
	pat[27] = 128; pat[28] = 128; pat[29] = 128;
	pat[30] = 0;   pat[31] = 0;   pat[32] = 128;
	pat[33] = 0;   pat[34] = 128; pat[35] = 128;
	pat[36] = 0;   pat[37] = 128; pat[38] = 0;
	pat[39] = 128; pat[40] = 0;   pat[41] = 128;
	pat[42] = 128; pat[43] = 128; pat[44] = 0;
	pat[45] = 128; pat[46] = 0;   pat[47] = 0;


	//h           s               v
	cor[0] = 0;    cor[1] = 0;    cor[2] = 0;
	cor[3] = 0;    cor[4] = 0;    cor[5] = 100;
	cor[6] = 0;    cor[7] = 100;  cor[8] = 100;
	cor[9] = 120;  cor[10] = 100; cor[11] = 100;
	cor[12] = 240; cor[13] = 100; cor[14] = 100;
	cor[15] = 60;  cor[16] = 100; cor[17] = 100;
	cor[18] = 180; cor[19] = 100; cor[20] = 100;
	cor[21] = 300; cor[22] = 100; cor[23] = 100;
	cor[24] = 0;   cor[25] = 0;   cor[26] = 75;
	cor[27] = 0;   cor[28] = 0;   cor[29] = 50;
	cor[30] = 0;   cor[31] = 100; cor[32] = 50;
	cor[33] = 60;  cor[34] = 100; cor[35] = 50;
	cor[36] = 120; cor[37] = 100; cor[38] = 50;
	cor[39] = 300; cor[40] = 100; cor[41] = 50;
	cor[42] = 180; cor[43] = 100; cor[44] = 50;
	cor[45] = 240; cor[46] = 100; cor[47] = 50;

	muRGB2HSV(testPattern, hsv);

	temp = (MU_16U *)hsv->imagedata;
	bmp = (MU_8U *)testPattern->imagedata;

	printf("\n\ntest case2\n");
	for(i=0; i<testPattern->width*testPattern->height*3; i+=3)
	{
		printf("r:%d g:%d b:%d -> h:%d s:%d v:%d\n", bmp[i], bmp[i+1], bmp[i+2], temp[i], temp[i+1], temp[i+2]);
		if((temp[i] != cor[i]) || (temp[i+1] != cor[i+1]) || (temp[i+2] != cor[i+2]))
			return -1;
	}

	if(testPattern)
		muReleaseImage(&testPattern);
	if(hsv)
		muReleaseImage(&hsv);


	return 0;
}


int testMuCore()
{

}
