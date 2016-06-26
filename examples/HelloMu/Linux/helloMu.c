
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
 * Module: helloMu.c
 * Author: Joe Lin
 *
 * Description:
 *    This is a Linux environment build example
 *
 -------------------------------------------------------------------------- */

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
