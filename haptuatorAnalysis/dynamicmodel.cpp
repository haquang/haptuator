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

void DynamicModel::initialize(vector<double> a,vector<double>b) {
	_a0 = a[0];
	_a1 = a[1];
	_a2 = a[2];
	_a3 = a[3];
	_a4 = a[4];

	_b0 = b[0];
	_b1 = b[1];
	_b2 = b[2];
	_b3 = b[3];
	_b4 = b[4];

	_input.clear();
	_output.clear();

	for (int i = 0 ;i <= 4; i++){
		_input.push_back(0.0f);
		_output.push_back(0.0f);
	}
}

void DynamicModel::operator ()(const vector<double>& x, vector<double>& dxdt,
		const double constDouble) {
	dxdt[0] = x[1];
	dxdt[1] = x[2];
	dxdt[2] = x[3];
	dxdt[3] = 1/_b4 * (_a4*_input[4] + _a3*_input[3] + _a2*_input[2] + _a1*_input[1] + _a0*_input[0] - _b3*x[3] - _b2*x[2] - _b1*x[1] - _b0*x[0]);
}

DynamicModel::~DynamicModel(){}
