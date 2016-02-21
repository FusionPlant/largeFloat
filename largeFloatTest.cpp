#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>

#include "largeFloat.h"

using namespace std;



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

	int numCount = 0;
	largeFloat ans("0", 20); //20 significant digits
	string line, number;
	stringstream ioss;
	while (getline(floatFile, line)) {
		ioss << line;
		while (getline(ioss, number, ' ')) {
			cout << ans.str() << " + ";
			largeFloat lf(number, 20);
			ans += lf;
			cout << lf.str() << " = " << ans.str() << endl;
			numCount++;
		}
		ioss.clear();
	}
	cout << "Count: " << numCount << endl;
	cout << "Sum: " << ans.str();
	floatFile.close();
	getchar();
}
