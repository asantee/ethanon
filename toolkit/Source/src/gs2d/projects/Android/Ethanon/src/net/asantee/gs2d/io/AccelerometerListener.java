package net.asantee.gs2d.io;

import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.view.Display;
import android.view.Surface;

public class AccelerometerListener implements SensorEventListener {
	private final SensorManager sensorManager;
	private final Sensor accelerometer;
	private float[] gravs = new float[3];
	private float[] geoMag = new float[3];
	private boolean rotateOrientation;
	private Display display;

	public AccelerometerListener(Activity activity) {
		sensorManager = (SensorManager) activity.getSystemService(Activity.SENSOR_SERVICE);
		accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		display = activity.getWindowManager().getDefaultDisplay();
		updateOrientationInfo();
	}
	
	private void updateOrientationInfo() {
		rotateOrientation = (display.getRotation() == Surface.ROTATION_90) ? true : false;
	}

	public float getX() {
		return (rotateOrientation) ? gravs[1] :-gravs[0];
	}

	public float getY() {
		return (rotateOrientation) ? gravs[0] : gravs[1];
	}

	public float getZ() {
		return gravs[2];
	}

	public void onAccuracyChanged(Sensor sensor, int accuracy) {
	}

	public void onSensorChanged(SensorEvent event) {
		switch (event.sensor.getType()) {
		case Sensor.TYPE_ACCELEROMETER:
			gravs[0] = event.values[0];
			gravs[1] = event.values[1];
			gravs[2] = event.values[2];
			updateOrientationInfo();
			break;
		case Sensor.TYPE_MAGNETIC_FIELD:
			geoMag[0] = event.values[0];
			geoMag[1] = event.values[1];
			geoMag[2] = event.values[2];
			break;
		}
	}

	public void onResume() {
		sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_GAME);
	}

	public void onPause() {
		sensorManager.unregisterListener(this);
	}
}
