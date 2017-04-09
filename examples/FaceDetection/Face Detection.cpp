#include "iostream"
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/imgproc/imgproc.hpp>
#include "muCore.h"
#include "muGadget.h"
using namespace cv;

void transMatToMU(Mat &src, muImage_t & rst);

int main(){
	Mat frame = imread("FacesTest.jpg", 3);
	if(frame.data)
		printf("image load successfully!\n\n");
	else{
		printf("can't find the image!\n");
		return -1;
	}
	Mat grayImg;
	cvtColor(frame,grayImg,CV_RGB2GRAY);
	Mat tmpImg;
	frame.copyTo(tmpImg);
	imshow("img", frame);
	
	muImage_t MuImg;
	transMatToMU(grayImg, MuImg);

	//Load face detector
	MuSimpleDetector* FaceDetector = muLoadSimpleDetector( "haarcascade_frontalface_alt.txt" );

	//Run face detector, search faces from 20x20 to 300x300
	static muSize_t min = {20,20};
	static muSize_t max = {300,300};
	muSeq_t *MuObject = muObjectDetection(&MuImg, FaceDetector, 1.1, min, max);
	muMergeRectangles(MuObject, 3, 2);

	muRect_t *rectp;
	muSeqBlock_t *current = MuObject->first;
	while(current != NULL)
	{
		rectp = (muRect_t*) current->data;
		rectangle(tmpImg, Point(rectp->x,rectp->y),Point(rectp->x+rectp->width,rectp->y+rectp->height),Scalar( 255, 0, 255 ),2,8);
		current = current->next;
	}

	muReleaseSimpleDetector(FaceDetector);

	muClearSeq(&MuObject);

	imshow("result", tmpImg);
	imwrite("result.jpg", tmpImg);

	waitKey(0);
	return 1;
}

void transMatToMU(Mat &src, muImage_t & rst)
{
    rst.channels = src.channels();
    rst.depth = src.depth();
    rst.width = src.cols;
    rst.height = src.rows;
    rst.imagedata = src.data;
}