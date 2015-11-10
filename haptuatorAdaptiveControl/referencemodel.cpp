/*
 * referencemodel.cpp
 *
 *  Created on: Oct 22, 2015
 *      Author: haquang
 */

#include "referencemodel.h"

ReferenceModel::ReferenceModel() {
	// TODO Auto-generated constructor stub
	_Km = 0;
	_am1 = 0;
	_am2 = 0;
}
void ReferenceModel::operator()(const vector<double> &xm , vector<double> &dxmdt , const double /* t */ ){
	dxmdt[0] = xm[1];
	dxmdt[1] = (_Km* (_input[1] + _Bm * _input[0]) - _am1*xm[1] - _am2*xm[0]);
}

void ReferenceModel::initialize(double Km,double Bm, double am1, double am2) {
	_am1 = am1;
	_am2 = am2;
	_Km = Km;
	_Bm = Bm;
}

void ReferenceModel::setInput(vector<double> v) {
	_input = v;
}

ReferenceModel::~ReferenceModel() {
}

