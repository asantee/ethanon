package net.asantee.gs2d;

import android.opengl.GLSurfaceView;
import android.util.Log;

public class FrameHandler extends Thread {

	protected FrameHandler(GLSurfaceView view, long frameCap) {
		super(new FrameRunnable(view, frameCap), "FrameHandler");
		super.setPriority(6);
	}

	static protected class FrameRunnable implements Runnable {
		public FrameRunnable(GLSurfaceView view, long frameCap) {
			this.view = view;
			this.frameStride = (1000/frameCap);
		}

		public void run() {
			for (;;) {
				long beginTime = System.currentTimeMillis();
				view.requestRender();
				try {
					// maybe just paranoia, but how long does requestRender last?
					long elapsedTime = System.currentTimeMillis() - beginTime;
					long sleepTime = frameStride - elapsedTime;
					if (sleepTime > 1) {
						Thread.sleep(sleepTime);
					}
				} catch (InterruptedException e) {
					Log.e("FrameHandler", e.toString());
				}
			}
		}

		private GLSurfaceView view;
		private final long frameStride;
	}
}
