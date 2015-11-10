/*
 * Haptuator.h
 *
 *  Created on: Sep 3, 2015
 *      Author: haquang
 */

#ifndef HAPTUATOR_H_
#define HAPTUATOR_H_

#include <vector>
#include <cmath>
#include <numeric>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include "daqdevice.h"
#include "defines.h"
using namespace std;
/*
 *
 * Haptuator Model
 */

class Haptuator : public DaqDevice{
public:
	Haptuator();
	Haptuator(comedi_t* dev,int subDev,int channel,int range);
	float sinewave(float t,float freq,float mag);
	float customSignal(float t);
	void setInterpolationParameter(float _A0,vector<float> _Ai,vector<float> _Bi,vector<float> _fi);
	void setGainInterpolation(vector<float> ref_freq,vector<float> ref_gain);
	void renderVibration(double t); // Driven data
	void renderVibration(double t,int amp); // Step
	void renderVibration(double t,float freq,float mag); // Chirp
	float getAccRender();
	void disablePSD();
	void reset();
	void setCtrl(double acc);
	void setAcc(double acc);
	int run();
	float interp1d(float val,vector<float> input,vector<float> out);
	virtual ~Haptuator();


private:
	bool _ctrl = false;
	// Signals
	float _A0;
	vector<float> _Ai;
	vector<float> _Bi;
	vector<float> _fi;
	vector<float> _ref_freq;
	vector<float> _ref_gain;
	vector<float> _gain;
	bool _isInterpolateSet;
	double _acc_set; // set acceleration
	double _acc_act; // actual acceleration - measure by accelerometer
	double _ctrl_signal; // control signal - after adaptive controller
	float _deltaT = (float) TIMER_HAPTUATOR / 1000000000;
	double _gamma = GAMMA;
	// for sensoray/ni card
	int _subdev;
	int _chanel;
	int _range_idx = 1;
	int _aref = AREF_GROUND;

	gsl_interp_accel *_gsl_acc;
	gsl_spline *_gsl_spline;
};

#endif /* HAPTUATOR_H_ */
