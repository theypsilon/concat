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

TEST_CASE( "Basic types, identity", "basic_id" ) {
	CHECK( concat(1)   == "1");
	CHECK( concat(1.0) == "1");
	CHECK( concat('a') == "a");
	CHECK( concat("a") == "a");
	CHECK( concat(std::string("a")) == "a");
	CHECK( concat(true) == "1");
}

TEST_CASE( "Basic types, basic concat", "basic_c" ) {
	CHECK( concat(1,2,3,4,5) == "12345" );
	CHECK( concat("1","2","3","4","5") == "12345" );
	CHECK( concat('1','2','3','4','5') == "12345" );
	CHECK( concat(1.0,2.0,3.0,4.0,5.0) == "12345" );
	CHECK( concat('a','a')                           == "aa");
	CHECK( concat("a","a")                           == "aa");
	CHECK( concat(std::string("a"),std::string("a")) == "aa");
	CHECK( concat(true, false, true, false) == "1010");
}


TEST_CASE( "Basic types, separators", "basic_s" ) {
	CHECK( concat(separator(", "),1,2,3,4,5)  == "1, 2, 3, 4, 5" );
	CHECK((concat<',', ' '>(1,2,3,4,5))       == "1, 2, 3, 4, 5" );
	CHECK( concat<','>(1,2,3,4,5)             == "1,2,3,4,5" );
	CHECK( concat<endl>(1,2,3,4,5)            == "1\n2\n3\n4\n5" );
	CHECK( concat<sep::plus>(1,2,3,4,5)       == "1 + 2 + 3 + 4 + 5" );
	CHECK( concat<' '>("Hello", "World!")     == "Hello World!" );
}

TEST_CASE( "Basic types, mixed", "basic_m" ) {
	CHECK( concat(1,"2",3,"4",5,"6") == "123456" );
	CHECK( concat("1",2,"3",4,"5",6) == "123456" );
	CHECK( concat("a",2,3.0,'f')     == "a23f" );
}

TEST_CASE( "Pointer types, identity", "pointer_id" ) {
	std::string temp;
	CHECK( concat(static_cast<const char *>(nullptr)) == "");
	CHECK( concat(static_cast<std::string*>(nullptr)) == "0");
	CHECK( concat(static_cast<std::string*>(&temp)  ) != "0");
	CHECK( concat(static_cast<const void *>(nullptr)) == "0");
}

TEST_CASE( "Container types, identity", "container_id" ) {
	CHECK( concat(std::vector<int>{1,2,3,4,5}) == "12345" );
	CHECK( concat(std::list<int>{1,2,3,4,5}) == "12345" );
	CHECK( concat(std::set<int>{1,2,3,4,5}) == "12345" );
}

TEST_CASE( "Array types, identity", "array" ) {
	int a[] = {1,2,3,4,5};
	CHECK( concat(a) == "12345" );
}

TEST_CASE( "Container types, mixed", "container_m" ) {
	std::vector<int> v = {1,2,3,4,5};

	CHECK( concat(1,2,3,4,5,v) == "1234512345" );
	CHECK( concat(v,1,2,3,4,5) == "1234512345" );
	CHECK( concat(1,2,3,v,4,5) == "1231234545" );
	CHECK( concat(v,v)         == "1234512345" );
	CHECK( concat(v,1,v)       == "12345112345" );
	CHECK( concat(1,v,v,1)     == "112345123451" );
	CHECK( concat(1,v,1,v,1)   == "1123451123451" );
	CHECK( concat(v,v,v)       == "123451234512345" );
	CHECK( concat("something",v) == "something12345" );
	CHECK( concat(v,"something") == "12345something" );
}

TEST_CASE( "Container types, text identities and separators", "container_s" ) {
	std::vector<std::string> s = {"hello"," ","world","!"};
	CHECK( concat(s)      == "hello world!" );
	CHECK( concat<' '>(s) == "hello   world !" );

	std::vector<const char*> c = {"hello"," ","world","!"};
	CHECK( concat(c)      == "hello world!" );
	CHECK( concat<' '>(c) == "hello   world !" );

	std::vector<char> ch = {'a','b','c'};
	CHECK( concat(ch)      == "abc" );
	CHECK( concat<' '>(ch) == "a b c" );
}

TEST_CASE( "Stream types, as host", "stream_host" ) {
	std::string temp;
	std::ostringstream s1;
	s1 << "hello";
	temp = concat<' '>(s1, "world!");
	CHECK( temp == s1.str() );
	CHECK( temp == "helloworld!");
	CHECK( concat(s1, 1,2,3) == "helloworld!123");
}

TEST_CASE( "Stream types, as guest", "stream_guest" ) {
	std::string temp;
	std::ostringstream s1, s2;
	s1 << "hello";
	s2 << "world!";
	temp = concat<' '>(static_cast<const std::ostringstream&>(s1), s2);
	CHECK( temp     != s1.str() );
	CHECK( temp     == "hello world!");
	CHECK( s1.str() == "hello");
}

TEST_CASE( "Stream types, as host and guest", "stream_hg" ) {
	std::string temp;
	std::ostringstream s1, s2;
	s1 << "hello";
	s2 << "world!";
	temp = concat<' '>(s1, s2);
	CHECK( temp == s1.str() );
	CHECK( temp == "helloworld!");
	CHECK( concat(s1, 1,2,3, s1, s1) == "helloworld!123helloworld!123helloworld!123helloworld!123");
}

TEST_CASE( "Stream types, mixed guest and host", "stream_m" ) {
	std::vector<std::string> s = {"hello"," ","world","!"};
	std::vector<const char*> c = {"hello"," ","world","!"};
	std::vector<char       > h = {'a','b','c'};

	std::ostringstream s1, s2;
	s1 << "hello";
	s2 << "world!";
	CHECK( concat(s1, s, c, h, s2, "amazing") == "hellohello world!hello world!abcworld!amazing");
	CHECK( s1.str()                           == "hellohello world!hello world!abcworld!amazing");
}

TEST_CASE( "Stream types, exception", "stream_exception" ) {
	std::stringstream s1, s2;
	s1.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit); 

	SECTION("failbit") {
		s2.setstate(std::ios::failbit);
		CHECK_THROWS_AS( concat(s1,s2), std::ios::failure );
	}

	SECTION("eofbit") {
		s2.setstate(std::ios::eofbit);
		CHECK_THROWS_AS( concat(s1,s2), std::ios::failure );
	}

	SECTION("badbit") {
		s2.setstate(std::ios::badbit);
		CHECK_THROWS_AS( concat(s1,s2), std::ios::failure );
	}

	SECTION("not cover") {
		s1.exceptions(std::ios::failbit);
		s2.setstate(std::ios::badbit);
		CHECK_NOTHROW( concat(s1,s2) );

		s1.exceptions(std::ios::failbit);
		s2.setstate(std::ios::eofbit);
		CHECK_NOTHROW( concat(s1,s2) );
	}

	SECTION("no throw") {
		s1.exceptions(std::ios::goodbit);

		s2.setstate(std::ios::failbit);
		CHECK_NOTHROW( concat(s1,s2) );

		s2.setstate(std::ios::badbit);
		CHECK_NOTHROW( concat(s1,s2) );

		s2.setstate(std::ios::eofbit);
		CHECK_NOTHROW( concat(s1,s2) );
	}
}

TEST_CASE( "Null text types, mixed", "nulltext" ) {
	const char* msg = nullptr;
	CHECK( concat<' '>(msg) == "");
	CHECK( concat<' '>("this is my message: ") == "this is my message: ");
	CHECK( concat("this is my message: ", msg) == "this is my message: ");
	CHECK( concat<' '>("this is my message:", msg) == "this is my message: ");
}

TEST_CASE( "Null stream types, mixed", "nullstream" ) {
	std::stringstream s;
	CHECK( concat("",s) == "");
	CHECK( concat("this is my message: ", s) == "this is my message: ");
	CHECK( concat<' '>("", s) == " ");
}

TEST_CASE( "Modifiers, mixed", "modifiers" ) {
	CHECK( concat<' '>(std::setprecision(2), 4.0/3.0, 1, 2) == "1.3 1 2");
}

TEST_CASE( "UTF types, identity", "utf_id" ) {
	CHECK( concat<wchar_t >(L"wstring") == L"wstring" );
	CHECK( concat<char16_t>(u"unicode") == u"unicode" );
	CHECK( concat<char32_t>(U"Unicode") == U"Unicode" );

	CHECK( concat<wchar_t >(separator(L""), L"wstring") == L"wstring" );
	CHECK( concat<char16_t>(separator(u""), u"unicode") == u"unicode" );
	CHECK( concat<char32_t>(separator(U""), U"Unicode") == U"Unicode" );
}

TEST_CASE( "UTF types, basic concat", "utf_c" ) {
	CHECK( concat<wchar_t >(L"This is", L"wstring") == L"This iswstring" );
	CHECK( concat<char16_t>(u"This is", u"unicode") == u"This isunicode" );
	CHECK( concat<char32_t>(U"This is", U"Unicode") == U"This isUnicode" );

	CHECK( concat<wchar_t >(separator(L" "), L"This is", L"wstring") == L"This is wstring" );
	CHECK( concat<char16_t>(separator(u" "), u"This is", u"unicode") == u"This is unicode" );
	CHECK( concat<char32_t>(separator(U" "), U"This is", U"Unicode") == U"This is Unicode" );
}

TEST_CASE( "initializer typed list, identity", "init_t_id" ) {
	auto&& 		a = {1,2,3,4,5};
	const auto& b = {1,2,3,4,5};
	auto 		c = {1,2,3,4,5};
	CHECK( concat(a) == "12345" );
	CHECK( concat(b) == "12345" );
	CHECK( concat(c) == "12345" );
	CHECK( concat(std::initializer_list<int>{1,2,3,4,5}) == "12345" );

	using inplace = decltype(std::initializer_list<int>{1,2,3,4,5});
	static_assert(std::is_same<decltype(a), std::initializer_list<int>&&>::value, "");
	//static_assert(std::is_same<decltype(b), const std::initializer_list<int>&>::value, ""); works in clang but fails in gcc
	static_assert(std::is_same<decltype(c), std::initializer_list<int>>::value, "");
	static_assert(std::is_same<inplace, std::initializer_list<int>>::value, "");
}

TEST_CASE( "initializer non-inferred list, identity", "init_t_id" ) {
	CHECK( concat(ilist({1,2,3,4,5})) == "12345" );
	CHECK( concat(separator(", "), ilist({1,2,3,4,5})) == "1, 2, 3, 4, 5" );
}

TEST_CASE( "tuple, identity", "tuple_id" ) {
	CHECK( concat(make_tuple(1,2,3,4,5)) == "12345" );
	CHECK( concat<' '>(make_tuple("hello","world!")) == "hello world!" );
}

TEST_CASE( "tuple, nested", "tuple_nested" ) {
	CHECK( concat(make_tuple(1,2,3,make_tuple(4,5))) == "12345" );
	CHECK( concat<' '>(make_tuple("hello",make_tuple("world!"))) == "hello world!" );
	CHECK( concat<' '>(make_tuple(make_tuple("hello", "my"),make_tuple("world!"))) == "hello my world!" );
}

TEST_CASE( "tuple, mixed", "tuple_mixed" ) {
	CHECK( concat(make_tuple(1,2,3,4),5) == "12345" );
	CHECK( concat(make_tuple(1,2,3),make_tuple(4,5)) == "12345" );
	CHECK( concat<' '>(make_tuple("hello","world!"), "goodbye", "friend!") == "hello world! goodbye friend!" );
}