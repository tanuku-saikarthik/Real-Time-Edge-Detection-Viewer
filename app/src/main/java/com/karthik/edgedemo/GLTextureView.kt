package com.example.edgedemo

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet

class GLTextureView(context: Context, attrs: AttributeSet?) : GLSurfaceView(context, attrs) {
    private val renderer: GLRenderer

    init {
        setEGLContextClientVersion(2)
        renderer = GLRenderer(context)
        setRenderer(renderer)
        renderMode = RENDERMODE_CONTINUOUSLY
    }
}
