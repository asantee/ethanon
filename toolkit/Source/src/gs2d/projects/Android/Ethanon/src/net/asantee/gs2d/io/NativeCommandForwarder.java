package net.asantee.gs2d.io;

public class NativeCommandForwarder implements CommandForwarder {

	public static String KEY_PRESSED_CMD = "key_pressed";

	public void addCommand(String cmd) {
		commands = commands + "\n" + cmd;
	}

	public String getCommands() {
		String cmds = new String(commands);
		commands = "";
		return cmds;
	}

	private String commands = new String();
}
