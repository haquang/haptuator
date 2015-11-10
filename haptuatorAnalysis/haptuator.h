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
	void renderVibration(double t); // Driven data
	void renderVibration(double t,int amp); // Step
	void renderVibration(double t,float freq,float mag); // Chirp
	int run();
	float getAccRender();
	void reset();
	void setCtrl(double acc);

	virtual ~Haptuator();

private:
	bool _state = false;
	bool _ctrl = false;

	float _A0;
	vector<float> _Ai;
	vector<float> _Bi;
	vector<float> _fi;
	float _acc_render;
	double _ctrl_signal;
	float _u;
	int _subdev;
	int _chanel;
	int _range_idx = 1;
	int _aref = AREF_GROUND;
	bool first_cycle = true;
	float _deltaT = (float) TIMER_HAPTUATOR / 1000000000;

};

#endif /* HAPTUATOR_H_ */
