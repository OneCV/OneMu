package com.oneVipas.onedetector;

import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;

public class MenulistActivity extends ActionBarActivity {
	private String tag = "oneDetector";
	private Button oldButton, newButton;


	@Override
	protected void onCreate(Bundle savedInstanceState) {
        Log.i(tag,"MenulistActivity onCreate");
		super.onCreate(savedInstanceState);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_menulist);
        //DisplayMetrics metrics = new DisplayMetrics();
        //getWindowManager().getDefaultDisplay().getMetrics(metrics);

		handleButton();
	}
    @Override
    protected void onResume() {
        super.onResume();
        handleButton();
    }
    @Override
    public void onConfigurationChanged(Configuration newConfig){
        super.onConfigurationChanged(newConfig);
        Log.i(tag,"MenulistActivity onConfigurationChanged");
        // Checks the orientation of the screen
        if(newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE)
            Log.i(tag,"Landscape");
        else if(newConfig.orientation == Configuration.ORIENTATION_PORTRAIT)
            Log.i(tag,"portrait");
    }
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.menulist, menu);
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
	
	private void handleButton(){
        oldButton = (Button) findViewById(R.id.button1);
        newButton = (Button) findViewById(R.id.button2);

        oldButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				Log.i(tag, "onclick menulist button1");
				goExistingRecord();				
			}
		});

        newButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				Log.i(tag, "onclick menulist button2 ");
				goCamera();				
			}
		});
        if(ServerControl.saves==0)
            oldButton.setVisibility(View.INVISIBLE);
        else
            oldButton.setVisibility(View.VISIBLE);
	}
	
	private void goExistingRecord(){
		Intent it = new Intent(this, ExistingRecordActivity.class);
		it.putExtra("xxx", "xxx");
		startActivity(it);
	}

    private void goCamera(){
        Intent it = new Intent(this, CameraPreviewActivity.class);
        it.putExtra("newTraining", "goCamera");
        startActivity(it);
    }


	@Override
	public void onPause() {
		super.onPause();

	}

	
}
