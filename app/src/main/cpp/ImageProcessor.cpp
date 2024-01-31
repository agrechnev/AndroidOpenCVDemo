//
// Created by seymour on 1/29/24.
//

#include <opencv2/opencv.hpp>

#include "ImageProcessor.h"

void ImageProcessor::process(int rotation) {
    using namespace std;
    using namespace cv;

    // Smart rotation
    switch (rotation) {
        case 0:
            rotate(frameIn, frameRot, ROTATE_90_CLOCKWISE);
            break;
        case 1:
            copyTo(frameIn, frameRot, Mat());
            break;
        case 2:
            rotate(frameIn, frameRot, ROTATE_90_COUNTERCLOCKWISE);
            break;
        case 3:
            rotate(frameIn, frameRot, ROTATE_180);
            break;
    }

    // Process the image with Computer Vision algos
    engine.process(frameRot, frameOut);
}
