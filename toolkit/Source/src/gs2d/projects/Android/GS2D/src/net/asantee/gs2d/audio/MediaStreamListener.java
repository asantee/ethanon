/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

package net.asantee.gs2d.audio;

import net.asantee.gs2d.io.NativeCommandListener;
import android.app.Activity;

public class MediaStreamListener extends MediaStreamManager implements NativeCommandListener {

	static final String CMD_PLAY_MUSIC = "play_music";
	static final String CMD_LOAD_MUSIC = "load_music";
	static final String CMD_STOP_MUSIC = "stop_music";
	static final String CMD_DELETE_MUSIC = "delete_music";

	public MediaStreamListener(Activity activity) {
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
		if (pieces[0].equals(CMD_LOAD_MUSIC)) {
			super.load(pieces[1]);
		} else if (pieces[0].equals(CMD_PLAY_MUSIC)) {
			Float volume = Float.parseFloat(pieces[2]);
			Integer loop = Integer.parseInt(pieces[3]);
			Float speed = Float.parseFloat(pieces[4]);
			super.play(pieces[1], volume, speed, (loop.equals(0)) ? false : true);
		} else if (pieces[0].equals(CMD_STOP_MUSIC)) {
			super.stop();
		}
	}
}
