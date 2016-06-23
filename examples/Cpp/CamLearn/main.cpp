#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "muGadget.h"

using namespace cv;

/** Function Headers */
void onMouse(int Event,int x,int y,int flag,void* param);
void transMatToMu(Mat &src, muImage_t* rst);
int setROI(muRect_t &Sample);
void disDct(muSeq_t *MuObject, Mat &frame);

//=========================
//Record mouse event
//=========================
CvPoint VertexOne,VertexThree;//save first point and end point
//=========================

int main( int argc, const char** argv )
{
  int error = 0;
  Mat frame;
  Mat frame_gray;
  muImage_t *MuImg;
  char window_name[10] = "Capture";
  muRect_t Sample;
  VideoCapture capture(0);

  //Flag
  bool LearnFlag = false;
  bool DetectFlag = false;
  bool CloseFlag = false;

  //=============Initial parameter for mouse==============
  VertexOne=cvPoint(0,0);
  VertexThree=cvPoint(0,0);

  printf("//======================================// \n");
  printf("1 : Start Learning with Green Box \n");
  printf("2 : Mu~ Detect \n");
  printf("3 : Reset\n");
  printf("Esc : Exit\n");
  printf("//======================================// \n");

  cvNamedWindow(window_name,0);
  cvSetMouseCallback(window_name,onMouse,NULL);//make a handle to monitor mouse event
  MuSimpleDetector* MuDetector=NULL;

  if( capture.isOpened() )
  {
    while( !CloseFlag )
    {
  	  //Get frame image and transfer to gray
      capture.read(frame);
      cvtColor( frame, frame_gray, CV_BGR2GRAY );

      if( !frame.empty() )
      { 
        //Draw current selected rectangle
    		rectangle(frame,VertexOne,VertexThree,CV_RGB(0,255,0),3,CV_AA);
        
        if(LearnFlag)//User triger learning process
        {
          error = setROI(Sample);
          if(error == -1)
          {
            LearnFlag = false;
            printf("ERROR : Please Choose ROI to Learn\n");
          }
          else
          {
            //Negtive image for learning
            muImage_t *MuNeg;
            Mat NegPic1 = imread("NegPic1.jpg", CV_LOAD_IMAGE_COLOR);
            cvtColor( NegPic1, NegPic1, CV_BGR2GRAY );

            muSize_t ImSize = {frame_gray.cols, frame_gray.rows};
            MuImg = muCreateImage( ImSize, MU_IMG_DEPTH_8U, 1 );
            transMatToMu(frame_gray,MuImg);

            muSize_t NegImSize = {NegPic1.cols, NegPic1.rows};
            MuNeg = muCreateImage( NegImSize, MU_IMG_DEPTH_8U, 1 );
            transMatToMu(NegPic1,MuNeg);
            
            //=====================
            //Mu learning function
            //=====================
            printf("Start Learning\n");
            muObjectLearning_Init( MuImg, &Sample, MuNeg); //Boost
            printf("Learning Done!\n");

            //Clear Mouse, Flags and Release Images
            VertexOne = VertexThree = cvPoint(0,0);
            DetectFlag = false;
            LearnFlag = false;
            muReleaseImage(&MuImg);
            muReleaseImage(&MuNeg);
          }
        }
        else if (!LearnFlag && DetectFlag)
        {
		      //Get MuImage
          muSize_t ImSize = {frame_gray.cols, frame_gray.rows};
          MuImg = muCreateImage( ImSize, MU_IMG_DEPTH_8U, 1 );
          transMatToMu(frame_gray,MuImg);

          //Detect
          int WidthScale = MuDetector->orig_window_size.width;
          int HeightScale = MuDetector->orig_window_size.height;

          muSize_t min = {WidthScale-10,HeightScale-10};
          muSize_t max = {WidthScale+1,HeightScale+1};

          muSeq_t *MuObject = muObjectDetection(MuImg, MuDetector, 1.05, min, max);
          muMergeRectangles(MuObject, 3, 2);

          disDct(MuObject, frame);
          muClearSeq(&MuObject);
          muReleaseImage(&MuImg);
        }

    	  imshow( window_name, frame );
  	  }
      else
      { printf(" GG No Frame! Bye^^ "); break; }
	   

//======================================
      int keyFlag = waitKey(10);
      
      switch ((char)keyFlag)
      {
        case '1': //Get positive sample and learn
        {
          LearnFlag = true;
          DetectFlag = false;
          break;
        }
        case '2': //Start detection (read model)
        {
          if(MuDetector!=NULL)
            muReleaseSimpleDetector(MuDetector);

          //Load Model
          MuDetector = muLoadSimpleDetector( "MuDetector.txt" );
          printf("Detect\n");
          DetectFlag = true;
          break;
        }
        case '3': //Reset
        {
          printf("Reset\n");
          LearnFlag = false;
          DetectFlag = false;
          if(MuDetector!=NULL)
          {
            muReleaseSimpleDetector(MuDetector);
            MuDetector = NULL;
          }
          break;
        }
        case 27:
        {
          CloseFlag = true;
          muReleaseSimpleDetector(MuDetector);
          break;
        }

        default:
          break;
      }
      //======================================
    }
	  capture.release();
    cvDestroyWindow(window_name);
  }

  return 0;
}

void onMouse(int Event,int x,int y,int flag,void* param)
{
    if(Event==CV_EVENT_LBUTTONDOWN||Event==CV_EVENT_RBUTTONDOWN){//start point
        VertexOne = VertexThree = cvPoint(0,0);
        VertexOne=cvPoint(x,y);
    }
    if(Event==CV_EVENT_LBUTTONUP||Event==CV_EVENT_RBUTTONUP){//end point
        VertexThree=cvPoint(x,y);
    }
    if(flag==CV_EVENT_FLAG_LBUTTON||flag==CV_EVENT_FLAG_RBUTTON){//save shift
        VertexThree=cvPoint(x,y);
    }
}

void transMatToMu(Mat &src, muImage_t* rst)
{
    int i;   
    //rst->imagedata = src.data;
    for(i=0;i<rst->width*rst->height;i++)
      rst->imagedata[i] = src.data[i];
}

int setROI(muRect_t &Sample)
{
  int DrawWidth = abs(VertexThree.x - VertexOne.x);
  int Drawheight = abs(VertexThree.y - VertexOne.y);
  if(DrawWidth < 10 || Drawheight < 10)
    return -1;

  Sample.width = DrawWidth;
  Sample.height = Drawheight;

  Sample.x = VertexOne.x<VertexThree.x? VertexOne.x:VertexThree.x;
  Sample.y = VertexOne.y<VertexThree.y? VertexOne.y:VertexThree.y;

  printf("Sample( X = %d, Y = %d) \n",Sample.x,Sample.y);
  printf("Sample( width = %d, height = %d)\n",Sample.width,Sample.height);

  return 0;
}

void disDct(muSeq_t *MuObject, Mat &frame)
{
  muRect_t *rectp;
  muSeqBlock_t *current = MuObject->first;

  while(current != NULL)
  {
    rectp = (muRect_t*) current->data;
    rectangle(frame, Point(rectp->x,rectp->y),Point(rectp->x+rectp->width,rectp->y+rectp->height),Scalar( 255, 0, 100 ),5,8);
    current = current->next;
  }
}