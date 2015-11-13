/*
 * Haptuator.h
 *
 *  Created on: Sep 3, 2015
 *      Author: haquang
 */

#ifndef HAPTUATOR_H_
#define HAPTUATOR_H_

#include <vector>
#include <list>
#include <cmath>
#include <numeric>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <complex>
#include "daqdevice.h"
#include "defines.h"
using namespace std;

typedef complex<double> Complex;
/*
 *
 * Haptuator Model
 */

class Haptuator : public DaqDevice{
public:
	Haptuator();
	Haptuator(comedi_t* dev,int subDev,int channel,int range);
	double sinewave(double t,double freq,double mag);
	double rectWave(double t,double freq,double mag);
	double triangleWave(double t,double freq,double mag);
	double customSignal(double t);
	void setInterpolationParameter(float _A0,vector<float> _Ai,vector<float> _Bi,vector<float> _fi);
	void setGainInterpolation(vector<float> ref_freq,vector<float> ref_gain);
	void renderVibration(double t); // Driven data
	void renderVibration(double t,int amp); // Step
	void renderVibration(double t,double freq,double mag,int mode); // Chirp
	float getAccRender();
	void disablePSD();
	void reset();
	void setCtrl(double acc);
	void setAcc(double acc);
	int run();
	float interp1d(float val,vector<float> input,vector<float> out);
	virtual ~Haptuator();

	// Fourier Transform
	void initCoeff();
	void dft(); // For the first blocks_size samples, use DFT
	void sdft(); // For samples > blocks_size, use SDFT
	double mag(Complex& c);
	void powrSpectrum();
	void updatePSD();
	vector<double> getPowrSpetrum();
	void setPSD(vector<float> psd);
private:
	bool _ctrl = false;
	bool _isPSDinit = false;
	// Signals
	float _A0;
	vector<float> _Ai;
	vector<float> _Bi;
	vector<float> _fi;
	vector<float> _ref_freq;
	vector<float> _ref_gain;
	vector<float> _gain;
	vector<float> _psd;
	bool _isInterpolateSet;
	double _acc_set; // set acceleration

	double _ctrl_signal; // control signal - after adaptive controller
	float _deltaT = (float) TIMER_HAPTUATOR / 1000000000;
	double _gamma = GAMMA;

	int _blocks_size = BLOCK_SIZE; // Window size for fourier transform
	double _old_value; // Last value in previous sampling (use for Sliding DFT)
	double _new_value;
	vector<double> _acc_act; //  Vector of actual acceleration - measure by accelerometer (size is blocks_size)
	vector<Complex> _acc_dft; //  fourier transform of acceleration
	vector<Complex> coeffs;
	vector<double> _powr;
	// for sensoray/ni card
	int _subdev;
	int _chanel;
	int _range_idx = 1;
	int _aref = AREF_GROUND;

	gsl_interp_accel *_gsl_acc;
	gsl_spline *_gsl_spline;
};

#endif /* HAPTUATOR_H_ */
