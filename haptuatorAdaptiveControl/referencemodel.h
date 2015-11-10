/*
 * referencemodel.h
 *
 *  Created on: Oct 22, 2015
 *      Author: haquang
 */

#ifndef REFERENCEMODEL_H_
#define REFERENCEMODEL_H_
#include <vector>
#include <boost/numeric/odeint.hpp>

using namespace std;

class ReferenceModel {
	float _am1;
	float _am2;
	float _Km;
	float _Bm;
	vector<double> _input;
public:
	ReferenceModel();
	void operator()(const vector<double> &x , vector<double> &dxdt , const double /* t */ );
	void initialize(double Km,double Bm,double am1,double am2);
	void setInput(vector<double> v);
	virtual ~ReferenceModel();
};

#endif /* REFERENCEMODEL_H_ */
