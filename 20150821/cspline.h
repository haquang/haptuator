/*
 * cSpline.h
 *
 *  Created on: Aug 21, 2015
 *      Author: haquang
 */

#ifndef CSPLINE_H_
#define CSPLINE_H_

#include <functional>
#include <vector>

using namespace std;
class cSpline
{

public:

	/**  Constructor: Calculates the spline curve coefficients
    @param[in] x  The x points
    @param[in] y  The y points
	 */
	cSpline(vector<float> x, vector<float> y );

	/** Get the Y value of the spline curves for a particular X
    @param[in] x
    @return the y value
	 */
	float getY( float x);

private:

	// The fitted points
	vector<float> _x;
	vector<float> _y;

	/// The coefficients of the spline curve between two points
	struct SplineSet
	{
		double a;   // constant
		double b;   // 1st order coefficient
		double c;   // 2nd order coefficient
		double d;   // 3rd order coefficient
		double x;   // starting x value
	};

	/// The coefficients of the spline curves between all points
	std::vector< SplineSet > spline_set;

};


#endif /* CSPLINE_H_ */
