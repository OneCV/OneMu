/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <math.h>
#include "cvtypes.h"
#include "cvbase.h"
#include "cvcore.h"
#include "cvapp.h"
#include "MarkCascadeModel_40x25.h"

static CvExaminator gExaminator; 


static CV_32S DrawBoundingBox(cvImage_t *input, cvRect_t rect)
{
	CV_32S width, height;
	CV_32S corx=0, cory=0, maxcorx=0, maxcory=0;

	width = input->width;
	height = input->height;
	
	//ready to draw the bounding box
	corx = rect.x;//show the Region position.
	cory = rect.y;
	maxcorx = corx + (rect.width - 1);
	maxcory = cory + (rect.height - 1);
	for(cory; cory <= maxcory; cory++)//region the object with white line in WholeBackproject and GrayImg.
	{         
		for(corx; corx <= maxcorx; corx++)
		{				
			if( (cory == rect.y) || (cory == maxcory) )
				input->imagedata[ cory * width + corx ] = 255;
			else
				if( (corx == rect.x) || (corx == maxcorx) )
					input->imagedata[ cory * width + corx ] = 255;

		}

		corx = rect.x;
	}
	return 0;
}

JNIEXPORT jboolean Java_com_oneVipas_onedetector_CameraPreview_cvObjectDetectionInit(JNIEnv * env, jobject thiz, jlong length, jbyteArray examBuffer)
{
	jbyte *pExamBuffer = NULL;	
	char fileName[50] = {0};
	FILE *img = NULL;
	int ret = 0;
	unsigned char *buffer = NULL;
	int bufLength;
	cvSize_t size;

#if 0
	sprintf(fileName, "\/storage\/sdcard1\/Examinator.dk");
	__android_log_print(ANDROID_LOG_INFO, "CV_JNIMSG", "open file %s", fileName);

	img = fopen(fileName, "rb");
	if(img == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "CV_JNIMSG", "open failed");
		return CV_FALSE;
	}
	
	fseek(img, 0L, SEEK_END);
	bufLength = ftell(img);
	fseek(img, 0L, SEEK_SET);


	buffer = (unsigned char *)malloc(bufLength*sizeof(unsigned char));
	fread(buffer, 1, bufLength, img);

	ret = memcmp(buffer, pExamBuffer, bufLength);
	if(ret){
		__android_log_print(ANDROID_LOG_INFO, "CV_JNIMSG", "buffer not match!");	
	}
	
	if(img != NULL)
	fclose(img);

	if(buffer != NULL)
		free(buffer);
#endif //debug


	pExamBuffer = (*env)->GetByteArrayElements(env, examBuffer, 0);
	
	__android_log_print(ANDROID_LOG_INFO, "CV_JNIMSG", "length = %d", length);

	if(pExamBuffer != NULL)
	{
		Examinator_Init_Buf(pExamBuffer, &gExaminator);	
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "CV_JNIMSG", "buffer  = null");	

		return 1;
	}

	//cvObjectDetectionInit(&MarkCascade, MarkCascadeStages, MarkCascadeClassifiers, Mark_CascadeParaTable);
}

#if 0
JNIEXPORT jboolean Java_com_oneVipas_onedetector_CameraPreview_getDetectorResult(JNIEnv *env, jobject thiz, jobject jRect, jint index)
{

	rectClass = (*env)->GetObjectClass(env, jRect);
	if(rectClass == 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "CV_JNIMSG", "return jRect failed");
		return CV_FALSE;
	}

	fid = (*env)->GetFieldID(env, rectClass, "x", "I");
	(*env)->SetIntField(env, jRect, fid, rect.x);	

	fid = (*env)->GetFieldID(env, rectClass, "y", "I");
	(*env)->SetIntField(env, jRect, fid, rect.y);

	fid = (*env)->GetFieldID(env, rectClass, "width", "I");
	(*env)->SetIntField(env, jRect, fid, rect.width);

	fid = (*env)->GetFieldID(env, rectClass, "height", "I");
	(*env)->SetIntField(env, jRect, fid, rect.height);

}
#endif

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
 // package name: com_example_hellojni
 // class name: HelloJni
JNIEXPORT jint Java_com_oneVipas_onedetector_CameraPreview_cvipProcessing( JNIEnv* env, jobject thiz, jint width, jint height, jbyteArray frameData)
{
	cvImage_t *src = NULL, *dimg = NULL;
	cvImage_t *out = NULL;
	cvSize_t size;
	cvSize_t dSize, tSize;
	cvRect_t *rect;
	cvRect_t *label, *label2;
	cvSize_t min, max;
	CV_8U data;
	cvSeq_t *detectList = NULL;
	cvSeqBlock_t *head, *current, *head2, *current2;
	cvBoundingBox_t box1, box2;
	CV_32S overlapSize;
	CV_32S tempX = 0, tempY = 0, tempWidth = 0, tempHeight = 0;
	int i = 0, j;
	int scalew = 0, scaleh = 0;
	int noneOverlap = 0;
	FILE *img = NULL;
	static int count = 0;
	char fileName[50];
	int ret;
	jclass rectClass;
	jfieldID fid;
	jint numTarget = 0;
	cvSeq_t *objects;
	

	// reassign the point from java to native C
	jbyte *pFrameData = (*env)->GetByteArrayElements(env, frameData, 0);
	
	__android_log_print(ANDROID_LOG_INFO, "CV_JNIMSG", "CV JNI!!!");

	size.width = width;
	size.height = height;

	gExaminator.ExamData.img = cvCreateImageHeader(size, CV_IMG_DEPTH_8U, 1);

	gExaminator.ExamData.img->imagedata = pFrameData;

	src = gExaminator.ExamData.img;

	Examinator_Run(&gExaminator);

	for(i=0; i<gExaminator.ExamData.TagNum; i++)
	{
		objects = gExaminator.Detector[i].Objects;
		if(objects != NULL)
		{
			j = 0;
			current = objects->first;
			while(current != NULL)
			{
				rect = (cvRect_t *)current->data;
				__android_log_print(ANDROID_LOG_INFO, "CV_JNIMSG", "CV JNI!!! i=%d idx=%d x =%d y =%d width =%d height =%d",i, j ,rect->x, rect->y, rect->width, rect->height);
				current = current->next;
			}
			j++;
		}
	}


	if(src != NULL)
		cvReleaseImageHeader(&src);

	// risk JNI_COMMIT
	(*env)->ReleaseByteArrayElements(env, frameData, pFrameData, 0);

    return numTarget;
}
