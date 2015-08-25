/*
 * cSpline.cpp
 *
 *  Created on: Aug 21, 2015
 *      Author: haquang
 */

#include "cspline.h"

#include <algorithm>
#include <vector>

#include "cspline.h"

using namespace std;

cSpline::cSpline(vector<float> x, vector<float> y)
{
    _x = x;
    _y = y;

    int n = x.size()-1;
    vector<float> a;
    a.insert(a.begin(), y.begin(), y.end());
    vector<float> b(n);
    vector<float> d(n);
    vector<float> h;

    for(int i = 0; i < n; ++i)
        h.push_back(x[i+1]-x[i]);

    vector<float> alpha;
    for(int i = 0; i < n; ++i)
        alpha.push_back( 3*(a[i+1]-a[i])/h[i] - 3*(a[i]-a[i-1])/h[i-1]  );

    vector<float> c(n+1);
    vector<float> l(n+1);
    vector<float> mu(n+1);
    vector<float> z(n+1);
    l[0] = 1;
    mu[0] = 0;
    z[0] = 0;

    for(int i = 1; i < n; ++i)
    {
        l[i] = 2 *(x[i+1]-x[i-1])-h[i-1]*mu[i-1];
        mu[i] = h[i]/l[i];
        z[i] = (alpha[i]-h[i-1]*z[i-1])/l[i];
    }

    l[n] = 1;
    z[n] = 0;
    c[n] = 0;

    for(int j = n-1; j >= 0; --j)
    {
        c[j] = z [j] - mu[j] * c[j+1];
        b[j] = (a[j+1]-a[j])/h[j]-h[j]*(c[j+1]+2*c[j])/3;
        d[j] = (c[j+1]-c[j])/3/h[j];
    }

    spline_set.resize(n);
    for(int i = 0; i < n; ++i)
    {
        spline_set[i].a = a[i];
        spline_set[i].b = b[i];
        spline_set[i].c = c[i];
        spline_set[i].d = d[i];
        spline_set[i].x = x[i];
    }
    return;
}

float cSpline::getY( float x)
{
    int j;
    for ( j = 0; j < spline_set.size(); j++ )
    {
        if( spline_set[j].x > x )
        {
            if( j == 0 )
                j++;
            break;
        }
    }
    j--;

    float dx = x - spline_set[j].x;
    float y = spline_set[j].a + spline_set[j].b * dx + spline_set[j].c * dx* dx +
               spline_set[j].d * dx* dx * dx;

    return y;

}


