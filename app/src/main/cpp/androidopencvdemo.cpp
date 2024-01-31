/// The JNI bridge, here I implement the ImageProcessorWrapper (kotlin class) methods in C++
/// This file is JNI-aware(uses JNI API from jni.h) and Android-aware,
/// the rest of the C++ code is pure (cross-platform, not using any JNI or Android APIs)

#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>

#include <opencv2/imgproc.hpp>

#include "ImageProcessor.h"

const char * TAG = "BRIANNA";

extern "C"
JNIEXPORT void JNICALL
Java_com_example_androidopencvdemo_ImageProcessorWrapper_ctorCPP(JNIEnv *env, jobject thisObj) {
//    __android_log_print(ANDROID_LOG_DEBUG, TAG, "=====> C++ : ctorCPP");
    ImageProcessor * pImageProcessor = new ImageProcessor();
    // Set pointer to handle (kotlin class field)
    jclass thisClass = env->GetObjectClass(thisObj);
    jfieldID fieldHandle = env->GetFieldID(thisClass, "handle", "J");  // "J" means long !
    env->SetLongField(thisObj, fieldHandle, (jlong)pImageProcessor);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_androidopencvdemo_ImageProcessorWrapper_close(JNIEnv *env, jobject thisObj) {
//    __android_log_print(ANDROID_LOG_DEBUG, TAG, "=====> C++ : close");
    // Get pointer from handle and delete the object
    jclass thisClass = env->GetObjectClass(thisObj);
    jfieldID fieldHandle = env->GetFieldID(thisClass, "handle", "J");  // "J" means long !
    jlong handle = env->GetLongField(thisObj, fieldHandle);
    ImageProcessor * pImageProcessor = (ImageProcessor *)handle;
    if (pImageProcessor != nullptr)
        delete pImageProcessor;
    // Very important: now set the field to 0, to avoid double delete segfault
    env->SetLongField(thisObj, fieldHandle, jlong(0));
}

/// Convert Android bitmap (RGBA) to a cv::Mat (BGR)
void bitmap2mat(JNIEnv *env, jobject bm, cv::Mat &img) {
    using namespace std;
    using namespace cv;

    // Get image size from bitmap
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, bm, &info);
    int imW = info.width, imH = info.height;
//    __android_log_print(ANDROID_LOG_INFO, TAG, "bitmap C++ %d x %d", imW, imH);

    // Lock pixels
    // The pixels are in RGBA !
    void *p;
    if (AndroidBitmap_lockPixels(env, bm, &p)) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "bitmap2mat: AndroidBitmap_lockPixels failure !");
        return;
    }

    // Wrap the pixels with cv::Mat, no copying
    Mat imgOrig(imH, imW, CV_8UC4, p);

    // RGBA -> BGR, img.data is created or reused as appropriate
    cvtColor(imgOrig, img, COLOR_RGBA2BGR);

    // Unlock pixels
    AndroidBitmap_unlockPixels(env, bm);
}

/// Convert a cv::Mat (BGR) to an Android bitmap (RGBA)
void mat2bitmap(JNIEnv *env, cv::Mat &img, jobject bm) {
    using namespace std;
    using namespace cv;

    // Get image size from bitmap
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, bm, &info);
    int imW = info.width, imH = info.height;
//    __android_log_print(ANDROID_LOG_INFO, TAG, "bitmap C++ %d x %d", imW, imH);

    // Lock pixels
    // The pixels are in RGBA !
    void *p;
    if (AndroidBitmap_lockPixels(env, bm, &p)) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "mat2bitmap: AndroidBitmap_lockPixels failure !");
        return;
    }

    if (imW != img.cols || imH != img.rows) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "mat2bitmap: size mismatch !");
        return;
    }

    // Wrap the pixels with cv::Mat, no copying
    Mat imgBM(imH, imW, CV_8UC4, p);

    // BGR -> RGBA
    cvtColor(img, imgBM, COLOR_BGR2RGBA);

    // Unlock pixels
    AndroidBitmap_unlockPixels(env, bm);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_androidopencvdemo_ImageProcessorWrapper_process(JNIEnv *env, jobject thisObj,
                                                                 jint rotation, jobject bmIn,
                                                                 jobject bmOut) {
    // Get pointer pImageProcessor from handle
    jclass thisClass = env->GetObjectClass(thisObj);
    jfieldID fieldHandle = env->GetFieldID(thisClass, "handle", "J");  // "J" means long !
    jlong handle = env->GetLongField(thisObj, fieldHandle);
    ImageProcessor * pImageProcessor = (ImageProcessor *)handle;

    // Convert bitmap to cv::Mat
    bitmap2mat(env, bmIn, pImageProcessor->frameIn);

    // Process it with opencv
    pImageProcessor->process(rotation);

    // Convert back to bitmap
    mat2bitmap(env, pImageProcessor->frameOut, bmOut);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_androidopencvdemo_ImageProcessorWrapper_setMode(JNIEnv *env, jobject thisObj,
                                                                 jint mode) {
    // Get pointer pImageProcessor from handle
    jclass thisClass = env->GetObjectClass(thisObj);
    jfieldID fieldHandle = env->GetFieldID(thisClass, "handle", "J");  // "J" means long !
    jlong handle = env->GetLongField(thisObj, fieldHandle);
    ImageProcessor * pImageProcessor = (ImageProcessor *)handle;

    pImageProcessor->setMode(mode);
}