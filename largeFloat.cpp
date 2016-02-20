#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>

using namespace std;

#define MAX_POWER_DIGIT 29

class largeFloat {
public:
	largeFloat(const string & str, int significantDigits = 100) {
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
		for (int i = 0; i < str.length(); i++) {
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
				if (beforeDot) {
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
			}
		}
	}
	int isAbsLargerThan(const largeFloat& lf) const{
		if (power > lf.power) {
			return 1;
		}
		else if (power < lf.power) {
			return -1;
		}
		//power equal
		int i, j;
		for (i = 0, j = 0; i < number.size() && j < lf.number.size(); i++, j++) {
			if (number[i] > number[j]) {
				return 1;
			}
			else if (number[i] < number[j]) {
				return -1;
			}
		}
		if (i < number.size()) {
			return 1;
		}
		else if (j < lf.number.size()) {
			return -1;
		}
		return 0;
	}
	~largeFloat() {
	}
	void operator += (const largeFloat& lf) {
		int i, j;
		//determine the digit where addition starts
		int sign, lfSign;
		if (isAbsLargerThan(lf) >= 0) {
			i = sigDigit - 1;
			j = sigDigit - (power - lf.power) - 1;
			sign = 1;
			lfSign = isPositive == lf.isPositive ? 1 : -1;
		}
		else {
			i = sigDigit - (lf.power - power) - 1;
			j = sigDigit - 1;
			power = lf.power;
			lfSign = 1;
			sign = isPositive == lf.isPositive ? 1 : -1;
			isPositive = lf.isPositive;
		}
		char overFlow = 0;
		while (i >= 0 && j >= 0) {
			number[i] = sign*(number[i]-'0') + lfSign*(lf.number[j]-'0') + '0' + overFlow;
			overFlow = 0;
			while (number[i] > '9') {
				number[i] -= 10;
				overFlow ++;
			}
			if (number[i] < '0') {
				number[i] += 10;
				overFlow--;
			}
			i--;
			j--;
		}
		//if extra number in j
		while (j >= 0) {
			assert(lfSign == 1);
			number.insert(0, 1, number[j] + overFlow);
			overFlow = 0;
			if (number[0] > '9') {
				number[0] -= 10;
				overFlow++;
			}
			if (number[i] < '0') {
				number[i] += 10;
				overFlow--;
			}
		}
		//if overflow still greater than 0 after both numbers have been added
		while (overFlow != 0) {
			if (i == 0) {
				assert(overFlow > 0);
				number.insert(0, 1, overFlow + '0');
				overFlow = 0;
			}
			else {
				assert(sign == 1);
				number[i] += overFlow;
				overFlow = 0;
				if (number[i] > '9') {
					number[i] -= 10;
					overFlow++;
				}
				if (number[i] < '0') {
					number[i] += 10;
					overFlow--;
				}
				i--;
			}
			power++;
		}
	}
	string str() {
		ostringstream oss;
		oss << number << 'E' << power;
		return oss.str();
	}
private:
	string number;
	int power;
	int sigDigit;
	bool isPositive;
	bool isValidFloat(const string & str) {
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
};

// Driver program to test above functions
int main(int argc, char *argv[])
{
	if (argc == 1 || argc > 2) {
		cout << "Error in arguments!" << endl;
		return -1;
	}
	ifstream floatFile(argv[1]);
	if (!floatFile.is_open()) {
		cout << "Cannot open file!" << endl;
		return -1;
	}
	string number;
	int numCount = 0;
	largeFloat ans("0", 20); //20 significant digits
	while (getline(floatFile, number, ' ')) {
		ans += largeFloat(number, 20);
		numCount++;
	}
	cout << "Count: " << numCount;
	cout << "Sum: " << ans.str();
	floatFile.close();
}
