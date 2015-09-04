/*
 * DriveDataParser.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: haquang
 */

#include "drivedataparser.h"

#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cmath>

DriveDataParser::DriveDataParser() {
	// TODO Auto-generated constructor stub

}

DriveDataParser::~DriveDataParser() {
	// TODO Auto-generated destructor stub
}

bool DriveDataParser::loadData(string filename) {
	ifstream ifs(filename.c_str());
	string line;
	stringstream split;
	int s_speed; // number of speed samples
	int s_freq;  // number of freq
	float f_val;

	if (ifs) {
		// read the first line for the size of ref_speed, A,B
		getline(ifs,line);
		split << line;
		split >> s_speed >> s_freq;

		cout<< "Size of Speed :" << s_speed << " and Freq: " << s_freq << endl;

		// read the second line for ref_speed
		getline(ifs,line);
		split.clear();
		split << line;
		while (split >> f_val){
			ref_speed.push_back(f_val);
		}

		// read the third line for A0
		getline(ifs,line);
		split.clear();
		split.str(line);
		while (split >> f_val)
			A0.push_back(f_val);

		// read the s_freq line for A
		for (int i = 0;i < s_freq;i++){
			getline(ifs,line);
			split.clear();
			split.str(line);
			vector<float> v;
			while (split >> f_val)
				v.push_back(f_val);
			A.push_back(v);
		}
		// read the rest line for B
		for (int i = 0;i < s_freq;i++){
			getline(ifs,line);
			split.clear();
			split.str(line);
			vector<float> v;
			while (split >> f_val)
				v.push_back(f_val);
			B.push_back(v);
		}

		_gsl_acc = gsl_interp_accel_alloc ();
		_gsl_spline = gsl_spline_alloc(gsl_interp_cspline, s_speed);

//				cout << "Speed: " << endl;
//				for (vector<float>::iterator it = ref_speed.begin() ; it != ref_speed.end(); ++it)
//					cout << ' ' << *it;
//				cout << '\n';

//					cout << "A0: " << endl;
//					for (vector<float>::iterator it = A0.begin() ; it != A0.end(); ++it)
//						cout << ' ' << *it;
//					cout << '\n';
		//
//					cout << "A1: " << endl;
//					for (vector<float>::iterator it = A[0].begin() ; it != A[0].end(); ++it)
//						cout << ' ' << *it;
//					cout << '\n';

//					cout << "A: " << endl;
//					vector< vector<float> >::iterator row;
//					vector<float>::iterator col;
//					for (row = A.begin(); row != A.end(); row++) {
//						for (col = row->begin(); col != row->end(); col++) {
//							 cout << *col << "  ";
//						}
//						cout << endl;
//					}
		//
		//			cout << "B: " << endl;
		//
		//			for (row = B.begin(); row != B.end(); row++) {
		//				for (col = row->begin(); col != row->end(); col++) {
		//					 cout << *col << "  ";
		//				}
		//				cout << endl;
		//			}
		return true;
	}
	return false;
}

vector<float> DriveDataParser::getRefSpeed() {
	return ref_speed;
}

float DriveDataParser::interp1d(float val, vector<float> in,vector<float> out) {
	//	cSpline spline(in,out);
	//	return spline.getY(val);

		double  *x, *y;
		x = new double[in.size()];
		y = new double[in.size()];
		float result;
		for (int i = 0;i<in.size();i++)
		{
			x[i] = in[i];

			y[i] = out[i];
		}
		gsl_spline_init(_gsl_spline, x, y, in.size());

		result =(float) (gsl_spline_eval (_gsl_spline, (double) val, _gsl_acc));
		delete x,y;
		return result;

}

float DriveDataParser::speedInterpA0(float speed) {
	return interp1d(speed,ref_speed,A0);
}

vector<float> DriveDataParser::speedInterpA(float speed) {
	vector<float> result;
	vector< vector<float> >::iterator it;
	float a;

	for (it = A.begin();it!= A.end();it++){
		result.push_back(interp1d(speed,ref_speed,*it));
	}

	return result;
}

vector<float> DriveDataParser::speedInterpB(float speed) {
	vector<float> result;
	vector< vector<float> >::iterator it;
	float a;
	for (it = B.begin();it!= B.end();it++){
		result.push_back(interp1d(speed,ref_speed,*it));
	}
	return result;
}

