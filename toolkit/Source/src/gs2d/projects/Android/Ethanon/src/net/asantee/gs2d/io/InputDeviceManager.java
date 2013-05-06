package net.asantee.gs2d.io;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;

import net.asantee.gs2d.GS2DJNI;

import android.annotation.TargetApi;
import android.os.Build;
import android.view.InputDevice;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;

@TargetApi(Build.VERSION_CODES.GINGERBREAD)
public abstract class InputDeviceManager {

	public static final int DPAD_UP = -2;
	public static final int DPAD_DOWN = -3;
	public static final int DPAD_LEFT = -4;
	public static final int DPAD_RIGHT = -5;

	public static final int KEYCODE_XPERIA_TRIANGLE = KeyEvent.KEYCODE_BUTTON_Y;
	public static final int KEYCODE_XPERIA_CIRCLE   = KeyEvent.KEYCODE_BACK;
	public static final int KEYCODE_XPERIA_X        = KeyEvent.KEYCODE_DPAD_CENTER;
	public static final int KEYCODE_XPERIA_SQUARE   = KeyEvent.KEYCODE_BUTTON_X;
	public static final int KEYCODE_XPERIA_L        = KeyEvent.KEYCODE_BUTTON_L1;
	public static final int KEYCODE_XPERIA_R        = KeyEvent.KEYCODE_BUTTON_R1;
	public static final int KEYCODE_XPERIA_START    = KeyEvent.KEYCODE_BUTTON_START;
	public static final int KEYCODE_XPERIA_SELECT   = KeyEvent.KEYCODE_BUTTON_SELECT;

	public static final int KEYCODE_XPERIA_1 = KEYCODE_XPERIA_TRIANGLE;
	public static final int KEYCODE_XPERIA_2 = KEYCODE_XPERIA_CIRCLE;
	public static final int KEYCODE_XPERIA_3 = KEYCODE_XPERIA_X;
	public static final int KEYCODE_XPERIA_4 = KEYCODE_XPERIA_SQUARE;
	public static final int KEYCODE_XPERIA_5 = KEYCODE_XPERIA_L;
	public static final int KEYCODE_XPERIA_6 = KEYCODE_XPERIA_R;
	public static final int KEYCODE_XPERIA_9 = KEYCODE_XPERIA_SELECT;
	public static final int KEYCODE_XPERIA_10 = KEYCODE_XPERIA_START;

	public static final int MAX_JOYSTICKS = 4;
	
	private ArrayList<InputDeviceState> devices;
	private HashMap<String, String> sharedParameterChangeRequests = new HashMap<String, String>();
	private final char DEFAULT_O_BUTTON_LABEL = 0x25CB; // hex for WHITE_CIRCLE
	private boolean xperiaPlayXKeySwapped = false;

	public static boolean isInstantiable() {
		final int sdkVersion = Integer.parseInt(Build.VERSION.SDK);
		return (sdkVersion >= Build.VERSION_CODES.GINGERBREAD);
	}

	public static InputDeviceManager instantiate() {
		if (InputDeviceManagerAPI12.isInstantiable()) {
			return new InputDeviceManagerAPI12();
		} else if (InputDeviceManagerAPI9.isInstantiable()) {
			return new InputDeviceManagerAPI9();
		} else {
			return null;
		}
	}

	protected InputDeviceManager() {
		detectJoysticks();
	}

	public void detectJoysticks() {
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
		updateJoystickButtonsReport();
		sendSharedParameterChangeRequest("ethanon.system.maxJoysticks", Integer.toString(MAX_JOYSTICKS));
		sendSharedParameterChangeRequest("ethanon.system.numJoysticks", Integer.toString(devices.size()));

		for (int d = 0; d < devices.size(); d++) {
			sendSharedParameterChangeRequest(assembleJoystickSharedDataPath(d, "numButtons"), Integer.toString(getMaxJoystickButtons()));
		}
	}

	private void sendSharedParameterChangeRequest(String key, String value) {
		sharedParameterChangeRequests.put(key, value);
	}

	public void updateSharedData() {
		updateJoystickButtonsReport();

		Iterator<Entry<String, String>> it = sharedParameterChangeRequests.entrySet().iterator();
		while (it.hasNext()) {
			Map.Entry<String, String> pairs = (Map.Entry<String, String>) it.next();
			GS2DJNI.setSharedData(pairs.getKey(), pairs.getValue());
		}
		sharedParameterChangeRequests.clear();
	}

	private void updateJoystickButtonsReport() {
		for (int d = 0; d < devices.size(); d++) {
			InputDeviceState device = devices.get(d);

			StringBuilder buttonLineBuilder = new StringBuilder();
			for (int b = 0; b < device.keys.size(); b++) {
				int key = device.keys.keyAt(b);
				if (device.keys.get(key) != 0) {
					buttonLineBuilder.append("b").append(keyCodeToButtonIndex(key)).append(";");
				}
			}
			sendSharedParameterChangeRequest(assembleJoystickSharedDataPath(d, "buttonPressedList"), buttonLineBuilder.toString());
		}
	}

	private String assembleJoystickSharedDataPath(int j, String parameter)
	{
		return "ethanon.system.joystick" + j + "." + parameter;
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

	public abstract int getMaxJoystickButtons();

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
