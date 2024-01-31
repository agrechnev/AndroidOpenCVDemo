package com.example.androidopencvdemo

import android.graphics.Bitmap

/// Kotlin Wrapper to the C++ code
/// Here I use the "wrap a C++ class with JNI" pattern,
// where a C++ pointer is kept in a long Java/Kotlin field "handle"
class ImageProcessorWrapper {
    companion object {
        const val TAG = "BRIANNA"
        // Load the c++ .so library with the C++ code
        init {
            System.loadLibrary("androidopencvdemo")
        }
    }

    init {
        ctorCPP()
    }

    /// Native handle (pointer to the C++ object)
    var handle : Long = 0L

    /// De-facto constructor in C++
    external fun ctorCPP()

    /// Set the mode (choose a CV algorithm)
    external fun setMode(mode: Int)

    /// Process the image from bitmap bmIn to bitmap bmOut
    external fun process(rotation: Int, bmIn: Bitmap, bmOut: Bitmap)

    /// Delete the C++ object, must be called explicitly !
    external fun close()
}