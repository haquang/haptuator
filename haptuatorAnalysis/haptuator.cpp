/*
 * Haptuator.cpp
 *
 *  Created on: Sep 3, 2015
 *      Author: haquang
 */

#include "haptuator.h"
#include <iostream>

Haptuator::Haptuator(){

}
Haptuator::Haptuator(comedi_t* dev,int subDev,int channel,int range = 0):DaqDevice(dev) {
	// TODO Auto-generated constructor stub
	_subdev = subDev;
	_chanel = channel;
	_range_idx = range;
	_state = false;
}

float Haptuator::sinewave(float t,float freq,float mag) {
	float result;
	result = mag*sin(2*M_PI*freq*t);
	return result;
}

void Haptuator::reset(){
	first_cycle = true;
}

float Haptuator::customSignal(float t) {
	int i;
	float result;
	if (!_state)
		result = 0.0f;
	else {
		result = _A0/2;
		for (i = 0;i< _Ai.size();i++){
			result += _Ai[i] *cos(2*M_PI*_fi[i]*t) + _Bi[i]*sin(2*M_PI*_fi[i]*t);
		}
	}
	return result;

}

void Haptuator::setInterpolationParameter(float A0, vector<float> Ai,vector<float> Bi, vector<float> fi) {
	_A0 = A0;
	_Ai = Ai;
	_Bi = Bi;
	_fi = fi;
	_state = true;
}

int Haptuator::renderVibration(float t) {
	_acc_render = customSignal(t);
	if (COMEDI_ERROR == writeData(_subdev,_chanel,_range_idx,_aref,_acc_render))
		return COMEDI_ERROR;
	else return COMEDI_OK;
}

int Haptuator::renderVibration(float t,int amp){
//	if (first_cycle){
		_acc_render = amp;
//		first_cycle = false;
//	} else {
//		_acc_render = 0;
//	}
	if (COMEDI_ERROR == writeData(_subdev,_chanel,_range_idx,_aref,_acc_render))
		return COMEDI_ERROR;
	else return COMEDI_OK;
} 

int Haptuator::renderVibration(float t,float freq,float mag){
	_acc_render = sinewave(t,freq,mag);

	return writeData(_subdev,_chanel,_range_idx,_aref,_acc_render);
}



float Haptuator::getAccRender() {
	return _acc_render;
}

Haptuator::~Haptuator() {
	// TODO Auto-generated destructor stub
}

