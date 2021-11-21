#include <stdio.h>
#include <stdlib.h>

#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

TEST(Opencv, gtest) {
    cv::Mat one = cv::Mat::ones(3, 3, CV_8UC1);
    bool ret = ((int)*one.data == 1);
    EXPECT_TRUE(ret);
}
