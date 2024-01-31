cmake_minimum_required(VERSION 3.22.1)
project("androidopencvdemo")
set(CMAKE_CXX_STANDARD 17)

# OpenCV
# This is Hardcoded!!! Change for your location !!!
# The directory must end with "OpenCV-android-sdk/sdk/native"
set(OPENCV_DIR /home/seymour/w/AndroidProj/OpenCV-android-sdk/sdk/native)
include_directories(${OPENCV_DIR}/jni/include)

# OpenCV static libs
set(OpenCV_LIBS_DIR1 ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/)
file(GLOB OpenCV_LIBS1 "${OpenCV_LIBS_DIR1}/*.a")
set(OpenCV_LIBS_DIR2 ${OPENCV_DIR}/3rdparty/libs/${ANDROID_ABI}/)
file(GLOB OpenCV_LIBS2 "${OpenCV_LIBS_DIR2}/*.a")
set(OpenCV_LIBS ${OpenCV_LIBS1} ${OpenCV_LIBS2} z)
message("OpenCV_LIBS=${OpenCV_LIBS}")

add_library(${CMAKE_PROJECT_NAME} SHARED
        androidopencvdemo.cpp
        ImageProcessor.cpp ImageProcessor.h
        #
        mills/Mill.h
        mills/MillCanny.h
        mills/MillNeg.cpp mills/MillNeg.h
        mills/MillOFLK.cpp mills/MillOFLK.h
        mills/MillORB.cpp mills/MillORB.h
        mills/MillContour.cpp mills/MillContour.h
        mills/MillEngine.cpp mills/MillEngine.h
        )

target_link_libraries(${CMAKE_PROJECT_NAME} android log jnigraphics ${OpenCV_LIBS})
