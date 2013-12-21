package net.asantee.gs2d.io;

import android.annotation.TargetApi;
import android.app.Activity;
import android.os.Build;
import android.view.KeyEvent;
import android.view.MotionEvent;

@TargetApi(Build.VERSION_CODES.GINGERBREAD)
public class InputDeviceManagerAPI9 extends InputDeviceManager {

	public static boolean isInstantiable() {
		final int sdkVersion = Build.VERSION.SDK_INT;
		return (sdkVersion >= Build.VERSION_CODES.GINGERBREAD);
	}	

	public int getMaxJoystickButtons() {
		return 14;
	}

	@Override
	public boolean onKeyDown(KeyEvent event) {
		if (isGameKey(event)) {
			InputDeviceState state = getStateObjectFromDevice(event.getDevice());
			if (state == null) {
				detectJoysticks(event.getDevice());
			}		
		}
		return super.onKeyDown(event);
	}

	@Override
	public int keyCodeToButtonIndex(int keyCode) {
		switch (keyCode) {
		case InputDeviceManager.KEYCODE_XPERIA_1:  return 0;
		case KeyEvent.KEYCODE_BUTTON_B:
		case InputDeviceManager.KEYCODE_XPERIA_2:  if (!isXperiaPlayXKeySwapped()) return 1; else return 2;
		case KeyEvent.KEYCODE_BUTTON_A:
		case InputDeviceManager.KEYCODE_XPERIA_3:  if (!isXperiaPlayXKeySwapped()) return 2; else return 1;
		case InputDeviceManager.KEYCODE_XPERIA_4:  return 3;
		case InputDeviceManager.KEYCODE_XPERIA_5:  return 4;
		case InputDeviceManager.KEYCODE_XPERIA_6:  return 5;
		case KeyEvent.KEYCODE_BUTTON_C:            return 6;
		case KeyEvent.KEYCODE_BUTTON_Z:            return 7;
		case InputDeviceManager.KEYCODE_XPERIA_9:  return 8;
		case InputDeviceManager.KEYCODE_XPERIA_10: return 9;
		case KeyEvent.KEYCODE_BUTTON_L2:           return 11;
		case KeyEvent.KEYCODE_BUTTON_R2:           return 12;
		case KeyEvent.KEYCODE_DPAD_UP: return InputDeviceManager.DPAD_UP;
		case KeyEvent.KEYCODE_DPAD_DOWN: return InputDeviceManager.DPAD_DOWN;
		case KeyEvent.KEYCODE_DPAD_LEFT: return InputDeviceManager.DPAD_LEFT;
		case KeyEvent.KEYCODE_DPAD_RIGHT: return InputDeviceManager.DPAD_RIGHT;
		default: return -1;
		}
	}

	@Override
	public boolean isGameInputDevice(int source) {
		return false; // gingerbread devices will be added on the go
	}

	@Override
	public boolean isGamepadButton(KeyEvent event) {
		int keyCode = event.getKeyCode();
		switch (keyCode) {
		case InputDeviceManager.KEYCODE_XPERIA_1:
		//case InputDeviceManager.KEYCODE_XPERIA_2: // SAME AS BACK
		case InputDeviceManager.KEYCODE_XPERIA_3:
		case InputDeviceManager.KEYCODE_XPERIA_4:
		case InputDeviceManager.KEYCODE_XPERIA_5:
		case InputDeviceManager.KEYCODE_XPERIA_6:
		case KeyEvent.KEYCODE_BUTTON_C:
		case KeyEvent.KEYCODE_BUTTON_Z:
		case KeyEvent.KEYCODE_BUTTON_A:
		case KeyEvent.KEYCODE_BUTTON_B:
		case InputDeviceManager.KEYCODE_XPERIA_9:
		case InputDeviceManager.KEYCODE_XPERIA_10:
		case KeyEvent.KEYCODE_BUTTON_L2:
		case KeyEvent.KEYCODE_BUTTON_R2:
			return true;
		case KeyEvent.KEYCODE_BACK:
			if (event.isAltPressed())
				return true;
		default:
			return false;
		}
	}

	@Override
	public int getMaxJoysticks() {
		return 1;
	}

	@Override
	public boolean onJoystickMotion(MotionEvent event, Activity activity) {
		return false;
	}
}
