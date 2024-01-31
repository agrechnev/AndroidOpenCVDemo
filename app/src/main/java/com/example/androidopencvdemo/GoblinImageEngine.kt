package com.example.androidopencvdemo

import android.graphics.Bitmap
import android.graphics.ImageFormat
import android.graphics.Matrix
import android.media.Image
import android.media.ImageReader
import android.util.Log
import android.util.Size
import androidx.activity.ComponentActivity
import java.util.concurrent.ArrayBlockingQueue
import kotlin.concurrent.thread

/// Provides an ImageEngine to receive an image from camera2
/// and all logic to process this image and eventually send it to a surface
/// Note: I use a separate thread to process+display image, connected to the
// image callback thread by a queue
///
class GoblinImageEngine(val activity: ComponentActivity, val surfaceView: GoblinSurfaceView) {
    /// A callback we to call whenever an FPS estimate is available: callbackFPS(fpsIn, fpsOut)
    var callbackFPS: ((Double, Double) -> Unit)? = null

    /// Imagereader: the image-retrieval sink for camera2
    var imageReader: ImageReader? = null

    /// A separate thread to process + display the image
    private lateinit var processThread: Thread

    /// The thread-safe queue connecting 2 threads
    private val queue = ArrayBlockingQueue<Bitmap>(Q_SIZE)

    /// The C++ image processor wrapped in kotlin
    private val imageProcessorWrapper = ImageProcessorWrapper()

    /// Thread+handler for the image callback
    private val imageWorker = GoblinWorker("Image")

    private val converter = YuvToRgbConverter(activity)

    /// Input and output (rotated sizes) and rotation stuff
    private lateinit var szIn: Size
    private lateinit var szOut: Size
    private var rotation: Int = -666

    /// Timestamp + frame counts to measure FPS
    /// IN = incoming frames, OUT = displayed frames, frame loss if the algo is too slow
    private var ts1: Long = -1
    private var frameCountIn: Int = 0
    private var frameCountOut: Int = 0

    /// We need this because of the C++ code in ImageProcessorWrapper
    fun close() {
        imageProcessorWrapper.close()
    }

    /// Set the mode (CV algo to use)
    fun setMode(mode: Int) {
        imageProcessorWrapper.setMode(mode)
    }

    /// Init the engine once we know the size
    fun initialize(sz: Size) {
        // Create ImageReader, start the thread
        imageReader =
            ImageReader.newInstance(sz.width, sz.height, ImageFormat.YUV_420_888, 3).apply {
                setOnImageAvailableListener(::imageCB, imageWorker.handler)
            }

        processThread = thread { processThreadCode() }

        // Check the screen orientation
        val display = activity.windowManager.defaultDisplay
        rotation = display.rotation
        Log.d(TAG, "GoblinImageEngine:initialize() : rotation=$rotation")
        szIn = sz
        szOut = rotateSize(sz)
        surfaceView.setSurfaceSize(szOut)
    }

    /// This is called for each incoming frame
    private fun imageCB(reader: ImageReader) {
        // Fetch the Image from ImageReader
        while (true) {
            val image: Image = reader.acquireLatestImage() ?: break

//            Log.d(
//                TAG,
//                "image ${image.width} x ${image.height}, format=${image.format}, ts = ${System.currentTimeMillis()}"
//            )
            frameCountIn++

            // Now convert the YuV image to an ARGB bitmap
            var bitmap: Bitmap =
                Bitmap.createBitmap(image.width, image.height, Bitmap.Config.ARGB_8888)
            converter.yuvToRgb(image, bitmap)

            // Always close the image asap, super-important !
            image.close()

            // Send bitmap to the queue, skip if the queue is full (frame loss happens here)
            if (queue.size < Q_SIZE) {
                queue.add(bitmap)
            }
        }
    }

    /// Process thread: get bitmaps from a buffer and process
    private fun processThreadCode() {
        while (true) {
            // Get bitmap from the queue
            if (queue.isEmpty()) {
                Thread.sleep(10)
                continue
            }
            val bitmapIn: Bitmap = queue.poll()!!

            estimateFPS()
//        Log.d(TAG, "bitmapIn: ${bitmapIn.width} x ${bitmapIn.height}")

            // Note: It's possible to rotate a bitmap in Kotlin, but a bit slow
            // It's also possible to rotate when displaying on a surface or canvas
            // However, I decided to rotate it in C++ before any computer vision logic
            // Thus bitmapOut will have the size of the ROTATED bitmapIn
            val bitmapOut = Bitmap.createBitmap(szOut.width, szOut.height, Bitmap.Config.ARGB_8888)

//        Log.d(TAG, "bitmapOut: ${bitmapOut.width} x ${bitmapOut.height}")

            // Process the bitmap in C++
            imageProcessorWrapper.process(rotation, bitmapIn, bitmapOut)

            // Show bitmap on the surface
            surfaceView.drawBitmap(bitmapOut)
        }
    }


    /// Estimate and log FPS
    private fun estimateFPS() {
        val ts: Long = System.currentTimeMillis()
        if (ts1 < 0) {
            ts1 = ts
            frameCountIn = 0
            frameCountOut = 0
        } else if (ts - ts1 > 5000) {
            // Every ~5 seconds
            val fpsIn: Double = 1000.0 * frameCountIn / (ts - ts1)
            val fpsOut: Double = 1000.0 * frameCountOut / (ts - ts1)
//            Log.d(TAG, "fpsIn=${fpsIn}, fpsOut=${fpsOut}")
            callbackFPS?.invoke(fpsIn, fpsOut)
            // Reset
            ts1 = ts
            frameCountIn = 0
            frameCountOut = 0
        }
        frameCountOut++
    }

    private fun rotateSize(sz: Size): Size {
        return if (rotation == 0 || rotation == 2) Size(sz.height, sz.width) else sz
    }

    companion object {
        const val Q_SIZE = 2
        const val TAG = "BRIANNA"
    }

}