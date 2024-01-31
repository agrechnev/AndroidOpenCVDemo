//
// Created by Oleksiy Grechnyev
//

#ifndef OPENCVFUN2_MILLCANNY_H
#define OPENCVFUN2_MILLCANNY_H

#include <opencv2/imgproc.hpp>

#include "./Mill.h"

namespace mills {

    /// Visualize canny edges
    class MillCanny : public Mill {
    public:
        void process(const cv::Mat &imgIn, cv::Mat &imgOut) override {
            using namespace cv;
            validate(imgIn);
            cvtColor(imgIn, gray, COLOR_BGR2GRAY);
            GaussianBlur(gray, gray, Size(7,7), 1.5, 1.5);
            Canny(gray, edges, 5, 30);
            cvtColor(edges, imgOut, COLOR_GRAY2BGR);
        }

        std::string description() override {
            return "Canny Edges";
        }

    private:
        cv::Mat gray, edges;
    };
}


#endif //OPENCVFUN2_MILLCANNY_H
