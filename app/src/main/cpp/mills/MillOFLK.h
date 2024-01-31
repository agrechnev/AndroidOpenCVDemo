//
// Created by Oleksiy Grechnyev
//

#ifndef OPENCVFUN2_MILLOFLK_H
#define OPENCVFUN2_MILLOFLK_H

#include <vector>

#include <opencv2/core.hpp>

#include "./Mill.h"


namespace mills {

    /// Visualize Lucas-Kanade sparse optical flow
    class MillOFLK: public Mill {
    public:  //======= Methods
        MillOFLK();

        void process(const cv::Mat &imgIn, cv::Mat &imgOut) override;

        void reset() override;

        std::string description() override;

    private:  //======= Methods
        /// Generate a uniform point grid
        void generatePoints();

        /// Make one LK step
        void runLKStep();

        /// Calculate fraction (percentage) of good points
        double goodFraction();


    private:  //======= Fields
        /// If too few points are left, reset
        static constexpr double MIN_GOOD_FRACTION = 0.3;
        static constexpr int N_COLORS = 12;

        cv::Mat gray;
        cv::Mat grayPrev;
        /// Current points
        std::vector<cv::Point2f> points;
        /// Current point flags (false=dead point, true=good)
        std::vector<bool> pointFlags;
        std::vector<cv::Scalar> brightColors;
    };

} // mill

#endif //OPENCVFUN2_MILLOFLK_H
