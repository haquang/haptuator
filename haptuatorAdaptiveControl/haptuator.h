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
#include "referencemodel.h"
#include "filtermodel.h"
#include <boost/numeric/odeint.hpp>
using namespace std;
using namespace boost::numeric::odeint;
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
	float getAccRender();
	void reset();
	void setCtrl(double acc);
	double getAccReference();
	void setAcc(double acc);
	vector<double> getTheta();
	vector<double> getOmega();
	double getError();
	double getCtrlSignal();
	void saturation();

	/*
	 *	Controller
	 */
	void initModel(double Km,double Bm, double am1, double am2); // Wm = (Km*s)/(s^2 + am1*s + am2)
	void setGamma(double gamma);
	void adaptationLaw();
	void update(double t); // update new step for model, controller,...
	int run();

	virtual ~Haptuator();


private:
	bool _ctrl = false;
	// Signals
	float _A0;
	vector<float> _Ai;
	vector<float> _Bi;
	vector<float> _fi;
	bool _isInterpolateSet;
	vector<double> _acc_set; // set acceleration <acc,acc'>
	vector<double> _acc_ref; // acceleration from reference model <acc,acc'>
	vector<double> _acc_act; // actual acceleration - measure by accelerometer <acc,acc'>
	vector<double> _theta;  // adjustable control parameter
	vector<double> _omega;
	vector<double> _ctrl_signal; // control signal - after adaptive controller
	float _deltaT = (float) TIMER_HAPTUATOR / 1000000000;
	double _gamma = GAMMA;
	// Controller
	ReferenceModel _ref_model;
	FilterModel _input_filter;
	FilterModel _output_filter;
	runge_kutta4< vector<double> > _solver;
	double _err;
	// for sensoray/ni card
	int _subdev;
	int _chanel;
	int _range_idx = 1;
	int _aref = AREF_GROUND;
};

#endif /* HAPTUATOR_H_ */
