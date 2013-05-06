package net.asantee.gs2d.io;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Vibrator;

public class DefaultCommandListener implements NativeCommandListener {
	
	protected Activity activity;

	public DefaultCommandListener(Activity activity) {
		this.activity = activity;
	}

	public void parseAndExecuteCommands(String commands) {
		if (!commands.equals("")) {
			String[] commandArray = commands.split("\n");
			for (int t = 0; t < commandArray.length; t++) {
				executeCommand(commandArray[t]);
			}
		}
	}

	private void executeCommand(String cmd) {
		String[] words = cmd.split(" ");
		if (words[0].equals("vibrate")) {
			vibrateEvent(words);
		} else if (words[0].equals("open_url")) {
			urlEvent(words);
		}
	}

	private void urlEvent(final String[] words) {
		Intent intent = new Intent("android.intent.action.VIEW", Uri.parse(words[1]));
		activity.startActivity(intent);
	}

	private void vibrateEvent(String[] words) {
		final long time = Long.parseLong(words[1]);
		((Vibrator) activity.getSystemService(Activity.VIBRATOR_SERVICE)).vibrate(time);
	}
}
