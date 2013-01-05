/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

package net.asantee.gs2d;

import java.io.File;

import net.asantee.gs2d.audio.MediaStreamListener;
import net.asantee.gs2d.audio.SoundCommandListener;
import net.asantee.gs2d.io.AccelerometerListener;
import net.asantee.gs2d.io.KeyEventListener;
import net.asantee.gs2d.io.NativeCommandListener;
import android.app.Activity;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.os.Environment;
import android.view.Gravity;
import android.view.WindowManager;
import android.widget.Toast;

public class GS2DActivity extends KeyEventListener {

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setupExternalStorageDirectories();
	}

	@Override
	protected void onStart() {
		super.onStart();
		soundCmdListener = new SoundCommandListener(this);
		accelerometerListener = new AccelerometerListener(this);
		mediaStreamListener = new MediaStreamListener(this);
		surfaceView = new GL2JNIView(this, retrieveApkPath(), soundCmdListener, accelerometerListener, this, customCommandListener, mediaStreamListener);
		setContentView(surfaceView);
		/*
		 * this.runOnUiThread(new Runnable() { public void run() { Log.i("GS2D",
		 * GS2DJNI.runOnUIThread("null")); } });
		 */
	}

	public void setCustomCommandListener(NativeCommandListener commandListener) {
		this.customCommandListener = commandListener;
	}

	@Override
	protected void onPause() {
		super.onPause();
		accelerometerListener.onPause();
		surfaceView.onPause();

		surfaceView.destroy();
		soundCmdListener.clearAll();
		mediaStreamListener.stop();
	}

	@Override
	protected void onResume() {
		super.onResume();
		accelerometerListener.onResume();
		surfaceView.onResume();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		mediaStreamListener.release();
	}
	
	String retrieveApkPath() {
		String apkFilePath = null;
		ApplicationInfo appInfo = null;
		PackageManager packMgmr = getPackageManager();
		try {
			appInfo = packMgmr.getApplicationInfo(getPackageName(), 0);
		} catch (NameNotFoundException e) {
			e.printStackTrace();
			throw new RuntimeException("Unable to locate assets, aborting...");
		}
		apkFilePath = appInfo.sourceDir;
		return (apkFilePath);
	}

	private void setupExternalStorageDirectories() {
		externalStoragePath = Environment.getExternalStorageDirectory() + "/Android/data/" + this.getPackageName() + "/files/";
		{
			File dir = new File(externalStoragePath + LOG_DIRECTORY_NAME);
			dir.mkdirs();
		}
		{
			File dir = new File(Environment.getExternalStorageDirectory() + "/" + NON_CONTEXT_LOG_DIRECTORY_NAME);
			dir.mkdirs();
		}
	}
	
	public String getExternalStoragePath() {
		return externalStoragePath;
	}

	public static void toast(final String str, final Activity context) {
		context.runOnUiThread(new Runnable() {
			public void run() {
				Toast toast = Toast.makeText(context, str, Toast.LENGTH_LONG);
				toast.setGravity(Gravity.CENTER, toast.getXOffset() / 2, toast.getYOffset() / 2);
				toast.show();
			}
		});
	}

	private String externalStoragePath;
	private MediaStreamListener mediaStreamListener;
	private AccelerometerListener accelerometerListener;
	private SoundCommandListener soundCmdListener;
	private static final String LOG_DIRECTORY_NAME = "log";
	private static final String NON_CONTEXT_LOG_DIRECTORY_NAME = "gs2dlog";
	private GL2JNIView surfaceView;
	private NativeCommandListener customCommandListener = null;
}