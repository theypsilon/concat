#include "concat.hpp"
#include "catch.hpp"

#include <iostream>

using namespace theypsilon;
using namespace std;

int main (int argc, char* argv[]) {
	std::stringstream ss;
	ss << "10, ";
	int i = 3;
	cout << concat<separator::coma>(1,2,3,4,5,6,7,8) << endl;
	return 0;
}