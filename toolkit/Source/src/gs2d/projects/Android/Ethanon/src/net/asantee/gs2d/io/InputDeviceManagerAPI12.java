package net.asantee.gs2d.io;

import android.annotation.TargetApi;
import android.app.Activity;
import android.os.Build;
import android.view.InputDevice;
import android.view.InputDevice.MotionRange;
import android.view.KeyEvent;
import android.view.MotionEvent;

@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR1)
public class InputDeviceManagerAPI12 extends InputDeviceManager {

	public static boolean isInstantiable() {
		final int sdkVersion = Integer.parseInt(Build.VERSION.SDK);
		return (sdkVersion >= Build.VERSION_CODES.HONEYCOMB_MR1);
	}

	InputDeviceManagerAPI12() {
		super();
	}

	@Override
	public void detectJoysticks(InputDevice specificDevice) {
		super.detectJoysticks(specificDevice);

		for (int d = 0; d < devices.size(); d++) {
			InputDeviceState deviceState = devices.get(d);
			InputDevice device = deviceState.device;
			int numAxes = 0;
			for (MotionRange range : device.getMotionRanges()) {
				if (isGameInputDevice(range.getSource())) {

					String sharedDataParam = assembleJoystickAxisValueSharedDataPath(deviceState.gs2dIndex, numAxes);
					sendSharedParameterChangeRequest(sharedDataParam, Float.toString(0.0f));

					numAxes += 1;
				}
			}
			
			deviceState.axes		= new int[numAxes];
			deviceState.axisValues	= new float[numAxes];
			
			int i = 0;
			for (MotionRange range : device.getMotionRanges()) {
				if (isGameInputDevice(range.getSource())) {
					deviceState.axes[i++] = range.getAxis();
				}
			}
		}
	}

	@Override
	public boolean onJoystickMotion(MotionEvent event, Activity activity) {
		if (event.getAction() != MotionEvent.ACTION_MOVE)
			return activity.onGenericMotionEvent(event);

		InputDeviceState state = getStateObjectFromDevice(event.getDevice());
		if (state == null)
			return activity.onGenericMotionEvent(event);

		for (int i = 0; i < state.axes.length; i++) {
			int axis = state.axes[i];

			float value = event.getAxisValue(axis);
			if (value != state.axisValues[i]) {
				state.axisValues[i] = value;
				float processedValue = processAxis(state.device.getMotionRange(axis), value);
				String sharedDataParam = assembleJoystickAxisValueSharedDataPath(state.gs2dIndex, i);
				sendSharedParameterChangeRequest(sharedDataParam, Float.toString(processedValue));
			}
		}
		return activity.onGenericMotionEvent(event);
	}

	public int getMaxJoystickButtons() {
		return 16;
	}
	
	@Override
	public int keyCodeToButtonIndex(int keyCode) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BUTTON_1: return 0;
		case KeyEvent.KEYCODE_BUTTON_2: return 1;
		case KeyEvent.KEYCODE_BUTTON_3: return 2;
		case KeyEvent.KEYCODE_BUTTON_4: return 3;
		case KeyEvent.KEYCODE_BUTTON_5: return 4;
		case KeyEvent.KEYCODE_BUTTON_6: return 5;
		case KeyEvent.KEYCODE_BUTTON_7: return 6;
		case KeyEvent.KEYCODE_BUTTON_8: return 7;
		case KeyEvent.KEYCODE_BUTTON_9: return 8;
		case KeyEvent.KEYCODE_BUTTON_10: return 9;
		case KeyEvent.KEYCODE_BUTTON_11: return 10;
		case KeyEvent.KEYCODE_BUTTON_12: return 11;
		case KeyEvent.KEYCODE_BUTTON_13: return 12;
		case KeyEvent.KEYCODE_BUTTON_14: return 13;
		case KeyEvent.KEYCODE_BUTTON_15: return 14;
		case KeyEvent.KEYCODE_BUTTON_16: return 15;
		case KeyEvent.KEYCODE_DPAD_UP: return InputDeviceManager.DPAD_UP;
		case KeyEvent.KEYCODE_DPAD_DOWN: return InputDeviceManager.DPAD_DOWN;
		case KeyEvent.KEYCODE_DPAD_LEFT: return InputDeviceManager.DPAD_LEFT;
		case KeyEvent.KEYCODE_DPAD_RIGHT: return InputDeviceManager.DPAD_RIGHT;
		default: return -1;
		}
	}

	@Override
	public boolean isGameInputDevice(int source) {
		return ((source & InputDevice.SOURCE_CLASS_JOYSTICK) != 0);
	}

	@Override
	public boolean isGamepadButton(KeyEvent event) {
		return KeyEvent.isGamepadButton(event.getKeyCode());
	}

	@Override
	public int getMaxJoysticks() {
		return 4;
	}

}
