package net.asantee.gs2d.io;

import android.app.Activity;
import android.view.KeyEvent;

public abstract class KeyEventListener extends Activity implements CommandForwarder {

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			forwarder.addCommand(NativeCommandForwarder.KEY_PRESSED_CMD + " " + "back");
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}

	public String getCommands() {
		return forwarder.getCommands();
	}

	private NativeCommandForwarder forwarder = new NativeCommandForwarder();
}
