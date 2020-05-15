#include "cv_img_dev.h"

double img_dev(cv::Mat &imgin, int channel, double mean) {
    double acc = 0;
    for (int r=0; r<imgin.size[0]; ++r) { // Iterate Rows
        for (int c=0; c<imgin.size[1]; ++c) { // Iterate Columns
            acc += pow(imgin.data[r*imgin.step + c*imgin.channels() + channel] - mean, 2);
        }
    }
    return sqrt(acc / imgin.size[0] / imgin.size[1]);
}
