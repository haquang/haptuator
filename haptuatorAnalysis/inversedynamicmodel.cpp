/*
 * DynamicSimulation.cpp
 *
 *  Created on: Sep 21, 2015
 *      Author: haquang
 */

#include "inversedynamicmodel.h"
#include <iostream>
InverseDynamicModel::InverseDynamicModel() {
	// TODO Auto-generated constructor stub
}

int InverseDynamicModel::OdeFunc(double t, const double y[], double f[]) {
	f[0] = 1/_a1 * (_b1*_input.e3 +  _b2*_input.e2 + _b3*_input.e1 + _b4*_input.e0 - _a2*y[0]);
	return GSL_SUCCESS;
}

int InverseDynamicModel::Jacobian(double t, const double y[], double* dfdy,double dfdt[]) {
	float m1 = -_a2/_a1;
	dfdy_mat = gsl_matrix_view_array (dfdy, 1, 1);
	matrix = &dfdy_mat.matrix;
	gsl_matrix_set (matrix, 0, 0, m1);
	dfdt[0] = 0.0;
	return GSL_SUCCESS;
}



float InverseDynamicModel::run(double t,struct inputInverse u) {
	_input = u;
	if (GSL_SUCCESS == gsl_odeiv2_driver_apply (_driver, &start_time, t, _output)){
		_acc = _output[0];
	}
	return _acc;
}

void InverseDynamicModel::resetAcc() {
	_acc = 0;
	_output[0] = 0;
}

int InverseDynamicModel::staticOdeFunc(double t, const double y[], double f[],
		void* param) {
	return ((InverseDynamicModel*)param)->OdeFunc(t,y,f);
}

int InverseDynamicModel::staticJacobian(double t, const double y[], double* dfdy,
		double dfdt[], void* params) {
	return ((InverseDynamicModel*)params)->Jacobian(t,y,dfdy,dfdt);
}

InverseDynamicModel::~InverseDynamicModel() {
	// TODO Auto-generated destructor stub
	gsl_odeiv2_driver_free (_driver);
}


void InverseDynamicModel::initialization(float a1, float a2, float b1, float b2,
		float b3, float b4) {
	_a1 = a1;
	_a2 = a2;
	_b1 = b1;
	_b2 = b2;
	_b3 = b3;
	_b4 = b4;
	start_time = 0;

	_sys.function = staticOdeFunc;
	_sys.jacobian = staticJacobian;;
	_sys.dimension = 1;
	_sys.params = this;
	double h_start = (double) 1/TIMER_HAPTUATOR;
	double eps_abs = (double) 1/TIMER_HAPTUATOR;
	_driver = gsl_odeiv2_driver_alloc_y_new (&_sys, gsl_odeiv2_step_rk8pd,h_start, eps_abs, 0.0);
}
