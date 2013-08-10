package net.asantee.gs2d.io;

public class NativeCommandForwarder implements CommandForwarder {

	public static String KEY_PRESSED_CMD = "key_pressed";

	public void addCommand(String cmd) {
		commands.append("\n").append(cmd);
	}

	public void appendCommands(StringBuilder builder) {
		builder.append(commands);
		commands.setLength(0);
	}

	private StringBuilder commands = new StringBuilder();
}
