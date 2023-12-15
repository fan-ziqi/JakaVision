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

#include "OpenCVFuncs.h"

using namespace std;
using namespace cv;

/**
 * 获取鼠标点击位置
 * */
void get_xy(int EVENT, int x, int y, __attribute__((unused)) int flags, __attribute__((unused)) void *userdata)
{
	if(EVENT == EVENT_LBUTTONDOWN)
	{
		cout << "x:" << x << " y:" << y << endl;
	}
}

/**
 * 阈值调节
 * */
void color_mask(VideoCapture &cap)
{
	// 设定初始值
	int hmin = 0, smin = 0, vmin = 0;
	int hmax = 179, smax = 255, vmax = 255;

	Scalar lower;
	Scalar upper;

	// 创建遮罩
	Mat mask;
	// 定义窗口名
	namedWindow("Trackbars");
	// 创建Trackbar
	createTrackbar("Hue Min", "Trackbars", &hmin, 179);
	createTrackbar("Hue Max", "Trackbars", &hmax, 179);
	createTrackbar("Sat Min", "Trackbars", &smin, 255);
	createTrackbar("Sat Max", "Trackbars", &smax, 255);
	createTrackbar("Val Min", "Trackbars", &vmin, 255);
	createTrackbar("Val Max", "Trackbars", &vmax, 255);

	Mat img;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

	while(true)
	{
		cap.read(img);

		// 中值滤波
		medianBlur(img, img, 3);

		Mat HSV_img;
		cvtColor(img, HSV_img, COLOR_BGR2HSV);

		// 在HSV空间中查找颜色更加容易，所以先转化为HSV颜色空间
		lower = Scalar(hmin, smin, vmin);
		upper = Scalar(hmax, smax, vmax);
		inRange(HSV_img, lower, upper, mask);

		Mat colorDil, colorErode;
		erode(mask, colorErode, kernel);
		dilate(colorErode, colorDil, kernel);

		imshow("Image colorDil", colorDil);
		waitKey(1);

		// Exit on Esc key press
		if(waitKey(1) == 27) break;
	}

}

/**
 * 轮廓检测
 * */
void find_color(Mat &HSV_img, Mat &color_mat, int *color_value, vector<vector<Point>> &color_contours,
                vector<Vec4i> &color_hierarchy)
{
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

	//对颜色进行阈值限制,腐蚀及膨胀
	Scalar lower = Scalar(color_value[0], color_value[2], color_value[4]);
	Scalar upper = Scalar(color_value[1], color_value[3], color_value[5]);
	inRange(HSV_img, lower, upper, color_mat);
	Mat colorDil, colorErode;
	erode(color_mat, colorErode, kernel);
	dilate(colorErode, colorDil, kernel);

	//找到轮廓
	Mat color_8U;
	colorDil.convertTo(color_8U, CV_8U);
	findContours(color_8U, color_contours, color_hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
}

/**
 * 函数功能：绘制一些点
 * 输入：点集，颜色
 * */
void DrawPoints(Mat &show_img, vector<Point> points, const Scalar &color)
{
	for(int i = 0; i < points.size(); i++)
	{
		circle(show_img, points[i], 10, color, FILLED);
		putText(show_img, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
	}
}

/**
 * 函数功能：重新排列四个角点的顺序
 * 最终顺序为： 0  1
 * 			  3  2
 * 			  数组中为左上角-右上角-右下角-左下角
 * */
void ReorderPoints(Point2f *points, Point2f *newPoints)
{
	vector<float> sumPoints;
	vector<float> subPoints;

	// OpenCV中左上顶点为(0,0)，右为x轴正向，下为y轴正向。
	for(int i = 0; i < 4; i++)
	{
		// 将每个点的xy坐标值相加(x+y)，左上角的点的坐标和应该是最小的，右下角的点的坐标和应该是最大的
		sumPoints.push_back(points[i].x + points[i].y);
		// 将每个点的xy坐标值相减(x-y)，左下角的点的坐标差应该是最小的，右上角的点的坐标差应该是最大的
		subPoints.push_back(points[i].x - points[i].y);
	}

	// 重新排列
	newPoints[0] = points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]; // 0 和的最小值 左上角
	newPoints[1] = points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]; // 1 差的最大值 右上角
	newPoints[2] = points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]; // 2 和的最大值 右下角
	newPoints[3] = points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]; // 3 差的最小值 左下角

}

/**
 * 根据八组坐标点找到单应矩阵
 * */
void FindHomo(Mat &H)
{
	Point2d cordinate;
	//将图像1中对应实际的点的坐标输入到real_points中
	vector<Point2d> real_points;
	cordinate.x = -393;//469
	cordinate.y = -254;//274
	real_points.push_back(cordinate);
	cordinate.x = -253;
	cordinate.y = -145;
	real_points.push_back(cordinate);
	cordinate.x = -144;
	cordinate.y = -285;
	real_points.push_back(cordinate);
	cordinate.x = -283;
	cordinate.y = -390;
	real_points.push_back(cordinate);
	cordinate.x = -317.921;
	cordinate.y = -259.574;
	real_points.push_back(cordinate);
	cordinate.x = -257.596;
	cordinate.y = -213.363;
	real_points.push_back(cordinate);
	cordinate.x = -213.492;
	cordinate.y = -271.430;
	real_points.push_back(cordinate);
	cordinate.x = -271.110;
	cordinate.y = -318.451;
	real_points.push_back(cordinate);

	vector<Point2d> img_points;
	cordinate.x = 121;
	cordinate.y = 46;
	img_points.push_back(cordinate);
	cordinate.x = 439;
	cordinate.y = 35;
	img_points.push_back(cordinate);
	cordinate.x = 497;
	cordinate.y = 334;
	img_points.push_back(cordinate);
	cordinate.x = 73;
	cordinate.y = 352;
	img_points.push_back(cordinate);
	cordinate.x = 208;
	cordinate.y = 113;
	img_points.push_back(cordinate);
	cordinate.x = 354;
	cordinate.y = 109;
	img_points.push_back(cordinate);
	cordinate.x = 366;
	cordinate.y = 234;
	img_points.push_back(cordinate);
	cordinate.x = 203;
	cordinate.y = 239;
	img_points.push_back(cordinate);

	//求出单应矩阵H
	H = findHomography(img_points, real_points);
	cout << "单应矩阵为：" << endl;
	cout << H << endl;
}