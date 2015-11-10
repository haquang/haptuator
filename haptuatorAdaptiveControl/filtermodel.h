/*
 * filtermodel.h
 *
 *  Created on: Oct 22, 2015
 *      Author: haquang
 */

#ifndef FILTERMODEL_H_
#define FILTERMODEL_H_
#include <vector>

using namespace std;

class FilterModel {
	float _Km;
	float _Bm;
	vector<double> _input;
public:
	FilterModel();
	void operator()(const vector<double> &x , vector<double> &dxdt , const double /* t */ );
	void initialize(double Km,double Bm);
	void setInput(vector<double> v);
	virtual ~FilterModel();
};

#endif /* FILTERMODEL_H_ */
