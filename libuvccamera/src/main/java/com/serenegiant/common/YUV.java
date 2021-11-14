package com.serenegiant.common;

import android.graphics.Rect;
import android.graphics.YuvImage;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class YUV {

    static
    private void saveYuv2Jpeg(String path, byte[] data, int imag_format, int width, int height) {
        //ImageFormat.NV21
        YuvImage yuvImage = new YuvImage(data, imag_format, width, height, null);
        ByteArrayOutputStream bos = new ByteArrayOutputStream(data.length);
        boolean result = yuvImage.compressToJpeg(new Rect(0, 0, width, height), 100, bos);
        if (result) {

            byte[] buffer = bos.toByteArray();
            File file = new File(path);
            FileOutputStream fos = null;
            try {
                fos = new FileOutputStream(file);
                // fixing bm is null bug instead of using BitmapFactory.decodeByteArray
                fos.write(buffer);
                fos.close();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        try {
            bos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
