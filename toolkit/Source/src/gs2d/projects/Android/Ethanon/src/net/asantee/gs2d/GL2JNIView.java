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
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import net.asantee.gs2d.io.AccelerometerListener;
import net.asantee.gs2d.io.ApplicationCommandListener;
import net.asantee.gs2d.io.DefaultCommandListener;
import net.asantee.gs2d.io.KeyEventListener;
import net.asantee.gs2d.io.NativeCommandListener;
import android.app.Activity;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class GL2JNIView extends GLSurfaceView {
	private static String TAG = "GL2JNIView";
	protected static int displayWidth, displayHeight;
	private static final boolean DEBUG = false;
	public final String apkPath;
	private static int MAXIMUM_TOUCHES = 5;
	protected static Vector2[] currentTouch = new Vector2[MAXIMUM_TOUCHES];
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

	public GL2JNIView(GS2DActivity activity, String apkPath, AccelerometerListener accelerometerListener,
			KeyEventListener keyEventListener, ArrayList<NativeCommandListener> commandListeners) {
		super(activity.getApplication());
		GL2JNIView.accelerometerListener = accelerometerListener;
		this.apkPath = apkPath;
		this.commandListeners = commandListeners;
		GL2JNIView.keyEventListener = keyEventListener;
		init(false, 1, 0, activity);
		retrieveScreenSize(activity);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
		frameHandler = new FrameHandler(this, 100);
		frameHandler.start();
	}

	@Override
	public void onPause() {
		super.onPause();
		GL2JNIView.keyEventListener.emulateKey("pause");
	}

	private void init(boolean translucent, int depth, int stencil, GS2DActivity activity) {
		if (translucent) {
			this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
		}
		setEGLContextFactory(new ContextFactory());
		setEGLConfigChooser(translucent ? new ConfigChooser(8, 8, 8, 8, depth, stencil) : new ConfigChooser(5, 6, 5, 0, depth, stencil));
		setRenderer(new Renderer(apkPath, commandListeners, activity));
	}

	private static class ContextFactory implements GLSurfaceView.EGLContextFactory {
		private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

		public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
			Log.w(TAG, "creating OpenGL ES 2.0 context");
			checkEglError("Before eglCreateContext", egl);
			int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
			EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
			checkEglError("After eglCreateContext", egl);
			return context;
		}

		public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
			egl.eglDestroyContext(display, context);
		}
	}

	private static void checkEglError(String prompt, EGL10 egl) {
		int error;
		while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
			Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
		}
	}

	private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser {
		public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
			mRedSize = r;
			mGreenSize = g;
			mBlueSize = b;
			mAlphaSize = a;
			mDepthSize = depth;
			mStencilSize = stencil;
		}

		private static int EGL_OPENGL_ES2_BIT = 4;
		private static int[] s_configAttribs2 = { EGL10.EGL_RED_SIZE, 4, EGL10.EGL_GREEN_SIZE, 4, EGL10.EGL_BLUE_SIZE, 4,
				EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL10.EGL_NONE };

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {

			int[] num_config = new int[1];
			egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

			int numConfigs = num_config[0];

			if (numConfigs <= 0) {
				throw new IllegalArgumentException("No configs match configSpec");
			}

			EGLConfig[] configs = new EGLConfig[numConfigs];
			egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs, num_config);

			if (DEBUG) {
				printConfigs(egl, display, configs);
			}

			return chooseConfig(egl, display, configs);
		}

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs) {
			for (EGLConfig config : configs) {
				int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
				int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);

				// We need at least mDepthSize and mStencilSize bits
				if (d < mDepthSize || s < mStencilSize)
					continue;

				// We want an *exact* match for red/green/blue/alpha
				int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
				int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
				int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
				int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);

				if (r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize)
					return config;
			}
			return null;
		}

		private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue) {
			if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
				return mValue[0];
			}
			return defaultValue;
		}

		private void printConfigs(EGL10 egl, EGLDisplay display, EGLConfig[] configs) {
			int numConfigs = configs.length;
			Log.w(TAG, String.format("%d configurations", numConfigs));
			for (int i = 0; i < numConfigs; i++) {
				Log.w(TAG, String.format("Configuration %d:\n", i));
				printConfig(egl, display, configs[i]);
			}
		}

		private void printConfig(EGL10 egl, EGLDisplay display, EGLConfig config) {
			int[] attributes = { EGL10.EGL_BUFFER_SIZE, EGL10.EGL_ALPHA_SIZE, EGL10.EGL_BLUE_SIZE, EGL10.EGL_GREEN_SIZE,
					EGL10.EGL_RED_SIZE, EGL10.EGL_DEPTH_SIZE, EGL10.EGL_STENCIL_SIZE, EGL10.EGL_CONFIG_CAVEAT, EGL10.EGL_CONFIG_ID,
					EGL10.EGL_LEVEL, EGL10.EGL_MAX_PBUFFER_HEIGHT, EGL10.EGL_MAX_PBUFFER_PIXELS, EGL10.EGL_MAX_PBUFFER_WIDTH,
					EGL10.EGL_NATIVE_RENDERABLE, EGL10.EGL_NATIVE_VISUAL_ID, EGL10.EGL_NATIVE_VISUAL_TYPE,
					0x3030, // EGL10.EGL_PRESERVED_RESOURCES,
					EGL10.EGL_SAMPLES, EGL10.EGL_SAMPLE_BUFFERS, EGL10.EGL_SURFACE_TYPE, EGL10.EGL_TRANSPARENT_TYPE,
					EGL10.EGL_TRANSPARENT_RED_VALUE, EGL10.EGL_TRANSPARENT_GREEN_VALUE, EGL10.EGL_TRANSPARENT_BLUE_VALUE, 0x3039, // EGL10.EGL_BIND_TO_TEXTURE_RGB,
					0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
					0x303B, // EGL10.EGL_MIN_SWAP_INTERVAL,
					0x303C, // EGL10.EGL_MAX_SWAP_INTERVAL,
					EGL10.EGL_LUMINANCE_SIZE, EGL10.EGL_ALPHA_MASK_SIZE, EGL10.EGL_COLOR_BUFFER_TYPE, EGL10.EGL_RENDERABLE_TYPE, 0x3042 // EGL10.EGL_CONFORMANT
			};
			String[] names = { "EGL_BUFFER_SIZE", "EGL_ALPHA_SIZE", "EGL_BLUE_SIZE", "EGL_GREEN_SIZE", "EGL_RED_SIZE", "EGL_DEPTH_SIZE",
					"EGL_STENCIL_SIZE", "EGL_CONFIG_CAVEAT", "EGL_CONFIG_ID", "EGL_LEVEL", "EGL_MAX_PBUFFER_HEIGHT",
					"EGL_MAX_PBUFFER_PIXELS", "EGL_MAX_PBUFFER_WIDTH", "EGL_NATIVE_RENDERABLE", "EGL_NATIVE_VISUAL_ID",
					"EGL_NATIVE_VISUAL_TYPE", "EGL_PRESERVED_RESOURCES", "EGL_SAMPLES", "EGL_SAMPLE_BUFFERS", "EGL_SURFACE_TYPE",
					"EGL_TRANSPARENT_TYPE", "EGL_TRANSPARENT_RED_VALUE", "EGL_TRANSPARENT_GREEN_VALUE", "EGL_TRANSPARENT_BLUE_VALUE",
					"EGL_BIND_TO_TEXTURE_RGB", "EGL_BIND_TO_TEXTURE_RGBA", "EGL_MIN_SWAP_INTERVAL", "EGL_MAX_SWAP_INTERVAL",
					"EGL_LUMINANCE_SIZE", "EGL_ALPHA_MASK_SIZE", "EGL_COLOR_BUFFER_TYPE", "EGL_RENDERABLE_TYPE", "EGL_CONFORMANT" };
			int[] value = new int[1];
			for (int i = 0; i < attributes.length; i++) {
				int attribute = attributes[i];
				String name = names[i];
				if (egl.eglGetConfigAttrib(display, config, attribute, value)) {
					Log.w(TAG, String.format("  %s: %d\n", name, value[0]));
				} else {
					// Log.w(TAG, String.format("  %s: failed\n", name));
					while (egl.eglGetError() != EGL10.EGL_SUCCESS)
						;
				}
			}
		}

		protected int mRedSize;
		protected int mGreenSize;
		protected int mBlueSize;
		protected int mAlphaSize;
		protected int mDepthSize;
		protected int mStencilSize;
		private int[] mValue = new int[1];
	}

	static class Renderer implements GLSurfaceView.Renderer {
		String apkPath;
		GS2DActivity activity;
		ArrayList<NativeCommandListener> commandListeners;

		Renderer(String apkPath, ArrayList<NativeCommandListener> commandListeners, GS2DActivity activity) {
			this.apkPath = apkPath;
			this.commandListeners = commandListeners;
			this.commandListeners.add(new DefaultCommandListener(activity));
			this.commandListeners.add(new ApplicationCommandListener(activity));
			this.activity = activity;
		}

		public void onDrawFrame(GL10 gl) {
			final String commands = GS2DJNI.mainLoop(GL2JNIView.getTouchDataString() + GL2JNIView.getAccelerometerDataString()
					+ GL2JNIView.keyEventListener.getCommands());
			if (!commands.equals("")) {
				activity.runOnUiThread(new Runnable() {
					public void run() {
						for (Iterator<NativeCommandListener> i = commandListeners.iterator(); i.hasNext();) {
							NativeCommandListener current = i.next();
							if (current != null)
								current.parseAndExecuteCommands(commands);
						}
					}
				});
				Log.i("GS2D_Native_command", commands);
			}
		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {
			GS2DJNI.resize(width, height);
			GS2DJNI.restore();
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			GS2DJNI.start(apkPath, activity.getExternalStoragePath(), activity.getGlobalExternalStoragePath(),
						  GL2JNIView.displayWidth, GL2JNIView.displayHeight);
		}
	}

	protected static String dataSegment(float v) {
		return "" + v + " ";
	}

	protected static String getTouchDataString() {
		String out = "" + MAXIMUM_TOUCHES + " ";
		for (int t = 0; t < MAXIMUM_TOUCHES; t++) {
			Vector2 touchPos = currentTouch[t];
			if (touchPos != null) {
				out += dataSegment(touchPos.x);
				out += dataSegment(touchPos.y);
			} else {
				out += dataSegment(-1.0f);
				out += dataSegment(-1.0f);
			}
		}
		return out;
	}

	protected static String getAccelerometerDataString() {
		String out = "";
		out += dataSegment(accelerometerListener.getX());
		out += dataSegment(accelerometerListener.getY());
		out += dataSegment(accelerometerListener.getZ());
		return out;
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		final int action = event.getAction();
		final int maximumTouchId = Math.min(event.getPointerCount(), MAXIMUM_TOUCHES);

		final int pact = action & MotionEvent.ACTION_MASK;
		int pid = 0;
		if (pact == MotionEvent.ACTION_POINTER_DOWN || pact == MotionEvent.ACTION_POINTER_UP) {
			final int pind = (action & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;
			pid = event.getPointerId(pind);
		} else if (pact == MotionEvent.ACTION_DOWN) {
			pid = event.getPointerId(0);
		}

		switch (pact) {
		case MotionEvent.ACTION_DOWN:
		case MotionEvent.ACTION_POINTER_DOWN:
			if (pid < maximumTouchId) {
				currentTouch[pid] = new Vector2(event.getX(pid), event.getY(pid));
			}
			break;

		case MotionEvent.ACTION_UP:
			for (int i = 0; i < MAXIMUM_TOUCHES; ++i)
				currentTouch[i] = null;
			break;

		case MotionEvent.ACTION_POINTER_UP:
			if (pid < maximumTouchId) {
				currentTouch[pid] = null;
			}
			break;

		case MotionEvent.ACTION_MOVE:
			for (int i = 0; i < maximumTouchId; ++i) {
				int p = event.getPointerId(i);
				if (p < maximumTouchId) {
					currentTouch[p] = new Vector2(event.getX(i), event.getY(i));
				}
			}
			break;
		}

		return true;
	}

	public void destroy() {
		GS2DJNI.destroy();
	}
}
