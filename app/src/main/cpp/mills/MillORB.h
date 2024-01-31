//
// Created by Oleksiy Grechnyev
//

#ifndef OPENCVFUN2_MILLORB_H
#define OPENCVFUN2_MILLORB_H


#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include "./Mill.h"

namespace mills {

    /// Detect and visualize ORB keypoints, nothing is done to them
    class MillORB: public Mill {
    public:  //======= Methods
        void process(const cv::Mat &imgIn, cv::Mat &imgOut) override;

        std::string description() override;

    private:  //======= Fields
        cv::Mat gray;
        /// The ORB detector
        cv::Ptr<cv::ORB> detector = cv::ORB::create();
    };

} // mills

#endif //OPENCVFUN2_MILLORB_H
