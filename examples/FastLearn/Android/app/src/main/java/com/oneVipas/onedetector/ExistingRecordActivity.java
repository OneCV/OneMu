package com.oneVipas.onedetector;

import java.io.File;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.List;

import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;

import android.os.Environment;
import android.support.v7.app.ActionBarActivity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

public class ExistingRecordActivity extends ActionBarActivity {

	private Button button1, button2, button3;
    private ServerControl httpControl;

    @Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		requestWindowFeature(Window.FEATURE_NO_TITLE);	
		setContentView(R.layout.activity_existing_record);
		buttonHandler();
        httpControl = new ServerControl();
	}

    @Override
    protected void onResume() {
        super.onResume();
        buttonHandler();
    }

    @Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.existing_record, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
	
	public void buttonHandler() {
		
		button1 = (Button) findViewById(R.id.button1);
		button2 = (Button) findViewById(R.id.button2);
		button3 = (Button) findViewById(R.id.button3);

		button1.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
                loadFile(1);
			}
		});
        button2.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                loadFile(2);
            }
        });
        button3.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                loadFile(3);
            }
        });

        if(ServerControl.saves<3)
            button3.setVisibility(View.INVISIBLE);
        if(ServerControl.saves<2)
            button2.setVisibility(View.INVISIBLE);
	}
    private void loadFile(int selected)
    {
        List<NameValuePair> post = new ArrayList<NameValuePair>();
        post.add(new BasicNameValuePair("NUM",""+selected));

        httpControl.httpHandleCmd(httpControl.url_download, post, new ServerDone() {
            @Override
            public void execute(byte[] result) {
                try {
                    // result is the DK001.dkdk.dk file streaming

                    //todo: import result to JNI

                    //建立FileOutputStream物件，路徑為SD卡中的output.txt
                    /* Saving DK001.dkdk.dk to file for checking
                    File dir = new File(Environment.getExternalStorageDirectory() + "/USB/");
                    dir.mkdirs();

                    FileOutputStream output = new FileOutputStream(Environment.getExternalStorageDirectory() + "/USB/DK001.dkdk.dk");

                    output.write(result);
                    output.close();
                    */

                    Toast.makeText(ExistingRecordActivity.this, "Finish Download", Toast.LENGTH_LONG).show();

                    Intent it = new Intent(ExistingRecordActivity.this, CameraPreviewActivity.class);
                    it.putExtra("record", "pass");
                    it.putExtra("dkfile", result);
                    startActivity(it);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });


        //finish();
    }
}
