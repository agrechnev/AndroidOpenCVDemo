//
// Created by Oleksiy Grechnyev
//

#include <numeric>
#include <iostream>

#include <opencv2/opencv.hpp>

#include "MillOFLK.h"

namespace mills {

//======================================================================================================================
    void MillOFLK::process(const cv::Mat &imgIn, cv::Mat &imgOut) {
        using namespace std;
        using namespace cv;
        validate(imgIn);
        copyTo(imgIn, imgOut, Mat());
        cvtColor(imgIn, gray, COLOR_BGR2GRAY);

        if (grayPrev.empty() || grayPrev.size() != gray.size() || goodFraction() < MIN_GOOD_FRACTION) {
            // Perform an actual reset, generate new points
            cout << "GENERATE !!!" << endl;
            generatePoints();
        } else {
            // Run LK, update points
            runLKStep();
        }
        CV_Assert(points.size() == pointFlags.size());

        //  visualize points
        for (int i = 0; i < points.size(); ++i) {
            if (pointFlags[i]) {
                circle(imgOut, points[i], 5, brightColors[i % N_COLORS], -1);
            }
        }

        grayPrev = gray.clone();
    }

//======================================================================================================================
    void MillOFLK::generatePoints() {
        using namespace std;
        using namespace cv;
        goodFeaturesToTrack(gray, points, 100, 0.01, 10, Mat(), 3, 3, false, 0.04);
        pointFlags.assign(points.size(), true);
    }

//======================================================================================================================
    void MillOFLK::runLKStep() {
        using namespace std;
        using namespace cv;

        // Generate good poits
        vector<Point2f> pp0; // Good points
        vector<int> indices; // Indices of good points in the total points list

        for (int i = 0; i < points.size(); ++i) {
            if (pointFlags[i]) {
                pp0.push_back(points[i]);
                indices.push_back(i);
            }
        }

        // Run the actual LK
        vector<Point2f> pp1; // Updated points from pp0
        vector<uchar> status;
        vector<float> err;
        TermCriteria criteria = TermCriteria(TermCriteria::COUNT|TermCriteria::EPS, 20, 0.03);
        calcOpticalFlowPyrLK(grayPrev, gray, pp0, pp1, status, err, Size(31, 31), 3, criteria);

        // Update the big point list with flags
        for (int j = 0; j < pp0.size(); ++j) {
            int idx = indices[j];
            bool sj = status[j] > 0;
            pointFlags[idx] = sj;
            if (sj) {
                points[idx] = pp1[j];
            }
        }
    }

//======================================================================================================================
    void MillOFLK::reset() {
        grayPrev.release();
        points.clear();
        pointFlags.clear();
    }

//======================================================================================================================
    double MillOFLK::goodFraction() {
        using namespace std;

        if (points.empty() || pointFlags.empty()) {
            return 0.0;
        }
        int sum = accumulate(pointFlags.begin(), pointFlags.end(), 0);
        double frac = 1.0 * sum / pointFlags.size();
//        cout << "frac = " << sum << " / " << points.size() << " = " << frac << endl;

        return frac;
    }

//======================================================================================================================
    MillOFLK::MillOFLK() {
        using namespace std;
        using namespace cv;

        /// Create bright colors for LK points
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

    std::string MillOFLK::description() {
        return "Optical Flow: Lucas-Kanade";
    }

//======================================================================================================================

} // mill