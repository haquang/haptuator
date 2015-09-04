/*
 * phantom.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: haquang
 */

#include "phantom.h"

Phantom::Phantom() {
	// TODO Auto-generated constructor stub

}

void Phantom::Saturation() {
	if (_force[0]<-MAX_FORCE) _force[0]=-MAX_FORCE;
	if (_force[1]<-MAX_FORCE) _force[1]=-MAX_FORCE;
	if (_force[2]<-MAX_FORCE) _force[2]=-MAX_FORCE;
	if (_force[0]>MAX_FORCE) _force[0]=MAX_FORCE;
	if (_force[1]>MAX_FORCE) _force[1]=MAX_FORCE;
	if (_force[2]>MAX_FORCE) _force[2]=MAX_FORCE;
}


void Phantom::Open() {
	HDErrorInfo error;
	_Device = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		//hduPrintError(stderr, &error, "Failed to initialize haptic device");
		cout<< "Failed to initialize haptic device"<< endl; //: %s", &error);
		return;
	}
	printf("Found %s.\n\n", hdGetString(HD_DEVICE_MODEL_TYPE));
	hdEnable(HD_FORCE_OUTPUT);
}
void Phantom::setSchedule(HDSchedulerHandle &hSchedule){
	_hPhantomMain = hSchedule;
}
void Phantom::Close() {
	hdStopScheduler();
	hdUnschedule(_hPhantomMain);
	hdDisableDevice(_Device);
}

void Phantom::Calibrate() {
	int calibrationStyle;
	int supportedCalibrationStyles;
	HDErrorInfo error;

	hdGetIntegerv(HD_CALIBRATION_STYLE, &supportedCalibrationStyles);
	if (supportedCalibrationStyles & HD_CALIBRATION_ENCODER_RESET)
	{
		calibrationStyle = HD_CALIBRATION_ENCODER_RESET;
		printf("HD_CALIBRATION_ENCODER_RESE..\n\n");
	}
	if (supportedCalibrationStyles & HD_CALIBRATION_INKWELL)
	{
		calibrationStyle = HD_CALIBRATION_INKWELL;
		printf("HD_CALIBRATION_INKWELL..\n\n");
	}
	if (supportedCalibrationStyles & HD_CALIBRATION_AUTO)
	{
		calibrationStyle = HD_CALIBRATION_AUTO;
		printf("HD_CALIBRATION_AUTO..\n\n");
	}

	do
	{
		hdUpdateCalibration(calibrationStyle);
		printf("Calibrating.. (put stylus in well)\n");
		if (HD_DEVICE_ERROR(error = hdGetError()))
		{
			hduPrintError(stderr, &error, "Reset encoders reset failed.");
			break;
		}
	}   while (hdCheckCalibration() != HD_CALIBRATION_OK);


	printf("\n\nCalibration complete.\n");
}


void Phantom::Run() {
	hdStartScheduler();
	if (HD_DEVICE_ERROR(_error = hdGetError()))
	{
		cout << "Failed to start the scheduler" << endl;//, &error);
		return;
	}
	Calibrate();
}

bool Phantom::isInitialized() {
	return _initialized;
}

void Phantom::setPosition(hduVector3Dd pos) {
	if (!isInitialized()){
		_init_pos = pos;
		_initialized = true;
	} else {

		_posistion = pos - _init_pos;
		_prv_pos_x = _pos_x;
		_prv_pos_y = _pos_y;
		_prv_pos_z = _pos_z;

		_pos_x = _posistion[0];
		_pos_y = _posistion[2];
		_pos_z = _posistion[1];

		_speed_y = (_pos_y - _prv_pos_y)/_sample_time;
		_speed_z = (_pos_z - _prv_pos_z)/_sample_time;

		v_speed_x.push_back((_pos_x - _prv_pos_x)/_sample_time);
		v_speed_x.erase(v_speed_x.begin());
		_speed_x= accumulate(v_speed_x.begin(),v_speed_x.end(),0.0f);
		_speed_x = _speed_x/v_speed_x.size();

		v_speed_y.push_back((_pos_y - _prv_pos_y)/_sample_time);
		v_speed_y.erase(v_speed_y.begin());
		_speed_y= accumulate(v_speed_y.begin(),v_speed_y.end(),0.0f);
		_speed_y = _speed_y/v_speed_y.size();


		v_speed_z.push_back((_pos_z - _prv_pos_z)/_sample_time);
		v_speed_z.erase(v_speed_z.begin());
		_speed_z= accumulate(v_speed_z.begin(),v_speed_z.end(),0.0f);
		_speed_z = _speed_z/v_speed_z.size();
	}
}

hduVector3Dd Phantom::getPosition() {
	return _posistion;
}

float Phantom::getPosX() {
	return _pos_x;
}

float Phantom::getPosY() {
	return _pos_y;
}

float Phantom::getPosZ() {
	return _pos_z;
}

float Phantom::getSpeedX() {
	return _speed_x;
}

float Phantom::getSpeedY() {
	return _speed_y;
}

float Phantom::getSpeedZ() {
	return _speed_z;
}

hduVector3Dd Phantom::getForce(float virtualWallSpring,float constraintSpringX = 0.0f,float constrainSpringY = 0.0f) {

	if (_pos_z > 0)
		_force[1] =  0;
	else
		_force[1] = -virtualWallSpring*_pos_z;

	_force[0] = -constraintSpringX * _pos_x;
	_force[2] = -constrainSpringY * _pos_y;
	Saturation();
	return _force;
}

Phantom::~Phantom() {
	// TODO Auto-generated destructor stub
}

float Phantom::getForceX() {
	return _force[0];
}

float Phantom::getForceY() {
	return _force[2];
}

float Phantom::getForceZ() {
	return _force[1];
}
