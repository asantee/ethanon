package net.asantee.gs2d.io;

import android.app.Activity;

public class ApplicationCommandListener implements NativeCommandListener {

	static final String CMD_QUIT_APP = "quit_app";
	private final Activity activity;

	public ApplicationCommandListener(Activity activity) {
		this.activity = activity;
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
		}
	}

	private void quitApp() {
		activity.runOnUiThread(new Runnable() {
			public void run() {
				activity.finish();
			}
		});
	}
}
