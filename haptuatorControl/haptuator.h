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

class Haptuator : public DaqDevice{
public:
	Haptuator();
	Haptuator(comedi_t* dev,int subDev,int channel,int range);
	float sinewave(float t,float freq,float mag);
	float customSignal(float t);
	float setInterpolationParameter(float _A0,vector<float> _Ai,vector<float> _Bi,vector<float> _fi);
	int renderVibration(float t);
	int renderVibration(float t,float freq,float mag);
	float getAccRender();
	virtual ~Haptuator();

private:
	bool _state = false;
	float _A0;
	vector<float> _Ai;
	vector<float> _Bi;
	vector<float> _fi;

	float _acc_render;
	int _subdev;
	int _chanel;
	int _range_idx = 1;
	int _aref = AREF_GROUND;

};

#endif /* HAPTUATOR_H_ */
