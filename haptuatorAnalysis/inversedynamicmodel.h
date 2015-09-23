/*
 * DynamicSimulation.h
 *
 *  Created on: Sep 21, 2015
 *      Author: haquang
 */

#ifndef INVERSEDYNAMICMODEL_H_
#define INVERSEDYNAMICMODEL_H_

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_ieee_utils.h>
#include <vector>
#include <boost/bind.hpp>
#include "defines.h"
using namespace std;
struct inputInverse {
	double e0; // e(t)
	double e1; // derivative of e(t)
	double e2; // derivative of e1(t)
	double e3; // derivative of e2(t)

};

class InverseDynamicModel {

	float _a1;
	float _a2;
	float _b1;
	float _b2;
	float _b3;
	float _b4;
	double start_time = 0;

	inputInverse _input;
	double _output[1] = { 0.0};
	float _acc;
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
	InverseDynamicModel();
	static int staticOdeFunc (double t, const double y[], double f[], void *param);
	static int staticJacobian (double t, const double y[], double *dfdy, double dfdt[], void *params);
	void initialization(float a1,float a2,float b1,float b2,float b3,float b4);
	int OdeFunc (double t, const double y[], double f[]);
	int Jacobian (double t, const double y[], double *dfdy, double dfdt[]);
	float run(double t,struct inputInverse u);
	void resetAcc();
	virtual ~InverseDynamicModel();

};

#endif /* INVERSEDYNAMICMODEL_H_ */
