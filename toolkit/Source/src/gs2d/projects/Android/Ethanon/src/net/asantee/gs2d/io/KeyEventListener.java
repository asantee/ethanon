package net.asantee.gs2d.io;

import net.asantee.gs2d.GS2DJNI;
import android.app.Activity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;

public abstract class KeyEventListener extends Activity implements CommandForwarder {

	private NativeCommandForwarder forwarder = new NativeCommandForwarder();

	protected InputDeviceManager inputDeviceManager;

	public static String KEY_STATE_DOWN = "down";
	public static String KEY_STATE_UP = "up";
	
	public static String KEY_CODE_UP = "ethanon.system.keyboard.up";
	public static String KEY_CODE_DOWN = "ethanon.system.keyboard.down";
	public static String KEY_CODE_LEFT = "ethanon.system.keyboard.left";
	public static String KEY_CODE_RIGHT = "ethanon.system.keyboard.right";
	public static String KEY_CODE_PAGE_UP = "ethanon.system.keyboard.pageUp";
	public static String KEY_CODE_PAGE_DOWN = "ethanon.system.keyboard.pageDown";
	public static String KEY_CODE_SPACE = "ethanon.system.keyboard.space";
	public static String KEY_CODE_ENTER = "ethanon.system.keyboard.enter";
	public static String KEY_CODE_HOME = "ethanon.system.keyboard.home";
	public static String KEY_CODE_INSERT = "ethanon.system.keyboard.insert";
	public static String KEY_CODE_ESCAPE = "ethanon.system.keyboard.esc";
	public static String KEY_CODE_TAB = "ethanon.system.keyboard.tab";
	public static String KEY_CODE_SHIFT = "ethanon.system.keyboard.shift";
	public static String KEY_CODE_ALT = "ethanon.system.keyboard.alt";
	public static String KEY_CODE_CTRL = "ethanon.system.keyboard.ctrl";
	public static String KEY_CODE_F1 = "ethanon.system.keyboard.f1";
	public static String KEY_CODE_F2 = "ethanon.system.keyboard.f2";
	public static String KEY_CODE_F3 = "ethanon.system.keyboard.f3";
	public static String KEY_CODE_F4 = "ethanon.system.keyboard.f4";
	public static String KEY_CODE_F5 = "ethanon.system.keyboard.f5";
	public static String KEY_CODE_F6 = "ethanon.system.keyboard.f6";
	public static String KEY_CODE_F7 = "ethanon.system.keyboard.f7";
	public static String KEY_CODE_F8 = "ethanon.system.keyboard.f8";
	public static String KEY_CODE_F9 = "ethanon.system.keyboard.f9";
	public static String KEY_CODE_F10 = "ethanon.system.keyboard.f10";
	public static String KEY_CODE_F11 = "ethanon.system.keyboard.f11";
	public static String KEY_CODE_F12 = "ethanon.system.keyboard.f12";
	public static String KEY_CODE_A = "ethanon.system.keyboard.a";
	public static String KEY_CODE_B = "ethanon.system.keyboard.b";
	public static String KEY_CODE_C = "ethanon.system.keyboard.c";
	public static String KEY_CODE_D = "ethanon.system.keyboard.d";
	public static String KEY_CODE_E = "ethanon.system.keyboard.e";
	public static String KEY_CODE_F = "ethanon.system.keyboard.f";
	public static String KEY_CODE_G = "ethanon.system.keyboard.g";
	public static String KEY_CODE_H = "ethanon.system.keyboard.h";
	public static String KEY_CODE_I = "ethanon.system.keyboard.i";
	public static String KEY_CODE_J = "ethanon.system.keyboard.j";
	public static String KEY_CODE_K = "ethanon.system.keyboard.k";
	public static String KEY_CODE_L = "ethanon.system.keyboard.l";
	public static String KEY_CODE_M = "ethanon.system.keyboard.m";
	public static String KEY_CODE_N = "ethanon.system.keyboard.n";
	public static String KEY_CODE_O = "ethanon.system.keyboard.o";
	public static String KEY_CODE_P = "ethanon.system.keyboard.p";
	public static String KEY_CODE_Q = "ethanon.system.keyboard.q";
	public static String KEY_CODE_R = "ethanon.system.keyboard.r";
	public static String KEY_CODE_S = "ethanon.system.keyboard.s";
	public static String KEY_CODE_T = "ethanon.system.keyboard.t";
	public static String KEY_CODE_U = "ethanon.system.keyboard.u";
	public static String KEY_CODE_V = "ethanon.system.keyboard.v";
	public static String KEY_CODE_X = "ethanon.system.keyboard.x";
	public static String KEY_CODE_Y = "ethanon.system.keyboard.y";
	public static String KEY_CODE_Z = "ethanon.system.keyboard.z";
	public static String KEY_CODE_W = "ethanon.system.keyboard.w";
	public static String KEY_CODE_0 = "ethanon.system.keyboard.0";
	public static String KEY_CODE_1 = "ethanon.system.keyboard.1";
	public static String KEY_CODE_2 = "ethanon.system.keyboard.2";
	public static String KEY_CODE_3 = "ethanon.system.keyboard.3";
	public static String KEY_CODE_4 = "ethanon.system.keyboard.4";
	public static String KEY_CODE_5 = "ethanon.system.keyboard.5";
	public static String KEY_CODE_6 = "ethanon.system.keyboard.6";
	public static String KEY_CODE_7 = "ethanon.system.keyboard.7";
	public static String KEY_CODE_8 = "ethanon.system.keyboard.8";
	public static String KEY_CODE_9 = "ethanon.system.keyboard.9";

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		inputDeviceManager = InputDeviceManager.instantiate();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (event.getRepeatCount() == 0) {
			if (inputDeviceManager != null) {
				if (inputDeviceManager.onKeyDown(event)) {
					return true;
				}
			}

			switch (keyCode) {
			case KeyEvent.KEYCODE_BACK:
				if (!event.isAltPressed()) {
					forwarder.addCommand(NativeCommandForwarder.KEY_PRESSED_CMD + " " + "back");
					return true;
				}
			case KeyEvent.KEYCODE_DPAD_UP:
				GS2DJNI.setSharedData(KEY_CODE_UP, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_DPAD_DOWN:
				GS2DJNI.setSharedData(KEY_CODE_DOWN, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_DPAD_LEFT:
				GS2DJNI.setSharedData(KEY_CODE_LEFT, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_DPAD_RIGHT:
				GS2DJNI.setSharedData(KEY_CODE_RIGHT, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_PAGE_UP:
				GS2DJNI.setSharedData(KEY_CODE_PAGE_UP, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_PAGE_DOWN:
				GS2DJNI.setSharedData(KEY_CODE_PAGE_DOWN, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_SPACE:
				GS2DJNI.setSharedData(KEY_CODE_SPACE, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_ENTER:
				GS2DJNI.setSharedData(KEY_CODE_ENTER, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_HOME:
				GS2DJNI.setSharedData(KEY_CODE_HOME, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_INSERT:
				GS2DJNI.setSharedData(KEY_CODE_INSERT, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_ESCAPE:
				GS2DJNI.setSharedData(KEY_CODE_ESCAPE, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_TAB:
				GS2DJNI.setSharedData(KEY_CODE_TAB, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_SHIFT_LEFT:
			case KeyEvent.KEYCODE_SHIFT_RIGHT:
				GS2DJNI.setSharedData(KEY_CODE_SHIFT, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_ALT_LEFT:
			case KeyEvent.KEYCODE_ALT_RIGHT:
				GS2DJNI.setSharedData(KEY_CODE_ALT, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_CTRL_LEFT:
			case KeyEvent.KEYCODE_CTRL_RIGHT:
				GS2DJNI.setSharedData(KEY_CODE_CTRL, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F1:
				GS2DJNI.setSharedData(KEY_CODE_F1, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F2:
				GS2DJNI.setSharedData(KEY_CODE_F2, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F3:
				GS2DJNI.setSharedData(KEY_CODE_F3, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F4:
				GS2DJNI.setSharedData(KEY_CODE_F4, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F5:
				GS2DJNI.setSharedData(KEY_CODE_F5, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F6:
				GS2DJNI.setSharedData(KEY_CODE_F6, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F7:
				GS2DJNI.setSharedData(KEY_CODE_F7, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F8:
				GS2DJNI.setSharedData(KEY_CODE_F8, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F9:
				GS2DJNI.setSharedData(KEY_CODE_F9, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F10:
				GS2DJNI.setSharedData(KEY_CODE_F10, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F11:
				GS2DJNI.setSharedData(KEY_CODE_F11, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F12:
				GS2DJNI.setSharedData(KEY_CODE_F12, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_A:
				GS2DJNI.setSharedData(KEY_CODE_A, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_B:
				GS2DJNI.setSharedData(KEY_CODE_B, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_C:
				GS2DJNI.setSharedData(KEY_CODE_C, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_D:
				GS2DJNI.setSharedData(KEY_CODE_D, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_E:
				GS2DJNI.setSharedData(KEY_CODE_E, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_F:
				GS2DJNI.setSharedData(KEY_CODE_F, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_G:
				GS2DJNI.setSharedData(KEY_CODE_G, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_H:
				GS2DJNI.setSharedData(KEY_CODE_H, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_I:
				GS2DJNI.setSharedData(KEY_CODE_I, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_J:
				GS2DJNI.setSharedData(KEY_CODE_J, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_K:
				GS2DJNI.setSharedData(KEY_CODE_K, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_L:
				GS2DJNI.setSharedData(KEY_CODE_L, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_M:
				GS2DJNI.setSharedData(KEY_CODE_M, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_N:
				GS2DJNI.setSharedData(KEY_CODE_N, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_O:
				GS2DJNI.setSharedData(KEY_CODE_O, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_P:
				GS2DJNI.setSharedData(KEY_CODE_P, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_Q:
				GS2DJNI.setSharedData(KEY_CODE_Q, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_R:
				GS2DJNI.setSharedData(KEY_CODE_R, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_S:
				GS2DJNI.setSharedData(KEY_CODE_S, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_T:
				GS2DJNI.setSharedData(KEY_CODE_T, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_U:
				GS2DJNI.setSharedData(KEY_CODE_U, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_V:
				GS2DJNI.setSharedData(KEY_CODE_V, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_X:
				GS2DJNI.setSharedData(KEY_CODE_X, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_Y:
				GS2DJNI.setSharedData(KEY_CODE_Y, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_Z:
				GS2DJNI.setSharedData(KEY_CODE_Z, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_W:
				GS2DJNI.setSharedData(KEY_CODE_W, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_0:
				GS2DJNI.setSharedData(KEY_CODE_0, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_1:
				GS2DJNI.setSharedData(KEY_CODE_1, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_2:
				GS2DJNI.setSharedData(KEY_CODE_2, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_3:
				GS2DJNI.setSharedData(KEY_CODE_3, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_4:
				GS2DJNI.setSharedData(KEY_CODE_4, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_5:
				GS2DJNI.setSharedData(KEY_CODE_5, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_6:
				GS2DJNI.setSharedData(KEY_CODE_6, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_7:
				GS2DJNI.setSharedData(KEY_CODE_7, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_8:
				GS2DJNI.setSharedData(KEY_CODE_8, KEY_STATE_DOWN);
				return true;
			case KeyEvent.KEYCODE_9:
				GS2DJNI.setSharedData(KEY_CODE_9, KEY_STATE_DOWN);
				return true;
			}
		}
		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		if (inputDeviceManager != null) {
			if (inputDeviceManager.onKeyUp(event)) {
				return true;
			}
		}
		
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_UP:
			GS2DJNI.setSharedData(KEY_CODE_UP, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_DPAD_DOWN:
			GS2DJNI.setSharedData(KEY_CODE_DOWN, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_DPAD_LEFT:
			GS2DJNI.setSharedData(KEY_CODE_LEFT, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			GS2DJNI.setSharedData(KEY_CODE_RIGHT, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_PAGE_UP:
			GS2DJNI.setSharedData(KEY_CODE_PAGE_UP, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_PAGE_DOWN:
			GS2DJNI.setSharedData(KEY_CODE_PAGE_DOWN, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_SPACE:
			GS2DJNI.setSharedData(KEY_CODE_SPACE, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_ENTER:
			GS2DJNI.setSharedData(KEY_CODE_ENTER, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_HOME:
			GS2DJNI.setSharedData(KEY_CODE_HOME, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_INSERT:
			GS2DJNI.setSharedData(KEY_CODE_INSERT, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_ESCAPE:
			GS2DJNI.setSharedData(KEY_CODE_ESCAPE, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_TAB:
			GS2DJNI.setSharedData(KEY_CODE_TAB, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_SHIFT_LEFT:
		case KeyEvent.KEYCODE_SHIFT_RIGHT:
			GS2DJNI.setSharedData(KEY_CODE_SHIFT, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_ALT_LEFT:
		case KeyEvent.KEYCODE_ALT_RIGHT:
			GS2DJNI.setSharedData(KEY_CODE_ALT, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_CTRL_LEFT:
		case KeyEvent.KEYCODE_CTRL_RIGHT:
			GS2DJNI.setSharedData(KEY_CODE_CTRL, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F1:
			GS2DJNI.setSharedData(KEY_CODE_F1, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F2:
			GS2DJNI.setSharedData(KEY_CODE_F2, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F3:
			GS2DJNI.setSharedData(KEY_CODE_F3, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F4:
			GS2DJNI.setSharedData(KEY_CODE_F4, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F5:
			GS2DJNI.setSharedData(KEY_CODE_F5, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F6:
			GS2DJNI.setSharedData(KEY_CODE_F6, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F7:
			GS2DJNI.setSharedData(KEY_CODE_F7, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F8:
			GS2DJNI.setSharedData(KEY_CODE_F8, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F9:
			GS2DJNI.setSharedData(KEY_CODE_F9, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F10:
			GS2DJNI.setSharedData(KEY_CODE_F10, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F11:
			GS2DJNI.setSharedData(KEY_CODE_F11, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F12:
			GS2DJNI.setSharedData(KEY_CODE_F12, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_A:
			GS2DJNI.setSharedData(KEY_CODE_A, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_B:
			GS2DJNI.setSharedData(KEY_CODE_B, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_C:
			GS2DJNI.setSharedData(KEY_CODE_C, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_D:
			GS2DJNI.setSharedData(KEY_CODE_D, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_E:
			GS2DJNI.setSharedData(KEY_CODE_E, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_F:
			GS2DJNI.setSharedData(KEY_CODE_F, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_G:
			GS2DJNI.setSharedData(KEY_CODE_G, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_H:
			GS2DJNI.setSharedData(KEY_CODE_H, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_I:
			GS2DJNI.setSharedData(KEY_CODE_I, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_J:
			GS2DJNI.setSharedData(KEY_CODE_J, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_K:
			GS2DJNI.setSharedData(KEY_CODE_K, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_L:
			GS2DJNI.setSharedData(KEY_CODE_L, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_M:
			GS2DJNI.setSharedData(KEY_CODE_M, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_N:
			GS2DJNI.setSharedData(KEY_CODE_N, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_O:
			GS2DJNI.setSharedData(KEY_CODE_O, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_P:
			GS2DJNI.setSharedData(KEY_CODE_P, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_Q:
			GS2DJNI.setSharedData(KEY_CODE_Q, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_R:
			GS2DJNI.setSharedData(KEY_CODE_R, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_S:
			GS2DJNI.setSharedData(KEY_CODE_S, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_T:
			GS2DJNI.setSharedData(KEY_CODE_T, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_U:
			GS2DJNI.setSharedData(KEY_CODE_U, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_V:
			GS2DJNI.setSharedData(KEY_CODE_V, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_X:
			GS2DJNI.setSharedData(KEY_CODE_X, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_Y:
			GS2DJNI.setSharedData(KEY_CODE_Y, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_Z:
			GS2DJNI.setSharedData(KEY_CODE_Z, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_W:
			GS2DJNI.setSharedData(KEY_CODE_W, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_0:
			GS2DJNI.setSharedData(KEY_CODE_0, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_1:
			GS2DJNI.setSharedData(KEY_CODE_1, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_2:
			GS2DJNI.setSharedData(KEY_CODE_2, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_3:
			GS2DJNI.setSharedData(KEY_CODE_3, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_4:
			GS2DJNI.setSharedData(KEY_CODE_4, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_5:
			GS2DJNI.setSharedData(KEY_CODE_5, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_6:
			GS2DJNI.setSharedData(KEY_CODE_6, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_7:
			GS2DJNI.setSharedData(KEY_CODE_7, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_8:
			GS2DJNI.setSharedData(KEY_CODE_8, KEY_STATE_UP);
			return true;
		case KeyEvent.KEYCODE_9:
			GS2DJNI.setSharedData(KEY_CODE_9, KEY_STATE_UP);
			return true;
		}
		return super.onKeyUp(keyCode, event);
	}

	@Override
	protected void onResume() {
		super.onResume();
		if (inputDeviceManager != null) {
			inputDeviceManager.detectJoysticks();
		}
	}

	// this method is expected to override parent's method above API level 12
	// It must call its SUPER method at some point
	public boolean dispatchGenericMotionEvent(MotionEvent event) {
		if (inputDeviceManager != null)
			return (inputDeviceManager.onJoystickMotion(event, this));
		else
			return true;
	}

	public void emulateKey(String key) {
		forwarder.addCommand(NativeCommandForwarder.KEY_PRESSED_CMD + " " + key);
	}

	public void appendCommands(StringBuilder builder) {
		forwarder.appendCommands(builder);
	}
}
