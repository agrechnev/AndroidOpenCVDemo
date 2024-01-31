Computer Vision Example with OpenCV (C++) on Android
=====================================

By Oleksiy Grechnyev, 31 Jan 2024

Computer Vision with OpenCV in C++ or Python is a good fun. 
Camera (`VideoCapture`) and display (`imshow`) are easy to use.
Things get a bit trickier when deploying OpenCV algorithms
in C++ on Android. Here I give a complete working example.

Minimal knowledge of Android Development, C++, JNI, OpenCV is recommended to understand the code.

The code takes a video stream from the camera, processes it with OpenCV (C++),
and displays the result on the screen. There are 5 example OpenCV algorithms you can switch on the fly:

1. Photo negative of the central 1/9  
2. Canny Edges  
3. Thresholding and contour detection  
4. ORB keypoints visualized  
5. Lucas-Kanade sparse optical flow  

A release (APK file) is available here on GitHub, fell free to try it on your phone.

Enjoy!


Some technical details
---------

Warning: I am not a professional Android dev (I'm better with C++), and the code might be not of a production quality.
But I tried my best, and it basically works.

I had to use Camera2 to access the camera. CameraX has "analyze", but it is not powerful enough.
The camera resolution is set to 1280x960, which gives a reasonable FPS on a typical phone.

The code estimates both the `fpsIn` of incoming frames (typically 30), and `fpsOut` of
processed frames displayed on the screen, which is typically below 30, as the code loses frames
when algorithms are too slow.

The camera frames are converted from YuV Image to RGBA Bitmap. The Android Bitmap can be
directly passed into C++ and accessed via the `jnigraphics` library. I convert it
to `cv::Mat`. 

Finally, the resulting `cv::Mat` is converted back to a Bitmap and displayed on a 
SurfaceView (scaled to the screen size, with a proper aspect ratio).

I use several Kotlin threads (with and without Handlers) to create an efficient pipeline.

Building
---------

Building this project is not trivial. 

First, you'll need Android Studio with Android SDK, of course. 

Second, you'll have to open the "SDK Manager" and install the latest "NDK (Side by Side)" and CMake, in order to get C++ for Android. 

Third, go to the OpenCV website and download OpenCV for Android. I used version 4.9, file `opencv-4.9.0-android-sdk.zip`.
Unzip it somewhere on your computer. 

Finally, open the file `CMakeLists.txt` and **modify** it (change `OPENCV_DIR`) to hardcode the path to where you unzipped OpenCV on your computer.

Note: If you google for "OpenCV Android", you'll find lots of very confusing guides on the internet, most
of which simply **do not work** (seriously outdated or just broken). My method is not perfect but it
definitely works (as of Jan 2024). And I am linking OpenCV directly in `CMakeLists.txt`, Android
Studio isn't aware of it, no need for confusing stuff like "Importing Modules"! If there is a better
way to use OpenCV for Android (using Gradle and with no hardcoded path), I am not aware of it.

Important: I am using OpenCV in C++, **never** the thing called "Java OpenCV wrapper", this is much more 
powerful, plus you can deploy your cross-platform OpenCV/C++ algorithms on different platforms. I am also
using the official OpenCV release, no third-party versions (which are usually incomplete or outdated).

Important: You should use the **static** OpenCV C++ library on Android, thus the C++ STL must be also
**static** (the default mode of the NDK build). The **shared** OpenCV on Android is used for the
OpenCV Java wrapper and it is rather incomplete!

