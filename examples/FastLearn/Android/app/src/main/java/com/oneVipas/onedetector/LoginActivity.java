package com.oneVipas.onedetector;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;

import android.support.v7.app.ActionBarActivity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class LoginActivity extends ActionBarActivity {
	
	private String tag = "oneDetector";
	private boolean LOGGED = false;
	private EditText editText1,editText2;
	private TextView textView1;
	private Button button1,button2,button3,button4,button5;
	public ServerControl httpControl;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		requestWindowFeature(Window.FEATURE_NO_TITLE);	
		setContentView(R.layout.activity_login);
		Log.i(tag, "onCreate");
		httpControl = new ServerControl();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.login, menu);
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
	
	public void setText(String text){
		textView1.setText(text);
	}
	
	
    public void change_ui(boolean b) {
    	LOGGED = b;
    	
    	int A = LOGGED ? View.INVISIBLE : View.VISIBLE;
    	int B = LOGGED ? View.VISIBLE : View.INVISIBLE;    	
    	
    	editText1.setVisibility(A);
    	editText2.setVisibility(A);
    	button1.setVisibility(A);
    	button2.setVisibility(A);
    	/*
    	button3.setVisibility(B);
    	button4.setVisibility(B);
    	button5.setVisibility(B);
    	*/
    	if(b)
    	{
    		Intent it = new Intent(this, MenulistActivity.class);
    		it.putExtra("login", "pass");
    		startActivity(it);
    		finish();
    	}
    	
    	Toast.makeText(LoginActivity.this,"logged : " + b,Toast.LENGTH_LONG).show();
	}
	
	
	
	public void setLoginView() {
		editText1 = (EditText) findViewById(R.id.editText1);
		editText2 = (EditText) findViewById(R.id.editText2);
		textView1 = (TextView) findViewById(R.id.testView1);
		button1 = (Button) findViewById(R.id.button1);
		button2 = (Button) findViewById(R.id.button2);
		button3 = (Button) findViewById(R.id.button3);
		button4 = (Button) findViewById(R.id.button4);
		button5 = (Button) findViewById(R.id.button5);

		button1.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				textView1.setText("loading...");
                ServerControl.user = editText1.getText().toString();
                ServerControl.pass = editText2.getText().toString();
				List<NameValuePair> post = new ArrayList<NameValuePair>();
				httpControl.httpHandleCmd(httpControl.url_log_in, post,new ServerDone() {
                    @Override
                    public void execute(byte[] result) {
                        String str = new String(result);
                        if(str.substring(1).equals("login success!")) {
                            httpControl.saves = Integer.parseInt(str.charAt(0) + "");
                            Log.i(tag, "saves = " + httpControl.saves);
                            change_ui(true);
                        }
                        else
                            textView1.setText("login failed \n"+str);
                    }
                });
			}
		});
		
		 button2.setOnClickListener(new OnClickListener(){
				@Override
				public void onClick(View arg0) {// get activity_prompts.xmlompts.xml view
					LayoutInflater li = LayoutInflater.from(getBaseContext());
					View promptsView = li.inflate(R.layout.activity_prompts, null);
					AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(LoginActivity.this);	 
					alertDialogBuilder.setView(promptsView);	 
					final EditText userInput = (EditText)promptsView.findViewById(R.id.editTextDialogUserInput);
					alertDialogBuilder.setCancelable(false)
						.setPositiveButton("OK",new DialogInterface.OnClickListener() {
						    public void onClick(DialogInterface dialog,int id) {
					    		textView1.setText("loading...");

                                ServerControl.user = editText1.getText().toString();
                                ServerControl.pass = editText2.getText().toString();
								List<NameValuePair> post = new ArrayList<NameValuePair>();
								post.add(new BasicNameValuePair("MAIL",userInput.getText().toString()));

								httpControl.httpHandleCmd(httpControl.url_sign_up, post, new ServerDone() {
                                    @Override
                                    public void execute(byte[] result) {
                                        textView1.setText(new String(result));
                                    }
                                });
						    }
						  })
						.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                dialog.cancel();
                            }
                        });

					AlertDialog alertDialog = alertDialogBuilder.create();
					alertDialog.show();	 
				}
		});      
		
		button3.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				change_ui(false);
			}
		});
		button4.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				textView1.setText("loading...");
				/*
				 * File extDir = Environment.getExternalStorageDirectory();
				 * String filename = "downloadedMusic.mp3"; File fullFilename =
				 * new File(extDir, filename);
				 * 
				 * try { fullFilename.createNewFile();
				 * fullFilename.setWritable(Boolean.TRUE);
				 * //songtaste.stDownloadFromUrl(strSongUrl, fullFilename); }
				 * catch (IOException e) {
				 * e.printStackTrace(); }
				 */
				httpControl.httpHandleCmd(httpControl.url_download, null,
						 new ServerDone() {
                            @Override
                            public void execute(byte[] result) {
                                String str = "";
                                try {
                                    str = new String(Base64.decode(new String(result)));
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                                Toast.makeText(getBaseContext(), str, Toast.LENGTH_LONG).show();
                            }
                        });

			}
		});
		button5.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				textView1.setText("loading...");
				File fd = new File(Environment.getExternalStorageDirectory()
						+ "/USB/output2.jpg");
				byte[] data = new byte[(int) fd.length()];
				FileInputStream output;
				try {
					output = new FileInputStream(Environment
							.getExternalStorageDirectory() + "/USB/output2.jpg");
					output.read(data);
				} catch (FileNotFoundException e) {
					e.printStackTrace();
				} catch (IOException e) {
					e.printStackTrace();
				}
				List<NameValuePair> post = new ArrayList<NameValuePair>();
				post.add(new BasicNameValuePair("FILE", new String(data)));
				httpControl.httpHandleCmd(httpControl.url_upload, null,
						new ServerDone() {
                            @Override
                            public void execute(byte[] result) {
                                textView1.setText(new String(result));
                                Toast.makeText(getBaseContext(),new String(result),Toast.LENGTH_LONG).show();
                            }
                        });
				// new Thread(new
				// HTTP_Runnable(url_upload,post,UPLOAD)).start();
			}
		});
	}
	
	
	@Override
	public void onPause() {
		super.onPause();
		Log.i(tag, "onPause");
	}
	
	@Override
	public void onResume() {
		super.onResume();
		Log.i(tag, "onResume");
		LOGGED = false;
		setLoginView();
	}
	
	
}
