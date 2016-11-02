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

int testDrawRectangle(char* rgbFile)
{
	muImage_t *testImage;
	muSize_t size;
	muError_t ret;
	muPoint_t p1;// = NULL;
	muPoint_t p2;// = NULL;
	
	ret = fileExist(rgbFile);
	if(ret)
	{
		return 1;
	}
	
	testImage = muLoadBMP(rgbFile);

	p1.x = 7;
	p1.y = 10;

	p2.x = 160;
	p2.y = 99;

	ret = muDrawRectangle(testImage, p1, p2, 'b');

	if (ret != MU_ERR_SUCCESS)
	{
		MU_DBG("Draw error\n");
	}


	//save a whole black pictur
	ret = muSaveBMP("2.bmp", testImage);
	if (ret != MU_ERR_SUCCESS)
	{
		MU_DBG("save testImage error\n");
	}

	return 0;
}


int testMuBase()
{

}
