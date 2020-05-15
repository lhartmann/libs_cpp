#include "cv_img_avg.h"
#include <stdint.h>

double img_avg(cv::Mat &imgin, int channel) {
    uint64_t acc = 0;
    for (int r=0; r<imgin.size[0]; ++r) { // Iterate Rows
        for (int c=0; c<imgin.size[1]; ++c) { // Iterate Columns
            acc += imgin.data[r*imgin.step + c*imgin.channels() + channel];
        }
    }
    return double(acc) / imgin.size[0] / imgin.size[1];
}
