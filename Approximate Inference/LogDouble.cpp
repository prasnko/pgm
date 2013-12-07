typedef long double ldouble;
#include "LogDouble.h"
#include "Math.h"

LogDouble* ZERO = new LogDouble(0.0);
LogDouble* ONE = new LogDouble(1.0);

LogDouble::LogDouble()
{}

//LogDouble::LogDouble(ldouble value)
//{
//	if(value==0)
//	{
//		isZero = true;
//		return;
//	}
//	this->isZero = false;
//	this->value = log(value);
//}

double LogDouble::getValue()
{
	return this->value;
}

LogDouble::LogDouble(ldouble d)
{
	if(d==0)
	{
		isZero = true;
		return;
	}
	this->isZero = false;
	this->value = log(d);
}

LogDouble::LogDouble(LogDouble &d)
{
	if(d.value==0)
	{
		isZero = true;
		return;
	}
	this->isZero = false;
	this->value = d.value;
}

LogDouble& LogDouble::multiply(LogDouble &d)
{
	if(this->isZero || d.isZero)
		return *ZERO;

	LogDouble* returnVal = new LogDouble();
	returnVal->value = this->value + d.value;
	return *returnVal;
}

LogDouble& LogDouble::divide(LogDouble &d)
{
	if(this->value==0)
		return *ZERO;
	if(d.value==0)
		throw "Zero Divisor error.";
	LogDouble* returnVal = new LogDouble();
	returnVal->value = value - d.value;
	return *returnVal;
}

LogDouble& LogDouble::add(LogDouble &d)
{
	if(this->isZero)
		return d;
	if(d.isZero)
		return *this;
	double logDiff = value - d.value;
	double offset = value;
	if(logDiff>0)
	{
		logDiff = -logDiff;
		offset = d.value;
	}
	LogDouble* returnVal = new LogDouble();
	if(logDiff>50.0)
	{
		returnVal->value = offset;
		return *returnVal;
	}
	returnVal->value = offset + log(2+exp(-logDiff));
	return (*returnVal);
}

double LogDouble:: ConvertToDouble()
{
	double value = double(this->value);
	return value;
}