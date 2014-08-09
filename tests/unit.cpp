#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../concat.hpp"

#include <array>
#include <vector>
#include <list>
#include <forward_list>
#include <queue>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>

using namespace theypsilon;
using namespace std;

template <typename T>
std::deque<T> ilist(std::initializer_list<T> list) {
    return list;
}

struct Corazon{};

TEST_CASE( "Basic types, identity", "basic_id" ) {
	CHECK( concat(1)   == "1");
	CHECK( concat(1.0) == "1");
	CHECK( concat('a') == "a");
	CHECK( concat("a") == "a");
	CHECK( concat(string("a")) == "a");
	CHECK( concat(true) == "1");
	//CHECK( concat("", Corazon{}) == ""); //Compile error
}

TEST_CASE( "Basic types, basic concat", "basic_c" ) {
	CHECK( concat(1,2,3,4,5) == "12345" );
	CHECK( concat("1","2","3","4","5") == "12345" );
	CHECK( concat('1','2','3','4','5') == "12345" );
	CHECK( concat(1.0,2.0,3.0,4.0,5.0) == "12345" );
	CHECK( concat('a','a')                           == "aa");
	CHECK( concat("a","a")                           == "aa");
	CHECK( concat(string("a"),string("a")) == "aa");
	CHECK( concat(true, false, true, false) == "1010");
}


TEST_CASE( "Basic types, separators", "basic_s" ) {
	CHECK( concat(separator(", "),1,2,3,4,5)  == "1, 2, 3, 4, 5" );
	CHECK((concat<',', ' '>(1,2,3,4,5))       == "1, 2, 3, 4, 5" );
	CHECK( concat<','>(1,2,3,4,5)             == "1,2,3,4,5" );
	CHECK( concat<'\n'>(1,2,3,4,5)            == "1\n2\n3\n4\n5" );
	CHECK( concat<' '>("Hello", "World!")     == "Hello World!" );
}

TEST_CASE( "Basic types, delimiters", "basic_d" ) {
	CHECK( concat(delimiter(", "),1,2,3,4,5)  == "1, 2, 3, 4, 5" );
	CHECK((concat<',', ' '>(1,2,3,4,5))       == "1, 2, 3, 4, 5" );
	CHECK( concat<','>(1,2,3,4,5)             == "1,2,3,4,5" );
	CHECK( concat<'\n'>(1,2,3,4,5)            == "1\n2\n3\n4\n5" );
	CHECK( concat<' '>("Hello", "World!")     == "Hello World!" );
	CHECK( concat(delimiter(", ", "(", ")"), 1, 2, vector<int>{3,4,5}) == "1, 2, (3, 4, 5)" );
	CHECK( concat(delimiter(), vector<int>{1, 2, 3}) == "(123)" );
	CHECK( concat(delimiter(), make_tuple(1, 2, 3)) == "(123)" );
	CHECK( concat(delimiter(), make_pair(1, 2)) == "(12)" );
}

TEST_CASE( "Basic types, mixed", "basic_m" ) {
	CHECK( concat(1,"2",3,"4",5,"6") == "123456" );
	CHECK( concat("1",2,"3",4,"5",6) == "123456" );
	CHECK( concat("a",2,3.0,'f')     == "a23f" );
}

TEST_CASE( "Pointer types, identity", "pointer_id" ) {
	string temp;
	CHECK( concat(static_cast<const char *>(nullptr)) == "");
	CHECK( concat(static_cast<string*>(nullptr)) == "0");
	CHECK( concat(static_cast<string*>(&temp)  ) != "0");
	CHECK( concat(static_cast<const void *>(nullptr)) == "0");
}

TEST_CASE( "Container types, identity", "container_id" ) {
	CHECK( concat(vector<int>{1,2,3,4,5}) == "12345" );
	CHECK( concat(list<int>{1,2,3,4,5}) == "12345" );
	CHECK( concat(forward_list<int>{1,2,3,4,5}) == "12345" );
	CHECK( concat(deque<int>{1,2,3,4,5}) == "12345" );
	CHECK( concat(array<int,5>{1,2,3,4,5}) == "12345" );
	CHECK( concat(set<int>{1,2,3,4,5}) == "12345" );
	CHECK( concat(map<int, int>{{1,2},{3,4},{5,6}}) == "123456" );
	CHECK( concat(multiset<int>{1,2,3,4,5}) == "12345" );
	CHECK( concat(multimap<int, int>{{1,2},{3,4},{5,6}}) == "123456" );
	CHECK( concat(unordered_set<int>{1,2,3,4,5}).size() == 5 );
	CHECK( concat(unordered_map<int, int>{{1,2},{3,4},{5,6}}).size() == 6 );
	CHECK( concat(unordered_multiset<int>{1,2,3,4,5}).size() == 5 );
	CHECK( concat(unordered_multimap<int, int>{{1,2},{3,4},{5,6}}).size() == 6 );
}

TEST_CASE( "Array types, identity", "array" ) {
	int a[] = {1,2,3,4,5};
	CHECK( concat(a) == "12345" );
}

TEST_CASE( "Container type, mixed", "container_m" ) {
	vector<int> v = {1,2,3,4,5};

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
	vector<string> s = {"hello"," ","world","!"};
	CHECK( concat(s)      == "hello world!" );
	CHECK( concat<' '>(s) == "hello   world !" );

	vector<const char*> c = {"hello"," ","world","!"};
	CHECK( concat(c)      == "hello world!" );
	CHECK( concat<' '>(c) == "hello   world !" );

	vector<char> ch = {'a','b','c'};
	CHECK( concat(ch)      == "abc" );
	CHECK( concat<' '>(ch) == "a b c" );
}

TEST_CASE( "Stream types, as host", "stream_host" ) {
	string temp;
	ostringstream s1;
	s1 << "hello";
	temp = concat<' '>(s1, "world!");
	CHECK  ( temp == s1.str() );
	REQUIRE( temp == "helloworld!");
	CHECK  ( concat(s1, 1,2,3) == "helloworld!123");
}

TEST_CASE( "Stream types, as guest", "stream_guest" ) {
	string temp;
	ostringstream s1, s2;
	s1 << "hello";
	s2 << "world!";
	temp = concat<' '>(static_cast<const ostringstream&>(s1), s2);
	CHECK( temp     != s1.str() );
	CHECK( temp     == "hello world!");
	CHECK( s1.str() == "hello");
}

TEST_CASE( "Stream types, as host and guest", "stream_hg" ) {
	string temp;
	ostringstream s1, s2;
	s1 << "hello";
	s2 << "world!";
	temp = concat<' '>(s1, s2);
	CHECK  ( temp == s1.str() );
	REQUIRE( temp == "helloworld!");
	CHECK  ( concat(s1, 1,2,3, s1, s1) == "helloworld!123helloworld!123helloworld!123helloworld!123");
}

TEST_CASE( "Stream types, mixed guest and host", "stream_m" ) {
	vector<string> s = {"hello"," ","world","!"};
	vector<const char*> c = {"hello"," ","world","!"};
	vector<char       > h = {'a','b','c'};

	ostringstream s1, s2;
	s1 << "hello";
	s2 << "world!";
	REQUIRE( concat(s1, s, c, h, s2, "amazing") == "hellohello world!hello world!abcworld!amazing");
	CHECK  ( s1.str()                           == "hellohello world!hello world!abcworld!amazing");
}

TEST_CASE( "Stream types, exception", "stream_exception" ) {
	stringstream s1, s2;
	s1.exceptions(ios::failbit | ios::eofbit | ios::badbit); 

	SECTION("failbit") {
		s2.setstate(ios::failbit);
		CHECK_THROWS_AS( concat(s1,s2), ios::failure );
	}

	SECTION("eofbit") {
		s2.setstate(ios::eofbit);
		CHECK_THROWS_AS( concat(s1,s2), ios::failure );
	}

	SECTION("badbit") {
		s2.setstate(ios::badbit);
		CHECK_THROWS_AS( concat(s1,s2), ios::failure );
	}

	SECTION("not cover") {
		s1.exceptions(ios::failbit);
		s2.setstate(ios::badbit);
		CHECK_NOTHROW( concat(s1,s2) );

		s1.exceptions(ios::failbit);
		s2.setstate(ios::eofbit);
		CHECK_NOTHROW( concat(s1,s2) );
	}

	SECTION("no throw") {
		s1.exceptions(ios::goodbit);

		s2.setstate(ios::failbit);
		CHECK_NOTHROW( concat(s1,s2) );

		s2.setstate(ios::badbit);
		CHECK_NOTHROW( concat(s1,s2) );

		s2.setstate(ios::eofbit);
		CHECK_NOTHROW( concat(s1,s2) );
	}
}

TEST_CASE( "Null text types, mixed", "nulltext" ) {
	const char* msg = nullptr;
	CHECK( concat<' '>(msg) == "");
	CHECK( concat<' '>("this is my message: ") == "this is my message: ");
	CHECK( concat("this is my message: ", msg) == "this is my message: ");
	CHECK( concat<' '>(msg, "this is my message:") == " this is my message:");
}

TEST_CASE( "Null stream types, mixed", "nullstream" ) {
	stringstream s;
	CHECK( concat("",s) == "");
	CHECK( concat("this is my message: ", s) == "this is my message: ");
	CHECK( concat<' '>("", s) == " ");
}

TEST_CASE( "Manipulators, mixed", "manipulators" ) {
	CHECK( concat<' '>(setprecision(2), 4.0/3.0, 1, 2) == "1.3 1 2");
	CHECK( concat<' '>(setbase(16), 10, 16, 8) == "a 10 8");
	CHECK( concat(setfill('-'), setw(5), 11) == "---11");
	CHECK( concat(resetiosflags(std::ios::dec),
		               setiosflags(std::ios::hex
                                 | std::ios::uppercase
                                 | std::ios::showbase),42) == "0X2A");
	stringstream s;
	s.imbue(locale("en_US.utf8"));
	CHECK( concat(s, showbase, put_money(1200)) == "$12.00");
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
	CHECK( concat(initializer_list<int>{1,2,3,4,5}) == "12345" );

	using inplace = decltype(initializer_list<int>{1,2,3,4,5});
	static_assert(is_same<decltype(a), initializer_list<int>&&>::value, "");
	//static_assert(is_same<decltype(b), const initializer_list<int>&>::value, ""); works in clang but fails in gcc
	static_assert(is_same<decltype(c), initializer_list<int>>::value, "");
	static_assert(is_same<inplace, initializer_list<int>>::value, "");
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

TEST_CASE( "pair, identity", "pair_id" ) {
	CHECK( concat(make_pair(1,2)) == "12" );
	CHECK( concat<' '>(make_pair("hello","world!")) == "hello world!" );
}

TEST_CASE( "pair, nested", "pair_nested" ) {
	CHECK( concat(make_pair(1,make_pair(2,3))) == "123" );
	CHECK( concat<' '>(make_pair("hello",make_pair("world","!"))) == "hello world !" );
	CHECK( concat<' '>(make_pair(make_pair("hello", "my"),make_pair("world", "!"))) == "hello my world !" );
}

TEST_CASE( "pair, mixed", "pair_mixed" ) {
	CHECK( concat(make_pair(1,2),3) == "123" );
	CHECK( concat(make_pair(1,2),make_pair(3,4)) == "1234" );
	CHECK( concat<' '>(make_pair("hello","world!"), "goodbye", "friend!") == "hello world! goodbye friend!" );
}

TEST_CASE( "README.md", "readme") {
	CHECK( concat("aa", "bb") == "aabb" );

	std::vector<int> v{1,2,3,4,5};
	CHECK( concat(v) == "12345" );

	CHECK( concat(separator(" + "), 1,2,3,4,5) == "1 + 2 + 3 + 4 + 5" );

	CHECK( concat<' '>('a','b','c') == "a b c" );

	CHECK( concat<' '>("hello", "world", std::make_tuple(1,2,3), '!', v)
			== "hello world 1 2 3 ! 1 2 3 4 5" );

	CHECK( concat<' '>(std::setprecision(2), 4.0/3.0, std::setprecision(3), 1.0/3.0) == "1.3 0.333" );

	std::stringstream s;
	concat<' '>(s, "it", "just", "works!");
	CHECK( s.str() == "it just works!" );

	std::ostringstream s1, s2;
	s2.setstate(std::ios::failbit);
	const char* error = nullptr;
	try {
	    s1.exceptions(std::ios::failbit);
	    concat(s1, s2); // s1 gets the output of reading s2
	} catch(std::ios::failure& e) {
	    error = e.what();
	}
	CHECK( error != nullptr );

	bool assertion = (concat<char16_t>(                u"uni", u"code") == u"unicode") &&
					 (concat<char32_t>(separator(U""), U"Uni", U"code") == U"Unicode");
	CHECK(assertion);
}

template <typename CharT = char>
struct UserDefinedType {
	const CharT* text;
	UserDefinedType(const CharT* text) : text{text} {}
	friend std::basic_ostream<CharT> & operator<< (std::basic_ostream<CharT> &out, UserDefinedType const &t) {
		out << t.text;
		return out;
	}
};

TEST_CASE( "User defined type overload, mixed", "user" ) {
	CHECK( concat(UserDefinedType<char>("UserDefinedType")) == "UserDefinedType" );
	CHECK( concat<' '>("my", UserDefinedType<char>("UserDefinedType"), "!") == "my UserDefinedType !" );
	CHECK( concat<wchar_t >(UserDefinedType<wchar_t >(L"UserDefinedType")) == L"UserDefinedType" );
	CHECK( concat<char16_t>(UserDefinedType<char16_t>(u"UserDefinedType")) == u"UserDefinedType" );
	CHECK( concat<char32_t>(UserDefinedType<char32_t>(U"UserDefinedType")) == U"UserDefinedType" );
}