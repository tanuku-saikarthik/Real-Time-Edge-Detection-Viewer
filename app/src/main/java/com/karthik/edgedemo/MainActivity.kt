package com.example.edgedemo

import android.Manifest
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.SurfaceTexture
import android.os.Bundle
import android.util.Log
import android.view.TextureView
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import kotlin.concurrent.thread

class MainActivity : AppCompatActivity() {
    companion object {
        init { System.loadLibrary("native-lib") }
        private const val TAG = "MainActivity"
        private const val REQUEST_CAMERA = 123
    }

    private lateinit var textureView: TextureView
    private lateinit var glView: GLTextureView
    private lateinit var toggleBtn: Button
    private var showEdges = true

    // throttle processing to avoid UI jank
    @Volatile private var processing = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        textureView = findViewById(R.id.camera_texture)
        glView = findViewById(R.id.gl_view)
        toggleBtn = findViewById(R.id.btn_toggle)

        toggleBtn.setOnClickListener {
            showEdges = !showEdges
            setShowEdgesNative(if (showEdges) 1 else 0)
        }

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
            != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.CAMERA), REQUEST_CAMERA)
        } else {
            startCamera()
        }

        textureView.surfaceTextureListener = object : TextureView.SurfaceTextureListener {
            override fun onSurfaceTextureAvailable(surface: SurfaceTexture, width: Int, height: Int) {
                Log.d(TAG, "Texture available $width x $height")
                setupCameraAndNative(surface, width, height)
            }
            override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture, width: Int, height: Int) {}
            override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean { return true }
            override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {
                // Throttle: process only if not already processing
                if (processing) return
                processing = true
                // Capture a Bitmap on UI thread then send to background thread
                val bmp: Bitmap? = textureView.bitmap
                if (bmp != null) {
                    val w = bmp.width
                    val h = bmp.height
                    val pixels = IntArray(w * h)
                    bmp.getPixels(pixels, 0, w, 0, 0, w, h)
                    bmp.recycle()
                    thread {
                        processFrameNative(pixels, w, h)
                        processing = false
                    }
                } else {
                    processing = false
                }
            }
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_CAMERA) {
            if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                startCamera()
            }
        }
    }

    private fun startCamera() {
        Camera1Helper.startCamera(this, textureView)
    }

    override fun onResume() {
        super.onResume()
        glView.onResume()
    }

    override fun onPause() {
        super.onPause()
        glView.onPause()
        Camera1Helper.stopCamera()
    }

    // Native methods
    private external fun setupCameraAndNative(surfaceTexture: SurfaceTexture, width: Int, height: Int)
    private external fun setShowEdgesNative(flag: Int)
    private external fun processFrameNative(pixels: IntArray, width: Int, height: Int)
}
