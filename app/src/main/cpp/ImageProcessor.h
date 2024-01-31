//
// Created by seymour on 1/29/24.
//

#ifndef ANDROIDOPENCVDEMO_IMAGEPROCESSOR_H
#define ANDROIDOPENCVDEMO_IMAGEPROCESSOR_H


#include <opencv2/core.hpp>

#include "mills/MillEngine.h"

struct ImageProcessor {
    /// OpenCV frames, implemented as fields for optimization to avoid creating/deleting cv::Mat objects
    cv::Mat frameIn, frameOut, frameRot;

    /// Computer vision algos are here
    mills::MillEngine engine;

    /// Process frameIn to frameOut
    void process(int rotation);

    void setMode(int mode) {
        engine.setMode(mode);
    }
};


#endif //ANDROIDOPENCVDEMO_IMAGEPROCESSOR_H
