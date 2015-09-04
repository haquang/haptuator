/*
 * phantom.h
 *
 *  Created on: Sep 4, 2015
 *      Author: haquang
 */

#ifndef PHANTOM_H_
#define PHANTOM_H_
#include <iostream>
#include <vector>
#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>
#include <numeric>
#include <algorithm>

#define MAX_FORCE 2

using namespace std;
class Phantom {
private:
	HHD _Device;
	HDSchedulerHandle _hPhantomMain;
	HDErrorInfo _error;

	bool _initialized = false;
	hduVector3Dd _init_pos;
	hduVector3Dd _posistion;
	hduVector3Dd _force;

	float _speed_x,_speed_y,_speed_z; // velocity in X,Y,Z (mean value)
	float _pos_x,_pos_y,_pos_z;		  // position in X,Y,Z
	float _prv_pos_x,_prv_pos_y,_prv_pos_z;
	vector<float> v_speed_x {0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};  // list for calculate mean of speed
	vector<float> v_speed_y {0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
	vector<float> v_speed_z {0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
	const float _sample_time = 0.001f;
public:
	Phantom();
	void Saturation();
	void Run();
	void Open();
	void Close();
	void Calibrate();
	void setSchedule(HDSchedulerHandle&);
	bool isInitialized();
	void setPosition(hduVector3Dd pos);
	hduVector3Dd getPosition();
	float getPosX();
	float getPosY();
	float getPosZ();
	float getSpeedX();
	float getSpeedY();
	float getSpeedZ();
	float getForceX();
	float getForceY();
	float getForceZ();

	hduVector3Dd getForce(float virtualWallSpring,float constraintSpringX,float constrainSpringY);
	virtual ~Phantom();
};

#endif /* PHANTOM_H_ */
