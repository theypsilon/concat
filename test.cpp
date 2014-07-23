#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "concat.hpp"

#include <list>
#include <queue>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

using namespace theypsilon;
using namespace std;

TEST_CASE( "Basic types, identity", "[concat]" ) {
	REQUIRE( concat(1)   == "1");
	REQUIRE( concat(1.0) == "1");
	REQUIRE( concat('a') == "a");
	REQUIRE( concat("a") == "a");
	REQUIRE( concat(std::string("a")) == "a");
	REQUIRE( concat(true) == "1");
}

TEST_CASE( "Basic types, basic concat", "[concat]" ) {
	REQUIRE( concat(1,2,3,4,5) == "12345" );
	REQUIRE( concat("1","2","3","4","5") == "12345" );
	REQUIRE( concat('1','2','3','4','5') == "12345" );
	REQUIRE( concat(1.0,2.0,3.0,4.0,5.0) == "12345" );
	REQUIRE( concat('a','a')                           == "aa");
	REQUIRE( concat("a","a")                           == "aa");
	REQUIRE( concat(std::string("a"),std::string("a")) == "aa");
	REQUIRE( concat(true, false, true, false) == "1010");
}


TEST_CASE( "Basic types, separators", "[concat]" ) {
	REQUIRE( concat(separator(", "),1,2,3,4,5)  == "1, 2, 3, 4, 5" );
	REQUIRE((concat<',', ' '>(1,2,3,4,5))       == "1, 2, 3, 4, 5" );
	REQUIRE( concat<','>(1,2,3,4,5)             == "1,2,3,4,5" );
	REQUIRE( concat<endl>(1,2,3,4,5)            == "1\n2\n3\n4\n5" );
	REQUIRE( concat<separator::plus>(1,2,3,4,5) == "1 + 2 + 3 + 4 + 5" );
	REQUIRE( concat<' '>("Hello", "World!")     == "Hello World!" );
}

TEST_CASE( "Basic types, mixed", "[concat]" ) {
	REQUIRE( concat(1,"2",3,"4",5,"6") == "123456" );
	REQUIRE( concat("1",2,"3",4,"5",6) == "123456" );
	REQUIRE( concat("a",2,3.0,'f')     == "a23f" );
}

TEST_CASE( "Pointer types, identity", "[concat]" ) {
	std::string temp;
	REQUIRE( concat(static_cast<const char *>(nullptr)) == "");
	REQUIRE( concat(static_cast<std::string*>(nullptr)) == "0");
	REQUIRE( concat(static_cast<std::string*>(&temp)  ) != "0");
	REQUIRE( concat(static_cast<const void *>(nullptr)) == "0");
}

TEST_CASE( "Container types, identity", "[concat]" ) {
	REQUIRE( concat(std::vector<int>{1,2,3,4,5}) == "12345" );
	REQUIRE( concat(std::list<int>{1,2,3,4,5}) == "12345" );
	REQUIRE( concat(std::set<int>{1,2,3,4,5}) == "12345" );
}

TEST_CASE( "Container types, mixed", "[concat]" ) {
	std::vector<int> v = {1,2,3,4,5};

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
}

TEST_CASE( "Container types, text identities and separators", "[concat]" ) {
	std::vector<std::string> s = {"hello"," ","world","!"};
	REQUIRE( concat(s)      == "hello world!" );
	REQUIRE( concat<' '>(s) == "hello   world !" );

	std::vector<const char*> c = {"hello"," ","world","!"};
	REQUIRE( concat(c)      == "hello world!" );
	REQUIRE( concat<' '>(c) == "hello   world !" );

	std::vector<char> ch = {'a','b','c'};
	REQUIRE( concat(ch)      == "abc" );
	REQUIRE( concat<' '>(ch) == "a b c" );
}

TEST_CASE( "Stream types, as host", "[concat]" ) {
	std::string temp;
	std::ostringstream s1, s2;
	s1 << "hello";
	s2 << "world!";
	temp = concat<' '>(s1, s2);
	REQUIRE( temp == s1.str() );
	REQUIRE( temp == "helloworld!");
	REQUIRE( concat(s1, 1,2,3, s1, s1) == "helloworld!123helloworld!123helloworld!123helloworld!123");
}

TEST_CASE( "Stream types, as guest", "[concat]" ) {
	std::string temp;
	std::ostringstream s1, s2;
	s1 << "hello";
	s2 << "world!";
	temp = concat<' '>(static_cast<const std::ostringstream&>(s1), s2);
	REQUIRE( temp     != s1.str() );
	REQUIRE( temp     == "hello world!");
	REQUIRE( s1.str() == "hello");
}

TEST_CASE( "Stream types, mixed guest and host", "[concat]" ) {
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
	s1 << "hello";
	s2 << "world!";
	REQUIRE( concat(s1, s, c, ch, s2, "amazing") == "hellohello world!hello world!abcworld!amazing");
	REQUIRE( s1.str()                            == "hellohello world!hello world!abcworld!amazing");
}

TEST_CASE( "Null text types, mixed", "[concat]" ) {
	const char* msg = nullptr;
	REQUIRE( concat<' '>(msg) == "");
	REQUIRE( concat<' '>("this is my message: ") == "this is my message: ");
	REQUIRE( concat("this is my message: ", msg) == "this is my message: ");
}

TEST_CASE( "Modifiers, mixed", "[concat]" ) {
	REQUIRE( concat<' '>(std::setprecision(2), 4.0/3.0, 1, 2) == "1.3 1 2");
}

TEST_CASE( "UTF text types, identity", "[concat]" ) {
//	REQUIRE( concat(std::wstring()) == "");
//	REQUIRE( concat(u"This is a Unicode Character: \u2018.") == "" );
}