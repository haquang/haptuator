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
	_acc_act = 0;
	_acc_set = 0;
	_ctrl_signal = 0;
}

float Haptuator::sinewave(float t,float freq,float mag) {
	float result;
	result = mag*sin(2*M_PI*freq*t);
	return result;
}

float Haptuator::customSignal(float t) {
	int i;
	float result;
	if (!_isInterpolateSet){
		result = 0.0f;
	}

	else {
		result = _A0/2;
		for (i = 0;i< _Ai.size();i++){
			result += 1/_gain[i] * (_Ai[i] *cos(2*M_PI*_fi[i]*t) + _Bi[i]*sin(2*M_PI*_fi[i]*t));
		}
	}

	return result;

}

void Haptuator::setInterpolationParameter(float A0, vector<float> Ai,vector<float> Bi, vector<float> fi) {
	_A0 = A0;
	_Ai = Ai;
	_Bi = Bi;
	_fi = fi;
	_isInterpolateSet = true;
}

void Haptuator::renderVibration(double t) {
	_acc_set = customSignal(t);
}

void Haptuator::renderVibration(double t,int amp){
	_acc_set = amp;
} 

void Haptuator::renderVibration(double t,float freq,float mag){
	_acc_set = sinewave(t,freq,mag);
}

int Haptuator::run(){
	float u;
	if (_ctrl)
		u = _ctrl_signal;
	else
		u = _acc_set;
	return writeData(_subdev,_chanel,_range_idx,_aref,u);
}

float Haptuator::getAccRender() {
	return _acc_set;
}

void Haptuator::setCtrl(double acc) {
	_ctrl_signal = acc;
	_ctrl = true;
}

void Haptuator::setAcc(double acc) {
	_acc_act = acc;
}
void Haptuator::reset(){
	_acc_act = 0;
	_acc_set = 0;
	_ctrl_signal = 0;
}
Haptuator::~Haptuator() {
	// TODO Auto-generated destructor stub
}

void Haptuator::setGainInterpolation(vector<float> ref_freq,
		vector<float> ref_gain) {
	_ref_freq = ref_freq;
	_ref_gain = ref_gain;
	_gsl_acc = gsl_interp_accel_alloc ();
	_gsl_spline = gsl_spline_alloc(gsl_interp_cspline, _ref_freq.size());
	_gain.clear();
	if (_isInterpolateSet){
		for (int i = 0;i<_fi.size();i++){
			_gain.push_back(interp1d(_fi[i],_ref_freq,_ref_gain));
			cout <<_gain[i] << endl;
		}
	}

}

void Haptuator::disablePSD() {
	_gain.clear();
	for (int i = 0;i<_fi.size();i++){
		_gain.push_back(1);
	}
}

float Haptuator::interp1d(float val, vector<float> in, vector<float> out) {
	double  *x, *y;
	x = new double[in.size()];
	y = new double[in.size()];
	float result;
	for (int i = 0;i<in.size();i++)
	{
		x[i] = in[i];
		y[i] = out[i];
	}
	gsl_spline_init(_gsl_spline, x, y, in.size());

	result =(float) (gsl_spline_eval (_gsl_spline, (double) val, _gsl_acc));
	delete x,y;
	return result;
}
