//
// Created by Oleksiy Grechnyev
//

#ifndef OPENCVFUN2_MILLNEG_H
#define OPENCVFUN2_MILLNEG_H

#include <opencv2/core.hpp>

#include "./Mill.h"


namespace mills {

    /// A trivial mill, apply photo negative to the central 1/9 rectangle
    class MillNeg : public Mill {
    public:
        void process(const cv::Mat &imgIn, cv::Mat &imgOut) override;

        std::string description() override;
    };

} // mills

#endif //OPENCVFUN2_MILLNEG_H
