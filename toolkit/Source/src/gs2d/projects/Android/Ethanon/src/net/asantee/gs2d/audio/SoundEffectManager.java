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

import java.io.IOException;
import java.util.HashMap;

import net.asantee.gs2d.GS2DActivity;
import android.app.Activity;
import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.SoundPool;
import android.util.Log;

public class SoundEffectManager {

	private static final int MAXIMUM_SIMULTANEOUS_SFX = 8;

	public SoundEffectManager(Activity activity) {
		this.pool = new SoundPool(MAXIMUM_SIMULTANEOUS_SFX, AudioManager.STREAM_MUSIC, 0);
		this.manager = (AudioManager) activity.getSystemService(Context.AUDIO_SERVICE);
		this.assets = activity.getAssets();
		this.activity = activity;
		activity.setVolumeControlStream(AudioManager.STREAM_MUSIC);

	}

	protected static String removePrefix(String str, String prefix) {
		int idx = str.indexOf(prefix);
		if (idx == 0) {
			return str.substring(prefix.length());
		} else {
			return str;
		}
	}

	public boolean load(String fileName) {
		if (samples.get(fileName) == null) {
			String relativeFileName = removePrefix(fileName, PREFIX);

			AssetFileDescriptor afd;
			try {
				afd = assets.openFd(relativeFileName);
				samples.put(fileName, pool.load(afd, 1));
				if (afd != null) {
					try {
						afd.close();
					} catch (IOException e) {
						GS2DActivity.toast(fileName + " couldn't close asset", activity);
					}
				}
			} catch (IOException e) {
				GS2DActivity.toast(fileName + " file not found", activity);
				return false;
			}
		}
		return true;
	}

	public void play(String fileName, float volume, float speed) {
		float streamVolume = manager.getStreamVolume(AudioManager.STREAM_MUSIC);
		streamVolume /= manager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
		streamVolume *= MediaStreamManager.getGlobalVolume() * volume;
		streamVolume = Math.min(streamVolume, 0.99f);
		
		if (streamVolume > 0) {
			Integer id = samples.get(fileName);
			if (id != null) {
				if (pool.play(id, streamVolume, streamVolume, 1, 0, Math.max(Math.min(2.0f, speed), 0.5f)) == 0) {
					Log.e("GS2DError", fileName + " playback failed. Trying to reload");
				}
			} else {
				Log.w("GS2DError", fileName + " playback failed. file has not been loaded yet. It might be just a concurrency issue");
			}
		}
	}

	public boolean release(String fileName) {
		Integer id = samples.get(fileName);
		if (id != null) {
			samples.remove(fileName);
			return pool.unload(id);
		} else {
			return false;
		}
	}

	public void clearAll() {
		pool.release();
		samples.clear();
	}

	public static String PREFIX = "assets/";
	private Activity activity;
	private SoundPool pool;
	private AudioManager manager;
	private HashMap<String, Integer> samples = new HashMap<String, Integer>();
	private AssetManager assets;
}
