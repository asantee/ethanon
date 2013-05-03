package net.asantee.gs2d.io;

import java.util.ArrayList;

import net.asantee.gs2d.GS2DJNI;
import android.annotation.TargetApi;
import android.os.Build;
import android.util.Log;
import android.view.InputDevice;
import android.view.KeyEvent;

@TargetApi(Build.VERSION_CODES.GINGERBREAD)
public abstract class InputDeviceManager {

	private StringBuilder report = new StringBuilder();
	private ArrayList<InputDeviceState> devices;

	public static boolean isInstantiable() {
		final int sdkVersion = Integer.parseInt(Build.VERSION.SDK);
	    return (sdkVersion >= Build.VERSION_CODES.GINGERBREAD);
	}

	public static InputDeviceManager instantiate() {
		if (isInstantiable()) {
			return new InputDeviceManagerAPI9();
		} else {
			return null;
		}
	}
	
	protected InputDeviceManager() {
		devices = new ArrayList<InputDeviceState>();

		int[] deviceIds = InputDevice.getDeviceIds();
		for (int t = 0; t < deviceIds.length; t++) {
			InputDevice device = InputDevice.getDevice(deviceIds[t]);
			Log.e("INPUT DEVICE:", device.getName());
			if (isGameInputDevice(device.getSources())) {
				Log.e("INPUT DEVICE ADDED:", device.getName());
				devices.add(new InputDeviceState(device));
			}
		}
		updateReport();
	}

	public void updateReport() {
		report.setLength(0);
		report.append("report:\n");
		for (int d = 0; d < devices.size(); d++) {
			InputDeviceState device = devices.get(d);
			report.append(device.device.getName()).append(": ");
			for (int b = 0; b < device.keys.size(); b++) {
				int key = device.keys.keyAt(b);
				if (device.keys.get(key) != 0) {
					report.append(keyCodeToButtonIndex(key));
				}
			}
			report.append("\n");
		}
		GS2DJNI.setSharedData("inputStuff", report.toString());
	}
	
	protected InputDeviceState getStateObjectFromDevice(InputDevice device) {
		for (int t = 0; t < devices.size(); t++) {
			InputDeviceState deviceState = devices.get(t);
			if (deviceState.device == device) {
				return deviceState;
			}
		}
		return null;
	}
	
	public boolean onKeyUp(KeyEvent event) {
		InputDeviceState state = getStateObjectFromDevice(event.getDevice());
		if (state == null)
			return false;

		int keyCode = event.getKeyCode();
		if (isGameKey(keyCode)) {
			state.keys.put(keyCode, 0);
			return true;
		}
		return false;
	}

	public boolean onKeyDown(KeyEvent event) {
		InputDeviceState state = getStateObjectFromDevice(event.getDevice());
		if (state == null)
			return false;
		
		int keyCode = event.getKeyCode();
		if (event.getRepeatCount() == 0) {
			if (isGameKey(keyCode)) {
				state.keys.put(keyCode, 1);
				updateReport();
				return true;
			}
		}
		return false;
	}

	private boolean isGameKey(int keyCode) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_UP:
		case KeyEvent.KEYCODE_DPAD_DOWN:
		case KeyEvent.KEYCODE_DPAD_LEFT:
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			return true;
		default:
			return isGamepadButton(keyCode);
		}
	}

	public static boolean isGameInputDevice(int source) {
		return ((source & InputDevice.SOURCE_CLASS_JOYSTICK) != 0)/*
				|| ((source & InputDevice.SOURCE_GAMEPAD) != 0)
				|| ((source & InputDevice.SOURCE_DPAD) != 0)
				|| ((source & InputDevice.SOURCE_JOYSTICK) != 0)*/;
	}

	public abstract boolean isGamepadButton(int keyCode);
	public abstract int keyCodeToButtonIndex(int keyCode);

	public static float processAxis(InputDevice.MotionRange range, float axisValue) {
		float deadZone = range.getFlat();
		if (Math.abs(axisValue) <= deadZone) {
			return 0.0f;
		}
		float cap = (axisValue < 0.0f) ? range.getMin() : range.getMax();
		return axisValue / cap;
	}
}
