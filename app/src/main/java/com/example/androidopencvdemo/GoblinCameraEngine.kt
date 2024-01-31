package com.example.androidopencvdemo

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.ImageFormat
import android.hardware.camera2.CameraCaptureSession
import android.hardware.camera2.CameraCharacteristics
import android.hardware.camera2.CameraDevice
import android.hardware.camera2.CameraManager
import android.hardware.camera2.CaptureRequest
import android.hardware.camera2.params.StreamConfigurationMap
import android.util.Log
import android.util.Size
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.result.contract.ActivityResultContract
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.content.ContextCompat
import androidx.lifecycle.DefaultLifecycleObserver
import androidx.lifecycle.LifecycleOwner
import java.lang.Exception
import kotlin.math.max

/// Open the camera with camera2 and grab frames
/// Lifecycle logic: Close camera in onPause(), open in onResume()
class GoblinCameraEngine(
    val activity: ComponentActivity,
    val goblinImageEngine: GoblinImageEngine
) : DefaultLifecycleObserver {
    private lateinit var cameraManager: CameraManager
    private lateinit var cameraSize: Size
    private var camera: CameraDevice? = null

    private var cameraInitialized: Boolean = false
    private var cameraActive: Boolean = false

    /// Thread+handler for the camera, don't use the default handler, inefficient !
    private val cameraWorker = GoblinWorker("Camera")

    override fun onCreate(owner: LifecycleOwner) {
        Log.d(TAG, "GoblinCameraEngine.onCreate")
        super.onCreate(owner)

        if (allPermissionsGranted()) {
            initCamera()
        } else {
            requestPermissions()
        }
    }

    override fun onResume(owner: LifecycleOwner) {
        Log.d(TAG, "GoblinCameraEngine.onResume")
        super.onResume(owner)
        openCamera()
    }

    override fun onPause(owner: LifecycleOwner) {
        Log.d(TAG, "GoblinCameraEngine.onPause")
        super.onPause(owner)
        closeCamera()
    }

    //==============================================================================================
    /// Print camera info to the log
    private fun cameraInfo() {
        val cameraIds: Array<String> = cameraManager.cameraIdList
        Log.d(TAG, "cameraIds=${cameraIds.contentToString()}")

        for (id: String in cameraIds) {
            Log.d(TAG, "=================================================")
            Log.d(TAG, "id=$id")
            val cameraCharacteristics: CameraCharacteristics =
                cameraManager.getCameraCharacteristics(id)
            val lFace: Int = cameraCharacteristics.get(CameraCharacteristics.LENS_FACING)!!
            val lFaceS: String =
                if (lFace == CameraCharacteristics.LENS_FACING_FRONT) "FRONT" else "BACK"
            Log.d(TAG, "LENS_FACING=${lFace} (${lFaceS})")
            Log.d(
                TAG,
                "ORIENTATION=${cameraCharacteristics.get(CameraCharacteristics.SENSOR_ORIENTATION)!!}"
            )
            // Print more variables if you want
            val ssMap: StreamConfigurationMap =
                cameraCharacteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP)!!
            Log.d(TAG, "outputFormats=${ssMap.outputFormats.contentToString()}")
            //  Emulator [1, 256, 34, 35] = RGBA_8888, JPEG, PRIVATE, YUV_420_888
            // ZenFone 8: [32, 256, 34, 35, 36, 37, 1212500294] = RAW_SENSOR, JPEG, PRIVATE, YUV_420_888, RAW_PRIVATE, RAW10, HEIC
            val fmt: Int = ImageFormat.YUV_420_888
            val oSizes: Array<Size> = ssMap.getOutputSizes(fmt)
            Log.d(TAG, "OUTPUT SIZES for fmt=${fmt} : ${oSizes.contentToString()}")
        }
        Log.d(TAG, "=================================================")
    }

    /// Choose camera image size (resolution) we want
    private fun selectSize(): Size {
        val cameraCharacteristics: CameraCharacteristics =
            cameraManager.getCameraCharacteristics(CAMERA_ID)
        val ssMap: StreamConfigurationMap =
            cameraCharacteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP)!!
        val fmt: Int = ImageFormat.YUV_420_888
        val oSizes: Array<Size> = ssMap.getOutputSizes(fmt)
        if (oSizes.isEmpty())
            throw Exception("Something is Wrong !")

        // Try 1280x960, if not, look for something else
        val preferredSize = Size(1280, 960)
        if (preferredSize in oSizes)
            return preferredSize

        var chosenSize = Size(0, 0)
        val maxRes: Int = 1280
        // Choose largest size, but only up to maxRes from the list
        for (sz in oSizes) {
            if (sz.width > maxRes || sz.height > maxRes)
                continue
            else if (max(sz.width, sz.height) > max(chosenSize.width, chosenSize.height))
                chosenSize = sz
        }

        // [4576x3432, 4576x2574, 4560x2052, 4096x2160, 3840x2160, 3432x3432, 2880x2160, 2560x1440, 2048x1080, 1920x1440, 1920x1080, 1920x864, 1440x1080, 1280x960, 1280x720, 1080x1080, 720x480, 640x480, 352x288, 320x240, 176x144]
        return chosenSize
    }

    /// Called once per activity lifecycle, once we have camera permission
    private fun initCamera() {
        Log.d(TAG, "GoblinCameraEngine.initCamera")
        cameraManager = activity.getSystemService(Context.CAMERA_SERVICE) as CameraManager

        // Print camera info if needed, for debugging if anything goes wrong
//        cameraInfo()

        // Choose the image size we are going to use (aka camera resolution)
        cameraSize = selectSize()
        Log.d(TAG, "cameraSize=$cameraSize")
        goblinImageEngine.initialize(cameraSize)

        cameraInitialized = true
        openCamera()
    }

    /// Executed after initCamera() and on each camera restart (onResume)
    @SuppressWarnings("MissingPermission")
    private fun openCamera() {
        Log.d(TAG, "GoblinCameraEngine.openCamera")
        // Safe to call any time and multiple times
        if (!cameraInitialized || cameraActive || camera != null)
            return
        cameraActive = true
        // We signal that we are opening the camera now, the actual camera field is set once opened
        cameraManager.openCamera(CAMERA_ID, object : CameraDevice.StateCallback() {
            override fun onOpened(camera: CameraDevice) {
                Log.d(TAG, "CameraDevice.StateCallback.onOpened")
                this@GoblinCameraEngine.camera = camera
                startCapture()
            }

            override fun onDisconnected(camera: CameraDevice) {
                Log.d(TAG, "CameraDevice.StateCallback.onDisconnected")
            }

            override fun onError(camera: CameraDevice, p1: Int) {
                Log.e(TAG, "CameraDevice.StateCallback.onError")
            }
        }, cameraWorker.handler)
    }

    private fun closeCamera() {
        Log.d(TAG, "GoblinCameraEngine.closeCamera")
        camera?.close()
        camera = null
        cameraActive = false
    }

    /// Once the camera is opened, we run this to start capturing frames
    private fun startCapture() {
        Log.d(TAG, "GoblinCameraEngine.startCapture")
        if (!cameraActive || camera == null)
            return
        val imageReader = goblinImageEngine.imageReader!!
        val surfaces = listOf(imageReader.surface)
        camera!!.createCaptureSession(
            surfaces,
            object : CameraCaptureSession.StateCallback() {
                override fun onConfigured(session: CameraCaptureSession) {
                    Log.d(TAG, "CameraCaptureSession.StateCallback.onConfigured")
                    val builder: CaptureRequest.Builder =
                        session.device.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW)
                    builder.addTarget(imageReader.surface)
                    session.setRepeatingRequest(builder.build(), null, cameraWorker.handler)
                }

                override fun onConfigureFailed(session: CameraCaptureSession) {
                    Log.e(TAG, "CameraCaptureSession.StateCallback.onConfigureFailed")
                }
            }, cameraWorker.handler
        )
    }

    //==============================================================================================
    private val activityResultLauncher =
        activity.registerForActivityResult(ActivityResultContracts.RequestMultiplePermissions()) { permissions ->
            var permissionGranted = true
            permissions.entries.forEach {
                if (it.key in REQUIRED_PERMISSIONS && !it.value)
                    permissionGranted = false
            }
            if (!permissionGranted) {
                Toast.makeText(activity, "Permission request denied !", Toast.LENGTH_LONG).show()
                activity.finish()
            } else {
                initCamera()
            }
        }

    private fun requestPermissions() {
        activityResultLauncher.launch(REQUIRED_PERMISSIONS)
    }

    private fun allPermissionsGranted() = REQUIRED_PERMISSIONS.all {
        ContextCompat.checkSelfPermission(
            activity.baseContext,
            it
        ) == PackageManager.PERMISSION_GRANTED
    }

    companion object {
        const val TAG = "BRIANNA"

        /// Ideally, we should look for the "back" camera, in real life it's "0"
        const val CAMERA_ID = "0"
        private val REQUIRED_PERMISSIONS = arrayOf(Manifest.permission.CAMERA)
    }
}