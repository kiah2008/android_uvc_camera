/*
 *  UVCCamera
 *  library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *  All files in the folder are under this Apache License, Version 2.0.
 *  Files in the libjpeg-turbo, libusb, libuvc, rapidjson folder
 *  may have a different license, see the respective files.
 */

package com.kiah.usbcameratest9;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.GregorianCalendar;
import java.util.Locale;
import java.util.Objects;

import android.Manifest;
import android.graphics.SurfaceTexture;
import android.hardware.usb.UsbDevice;
import android.os.Bundle;
import android.os.Environment;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView.SurfaceTextureListener;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.ImageButton;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.serenegiant.common.BaseActivity;
import com.serenegiant.common.BuildConfig;
import com.serenegiant.encoder.RecordParams;
import com.serenegiant.encoder.biz.H264EncodeConsumer;
import com.serenegiant.encoder.biz.Mp4MediaMuxer;
import com.serenegiant.usb.CameraDialog;
import com.serenegiant.usb.IFrameCallback;
import com.serenegiant.usb.USBMonitor;
import com.serenegiant.usb.USBMonitor.OnDeviceConnectListener;
import com.serenegiant.usb.USBMonitor.UsbControlBlock;
import com.serenegiant.usb.UVCCamera;

import com.kiah.widget.SimpleUVCCameraTextureView;

public final class MainActivity extends BaseActivity implements CameraDialog.CameraDialogParent {
	private static final boolean DEBUG = BuildConfig.BUILD_TYPE =="debug";	// set false when releasing
	private static final String TAG = MainActivity.class.getSimpleName();

	public static final String ROOT_PATH = Environment.getExternalStorageDirectory().getAbsolutePath()
			+ File.separator;

    private static final int CAPTURE_STOP = 0;
    private static final int CAPTURE_PREPARE = 1;
    private static final int CAPTURE_RUNNING = 2;

	private final Object mSync = new Object();
    // for accessing USB and USB camera
    private USBMonitor mUSBMonitor;
	private UVCCamera mUVCCamera;
	private SimpleUVCCameraTextureView mUVCCameraView;
	// for open&start / stop&close camera preview
	private ToggleButton mCameraButton;
	// for start & stop movie capture
	private ImageButton mCaptureButton;

	private int mCaptureState = 0;
	private Surface mPreviewSurface;

	@Override
	protected void onCreate(final Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_main);

		mCameraButton = (ToggleButton)findViewById(R.id.camera_button);
		mCameraButton.setOnCheckedChangeListener(mOnCheckedChangeListener);

		mCaptureButton = (ImageButton)findViewById(R.id.capture_button);
		mCaptureButton.setOnClickListener(mOnClickListener);

		mWidth = UVCCamera.DEFAULT_PREVIEW_WIDTH;
		mHeight = UVCCamera.DEFAULT_PREVIEW_HEIGHT;
		mUVCCameraView = (SimpleUVCCameraTextureView)findViewById(R.id.UVCCameraTextureView1);
		mUVCCameraView.setAspectRatio(UVCCamera.DEFAULT_PREVIEW_WIDTH / (float)UVCCamera.DEFAULT_PREVIEW_HEIGHT);
		mUVCCameraView.setSurfaceTextureListener(mSurfaceTextureListener);

		mUSBMonitor = new USBMonitor(this, mOnDeviceConnectListener);

		checkAndRequestPermissions(Arrays.asList(
				Manifest.permission.CAMERA,
				Manifest.permission.WRITE_EXTERNAL_STORAGE,
				Manifest.permission.READ_EXTERNAL_STORAGE,
				Manifest.permission.RECORD_AUDIO
		));
	}

	@Override
	protected void onStart() {
		super.onStart();
		synchronized (mSync) {
			if (mUSBMonitor != null) {
				mUSBMonitor.register();
			}
			if (mUVCCamera != null)
				mUVCCamera.startPreview();
		}
		setCameraButton(false);
		updateItems();
	}

	@Override
	protected void onStop() {
		synchronized (mSync) {
			if (mUVCCamera != null) {
				mUVCCamera.stopPreview();
			}
			mUSBMonitor.unregister();
		}
		setCameraButton(false);
		super.onStop();
	}

	@Override
	public void onDestroy() {
		synchronized (mSync) {
			if (mUVCCamera != null) {
				mUVCCamera.destroy();
				mUVCCamera = null;
			}
			if (mUSBMonitor != null) {
				mUSBMonitor.destroy();
				mUSBMonitor = null;
			}
		}
		mCameraButton = null;
		mCaptureButton = null;
		mUVCCameraView = null;
		super.onDestroy();
	}

	private final OnCheckedChangeListener mOnCheckedChangeListener = new OnCheckedChangeListener() {
		@Override
		public void onCheckedChanged(final CompoundButton buttonView, final boolean isChecked) {
			synchronized (mSync) {
				if (isChecked && mUVCCamera == null) {
					CameraDialog.showDialog(MainActivity.this);
				} else if (mUVCCamera != null) {
					mUVCCamera.destroy();
					mUVCCamera = null;
				}
			}
			updateItems();
		}
	};

	private final OnClickListener mOnClickListener = new OnClickListener() {
		@Override
		public void onClick(final View v) {
			if (checkPermissionWriteExternalStorage()) {
				if (mCaptureState == CAPTURE_STOP) {
//					startCapture();
					startVideoRecord();
				} else {
//					stopCapture();
					stopVideoRecord();
				}
			}
		}
	};

	private final OnDeviceConnectListener mOnDeviceConnectListener = new OnDeviceConnectListener() {
		@Override
		public void onAttach(final UsbDevice device) {
			Toast.makeText(MainActivity.this, "USB_DEVICE_ATTACHED", Toast.LENGTH_SHORT).show();
		}

		@Override
		public void onConnect(final UsbDevice device, final UsbControlBlock ctrlBlock, final boolean createNew) {
			synchronized (mSync) {
				if (mUVCCamera != null) {
					mUVCCamera.destroy();
					mUVCCamera = null;
				}
			}
			Log.d(TAG, "device connected "+ctrlBlock);
			queueEvent(new Runnable() {
				@Override
				public void run() {
					final UVCCamera camera = new UVCCamera();
					try {
						camera.open(ctrlBlock);
					} catch(Exception e) {
						Log.e(TAG, "open camera failed "+ e);
						return;
					}
					if (DEBUG) Log.i(TAG, "supportedSize:" + camera.getSupportedSize());
					if (mPreviewSurface != null) {
						mPreviewSurface.release();
						mPreviewSurface = null;
					}
					try {
						camera.setPreviewSize(UVCCamera.DEFAULT_PREVIEW_WIDTH, UVCCamera.DEFAULT_PREVIEW_HEIGHT, UVCCamera.FRAME_FORMAT_MJPEG);
					} catch (final IllegalArgumentException e) {
						try {
							// fallback to YUV mode
							camera.setPreviewSize(UVCCamera.DEFAULT_PREVIEW_WIDTH, UVCCamera.DEFAULT_PREVIEW_HEIGHT, UVCCamera.DEFAULT_PREVIEW_MODE);
						} catch (final IllegalArgumentException e1) {
							camera.destroy();
							return;
						}
					}
					final SurfaceTexture st = mUVCCameraView.getSurfaceTexture();
					if (st != null) {
						mPreviewSurface = new Surface(st);
						camera.setPreviewDisplay(mPreviewSurface);
						camera.startPreview();
						camera.setFrameCallback(mIFrameCallback, UVCCamera.PIXEL_FORMAT_YUV420SP);
					}
					synchronized (mSync) {
						mUVCCamera = camera;
					}
				}
			}, 30);
		}

		@Override
		public void onDisconnect(final UsbDevice device, final UsbControlBlock ctrlBlock) {
			// XXX you should check whether the comming device equal to camera device that currently using
			queueEvent(new Runnable() {
				@Override
				public void run() {
					synchronized (mSync) {
						if (mUVCCamera != null) {
							mUVCCamera.close();
						}
					}
					if (mPreviewSurface != null) {
						mPreviewSurface.release();
						mPreviewSurface = null;
					}
				}
			}, 0);
			setCameraButton(false);
		}

		@Override
		public void onDettach(final UsbDevice device) {
			Toast.makeText(MainActivity.this, "USB_DEVICE_DETACHED", Toast.LENGTH_SHORT).show();
		}

		@Override
		public void onCancel(final UsbDevice device) {
			setCameraButton(false);
		}
	};

	/**
	 * to access from CameraDialog
	 * @return
	 */
	@Override
	public USBMonitor getUSBMonitor() {
		return mUSBMonitor;
	}

	@Override
	public void onDialogResult(boolean canceled) {
		if (canceled) {
			setCameraButton(false);
		}
	}

	private void setCameraButton(final boolean isOn) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (mCameraButton != null) {
					try {
						mCameraButton.setOnCheckedChangeListener(null);
						mCameraButton.setChecked(isOn);
					} finally {
						mCameraButton.setOnCheckedChangeListener(mOnCheckedChangeListener);
					}
				}
				if (!isOn && (mCaptureButton != null)) {
					mCaptureButton.setVisibility(View.INVISIBLE);
				}
			}
		}, 0);
	}

//**********************************************************************
	private final SurfaceTextureListener mSurfaceTextureListener = new SurfaceTextureListener() {

		@Override
		public void onSurfaceTextureAvailable(final SurfaceTexture surface, final int width, final int height) {
		}

		@Override
		public void onSurfaceTextureSizeChanged(final SurfaceTexture surface, final int width, final int height) {
		}

		@Override
		public boolean onSurfaceTextureDestroyed(final SurfaceTexture surface) {
			if (mPreviewSurface != null) {
				mPreviewSurface.release();
				mPreviewSurface = null;
			}
			return true;
		}

		@Override
		public void onSurfaceTextureUpdated(final SurfaceTexture surface) {
		}
	};

	public interface OnEncodeResultListener {
		void onEncodeResult(byte[] data, int offset, int length, long timestamp, int type);

		void onRecordResult(String videoPath);
	}

	private H264EncodeConsumer mH264Consumer;
	private Mp4MediaMuxer mMuxer;
	public static OnEncodeResultListener mListener;
	private String mVideoPath;
	private int mWidth;
	private int mHeight;


	public boolean isRecording() {
		synchronized (mSync) {
			return (mUVCCamera != null) && (mH264Consumer != null);
		}
	}


	private final IFrameCallback mIFrameCallback = new IFrameCallback() {
		@Override
		public void onFrame(final ByteBuffer frame) {
//				final MediaVideoBufferEncoder videoEncoder;
//				synchronized (mSync) {
//					videoEncoder = mVideoEncoder;
//				}
//				if (videoEncoder != null) {
//					videoEncoder.frameAvailableSoon();
//					videoEncoder.encode(frame);
//				}
			int len = frame.capacity();
			final byte[] yuv = new byte[len];
			frame.get(yuv);
			// picture
//			new Thread(new Runnable() {
//				@Override
//				public void run() {
//					YUV.saveYuv2Jpeg(picPath, yuv);
//				}
//			}).start();
			// video
			if (mH264Consumer != null) {
				mH264Consumer.setRawYuv(yuv, mWidth, mHeight);
			}
		}
	};

	public void startVideoRecord() {
		if (DEBUG) Log.v(TAG, "startVideoRecord:");
		mVideoPath = ROOT_PATH + "USBCamera/videos/" + System.currentTimeMillis();

//        FileUtils.createfile(FileUtils.ROOT_PATH + "test666.h264");
		// if you want to record,please create RecordParams like this
		final RecordParams params = new RecordParams();
		params.setRecordPath(mVideoPath);
		params.setRecordDuration(0);                        // auto divide saved,default 0 means not divided
		params.setVoiceClose(true);    // is close voice

		if (mH264Consumer == null && (mCaptureState == CAPTURE_STOP)) {
			mCaptureState = CAPTURE_PREPARE;
			File file = new File(mVideoPath);
			if(! Objects.requireNonNull(file.getParentFile()).exists()) {
				file.getParentFile().mkdirs();
			}
			queueEvent(new Runnable() {
				@Override
				public void run() {
					mMuxer = new Mp4MediaMuxer(params.getRecordPath(),
							params.getRecordDuration() * 60 * 1000, params.isVoiceClose());
					mH264Consumer = new H264EncodeConsumer(UVCCamera.DEFAULT_PREVIEW_WIDTH, UVCCamera.DEFAULT_PREVIEW_HEIGHT);
					mH264Consumer.setOnH264EncodeResultListener(new H264EncodeConsumer.OnH264EncodeResultListener() {
						@Override
						public void onEncodeResult(byte[] data, int offset, int length, long timestamp) {
							if (mListener != null) {
								mListener.onEncodeResult(data, offset, length, timestamp, 1);
							}
						}
					});
					mH264Consumer.start();
					// 添加混合器
					if (mMuxer != null) {
						if (mH264Consumer != null) {
							mH264Consumer.setTmpuMuxer(mMuxer);
						}
					}
					mCaptureState = CAPTURE_RUNNING;
				}
			}, 0);
			updateItems();
		}
	}

	public void stopVideoRecord() {
		if (DEBUG) Log.v(TAG, "stopVideoRecord:");
		if (mH264Consumer == null || mCaptureState == CAPTURE_STOP) {
			return;
		}
		mCaptureState = CAPTURE_STOP;
		updateItems();

		queueEvent(new Runnable() {
			@Override
			public void run() {
				// 停止混合器
				if (mMuxer != null) {
					mMuxer.release();
					mMuxer = null;
					Log.i(TAG, TAG + "---->停止本地录制");
				}

				if (mH264Consumer != null) {
					mH264Consumer.exit();
					mH264Consumer.setTmpuMuxer(null);
					try {
						Thread t2 = mH264Consumer;
						mH264Consumer = null;
						if (t2 != null) {
							t2.interrupt();
							t2.join();
						}
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}

				// 返回路径
				if (mListener != null) {
					mListener.onRecordResult(mVideoPath + ".mp4");
				}
			}
		},0);
	}

    private void updateItems() {
    	this.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				mCaptureButton.setVisibility(mCameraButton.isChecked() ? View.VISIBLE : View.INVISIBLE);
		    	mCaptureButton.setColorFilter(mCaptureState == CAPTURE_STOP ? 0 : 0xffff0000);
			}
    	});
    }

    /**
     * create file path for saving movie / still image file
     * @param type Environment.DIRECTORY_MOVIES / Environment.DIRECTORY_DCIM
     * @param ext .mp4 / .png
     * @return return null if can not write to storage
     */
    private static final String getCaptureFile(final String type, final String ext) {
		final File dir = new File(Environment.getExternalStoragePublicDirectory(type), "USBCameraTest");
		dir.mkdirs();	// create directories if they do not exist
        if (dir.canWrite()) {
        	return (new File(dir, getDateTimeString() + ext)).toString();
        }
    	return null;
    }

    private static final SimpleDateFormat sDateTimeFormat = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss", Locale.US);
    private static final String getDateTimeString() {
    	final GregorianCalendar now = new GregorianCalendar();
    	return sDateTimeFormat.format(now.getTime());
    }

}
