//
// Created by Oleksiy Grechnyev
//

#ifndef OPENCVFUN2_MILLCONTOUR_H
#define OPENCVFUN2_MILLCONTOUR_H

#include <opencv2/core.hpp>

#include "./Mill.h"


namespace mills {

    /// Example of thresholding and findContours
    class MillContour: public Mill {
    public:   //======= Methods
        MillContour();

        void process(const cv::Mat &imgIn, cv::Mat &imgOut) override;

        std::string description() override {
            return "Threshold and contours";
        }

    private:  //======= Fields
        static constexpr int N_COLORS = 12;
        cv::Mat gray, bin;
        std::vector<cv::Scalar> brightColors;
    };

} // mills

#endif //OPENCVFUN2_MILLCONTOUR_H
