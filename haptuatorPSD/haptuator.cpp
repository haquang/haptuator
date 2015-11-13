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
	_acc_set = 0;
	_ctrl_signal = 0;
}

double Haptuator::sinewave(double t,double freq,double mag) {
	double result;
	result = mag*sin(2*M_PI*freq*t);
	return result;
}


double Haptuator::rectWave(double t, double freq, double mag) {
	double result;
	result = (double) 4/M_PI * mag * (sin(2*M_PI*freq*t) + (double)1/3 * sin(6*M_PI*freq*t) + (double)1/5 * sin(10*M_PI*freq*t)  + (double)1/7 * sin(14*M_PI*freq*t) + (double) 1/9 * sin(18*M_PI*freq*t)+ (double)1/11 * sin(22*M_PI*freq*t));
	return result;
}

double Haptuator::triangleWave(double t, double freq, double mag) {
	double result;
	result = (double)8/(M_PI * M_PI) * mag * ( sin(2*M_PI*freq*t ) - (double)1/9 * sin(6*M_PI*freq*t) + (double)1/25 * sin(10*M_PI*freq*t)  - (double)1/49 * sin(14*M_PI*freq*t) + (double)1/81 * sin(18*M_PI*freq*t));
	return result;
}

double Haptuator::customSignal(double t) {
	int i;
	double result;
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
	initCoeff();
}

void Haptuator::renderVibration(double t) {
	_acc_set = customSignal(t);
}

void Haptuator::renderVibration(double t,int amp){
	_acc_set = amp;
} 

void Haptuator::renderVibration(double t,double freq,double mag,int mode){
	if (0 == mode)
		_acc_set = sinewave(t,freq,mag);
	else if (1 == mode)
		_acc_set = rectWave(t,freq,mag);
	else if (2 == mode)
		_acc_set = triangleWave(t,freq,mag);
	else
		_acc_set = 0;
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
	if (_acc_act.size() >= _blocks_size){
		_old_value = _acc_act[0];
		_acc_act.erase(_acc_act.begin());
	}
	_acc_act.push_back(acc);
	_new_value = acc;
}
void Haptuator::reset(){
	_acc_act.clear();
	_acc_set = 0;
	_ctrl_signal = 0;
	_acc_dft.clear();
	_isPSDinit = false;
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

void Haptuator::dft() {
	Complex sum ;
	for (int i = 3; i < 10; ++i) {
		sum = 0;
		for (int  j = 0; j < _blocks_size; ++j) {
			double a = -2.0 * M_PI * i * j /_blocks_size;
			sum += _acc_act[j] * Complex(cos(a),sin(a));
		}
		_acc_dft.push_back(sum);
	}
	_isPSDinit = true;

}

void Haptuator::initCoeff() {
	coeffs.clear();
	for (int i = 0; i < _fi.size(); ++i) {
		double a = -2.0 * M_PI * _fi[i]/SAMPLING;
		coeffs.push_back(Complex(cos(a), sin(a)));
	}
}

void Haptuator::sdft() {

	for (int i = 0; i < _fi.size(); ++i) {
		_acc_dft[i] = (_acc_dft[i] - _old_value + _new_value) * coeffs[i];
	}
}

double Haptuator::mag(Complex& c){
    return sqrt(c.real() * c.real() + c.imag() * c.imag());
}

void Haptuator::powrSpectrum()
{
	_powr.clear();
    for (int i = 0; i < _fi.size(); ++i) {
        _powr.push_back(mag(_acc_dft[i]));
    }
}

vector<double> Haptuator::getPowrSpetrum()
{
	return _powr;	
}
void Haptuator::updatePSD() {
	if ((!_isInterpolateSet) || (_acc_act.size() < _blocks_size))
		return; // if frequency is not set, return
	else if (_isPSDinit){
		sdft(); // Update fourier transform by sliding DFT
 	} else{		
 		dft();	// Initialize by DFT
 		
 	}

	// Calculating PSD
	powrSpectrum();
	// Update gain according to PSD
	for (int i = 0;i<_fi.size();i++){
		_gain[i] = _powr[i]/_psd[i];
	}
}

void Haptuator::setPSD(vector<float> psd){
	_psd = psd;
}