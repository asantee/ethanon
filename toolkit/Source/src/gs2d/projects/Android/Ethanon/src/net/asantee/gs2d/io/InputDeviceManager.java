package net.asantee.gs2d.io;

import java.util.ArrayList;

import net.asantee.gs2d.GS2DJNI;
import android.annotation.TargetApi;
import android.os.Build;
import android.view.InputDevice;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;

@TargetApi(Build.VERSION_CODES.GINGERBREAD)
public abstract class InputDeviceManager {

	private StringBuilder report = new StringBuilder();
	private ArrayList<InputDeviceState> devices;
	private final char DEFAULT_O_BUTTON_LABEL = 0x25CB; // hex for WHITE_CIRCLE
	private boolean xperiaPlayXKeySwapped = false;

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
			if (isGameInputDevice(device.getSources())) {
				devices.add(new InputDeviceState(device));

				// check for key swap on xperia play
				KeyCharacterMap kcm = KeyCharacterMap.load(deviceIds[t]);
				if (kcm != null && DEFAULT_O_BUTTON_LABEL == kcm.getDisplayLabel(KeyEvent.KEYCODE_DPAD_CENTER)) {
					xperiaPlayXKeySwapped = true;
				}
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

		if (isGameKey(event)) {
			state.keys.put(event.getKeyCode(), 0);
			return true;
		}
		return false;
	}

	public boolean onKeyDown(KeyEvent event) {
		InputDeviceState state = getStateObjectFromDevice(event.getDevice());
		if (state == null)
			return false;

		if (event.getRepeatCount() == 0) {
			if (isGameKey(event)) {
				state.keys.put(event.getKeyCode(), 1);
				updateReport();
				return true;
			}
		}
		return false;
	}

	public boolean isXperiaPlayXKeySwapped() {
		return xperiaPlayXKeySwapped;
	}
	
	private boolean isGameKey(KeyEvent event) {
		int keyCode = event.getKeyCode();
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_UP:
		case KeyEvent.KEYCODE_DPAD_DOWN:
		case KeyEvent.KEYCODE_DPAD_LEFT:
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			return true;
		default:
			return isGamepadButton(event);
		}
	}

	public static boolean isGameInputDevice(int source) {
		return ((source & InputDevice.SOURCE_CLASS_JOYSTICK) != 0);
	}

	public abstract boolean isGamepadButton(KeyEvent event);

	public abstract int keyCodeToButtonIndex(int keyCode);

	public static float processAxis(InputDevice.MotionRange range,
			float axisValue) {
		float deadZone = range.getFlat();
		if (Math.abs(axisValue) <= deadZone) {
			return 0.0f;
		}
		float cap = (axisValue < 0.0f) ? range.getMin() : range.getMax();
		return axisValue / cap;
	}
}
