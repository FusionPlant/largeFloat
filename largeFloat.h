#ifndef _LARGEFLOAT_H
#define _LARGEFLOAT_H

#include <string>

using namespace std;

#define MAX_POWER_DIGIT 29

class largeFloat {
public:
	//constructor
	//str: string represented float number, in the format of -123.456e-002, no space in between allowed
	//significantDigits: number of significant digits stored
	largeFloat(const string & str, int significantDigits);

	//compare the absolute value of current large float number and argument large float number
	//return 1 if current large float number has greater absolute value, -1 otherwise
	//return 0 if absolute values are equal within current float number's significant digits range
	int isAbsLargerThan(const largeFloat& lf) const;

	//destrutor
	~largeFloat();

	//add the argument large float number to current large float number, keep the current significant digits setting
	void operator += (const largeFloat& lf);

	//return the string representation of current large float number
	string str();

private:
	//the digits of large float number
	string number;

	//the power of the large float number
	//if power = 0, add "0." before the number variable would be the float number's value
	int power;

	//max significant digits
	int sigDigit;

	//true if it is not a negative float number
	bool isPositive;

	//return true if the string represent a valid float number
	bool isValidFloat(const string & str);
};
#endif
