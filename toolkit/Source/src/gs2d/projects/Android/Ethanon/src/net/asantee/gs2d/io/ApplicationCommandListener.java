package net.asantee.gs2d.io;

import android.app.Activity;

public class ApplicationCommandListener implements NativeCommandListener {

	static final String CMD_QUIT_APP = "quit_app";
	static final String CMD_DETECT_JOYSTICKS = "detect_joysticks";

	private InputDeviceManager inputDeviceManager;
	private Activity activity;

	public ApplicationCommandListener(Activity activity, InputDeviceManager inputDeviceManager) {
		this.activity = activity;
		this.inputDeviceManager = inputDeviceManager;
	}

	public void parseAndExecuteCommands(String commands) {
		String[] commandArray = commands.split("\n");
		for (int t = 0; t < commandArray.length; t++) {
			execute(commandArray[t]);
		}
	}

	private void execute(String command) {
		String[] pieces = command.split(" ");
		if (pieces[0].equals(CMD_QUIT_APP)) {
			quitApp();
		} else if (pieces[0].equals(CMD_DETECT_JOYSTICKS)) {
			inputDeviceManager.detectJoysticks();
		}
	}

	private void quitApp() {
		activity.finish();
	}
}
