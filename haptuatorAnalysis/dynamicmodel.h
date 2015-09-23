/*
 * DynamicSimulation.h
 *
 *  Created on: Sep 21, 2015
 *      Author: haquang
 */

#ifndef DYNAMICMODEL_H_
#define DYNAMICMODEL_H_

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_ieee_utils.h>
#include <vector>
#include <boost/bind.hpp>
#include "defines.h"
using namespace std;
struct input {
	double e0; // e(t)
	double e1; // derivative of e(t)
};

struct memory {
	double t;
	double acc;
	memory(double time,double a){
		t= time;
		acc = a;
	}
};

class DynamicModel {

	float _a1;
	float _a2;
	float _b1;
	float _b2;
	float _b3;
	float _b4;
	double start_time = 0;

	input _input;
	double _output[3] = { 0.0, 0.0,0.0 };
	vector<memory> _acc;
	gsl_odeiv2_system _sys;
	gsl_odeiv2_driver * _driver;
	gsl_matrix_view dfdy_mat;
	gsl_matrix * matrix;
/*
 *  Simulating the haptuator model
 *
 * 	Transfer function
 *
 *			   a1*s + a2
 *	 TF = --------------------------
 *			b1*s^3+b2*s^2+b3*s+b4
 */

public:
	DynamicModel();
	static int staticOdeFunc (double t, const double y[], double f[], void *param);
	static int staticJacobian (double t, const double y[], double *dfdy, double dfdt[], void *params);
	void initialization(float a1,float a2,float b1,float b2,float b3,float b4);
	int OdeFunc (double t, const double y[], double f[]);
	int Jacobian (double t, const double y[], double *dfdy, double dfdt[]);
	float run(double t,struct input u);

	void resetAcc();
	void setInput(struct input in);
	vector<memory> getAcc();
	virtual ~DynamicModel();

};

#endif /* DYNAMICMODEL_H_ */
