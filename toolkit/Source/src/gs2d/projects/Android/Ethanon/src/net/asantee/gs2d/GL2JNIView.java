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

import java.util.ArrayList;
import java.util.Iterator;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import net.asantee.gs2d.io.AccelerometerListener;
import net.asantee.gs2d.io.ApplicationCommandListener;
import net.asantee.gs2d.io.DefaultCommandListener;
import net.asantee.gs2d.io.InputDeviceManager;
import net.asantee.gs2d.io.KeyEventListener;
import net.asantee.gs2d.io.NativeCommandListener;
import android.app.Activity;
import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class GL2JNIView extends GLSurfaceView {
	private static String TAG = "GL2JNIView";
	protected static int displayWidth, displayHeight;
	protected static final boolean DEBUG = false;
	public final String apkPath;

	private static int MAXIMUM_TOUCHES = 5;
	protected static Vector2[] currentTouch = new Vector2[MAXIMUM_TOUCHES];
	protected static Boolean[] touchSlotReleasedLastFrame = new Boolean[MAXIMUM_TOUCHES];

	private FrameHandler frameHandler;

	private static AccelerometerListener accelerometerListener;
	private static KeyEventListener keyEventListener;
	private ArrayList<NativeCommandListener> commandListeners;

	static class Vector2 {
		Vector2(float x, float y) {
			this.x = x;
			this.y = y;
		}

		public float x, y;
	}

	private void retrieveScreenSize(Activity activity) {
		DisplayMetrics metrics = new DisplayMetrics();
		activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);
		displayWidth = metrics.widthPixels;
		displayHeight = metrics.heightPixels;
	}

	public GL2JNIView(Context context) {
		super(context, null);
		this.apkPath = null;
	}

	public GL2JNIView(GS2DActivity activity, String apkPath, AccelerometerListener accelerometerListener,
			KeyEventListener keyEventListener, ArrayList<NativeCommandListener> commandListeners,
			InputDeviceManager inputDeviceManager, int fixedFrameRate) {
		super(activity.getApplication());

		GL2JNIView.accelerometerListener = accelerometerListener;
		this.apkPath = apkPath;
		this.commandListeners = commandListeners;
		GL2JNIView.keyEventListener = keyEventListener;

		init(false, 1, 0, activity, inputDeviceManager);
		retrieveScreenSize(activity);

		int renderMode = (fixedFrameRate > 0) ? GLSurfaceView.RENDERMODE_WHEN_DIRTY : GLSurfaceView.RENDERMODE_CONTINUOUSLY;
		
		setRenderMode(renderMode);

		if (renderMode == GLSurfaceView.RENDERMODE_WHEN_DIRTY) {
			frameHandler = new FrameHandler(this, fixedFrameRate);
			frameHandler.start();
		}

		resetTouchMonitoringData();
	}

	@Override
	public void onPause() {
		super.onPause();
		GL2JNIView.keyEventListener.emulateKey("pause");
	}

	private void init(boolean translucent, int depth, int stencil, GS2DActivity activity,
			InputDeviceManager inputDeviceManager) {
		if (translucent) {
			this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
		}
		setEGLContextFactory(new GLContextFactory());
		setEGLConfigChooser(translucent ? new ConfigChooser(8, 8, 8, 8, depth, stencil) : new ConfigChooser(5, 6, 5, 0,
				depth, stencil));
		setRenderer(new Renderer(apkPath, commandListeners, activity, inputDeviceManager));
	}

	public static void checkEglError(String prompt, EGL10 egl) {
		int error;
		while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
			Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
		}
	}

	static class NativeCommandRunner implements Runnable {
		private String commands;
		private ArrayList<NativeCommandListener> commandListeners;

		NativeCommandRunner(String commands, ArrayList<NativeCommandListener> commandListeners) {
			this.commands = commands;
			this.commandListeners = commandListeners;
		}

		public void run() {
			for (Iterator<NativeCommandListener> i = commandListeners.iterator(); i.hasNext();) {
				NativeCommandListener current = i.next();
				if (current != null) {
					current.parseAndExecuteCommands(commands);
				}
			}
			Log.i("GS2D_Native_command", commands);
		}
	}

	static class Renderer implements GLSurfaceView.Renderer {
		String apkPath;
		GS2DActivity activity;
		ArrayList<NativeCommandListener> commandListeners;
		InputDeviceManager inputDeviceManager;

		Renderer(String apkPath, ArrayList<NativeCommandListener> commandListeners, GS2DActivity activity,
				InputDeviceManager inputDeviceManager) {
			this.inputDeviceManager = inputDeviceManager;
			this.apkPath = apkPath;
			this.commandListeners = commandListeners;
			this.commandListeners.add(new DefaultCommandListener(activity));
			this.commandListeners.add(new ApplicationCommandListener(activity, inputDeviceManager));
			this.activity = activity;
		}

		private String commands = new String();
		private StringBuilder commandBuilder = new StringBuilder();

		public void onDrawFrame(GL10 gl) {
			commandBuilder.setLength(0);
			GL2JNIView.appendTouchDataString(commandBuilder);
			GL2JNIView.appendAccelerometerDataString(commandBuilder);
			GL2JNIView.keyEventListener.appendCommands(commandBuilder);

			if (inputDeviceManager != null) {
				inputDeviceManager.updateSharedData();
			}

			commands = GS2DJNI.mainLoop(commandBuilder.toString());
			if (!commands.equals("")) {
				activity.runOnUiThread(new NativeCommandRunner(commands, commandListeners));
			}
		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {
			Log.i("Ethanon", "GS2DJNI.resize / GS2DJNI.restore");
			GS2DJNI.resize(width, height);
			GS2DJNI.restore();
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			Log.i("Ethanon", "GS2DJNI.start");
			GS2DJNI.start(apkPath, activity.getExternalStoragePath(), activity.getGlobalExternalStoragePath(),
					GL2JNIView.displayWidth, GL2JNIView.displayHeight);
		}
	}

	protected static void appendDataSegment(float v, StringBuilder builder) {
		builder.append(v).append(" ");
	}

	private static void resetTouchMonitoringData() {
		for (int t = 0; t < GL2JNIView.touchSlotReleasedLastFrame.length; t++) {
			GL2JNIView.touchSlotReleasedLastFrame[t] = Boolean.valueOf(false);
		}
	}

	protected static void appendTouchDataString(StringBuilder builder) {
		builder.append(MAXIMUM_TOUCHES).append(" ");
		for (int t = 0; t < MAXIMUM_TOUCHES; t++) {
			Vector2 touchPos = currentTouch[t];
			if (touchPos != null && !GL2JNIView.touchSlotReleasedLastFrame[t].booleanValue()) {
				appendDataSegment(touchPos.x, builder);
				appendDataSegment(touchPos.y, builder);
			} else {
				appendDataSegment(-1.0f, builder);
				appendDataSegment(-1.0f, builder);
			}
		}
		GL2JNIView.resetTouchMonitoringData();
	}

	protected static void appendAccelerometerDataString(StringBuilder builder) {
		appendDataSegment(accelerometerListener.getX(), builder);
		appendDataSegment(accelerometerListener.getY(), builder);
		appendDataSegment(accelerometerListener.getZ(), builder);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		int action = event.getAction() & MotionEvent.ACTION_MASK;

		switch (action) {
		case MotionEvent.ACTION_DOWN: {
			int id = event.getPointerId(0);
			if (id < MAXIMUM_TOUCHES) {
				currentTouch[id] = new Vector2(event.getX(), event.getY());
			}
			break;
		}
		case MotionEvent.ACTION_MOVE: {
			int touchCounter = event.getPointerCount();
			for (int t = 0; t < touchCounter; t++) {
				int id = event.getPointerId(t);
				if (id < MAXIMUM_TOUCHES) {
					currentTouch[id] = new Vector2(event.getX(t), event.getY(t));
				}
			}
		}
		case MotionEvent.ACTION_POINTER_DOWN: {
			int id = event.getPointerId(getIndex(event));
			if (id < MAXIMUM_TOUCHES) {
				currentTouch[id] = new Vector2(event.getX(getIndex(event)), event.getY(getIndex(event)));
			}
			break;
		}
		case MotionEvent.ACTION_POINTER_UP: {
			int id = event.getPointerId(getIndex(event));
			if (id < MAXIMUM_TOUCHES) {
				currentTouch[id] = null;
				GL2JNIView.touchSlotReleasedLastFrame[id] = Boolean.valueOf(true);
			}
			break;
		}
		case MotionEvent.ACTION_UP: {
			int id = event.getPointerId(0);
			if (id < MAXIMUM_TOUCHES) {
				currentTouch[id] = null;
				GL2JNIView.touchSlotReleasedLastFrame[id] = Boolean.valueOf(true);
			}
			break;
		}
		}
		return true;
	}

	private int getIndex(MotionEvent event) {
		int idx = (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
		return idx;
	}

	public void destroy() {
		GS2DJNI.destroy();
	}
}
