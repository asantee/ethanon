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

public class SoundCommandListener extends SoundEffectManager implements NativeCommandListener {

	static final String CMD_PLAY_SOUND = "play_sound";
	static final String CMD_LOAD_SOUND = "load_sound";
	static final String CMD_DELETE_SOUND = "delete_sound";
	static final String CMD_SET_GLOBAL_VOLUME = "set_global_volume";

	public SoundCommandListener(Activity activity) {
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
		if (pieces[0].equals(CMD_LOAD_SOUND)) {
			super.load(pieces[1]);

		} else if (pieces[0].equals(CMD_PLAY_SOUND)) {
			Float volume = Float.parseFloat(pieces[2]);
			Float speed = Float.parseFloat(pieces[4]);
			super.play(pieces[1], volume, speed);

		} else if (pieces[0].equals(CMD_DELETE_SOUND)) {
			super.release(pieces[1]);
		} else if (pieces[0].equals(CMD_SET_GLOBAL_VOLUME)) {
			Float volume = Float.parseFloat(pieces[1]);
			MediaStreamManager.setGlobalVolume(volume);
		}
	}
}
