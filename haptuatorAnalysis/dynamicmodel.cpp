/*
 * DynamicSimulation.cpp
 *
 *  Created on: Sep 21, 2015
 *      Author: haquang
 */

#include "dynamicmodel.h"
#include <iostream>
DynamicModel::DynamicModel() {
	// TODO Auto-generated constructor stub
}

int DynamicModel::OdeFunc(double t, const double y[], double f[]) {
	f[0] = y[1];
	f[1] = y[2];
	f[2] = 1/_b1 * (_a1*_input.e1 + _a2 * _input.e0 - _b2*y[2] - _b3*y[1] - _b4*y[0]);
	return GSL_SUCCESS;
}

int DynamicModel::Jacobian(double t, const double y[], double* dfdy,double dfdt[]) {


	float m1 = -_b4/_b1;
	float m2 = -_b3/_b1;
	float m3 = -_b2/_b1;

	dfdy_mat = gsl_matrix_view_array (dfdy, 3, 3);
	matrix = &dfdy_mat.matrix;
	gsl_matrix_set (matrix, 0, 0, 0.0);
	gsl_matrix_set (matrix, 0, 1, 1.0);
	gsl_matrix_set (matrix, 0, 2, 0.0);

	gsl_matrix_set (matrix, 1, 0, 0.0);
	gsl_matrix_set (matrix, 1, 1, 0.0);
	gsl_matrix_set (matrix, 1, 2, 1.0);


	gsl_matrix_set (matrix, 2, 0, m1);
	gsl_matrix_set (matrix, 2, 1, m2);
	gsl_matrix_set (matrix, 2, 2, m3);

	dfdt[0] = 0.0;
	dfdt[1] = 0.0;
	dfdt[2] = 0.0;
	return GSL_SUCCESS;
}

void DynamicModel::setInput(struct input in){
	_input = in;
}



float DynamicModel::run(double t,struct input u) {
	_input = u;
	if (GSL_SUCCESS == gsl_odeiv2_driver_apply (_driver, &start_time, t, _output)){
		_acc.push_back(memory(t,_output[0]));
	}
}

void DynamicModel::resetAcc() {
	_acc.clear();
	_output[0] = 0;
	_output[1] = 0;
	_output[2] = 0;
}

vector<memory> DynamicModel::getAcc() {
	return _acc;
}

int DynamicModel::staticOdeFunc(double t, const double y[], double f[],
		void* param) {
	return ((DynamicModel*)param)->OdeFunc(t,y,f);
}

int DynamicModel::staticJacobian(double t, const double y[], double* dfdy,
		double dfdt[], void* params) {
	return ((DynamicModel*)params)->Jacobian(t,y,dfdy,dfdt);
}

DynamicModel::~DynamicModel() {
	// TODO Auto-generated destructor stub
	gsl_odeiv2_driver_free (_driver);
}


void DynamicModel::initialization(float a1, float a2, float b1, float b2,
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
	_sys.dimension = 3;
	_sys.params = this;
	double h_start = (double) 1/TIMER_HAPTUATOR;
	double eps_abs = (double) 1/TIMER_HAPTUATOR;
	_driver = gsl_odeiv2_driver_alloc_y_new (&_sys, gsl_odeiv2_step_rk8pd,h_start, eps_abs, 0.0);
}
