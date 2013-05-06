package net.asantee.gs2d.io;

import android.app.Activity;
import android.os.Bundle;
import android.view.KeyEvent;

public abstract class KeyEventListener extends Activity implements CommandForwarder {

	protected InputDeviceManager inputDeviceManager;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		inputDeviceManager = InputDeviceManager.instantiate();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			if (!event.isAltPressed()) {
				forwarder.addCommand(NativeCommandForwarder.KEY_PRESSED_CMD + " " + "back");
				return true;
			}
		}
		return super.onKeyDown(keyCode, event);
	}

	@Override
	protected void onResume() {
		super.onResume();
		if (inputDeviceManager != null) {
			inputDeviceManager.detectJoysticks();
		}
	}

	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		if (inputDeviceManager == null)
			return super.dispatchKeyEvent(event);
		switch (event.getAction()) {
		case KeyEvent.ACTION_DOWN:
			if (inputDeviceManager.onKeyDown(event)) {
				return true;
			}
			break;
		case KeyEvent.ACTION_UP:
			if (inputDeviceManager.onKeyUp(event)) {
				return true;
			}
			break;
		}
		return super.dispatchKeyEvent(event);
	}

	public void emulateKey(String key) {
		forwarder.addCommand(NativeCommandForwarder.KEY_PRESSED_CMD + " " + key);
	}

	public void appendCommands(StringBuilder builder) {
		forwarder.appendCommands(builder);
	}

	private NativeCommandForwarder forwarder = new NativeCommandForwarder();
}
