#ifndef GRADIENTORIENTATIONS_H
#define GRADIENTORIENTATIONS_H

#include <opencv2/opencv.hpp>

using namespace cv;

class GradientOrientations
{
private:
    // Threshold to reduce noise
    float mag_threshold_;

    // Sobel parameter
    int ddepth_;
public:
    GradientOrientations();
    void exec(const Mat& input, Mat& output);
};

#endif // GRADIENTORIENTATIONS_H
