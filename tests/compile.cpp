#include "../concat.hpp"

using namespace std;
using namespace theypsilon;

void run(...) {}

#define RUN()\
void run(int yes)

#ifdef TEST_FAIL_CUSTOM_TYPE
	struct Custom{};

	RUN() { 
		concat("nope", Custom{}); 
	}
#endif


#ifdef TEST_SUCCESS_CUSTOM_TYPE
	struct Custom{};

	std::ostream & operator<< (std::ostream &out, Custom const &t) {
		return out;
	}

	RUN() { 
		concat("nope", Custom{}); 
	}
#endif

#ifdef TEST_FAIL_CUSTOM_TYPE_WRONG_STREAM
	struct Custom{};

	std::basic_ostream<wchar_t> & operator<< (std::basic_ostream<wchar_t> &out, Custom const &t) {
		return out;
	}

	alias type = decltype(concat("nope", Custom{}));
#endif

#ifdef TEST_SUCCESS_STRING
	RUN() { concat("yeah", string{}); }	
#endif

#ifdef TEST_FAIL_CHAR_WSTRING
	RUN() { concat<char>(wstring(L"nope")); }
#endif

#ifdef TEST_FAIL_ENDL1
	RUN() { concat(endl); }
#endif

#ifdef TEST_FAIL_ENDL2
	RUN() { concat("nope", endl); }
#endif

#ifdef TEST_FAIL_COUT1
	RUN() { concat(cout); }
#endif

#ifdef TEST_FAIL_COUT2
	RUN() { concat("nope", cout); }
#endif

int main(int argc, char* argv[]) {
	run(0);
	return 0;
}