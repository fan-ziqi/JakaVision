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

#include "Jakauser.h"
#include <iostream>

using namespace std;

Jakauser::Jakauser(const char *ip)
{
	int is_enable = 0, is_powered = 0;

	ret = jaka.login_in(ip);
	if(ret) cout << jaka.m_errMsg << endl;
	jaka.getState(is_enable, is_powered);
	if(!is_powered)
	{
		ret = jaka.power(1);
		if(ret) cout << jaka.m_errMsg << endl;
		Sleep(5000);
	}
	if(!is_enable)
	{
		ret = jaka.enable(1);
		if(ret) cout << jaka.m_errMsg << endl;
		Sleep(5000);
	}
	jaka.initSetting();
	Sleep(1000);
}

Jakauser::~Jakauser()
{
	jaka.stopMotion();
	jaka.login_out();
}

/****运动基础函数****/
void Jakauser::joint_move(double *target_jointpos, double speed)
{
	cout << "正在运动" << endl;
	ret = jaka.moveJ(target_jointpos, speed, 0);
	if(ret) cout << jaka.m_errMsg << endl;

	ret = jaka.waitEndMove(3000);
	if(ret) cout << jaka.m_errMsg << endl;

	cout << endl;
}

void Jakauser::MoveToInit()
{
	cout << "开始初始化" << endl;

	OpenHand();

	joint_move(start_jointpos, 50.0);

	cout << "结束初始化" << endl;
}

void Jakauser::MoveToStandby()
{
	cout << "开始移动至待命位置" << endl;

	OpenHand();

	joint_move(standby_jointpos, 50.0);

	cout << "结束移动至待命位置" << endl;
}

void Jakauser::Grab(double grab_x, double grab_y, double grab_z, double grab_angle)
{
	cout << "开始抓取" << endl;

	double up_endpos[6] = {grab_x, grab_y, grab_z + 50, -180, 0, 38 + grab_angle};//物块上方位置
	double down_endpos[6] = {grab_x, grab_y, grab_z, -180, 0, 38 + grab_angle};//下落位置

	double up_jointpos[6];//物块上方位置关节角
	double down_jointpos[6];//下落位置关节角

	OpenHand();

	ret = jaka.iKinematics(standby_jointpos, up_endpos, up_jointpos);
	if(ret)
	{
		cout << "运动学逆解失败" << endl;
		return;
	}
	joint_move(up_jointpos, 40.0);

	ret = jaka.iKinematics(standby_jointpos, down_endpos, down_jointpos);
	if(ret)
	{
		cout << "运动学逆解失败" << endl;
		return;
	}
	joint_move(down_jointpos, 10.0);

	CloseHand();

	joint_move(up_jointpos, 10.0);//向上移动

	joint_move(standby_jointpos, 40.0);

	cout << "结束抓取" << endl;
}

void Jakauser::Place(double place_x, double place_y, double place_z, double place_angle)
{
	cout << "开始放置" << endl;

	double up_endpos[6] = {place_x, place_y, place_z + 50, -180, 0, 38 + place_angle};//放置上方位置
	double down_endpos[6] = {place_x, place_y, place_z, -180, 0, 38 + place_angle};//下落位置

	double up_jointpos[6];//放置上方位置关节角
	double down_jointpos[6];//下落位置关节角

	ret = jaka.iKinematics(standby_jointpos, up_endpos, up_jointpos);
	if(ret)
	{
		cout << "运动学逆解失败" << endl;
		return;
	}
	joint_move(up_jointpos, 40.0);

	ret = jaka.iKinematics(standby_jointpos, down_endpos, down_jointpos);
	if(ret)
	{
		cout << "运动学逆解失败" << endl;
		return;
	}
	joint_move(down_jointpos, 10.0);

	OpenHand();

	joint_move(up_jointpos, 10.0);//向上移动

	joint_move(standby_jointpos, 40.0);

	cout << "结束放置" << endl;
}

void Jakauser::OpenHand()
{
	cout << "张开夹爪" << endl;

	/***气爪开***/
	ret = jaka.setDout(5, 1);
	Sleep(500);

	cout << "关闭夹爪" << endl;
}

void Jakauser::CloseHand()
{
	cout << "张开夹爪" << endl;

	/***气爪开***/
	ret = jaka.setDout(5, 0);
	Sleep(500);

	cout << "关闭夹爪" << endl;
}