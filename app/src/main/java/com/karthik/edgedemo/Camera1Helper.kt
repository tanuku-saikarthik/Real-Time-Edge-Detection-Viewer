package com.example.edgedemo

import android.app.Activity
import android.hardware.Camera
import android.util.Log
import android.view.TextureView

object Camera1Helper {
    private var camera: Camera? = null
    private const val TAG = "Camera1Helper"

    fun startCamera(activity: Activity, textureView: TextureView) {
        try {
            camera = Camera.open()
            camera?.setPreviewTexture(textureView.surfaceTexture)
            camera?.setDisplayOrientation(90)
            camera?.startPreview()
            Log.d(TAG, "Camera started")
        } catch (e: Exception) {
            Log.e(TAG, "Camera start failed: ${e.message}")
        }
    }

    fun stopCamera() {
        try {
            camera?.stopPreview()
            camera?.release()
            camera = null
            Log.d(TAG, "Camera stopped")
        } catch (e: Exception) {
            // ignore
        }
    }
}
