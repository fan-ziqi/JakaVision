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

#include <iostream>
#include <fstream>

//OpenCV
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//subfuncs
#include "OpenCVFuncs.h"
#include "Jakauser.h"

//#define USE_ARM

using namespace cv;
using namespace std;

//本程序使用到的颜色枚举
typedef enum
{
	COLOR_RED = 0,
	COLOR_ORANGE = 1,
	COLOR_YELLOW = 2,
	COLOR_GREEN = 3,
	COLOR_BLUE = 4,
	COLOR_PURPLE = 5,
} color_t;

Mat img, show_img;


int main()
{
#ifdef USE_ARM
	//实例化机械臂结构体
	Jakauser arm("192.168.1.100");
	//移动到初始位置
	arm.MoveToInit();
	//移动到待命位置
	arm.MoveToStandby();
#endif

	//使用八组坐标点求单应矩阵
	Mat H;
	FindHomo(H);

	//打开摄像头
	VideoCapture cap(2);

	//颜色阈值
	int red_value[6] = {255, 255, 255, 255, 255, 255};
	int orange_value[6] = {0, 20, 110, 255, 200, 255};
	int yellow_value[6] = {15, 50, 100, 255, 190, 255};
	int green_value[6] = {50, 90, 80, 255, 80, 255};
	int blue_value[6] = {80, 120, 130, 255, 130, 255};
	int purple_value[6] = {120, 170, 60, 255, 130, 255};

	Mat red, orange, yellow, green, blue, purple;

	color_mask(cap);

	//读取图像到img
	cap.read(img);
	//绘图都在show_img上绘制,故拷贝一份
	show_img = img.clone();

	//转换成HSV颜色空间
	Mat HSV_img;
	cvtColor(img, HSV_img, COLOR_BGR2HSV);

	//按顺序对六种颜色进行抓取.每次将该颜色全部抓完后再进行下一个颜色
	for(int i = COLOR_RED; i <= COLOR_PURPLE; ++i)
	{
		Mat _color_mat;//设置对应颜色的Mat
		int *_color_value;//提取对应颜色的阈值
		string _color;//提取对应颜色的字符串
		Point2i _color_center;//设置对应颜色要被放置的中心位置
		//判断颜色
		switch(i)
		{
			case COLOR_RED:
				_color_mat = red;
				_color_value = red_value;
				_color = "RED";
				_color_center = Point2i(-351, -78);
				break;
			case COLOR_ORANGE:
				_color_mat = orange;
				_color_value = orange_value;
				_color = "ORANGE";
				_color_center = Point2i(-389, -109);
				break;
			case COLOR_YELLOW:
				_color_mat = yellow;
				_color_value = yellow_value;
				_color = "YELLOW";
				_color_center = Point2i(-430, -140);
				break;
			case COLOR_GREEN:
				_color_mat = green;
				_color_value = green_value;
				_color = "GREEN";
				_color_center = Point2i(-460, -100);
				break;
			case COLOR_BLUE:
				_color_mat = blue;
				_color_value = blue_value;
				_color = "BLUE";
				_color_center = Point2i(-420, -70);
				break;
			case COLOR_PURPLE:
				_color_mat = purple;
				_color_value = purple_value;
				_color = "PURPLE";
				_color_center = Point2i(-380, -40);
				break;
			default:
				break;
		}

		cout << "本次抓取的颜色为: " << _color << endl;

		//找到对应颜色的轮廓
		vector<vector<Point>> _color_contours;
		vector<Vec4i> _color_hierarchy;
		find_color(HSV_img, _color_mat, _color_value, _color_contours, _color_hierarchy);
		//输出当前颜色所有物块的轮廓,_color_contours的大小即为当前颜色物块的数量
		vector<RotatedRect> _minRect;
		for(int _color_contours_index = 0; _color_contours_index < _color_contours.size(); ++_color_contours_index)
		{
			_minRect.push_back(minAreaRect(_color_contours[_color_contours_index]));
		}

		Point2f _vertices[4];
		Point2f _reorder_vertices[4];

		Mat _img_point;
		Mat _arm_point;

		float _dir;//物块方向
		Point2i _center;//物块中心

		for(int _minRect_index = 0; _minRect_index < _minRect.size(); _minRect_index++)
		{
			//提取轮廓角点
			_minRect[_minRect_index].points(_vertices);

			//将四个角点重新排序,便于画图
			ReorderPoints(_vertices, _reorder_vertices);

			//画出四条轮廓线
			for(int _line = 0; _line < 4; _line++)
			{
				line(show_img, _reorder_vertices[_line], _reorder_vertices[(_line + 1) % 4], Scalar(0, 0, 0), 2);
			}

			//计算物块中心位置并画一个圆
			_center = (_reorder_vertices[0] + _reorder_vertices[2]) / 2;
			circle(show_img, _center, 5, Scalar(0, 0, 0), -1);

			//计算方向
			_dir = atan2((_reorder_vertices[1] - _reorder_vertices[0]).y,
			             (_reorder_vertices[1] - _reorder_vertices[0]).x);
			cout << _dir << "dir: " << _dir / M_PI * 180.0 << endl;

			//画出方向
			Point2i _delta0((int) (50 * cos(_dir)), (int) (50 * sin(_dir)));
			arrowedLine(show_img, _center - _delta0, _center + _delta0,
			            Scalar(0, 0, 0), 2);
			Point2i _delta1((int) (50 * cos(_dir + M_PI_2)),
			                (int) (50 * sin(_dir + M_PI_2)));
			arrowedLine(show_img, _center - _delta1, _center + _delta1,
			            Scalar(0, 0, 0), 2);

			//坐标变换,将图像坐标系下的物块中心坐标变化到机械臂坐标系中
			_img_point = Mat::zeros(cv::Size(1, 3), CV_64FC1);
			_arm_point = Mat::zeros(cv::Size(1, 3), CV_64FC1);
			_img_point.at<double>(0, 0) = _center.x;
			_img_point.at<double>(1, 0) = _center.y;
			_img_point.at<double>(2, 0) = 1.0;

			_arm_point = H * _img_point;

			cout << "img_point: "
			     << _img_point.at<double>(0, 0) << " "
			     << _img_point.at<double>(1, 0) << " "
			     << " -> arm_point: "
			     << _arm_point.at<double>(0, 0) / _arm_point.at<double>(2, 0)
			     << " "
			     << _arm_point.at<double>(1, 0) / _arm_point.at<double>(2, 0)
			     << " "
			     << endl;

			//在图像上绘制文字
			putText(show_img, _color, _reorder_vertices[2], FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 255, 0), 2);

			imshow("output", show_img);
			waitKey(1);

			cout << "正在抓第" << _minRect_index << "个" << _color << "，共" << _minRect.size() << "个" << endl;

			//计算抓取坐标
			double grab_x =
					_arm_point.at<double>(0, 0) / _arm_point.at<double>(2, 0);
			double grab_y =
					_arm_point.at<double>(1, 0) / _arm_point.at<double>(2, 0);

#ifdef USE_ARM
			//抓取
			arm.Grab(grab_x, grab_y, 105, -_dir / M_PI * 180.0);

			//放置
			int _angle = 0;//放置偏置角度
			arm.Place(_color_center.x, _color_center.y, 105 + 20 * _minRect_index, 0 + _angle);
#endif
		}
	}
	return 0;
}

