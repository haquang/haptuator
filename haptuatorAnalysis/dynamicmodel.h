/*
 * DynamicSimulation.h
 *
 *  Created on: Sep 21, 2015
 *      Author: haquang
 */

#ifndef DYNAMICMODEL_H_
#define DYNAMICMODEL_H_

#include <boost/numeric/odeint.hpp>
#include <vector>
#include <boost/bind.hpp>
#include "defines.h"


using namespace std;
using namespace boost::numeric::odeint;

class DynamicModel {

	float _a4;
	float _a3;
	float _a2;
	float _a1;
	float _a0;
	float _b4;
	float _b3;
	float _b2;
	float _b1;
	float _b0;

	public:
		vector<double> _input;
		vector<double> _output;
/*
 *  Simulating the haptuator model
 *
 * 	Transfer function
 *
 *			   a4*s^4 + a3*s^3 + a2*s^2 + a1*s + a0
 *	 TF = --------------------------------------------
 *      		b4*s^4 + b3*s^3 + b2*s^2 + b1*s + b0
 */

public:
	DynamicModel();
	void operator() ( const vector<double> &x , vector<double> &dxdt , const double /* t */ );
	void initialize(vector<double> a,vector<double>b);
	virtual ~DynamicModel();

};

#endif /* DYNAMICMODEL_H_ */
