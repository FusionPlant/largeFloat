#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>

#include "largeFloat.h"

using namespace std;

#define MAX_POWER_DIGIT 29

largeFloat::largeFloat(const string & str, int significantDigits = 100) {
	if (!isValidFloat(str)) {
		cout << "Error in float number format!" << endl;
		exit(-1);
	}
	sigDigit = significantDigits;
	bool beforeDot = true;
	bool beforeExp = true;
	int sdCount = 0;
	isPositive = true;
	power = 0;
	for (size_t i = 0; i < str.length(); i++) {
		// have read enough significant digits
		if (sdCount >= significantDigits) {
			int loc = str.find_first_of('.', i);
			if (loc != string::npos) {
				power += loc - i;
				i = loc;
			}
			loc = str.find_first_of("eEfE", i);
			int afterExp;
			istringstream iss(str.substr(loc, MAX_POWER_DIGIT));
			iss >> afterExp;
			power += afterExp;
			return;
		}
		// parser
		switch (str[i]) {
		case '.':
			beforeDot = false;
			break;
		case '-':
			if (beforeExp) {
				isPositive = false;
			}
			else {
				int afterExp;
				istringstream iss(str.substr(i, MAX_POWER_DIGIT));
				iss >> afterExp;
				power += afterExp;
				return;
			}
			break;
		case 'E':
		case 'e':
		case 'F':
		case 'f':
			beforeExp = false;
			break;
		case '+':
		case ' ':
			break;
		default: //digits
			if (beforeDot && beforeExp) {
				power++;
				number.append(1, str[i]);
			}
			else if (beforeExp) {
				number.append(1, str[i]);
			}
			else {
				int afterExp;
				istringstream iss(str.substr(i, MAX_POWER_DIGIT));
				iss >> afterExp;
				power += afterExp;
				return;
			}
			sdCount++;
		}
	}
	//remove leading zeros
	int i = number.find_first_not_of('0');
	if (i == string::npos) {
		number = "0";
		power = 0;
		isPositive = true;
	}
	else {
		int j = number.find_last_not_of('0');
		power -= i;
		number = number.substr(i, j - i + 1);
	}
}

int largeFloat::isAbsLargerThan(const largeFloat& lf) const{
	if (power > lf.power) {
		return 1;
	}
	else if (power < lf.power) {
		return -1;
	}
	//power equal
	size_t i, numSize = min(number.size(), lf.number.size());
	for (i = 0; i < numSize; i++) {
		if (number[i] > lf.number[i]) {
			return 1;
		}
		else if (number[i] < lf.number[i]) {
			return -1;
		}
	}
	if (i < number.size()) {
		return 1;
	}
	else if (i < lf.number.size()) {
		return -1;
	}
	return 0;
}

largeFloat::~largeFloat() {
}

void largeFloat::operator += (const largeFloat& lf) {
	int i, j;
	size_t lfSize = lf.number.size();
	//determine the digit where addition starts
	int sign, lfSign, comp = isAbsLargerThan(lf);
	if (comp > 0) {
		i = number.size() - 1;
		j = i - (power - lf.power);
		if (j < static_cast<int>(lfSize - 1)) {
			number.append(lfSize - j - 1, '0');
			i += lfSize - j - 1;
			j = lfSize - 1;
		}
		sign = 1;
		lfSign = (isPositive == lf.isPositive) ? 1 : -1;
	}
	else if (comp < 0) {
		i = number.size() - 1;
		j = i + (lf.power - power);
		if (j < static_cast<int>(lfSize - 1)) {
			number.append(lfSize - j - 1, '0');
			i += lfSize - j - 1;
			j = lfSize - 1;
		}
		lfSign = 1;
		sign = (isPositive == lf.isPositive) ? 1 : -1;
		isPositive = lf.isPositive;
	}
	else if(isPositive != lf.isPositive) {
		//same absolute value, opposite sign
		number = "0";
		power = 0;
		isPositive = true;
		return;
	}
	else {
		i = number.size() - 1;
		j = i;
		lfSign = 1;
		sign = 1;
	}

	int overFlow = 0;
	while (i >= 0 && j >= 0) {
		char lfDigit = '0';
		assert(i < number.size());
		if (j < lfSize) {
			lfDigit = lf.number[j];
		}
		number[i] = sign*(number[i] - '0') + lfSign*(lfDigit - '0') + '0' + overFlow;
		overFlow = 0;
		while (number[i] > '9') {
			number[i] -= 10;
			overFlow++;
		}
		while (number[i] < '0') {//if also works
			number[i] += 10;
			overFlow--;
		}
		i--;
		j--;
	}
	//if extra number in j
	while (j >= 0) {
		assert(lfSign == 1);
		char lfDigit = '0';
		if (j < lf.number.size()) {
			lfDigit = lf.number[j];
		}
		number.insert(0, 1, lfDigit + overFlow);
		power++;
		overFlow = 0;
		while (number[0] > '9') {//if also works
			number[0] -= 10;
			overFlow++;
		}
		while (number[0] < '0') {//if also works
			number[0] += 10;
			overFlow--;
		}
		j--;
	}
	//if overflow still greater than 0 after both numbers have been added
	while (overFlow != 0) {
		if (i < 0) {
			assert(i == -1 && overFlow > 0 && overFlow < 3);
			number.insert(0, 1, overFlow + '0');
			overFlow = 0;
			power++;
		}
		else {// i >= 0 after j == 0, thus sign must be 1
			assert(sign == 1);
			assert(i < number.size());
			number[i] += overFlow;
			overFlow = 0;
			while (number[i] > '9') {//if also works
				number[i] -= 10;
				overFlow++;
			}
			while (number[i] < '0') {//if also works
				number[i] += 10;
				overFlow--;
			}
			i--;
		}
	}
	//remove leading zeros
	i = number.find_first_not_of('0');
	assert(i != string::npos);
	j = number.find_last_not_of('0');
	power -= i;
	number = number.substr(i, j - i + 1);
}

string largeFloat::str() {
	ostringstream oss;
	if (!isPositive) {
		oss << "-";
	}
	oss << "0." << number << "E" << power;
	return oss.str();
}

bool largeFloat::isValidFloat(const string & str) {
	int state = 0;
	//0 before sign1;
	//1 after sign1;
	//2 after dot;
	//3 after e/E or f/F;
	//4 after sign2;
	//use state transition matix to implement
	//implement later
	return true;
}