package com.example.androidopencvdemo

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Matrix
import android.graphics.Paint
import android.util.AttributeSet
import android.util.Log
import android.util.Size
import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.constraintlayout.widget.ConstraintLayout

/// SurfaceView with a boolean flag surfaceActive and other goodies
class GoblinSurfaceView(context: Context, attrs: AttributeSet) : SurfaceView(context, attrs),
    SurfaceHolder.Callback {

    var surfaceActive: Boolean = false
    private val paint = Paint()

    /// Fixed image size we want to draw
    private lateinit var imSize: Size

    /// Matrix used to scale our bitmap when drawing
    private var matrix: Matrix? = null

    init {
        holder.addCallback(this)
    }

    fun setSurfaceSize(sz: Size) {
//        layoutParams.width = sz.width / 2
//        layoutParams.height = sz.height / 2

        // This is a bit tricky. First we set the aspect ratio programmatically to match the frame size
        // And let the layout manager do its job
        // Eventually (after the first layout) we get nonzero width x height
        val params: ConstraintLayout.LayoutParams = layoutParams as ConstraintLayout.LayoutParams
        params.dimensionRatio = "${sz.width}:${sz.height}"
        imSize = sz
    }

    /// Draw a bitmap on this surface, scaling as appropriate, keeping aspect ratio
    fun drawBitmap(bitmap: Bitmap) {
//        Log.d(TAG, "ACTUAL VIEW SIZE = ${width} x ${height}")
        if (!surfaceActive)
            return

        // Set up matrix to scale our frame to the view size, calculated only once
        if (matrix == null) {
            if (width == 0)
                 return
            val scale: Float = 1.0f * width / imSize.width
            Log.d(TAG, "scale=${scale}")
            matrix = Matrix().apply { postScale(scale, scale) }
        }

        val canvas: Canvas? = holder.lockCanvas()
        if (canvas != null) {
            // Scale the bitmap to the SurfaceView actual size
            // Probably scaling in C++ would be faster, but let's try it the Android way ...
            canvas.drawBitmap(bitmap, matrix!!, paint)
            holder.unlockCanvasAndPost(canvas)
        }
    }

    //================================================
    override fun surfaceCreated(p0: SurfaceHolder) {
        Log.d(TAG, "GoblinSurfaceView.surfaceCreated()")
        surfaceActive = true
    }

    override fun surfaceChanged(p0: SurfaceHolder, p1: Int, p2: Int, p3: Int) {
        Log.d(TAG, "GoblinSurfaceView.surfaceChanged()")
    }

    override fun surfaceDestroyed(p0: SurfaceHolder) {
        Log.d(TAG, "GoblinSurfaceView.surfaceDestroyed()")
        surfaceActive = false
        matrix = null
    }

    companion object {
        const val TAG = "BRIANNA"
    }
}