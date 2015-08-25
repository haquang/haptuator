/*
 * DriveDataParser.h
 *
 *  Created on: Aug 13, 2015
 *      Author: haquang
 */

#ifndef DRIVEDATAPARSER_H_
#define DRIVEDATAPARSER_H_
#include <vector>
#include <string>
#include "cspline.h"
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

using namespace std;
class DriveDataParser {
public:
	DriveDataParser();
	virtual ~DriveDataParser();

	bool loadData(string filename);
	vector<float> getRefSpeed();
	float interp1d(float val,vector<float> input,vector<float> out);
	float speedInterpA0(float speed); // A0
	vector<float> speedInterpA(float speed); // A
	vector<float> speedInterpB(float speed); // B

//	void swap(int i,int j);
	void sorting();
private:
	vector<float> ref_speed; // Reference speed
	vector<vector<float> > acc_signal;

	vector<float> A0;
	vector<vector<float> > A;
	vector<vector<float> > B;

	gsl_interp_accel *_gsl_acc;
	gsl_spline *_gsl_spline;

};

#endif /* DRIVEDATAPARSER_H_ */
