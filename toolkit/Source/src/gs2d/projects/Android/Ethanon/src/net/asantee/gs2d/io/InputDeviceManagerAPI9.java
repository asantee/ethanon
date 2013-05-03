package net.asantee.gs2d.io;

import android.annotation.TargetApi;
import android.os.Build;
import android.view.KeyEvent;

@TargetApi(Build.VERSION_CODES.GINGERBREAD)
public class InputDeviceManagerAPI9 extends InputDeviceManager {

	protected InputDeviceManagerAPI9() {
	}

	public int keyCodeToButtonIndex(int keyCode) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BUTTON_A: return 1;
		case KeyEvent.KEYCODE_BUTTON_B: return 2;
		case KeyEvent.KEYCODE_BUTTON_C: return 3;
		case KeyEvent.KEYCODE_BUTTON_X: return 4;
		case KeyEvent.KEYCODE_BUTTON_Y: return 5;
		case KeyEvent.KEYCODE_BUTTON_Z: return 6;
		case KeyEvent.KEYCODE_BUTTON_L1: return 7;
		case KeyEvent.KEYCODE_BUTTON_L2: return 8;
		case KeyEvent.KEYCODE_BUTTON_R1: return 9;
		case KeyEvent.KEYCODE_BUTTON_R2: return 10;
		case KeyEvent.KEYCODE_BUTTON_SELECT: return 11;
		case KeyEvent.KEYCODE_BUTTON_START: return 12;
		case KeyEvent.KEYCODE_BUTTON_THUMBR: return 13;
		case KeyEvent.KEYCODE_BUTTON_THUMBL: return 14;
		case KeyEvent.KEYCODE_BUTTON_MODE: return 15;
		case KeyEvent.KEYCODE_DPAD_UP: return -2;
		case KeyEvent.KEYCODE_DPAD_DOWN: return -3;
		case KeyEvent.KEYCODE_DPAD_LEFT: return -4;
		case KeyEvent.KEYCODE_DPAD_RIGHT: return -5;
		default: return -1;
		}
	}
	
	public boolean isGamepadButton(int keyCode) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BUTTON_A:
		case KeyEvent.KEYCODE_BUTTON_B:
		case KeyEvent.KEYCODE_BUTTON_C:
		case KeyEvent.KEYCODE_BUTTON_L1:
		case KeyEvent.KEYCODE_BUTTON_L2:
		case KeyEvent.KEYCODE_BUTTON_R1:
		case KeyEvent.KEYCODE_BUTTON_R2:
		case KeyEvent.KEYCODE_BUTTON_SELECT:
		case KeyEvent.KEYCODE_BUTTON_START:
		case KeyEvent.KEYCODE_BUTTON_THUMBR:
		case KeyEvent.KEYCODE_BUTTON_THUMBL:
		case KeyEvent.KEYCODE_BUTTON_X:
		case KeyEvent.KEYCODE_BUTTON_Y:
		case KeyEvent.KEYCODE_BUTTON_Z:
		case KeyEvent.KEYCODE_BUTTON_MODE:
			return true;
		default:
			return false;
		}
	}
}
