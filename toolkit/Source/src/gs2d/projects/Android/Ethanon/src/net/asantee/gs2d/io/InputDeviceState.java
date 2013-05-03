package net.asantee.gs2d.io;

import android.annotation.TargetApi;
import android.os.Build;
import android.util.SparseIntArray;
import android.view.InputDevice;

@TargetApi(Build.VERSION_CODES.GINGERBREAD)
public class InputDeviceState {
	protected InputDeviceState(InputDevice device) {
		this.device = device;
		keys = new SparseIntArray();
	}
	
	protected InputDevice device;
	protected int[] axes;
	protected float[] axisValues;
	protected SparseIntArray keys;
}
