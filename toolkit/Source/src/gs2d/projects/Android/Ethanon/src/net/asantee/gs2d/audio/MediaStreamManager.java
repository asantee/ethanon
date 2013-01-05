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

import net.asantee.gs2d.GS2DActivity;
import android.app.Activity;
import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.MediaPlayer;

public class MediaStreamManager {

	private static MediaPlayer mediaPlayer = null;
	private Activity activity;
	private AssetManager assets;
	private AudioManager manager;
	private static float globalVolume = 1.0f;
	private String currentTrack = "";

	public MediaStreamManager(Activity activity) {
		this.activity = activity;
		this.manager = (AudioManager) activity.getSystemService(Context.AUDIO_SERVICE);
		this.assets = activity.getAssets();
	}

	public static void setGlobalVolume(float volume) {
		globalVolume = volume;
		setVolume(volume);
	}

	public static float getGlobalVolume() {
		return globalVolume;
	}

	public boolean load(String fileName) {
		// dummy (will be loaded on play time)
		return true;
	}

	public void stop() {
		if (mediaPlayer != null) {
			mediaPlayer.stop(); // Hammertime!
		}
	}

	public void pause() {
		if (mediaPlayer != null) {
			mediaPlayer.pause();
		}
	}

	public void release() {
		if (mediaPlayer != null) {
			mediaPlayer.release();
		}
		mediaPlayer = null;
	}

	public static void setVolume(float volume) {
		if (mediaPlayer != null) {
			volume *= MediaStreamManager.getGlobalVolume();
			mediaPlayer.setVolume(volume, volume);
		}
	}

	private boolean mayKeepSong(String fileName) {
		return (fileName.equals(currentTrack) && mediaPlayer.isPlaying());
	}

	public void play(String fileName, float volume, float speed, boolean loop) {
		if (globalVolume * volume <= 0.0f)
			return;

		if (mediaPlayer != null) {
			if (mayKeepSong(fileName)) {
				return;
			} else {
				mediaPlayer.stop();
			}
		}
		currentTrack = fileName;

		float streamVolume = manager.getStreamVolume(AudioManager.STREAM_MUSIC);
		streamVolume /= manager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
		streamVolume *= volume;

		String relativeFileName = SoundEffectManager.removePrefix(fileName, SoundEffectManager.PREFIX);
		AssetFileDescriptor afd;
		try {
			afd = assets.openFd(relativeFileName);
			mediaPlayer = new MediaPlayer();
			mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
			mediaPlayer.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
			mediaPlayer.prepare();
			if (afd != null) {
				try {
					afd.close();
				} catch (IOException e) {
					GS2DActivity.toast(fileName + " couldn't close asset", activity);
				}
			}
		} catch (IOException e) {
			GS2DActivity.toast(fileName + " file not found", activity);
			return;
		}

		mediaPlayer.setLooping(loop);
		setVolume(streamVolume);
		mediaPlayer.start();
	}
}
