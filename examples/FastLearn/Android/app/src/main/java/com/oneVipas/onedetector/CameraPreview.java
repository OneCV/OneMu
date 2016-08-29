package com.oneVipas.onedetector;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Paint;
import android.graphics.PorterDuff.Mode;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.SurfaceHolder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

import static java.lang.Math.abs;

public class CameraPreview implements SurfaceHolder.Callback, Camera.PreviewCallback{

	private int gPreviewWidth, gPreviewHeight;
    private int gDispWidth, gDispHeight;
	private int imageFormat;
	private final String tag = "oneDetector";
	private boolean bProcessing = false;
	private byte[] frameData = null;
    private int resultData[] = new int[201];
	private SurfaceHolder gDrawHolder;
    private int taskStatus;
	Handler mHandler = new Handler(Looper.getMainLooper());
	public native boolean cvObjectDetectionInit(long length, byte[] examBuffer);
	public rectObject rectClass = new rectObject();
	public Camera mCamera = null;
    public final int TRAINING_TASK = 1;
    public final int CVPROCESS_TASK = 2;
    public static boolean initFlag = false;
    public final int previewWidth = 640, previewHeight = 480;

	
	public class rectObject{
		public int x;
		public int y;
		public int width;
		public int height;
	}

    public boolean DetectionInit(byte[] dkFile)
    {
        boolean ret = false;
        if(dkFile != null) {
            ret = cvObjectDetectionInit((long)dkFile.length, dkFile);
            if(ret){
                initFlag = true;
            }
        }
        return ret;
    }

	public CameraPreview(int dispWidth, int dispHeight, SurfaceHolder drawHolder)
	{
        gDispWidth = dispWidth;
        gDispHeight = dispHeight;
        gDrawHolder = drawHolder;
	}
	
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
        int supportFlag = 0;
        int tempW = 99999, tempH = 99999;
		Log.i(tag, "surfaceCreated");
		mCamera = Camera.open();

        for(int i=0; i< mCamera.getParameters().getSupportedPreviewSizes().size(); i++){
            Camera.Size CSize = mCamera.getParameters().getSupportedPreviewSizes().get(i);
            Log.i(tag, "supported preview size: " + CSize.width + " " + CSize.height);

            if(CSize.width == previewWidth && CSize.height == previewHeight){
                Log.i(tag, "preview size = 640x480");
                gPreviewWidth = previewWidth;
                gPreviewHeight = previewHeight;
                supportFlag = 1;
                break;

            }else{
                int diffW = abs(CSize.width-previewWidth);
                int diffH = abs(CSize.height-previewHeight);
                if(diffW < tempW){
                    tempW = diffW;
                    gPreviewWidth = CSize.width;
                }
                if(diffH < tempH){
                    tempH = diffH;
                    gPreviewHeight = CSize.height;
                }
            }
        }
        if(supportFlag == 0){
            Log.i(tag, "unSupport previewSize");
            Log.i(tag, "previewWidth ="+gPreviewWidth+"previewHeight = "+gPreviewHeight);
        }

        Parameters params;
        params = mCamera.getParameters();
        params.setPreviewSize(gPreviewWidth, gPreviewHeight);
        try	{
            mCamera.setParameters(params);
        }
        catch(Exception e) {
            Log.w(tag,e.toString());
        }
        Camera.Size size = mCamera.getParameters().getPreviewSize();
        Log.i(tag, "camera getpreview size w:"+size.width+"h:"+size.height);

		try
		{
			// bind surface holder to camera preview display
			mCamera.setPreviewDisplay(holder);
			//Camera.PreviewCallback -- for endless loop catch image
			mCamera.setPreviewCallback(this);
		}
		catch (IOException e) // exception, failed for release camera
		{
			mCamera.release();
			mCamera = null;
		}
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// camera parameters maybe runtime change, get latest parameters and reset the camera.
		Log.i(tag, String.format("surfaceChanged width =%d height = %d", width, height));
		Parameters params;
		params = mCamera.getParameters();
		imageFormat = params.getPreviewFormat();
        params.setFocusMode(Parameters.FOCUS_MODE_AUTO);
		params.setPreviewSize(gPreviewWidth, gPreviewHeight);
		try	{
			mCamera.setParameters(params);
		}
		catch(Exception e) {
			Log.w(tag,e.toString());
		}
		mCamera.startPreview();

	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
	    try {
            mCamera.stopPreview();
            mCamera.setPreviewCallback(null);
            mCamera.release();
            mCamera = null;
        }catch (Exception e) {
            e.printStackTrace();
        }
    }

	protected void onPause() {
		if(mCamera!=null)
		{
			Log.i(tag, "previewclass:onPause CameraPreview Success");
			mCamera.stopPreview();
		}
		else
			Log.i(tag, "previewclass:onPause CameraPreview Failed");				
		
	}
	protected void onResume() {
		if(mCamera!=null)
		{
			Log.i(tag, "previewclass:onResume CameraPreview Success");
			mCamera.startPreview();
		}
		else
			Log.i(tag, "previewclass:onResume CameraPreview Failed");
	}

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
        // preview call back for camera set callback, endless loop for catch image.
        if (imageFormat == ImageFormat.NV21)// camera preview format.
        {
            if (!bProcessing) {
                frameData = data;

                if((taskStatus == CVPROCESS_TASK) && initFlag)
                    mHandler.post(cvProcess);
            }

        }
    }

    public void autofocus(){
        try {
            mCamera.autoFocus(null);
        }catch (Exception err){
            Log.d(tag, err.toString());
        }
    }

    public void sendStatus(int status){
        taskStatus = status;
    }

    public Bitmap captureImage(){
        if(taskStatus == TRAINING_TASK){
            Log.i(tag, "CameraPreview:capture the image and save with show on image view");
            bProcessing = true;
            if(frameData != null) {
                YuvImage yuvimage = new YuvImage(frameData, ImageFormat.NV21, gPreviewWidth, gPreviewHeight, null);
                Log.i(tag, "CameraPreview: gW =" + gPreviewWidth + "gH=" + gPreviewHeight);
                Log.i(tag, "CameraPreview: w: " + yuvimage.getWidth() + "h:" + yuvimage.getHeight());
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                yuvimage.compressToJpeg(new Rect(0, 0, yuvimage.getWidth(), yuvimage.getHeight()), 90, baos);
                byte[] jdata = baos.toByteArray();
                BitmapFactory.Options bitmapFatoryOptions = new BitmapFactory.Options();
                bitmapFatoryOptions.inPreferredConfig = Bitmap.Config.RGB_565;
                Bitmap bmp = BitmapFactory.decodeByteArray(jdata, 0, jdata.length, bitmapFatoryOptions);
                bProcessing = false;
                return bmp;
            }
            return null;
        }else{
            Log.i(tag, "captureimage status error");
        }
            return null;
        }

// load JNI
// declare JNI api
public native int cvipProcessing(int width, int height, byte[] srcData, int[] resultData);


static
{
        System.loadLibrary("OneCV");
        System.loadLibrary("OneCVAPP");
        System.loadLibrary("OneDetectorJni");
        }



// thread process image thread;
	private Runnable cvProcess = new Runnable()
	{
		public void run()
		{
			Canvas canvas = null;
            int offset = 0;
            int count = 0;
            int id;
            float scaleX, scaleY;
			Paint paint = new Paint();			
			bProcessing = true;
			cvipProcessing(gPreviewWidth, gPreviewHeight, frameData, resultData);
			//Log.i(tag, String.format("outdata=%x", outData[10000]));
			Log.d(tag, String.format("tag=%d", resultData[0]));

            for(int i=0; i<resultData[0]; i++) {
                count++;
                id = resultData[count+offset];
                rectClass.x = resultData[count+offset+1];
                rectClass.y = resultData[count+offset+2];
                rectClass.width = resultData[count+offset+3];
                rectClass.height = resultData[count+offset+4];
                scaleX = (gDispWidth/(float)gPreviewWidth);
                scaleY = (gDispHeight/(float)gPreviewHeight);
                rectClass.x*=scaleX;
                rectClass.y*=scaleY;
                rectClass.width*=scaleX;
                rectClass.height*=scaleY;

                offset+=4;
                Log.d(tag, String.format("id=%d x=%d y=%d width=%d height=%d", id, rectClass.x, rectClass.y, rectClass.width, rectClass.height));
                paint.setColor(Color.CYAN);
                try {
                    canvas = gDrawHolder.lockCanvas();

                    paint.setXfermode(new PorterDuffXfermode(Mode.CLEAR));
                    canvas.drawPaint(paint);
                    paint.setXfermode(new PorterDuffXfermode(Mode.SRC));

                    paint.setColor(Color.RED);
                    paint.setStrokeWidth(2);
                    paint.setStyle(Paint.Style.STROKE);
                    canvas.drawRect(rectClass.x, rectClass.y, (rectClass.x + rectClass.width) - 1, (rectClass.y + rectClass.height) - 1, paint);

                } catch (Exception e) {
                    e.printStackTrace();
                } finally {
                    //Log.i(tag, String.format("ssssssss"));
                    if (canvas != null)
                        gDrawHolder.unlockCanvasAndPost(canvas);
                }
            }
            if(resultData[0] == 0)
            {
                //clean the canvas;
                try {
                    canvas = gDrawHolder.lockCanvas();
                    paint.setXfermode(new PorterDuffXfermode(Mode.CLEAR));
                    canvas.drawPaint(paint);
                }catch(Exception e){
                    e.printStackTrace();
                }finally {
                    if(canvas != null)
                        gDrawHolder.unlockCanvasAndPost(canvas);
                }

            }
			//bitmap.setPixels(outData, 0, gPreviewWidth, 0, 0, gPreviewWidth, gPreviewHeight);
			//gCamPreview.setImageBitmap(bitmap);
			bProcessing = false;
		}
	};
}
