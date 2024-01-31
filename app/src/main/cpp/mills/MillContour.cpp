//
// Created by Oleksiy Grechnyev
//

#include <vector>

#include <opencv2/opencv.hpp>


#include "MillContour.h"

namespace mills {
//======================================================================================================================
    void MillContour::process(const cv::Mat &imgIn, cv::Mat &imgOut) {
        using namespace std;
        using namespace cv;
        validate(imgIn);

        // Threshold to a binary images
        cvtColor(imgIn, gray, COLOR_BGR2GRAY);
        cvtColor(gray, imgOut, COLOR_GRAY2BGR);
        GaussianBlur(gray, gray, Size(7, 7), 3.5, 3.5);
        threshold(gray, bin, 0, 255, THRESH_BINARY | THRESH_OTSU);
//        adaptiveThreshold(gray, bin, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 31,0.5);

        // Find and draw contours
        vector<vector<Point2i>> contours;
        vector<Vec4i> hierarchy;
        findContours(bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
//        drawContours(imgOut, contours, -1, Scalar(0, 0, 255), 2, LINE_8, hierarchy);
        for (int i = 0; i < contours.size(); ++i) {
            drawContours(imgOut, contours, i, brightColors[i % N_COLORS], 2, LINE_8, hierarchy);
        }
    }

//======================================================================================================================
    MillContour::MillContour() {
        using namespace std;
        using namespace cv;

        /// Create bright colors for visualization
        Mat_<Vec3b> mIn(1, N_COLORS, Vec3b{0, 255, 255});
        Mat_<Vec3b> mOut(1, N_COLORS);
        for (int i = 0; i < N_COLORS; ++i) {
            mIn(0, i)[0] = uchar(i * 180 / N_COLORS);
        }
        cvtColor(mIn, mOut, COLOR_HSV2BGR);
        for (int i = 0; i < N_COLORS; ++i) {
            brightColors.emplace_back(mOut(0, i));
        }
    }
//======================================================================================================================

} // mills