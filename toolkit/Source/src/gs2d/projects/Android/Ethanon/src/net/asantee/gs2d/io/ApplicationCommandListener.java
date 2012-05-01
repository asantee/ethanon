package net.asantee.gs2d.io;

import android.app.Activity;

public class ApplicationCommandListener extends DefaultCommandListener {

	static final String CMD_QUIT_APP = "quit_app";

	public ApplicationCommandListener(Activity activity) {
		super(activity);
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
		activity.finish();
	}
}
