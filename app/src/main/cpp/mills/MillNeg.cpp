//
// Created by Oleksiy Grechnyev
//

#include <opencv2/opencv.hpp>

#include "MillNeg.h"

namespace mills {
    void MillNeg::process(const cv::Mat &imgIn, cv::Mat &imgOut) {
        using namespace cv;
        validate(imgIn);
        copyTo(imgIn, imgOut, Mat());
        int w0 = imgOut.cols / 3, h0 = imgOut.rows / 3;
        Mat m(imgOut, Rect2i(w0, h0, w0, h0));
        bitwise_not(m, m);
    }

    std::string MillNeg::description() {
        return "Photo negative";
    }
} // mills