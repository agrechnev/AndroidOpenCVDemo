//
// Created by Oleksiy Grechnyev
//

#include <opencv2/opencv.hpp>

#include "MillORB.h"

namespace mills {
    void MillORB::process(const cv::Mat &imgIn, cv::Mat &imgOut) {
        using namespace std;
        using namespace cv;
        validate(imgIn);

        cvtColor(imgIn, gray, COLOR_BGR2GRAY);
        vector<KeyPoint> kps;
        detector->detect(gray, kps);
        drawKeypoints(gray, kps, imgOut, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    }

    std::string MillORB::description() {
        return "ORB keypoints";
    }
} // mills