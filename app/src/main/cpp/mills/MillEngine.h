//
// Created by Oleksiy Grechnyev
//

#ifndef OPENCVFUN2_MILLENGINE_H
#define OPENCVFUN2_MILLENGINE_H

#include <vector>
#include <memory>

#include "./Mill.h"

namespace mills {

    /// A switchable mill, also add a text description on the frame
    class MillEngine : public Mill {
    public:   //======= Methods
        MillEngine();

        void process(const cv::Mat &imgIn, cv::Mat &imgOut) override;

        void reset() override;

        std::string description() override;

        int getMode() const;

        int getNumMills() const;

        void setMode(int mode);

    private:  //======= Fields
        int mode = 0;

        std::vector<std::unique_ptr<Mill>> childMills;
    };

} // mills

#endif //OPENCVFUN2_MILLENGINE_H
