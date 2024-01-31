package com.example.androidopencvdemo

import android.os.Handler
import android.os.HandlerThread

/// A thread with looper + handler
class GoblinWorker(name: String) : HandlerThread(name) {
    init {
        start()
    }
    val handler: Handler = Handler(looper)
}