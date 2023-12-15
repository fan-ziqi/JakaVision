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

#ifndef JAKAVISION_JAKAUSER_H
#define JAKAVISION_JAKAUSER_H

#include "JAKARobot_API.h"

class Jakauser
{
private:
	JAKAZuRobot jaka;
	int ret;
	double start_jointpos[6] = {0, 90, 0, 90, 180, 0};//初始直立位置
	double standby_jointpos[6] = {199.110, 83.153, -80.075, 86.922, 90.000, 71.110};//待命位置


public:
	explicit Jakauser(const char *ip);

	~Jakauser();

	void joint_move(double *target_jointpos, double speed);

	void MoveToInit();

	void OpenHand();

	void CloseHand();

	void MoveToStandby();

	void Grab(double grab_x, double grab_y, double grab_z, double grab_angle);

	void Place(double place_x, double place_y, double place_z, double place_angle);
};


#endif //JAKAVISION_JAKAUSER_H
