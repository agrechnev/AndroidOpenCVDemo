//
// Created by Oleksiy Grechnyev
//

#include <string>

#include <opencv2/imgproc.hpp>

#include "./MillNeg.h"
#include "./MillCanny.h"
#include "./MillContour.h"
#include "./MillORB.h"
#include "./MillOFLK.h"

#include "MillEngine.h"


namespace mills {
//======================================================================================================================
    void MillEngine::process(const cv::Mat &imgIn, cv::Mat &imgOut) {
        using namespace std;
        using namespace cv;
        unique_ptr<Mill> & mill = childMills.at(mode);
        mill->process(imgIn, imgOut);
        string text = to_string(mode) + " : " + mill->description();
        putText(imgOut, text, {20, 60}, FONT_HERSHEY_SIMPLEX, 1.5, {255, 0, 255}, 3);
    }

//======================================================================================================================
    MillEngine::MillEngine() {
        using namespace std;
        childMills.push_back(make_unique<MillNeg>());
        childMills.push_back(make_unique<MillCanny>());
        childMills.push_back(make_unique<MillContour>());
        childMills.push_back(make_unique<MillORB>());
        childMills.push_back(make_unique<MillOFLK>());
    }

//======================================================================================================================
    int MillEngine::getMode() const {
        return mode;
    }

    void MillEngine::setMode(int mode) {
        if (mode >=0 && mode < childMills.size()) {
            MillEngine::mode = mode;
            childMills.at(mode) -> reset();
        }
    }

    int MillEngine::getNumMills() const {
        return childMills.size();
    }

    void MillEngine::reset() {
        for (auto & m: childMills)
            m->reset();
    }

    std::string MillEngine::description() {
        return "Switchable Engine";
    }
//======================================================================================================================

} // mills