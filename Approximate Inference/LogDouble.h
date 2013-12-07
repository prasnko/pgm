#ifndef LOGDOUBLE_H
#define LOGDOUBLE_H
#include <iostream>
#include <vector>
#include <algorithm>
typedef long double ldouble;

struct LogDouble
{
	private:
		double value;
		bool isZero;
	public :
		LogDouble();
		LogDouble(ldouble d);
		LogDouble(LogDouble &d);
		double getValue();
		LogDouble& multiply(LogDouble &value);
		LogDouble& add(LogDouble &value);
	//	LogDouble subtract(LogDouble value);
		LogDouble& divide(LogDouble &value);
		double ConvertToDouble();
};

#endif