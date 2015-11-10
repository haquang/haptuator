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
	_acc_act = {0.0,0.0};
	_acc_ref = {0.0,0.0};
	_acc_set = {0.0,0.0};
	_ctrl_signal = {0.0,0.0};
	_omega = {0.0,0.0,0.0,0.0};
	_theta = {0.0,0.0,0.0,0.0};
	_isInterpolateSet = false;
	initModel(KM,BM,AM_1,AM_2);
}

float Haptuator::sinewave(float t,float freq,float mag) {
	float result;
	result = mag*sin(2*M_PI*freq*t);
	return result;
}

float Haptuator::customSignal(float t) {
	int i;
	float result;
	if (!_isInterpolateSet)
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
	_isInterpolateSet = true;
}

void Haptuator::renderVibration(double t) {
	double temp = customSignal(t);
	_acc_set[1] = (temp - _acc_set[0]) / _deltaT;
	_acc_set[0] = temp;
}

void Haptuator::renderVibration(double t,int amp){
	_acc_set[0] = amp;
} 

void Haptuator::renderVibration(double t,float freq,float mag){
	double temp = sinewave(t,freq,mag);
	_acc_set[1] = (temp - _acc_set[0]) / _deltaT;
	_acc_set[0] = temp;
}

int Haptuator::run(){
	float u;
	if (_ctrl)
		u = _ctrl_signal[0];
	else
		u = _acc_set[0];
	return writeData(_subdev,_chanel,_range_idx,_aref,u);
}

float Haptuator::getAccRender() {
	return _acc_set[0];
}

double Haptuator::getAccReference(){
	return _acc_ref[0];
}

void Haptuator::setCtrl(double acc) {
	_ctrl_signal[0] = acc;
	_ctrl = true;
}


void Haptuator::initModel(double Km,double Bm, double am1, double am2) {
	_ref_model.initialize(Km,Bm,am1,am2);
	_input_filter.initialize(Km,Bm);
	_output_filter.initialize(Km,Bm);
}

void Haptuator::setAcc(double acc) {
	_acc_act[1] = (acc - _acc_act[0])/_deltaT;
	_acc_act[0] = acc;
}
void Haptuator::adaptationLaw(){
	vector<double> dxdt = {0.0,0.0,0.0,0.0};
	for (int i=0;i<= 3;i++){
		dxdt[i] = _gamma * _err * _omega[i];
		_theta[i] = _theta[i] + dxdt[i]*_deltaT;
	}

}

vector<double> Haptuator::getTheta() {
	return _theta;
}

vector<double> Haptuator::getOmega() {
	return _omega;
}
void Haptuator::reset(){
	_isInterpolateSet = false;
	_acc_act = {0.0,0.0};
	_acc_ref = {0.0,0.0};
	_acc_set = {0.0,0.0};
	_ctrl_signal = {0.0,0.0};
	_omega = {0.0,0.0,0.0,0.0};
	_theta = {0.0,0.0,0.0,0.0};

}

double Haptuator::getCtrlSignal() {
	return _ctrl_signal[0];
}

void Haptuator::saturation() {
	if (_ctrl_signal[0] >= 3)
		_ctrl_signal[0] = 3;
	else if (_ctrl_signal[0] <= -3)
		_ctrl_signal[0] = -3;
}

double Haptuator::getError() {
	return _err;
}

void Haptuator::setGamma(double gamma) {
	_gamma = gamma;
}

void Haptuator::update(double t) {
	vector<double> v = {0.0,0.0};
	// Update state for reference model
	_ref_model.setInput(_acc_set);
	_solver.do_step(_ref_model,_acc_ref,t,_deltaT);
	// Update control parameters base on actual error & adaptation law
	_err = -_acc_ref[0] + _acc_act[0];

	_input_filter.setInput(_ctrl_signal);
	_solver.do_step(_input_filter,v,t,_deltaT);
	_omega[1] = v[0];

	_output_filter.setInput(_acc_act);
	_solver.do_step(_input_filter,v,t,_deltaT);
	_omega[2] = v[0];

	_omega[0] = _acc_set[0];
	_omega[3] = _acc_act[0];

	// Adaptation law
	adaptationLaw();
	// Adaptation controller
	_ctrl_signal[0] = _theta[0] * _omega[0] + _theta[1] * _omega[1] + _theta[2] * _omega[2] +_theta[3] * _omega[3];
	_ctrl = true;
	saturation();
}

Haptuator::~Haptuator() {
	// TODO Auto-generated destructor stub
}

