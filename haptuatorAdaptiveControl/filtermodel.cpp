/*
 * filtermodel.cpp
 *
 *  Created on: Oct 22, 2015
 *      Author: haquang
 */

#include "filtermodel.h"
#include <iostream>
FilterModel::FilterModel() {
	// TODO Auto-generated constructor stub

}

void FilterModel::operator ()(const vector<double>& x, vector<double>& dxdt,
		const double constDouble) {
	dxdt[0] = _input[0] - _Bm * x[0]; // 1/Km * (Km*input - Km*Bm*x)
}

void FilterModel::initialize(double Km, double Bm) {
	_Km = Km;
	_Bm = Bm;
}

void FilterModel::setInput(vector<double> v) {
	_input = v;
}

FilterModel::~FilterModel() {
	// TODO Auto-generated destructor stub
}

