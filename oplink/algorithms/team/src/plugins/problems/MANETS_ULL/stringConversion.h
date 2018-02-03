#ifndef STRING_CONVERSION_H
#define STRING_CONVERSION_H

#include <string>
#include <sstream>

using namespace std;

template<typename T> string toString(const T& source)
{
	ostringstream oss;
	oss << source;
	return oss.str();
}

#endif
