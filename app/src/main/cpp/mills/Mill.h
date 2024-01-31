//
// Created by Oleksiy Grechnyev
//

#ifndef OPENCVFUN2_MILL_H
#define OPENCVFUN2_MILL_H

#include <string>

#include <opencv2/core.hpp>

namespace mills {

    /// Abstract class for a mill (image-to-image processor)
    class Mill {
    public:
        /// Process an image, imgIn should not be modified
        virtual void process(const cv::Mat &imgIn, cv::Mat &imgOut) = 0;

        /// Reset the mill to a fresh state (e.g. if the resolution changes or something)
        virtual void reset() {}

        virtual std::string description() {return "";}

        virtual ~Mill() = default;

        static void validate(const cv::Mat &img) {
            CV_Assert(!img.empty());
            CV_Assert(img.type() == CV_8UC3);
        }


    };

} // mills

#endif //OPENCVFUN2_MILL_H
