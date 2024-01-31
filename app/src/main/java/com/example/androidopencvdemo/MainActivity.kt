package com.example.androidopencvdemo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.RadioButton
import android.widget.Toast
import com.example.androidopencvdemo.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding
    private lateinit var cameraEngine: GoblinCameraEngine
    private lateinit var imageEngine: GoblinImageEngine

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        imageEngine = GoblinImageEngine(this, binding.goblinSurfaceView)
        imageEngine.callbackFPS = { fpsIn, fpsOut ->
//            binding.goblinTextViewFPS.text = "fpsIn=${fpsIn}, fpsOut=${fpsOut}"
            binding.goblinTextViewFPS.text = "fpsIn=%.2f, fpsOut=%.2f".format(fpsIn, fpsOut)
        }
        cameraEngine = GoblinCameraEngine(this, imageEngine)
        lifecycle.addObserver(cameraEngine)
    }

    override fun onDestroy() {
        // To avoid C++ memory leak
        imageEngine.close()
        super.onDestroy()
    }

    /// Select algo with the radio button
    fun onRadioButtonClicked(view: View?) {
        if (view == null)
            return
        val rb = view as RadioButton
//        Toast.makeText(this, "SELECTED = ${rb.text}", Toast.LENGTH_LONG).show()
        val mode = rb.text.toString().toInt()   // Integer 0..4
        imageEngine.setMode(mode)
    }

    companion object {
        const val TAG = "BRIANNA"
    }
}