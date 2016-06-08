#include "muGadget.h"


int main()
{
	muImage_t *testImage;
	muSize_t size;
	muError_t ret;

	size.width = 720;
	size.height = 480;

	testImage = muCreateImage(size, MU_IMG_DEPTH_8U, 1);

	//save a whole black pictur
	ret = muSaveBMP("testImage.bmp", testImage);
	if (ret != MU_ERR_SUCCESS)
	{
		MU_DBG("save testImage error\n");
	}
	
	muReleaseImage(&testImage);
	
	MU_DBG("helloMU done!\n");

	return 0;
}
