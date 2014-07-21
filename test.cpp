#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "concat.hpp"

using namespace theypsilon;
using namespace std;

TEST_CASE( "Basic concat", "[concat]" ) {
	std::string temp;

	REQUIRE( concat(1,2,3,4,5)                  == "12345" );
	REQUIRE( concat(separator(", "),1,2,3,4,5)  == "1, 2, 3, 4, 5" );
	REQUIRE((concat<',', ' '>(1,2,3,4,5))       == "1, 2, 3, 4, 5" );
	REQUIRE( concat<','>(1,2,3,4,5)             == "1,2,3,4,5" );
	REQUIRE( concat<endl>(1,2,3,4,5)            == "1\n2\n3\n4\n5" );
	REQUIRE( concat<separator::plus>(1,2,3,4,5) == "1 + 2 + 3 + 4 + 5" );
	REQUIRE( concat(1,"2",3,"4",5,"6")          == "123456" );
	REQUIRE( concat("1",2,"3",4,"5",6)          == "123456" );
	REQUIRE( concat<' '>("Hello", "World!")     == "Hello World!" );
	REQUIRE( concat('a',3,"b")                  == "a3b" );
	REQUIRE( concat('a')              == "a");
	REQUIRE( concat("a")              == "a");
	REQUIRE( concat(std::string("a")) == "a");
	REQUIRE( concat(true, false, true, false) == "1010");
	REQUIRE( concat(static_cast<const char *>(nullptr)) == "");
	REQUIRE( concat(static_cast<std::string*>(nullptr)) == "0");
	REQUIRE( concat(static_cast<std::string*>(&temp)  ) != "0");
	REQUIRE( concat(static_cast<const void *>(nullptr)) == "0");
	REQUIRE( concat<' '>(std::setprecision(2), 4.0/3.0, 1, 2) == "1.3 1 2");

	std::vector<int> v = {1,2,3,4,5};

	REQUIRE( concat(v) == "12345" );
	REQUIRE( concat(1,2,3,4,5,v) == "1234512345" );
	REQUIRE( concat(v,1,2,3,4,5) == "1234512345" );
	REQUIRE( concat(1,2,3,v,4,5) == "1231234545" );
	REQUIRE( concat(v,v)         == "1234512345" );
	REQUIRE( concat(v,1,v)       == "12345112345" );
	REQUIRE( concat(1,v,v,1)     == "112345123451" );
	REQUIRE( concat(1,v,1,v,1)   == "1123451123451" );
	REQUIRE( concat(v,v,v)       == "123451234512345" );
	REQUIRE( concat("something",v) == "something12345" );
	REQUIRE( concat(v,"something") == "12345something" );

	std::vector<std::string> s = {"hello"," ","world","!"};
	REQUIRE( concat(s)      == "hello world!" );
	REQUIRE( concat<' '>(s) == "hello   world !" );

	std::vector<const char*> c = {"hello"," ","world","!"};
	REQUIRE( concat(c)      == "hello world!" );
	REQUIRE( concat<' '>(c) == "hello   world !" );

	std::vector<char> ch = {'a','b','c'};
	REQUIRE( concat(ch)      == "abc" );
	REQUIRE( concat<' '>(ch) == "a b c" );

	std::ostringstream s1, s2;
	s1.exceptions(std::ifstream::failbit);
	s1 << "hello";
	s2 << "world!";
	temp = concat<' '>(s1, s2);
	REQUIRE( temp == s1.str() );
	REQUIRE( temp == "helloworld!");
	REQUIRE( concat(s1, 1,2,3, s1, s1) == "helloworld!123helloworld!123helloworld!123helloworld!123");

	s1.str(""); s2.str("");	
	s1.clear(); s2.clear();
	s1 << "hello";
	s2 << "world!";
	temp = concat<' '>(static_cast<const std::ostringstream&>(s1), s2);
	REQUIRE( temp     != s1.str() );
	REQUIRE( temp     == "hello world!");
	REQUIRE( s1.str() == "hello");

	REQUIRE( concat(s1, s, c, ch, s2, "amazing") == "hellohello world!hello world!abcworld!amazing");
	REQUIRE( s1.str()                            == "hellohello world!hello world!abcworld!amazing");

	REQUIRE( concat(std::wstring()) == "");

	const char* msg = nullptr;
	REQUIRE( concat<' '>(msg) == "");
	REQUIRE( concat<' '>("this is my message: ") == "this is my message: ");
	REQUIRE( concat<' '>("this is my message: ", msg) == "this is my message: ");

	REQUIRE( concat(u"This is a Unicode Character: \u2018.") == "" );
}