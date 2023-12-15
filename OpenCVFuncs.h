/*
 * Copyright 2022 Fan Ziqi
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JAKAVISION_OPENCVFUNCS_H
#define JAKAVISION_OPENCVFUNCS_H

//OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

void get_xy(int EVENT, int x, int y, __attribute__((unused)) int flags, __attribute__((unused)) void *userdata);

void color_mask(cv::VideoCapture &cap);

void
find_color(cv::Mat &HSV_img, cv::Mat &color_mat, int *color_value, std::vector<std::vector<cv::Point>> &color_contours,
           std::vector<cv::Vec4i> &color_hierarchy);

void DrawPoints(cv::Mat &show_img, std::vector<cv::Point> points, const cv::Scalar &color);

void ReorderPoints(cv::Point2f *points, cv::Point2f *newPoints);

void FindHomo(cv::Mat &HH);

#endif //JAKAVISION_OPENCVFUNCS_H
