concat.hpp [![Build Status](https://travis-ci.org/theypsilon/concat.svg?branch=master)](https://travis-ci.org/theypsilon/concat)
======

Because string concatenation deserves one-liners in C++ too.


    std::cout << concat("aa", "bb") << std::endl;
    output: "aabb"
    
    
    
That simple. concat also works with containers and other scalar types.


    std::vector<int> v{1,2,3,4,5};
    std::cout << concat(v) << std::endl;
    output: "12345"


    
You may use separators in two ways:


    std::cout << concat(separator(" + "), 1,2,3,4,5) << std::endl;
    output: "1 + 2 + 3 + 4 + 5"
    
    std::cout << concat<' '>('a','b','c') << std::endl;
    output: "a b c"



    
It is possible to mix between different parameter types, because the under the hood we are using a std::ostringstream.


    std::cout << concat<' '>("hello", "world", std::make_tuple(1,2,3), std::string("!"), v) << std::endl;
    output: "hello world 1 2 3 ! 1 2 3 4 5"



    
Yeah, it also accepts tuples (even nested ones). You may also introduce modifiers.


    concat<' '>(setprecision(2), 4.0/3.0, setprecision(3), 1.0/3.0);
    output: "1.3 0.33"



    
And if you want fine-grained control of the underlying std::stringstream, you may also supply it. Just make sure that you pass it as the first parameter (second, if there is also a separator parameter).


    std::stringstream s;
    concat<' '>(s, "it", "just", "works!");
    std::cout << s.str() << endl;
    output: "it just works!"



    
If you supply the std:stringstream as the second or any other parameter, it just gonna be converted to std::string, so you are not writting on it.

Supplying the std::stringstream can be useful.


    std::ostringstream s1, s2;
    
    read_file(s2, "test.txt"); // this might cause s2.setstate(std::ios::failbit);
    
    try {
        s1.exceptions(std::ios::failbit);
        concat(s1, s2); // s1 gets the output of reading s2
    } catch(std::ios::failure& e) {
        std::cout << e.what() << std::endl;
    }
    
    output could be like this: "ios_base::clear: unspecified iostream_category error"



    
You can work with unicode, by specifing the char type as template parameter.


    assert((concat<char16_t>(                u"unicode") == u"unicode") &&
           (concat<char32_t>(separator(U""), U"Unicode") == U"Unicode"));
           
    that's true!
    
    

By the way, the only way to specify a separator with UTF parameters is that one.

String type conversion between different UTF charsets is not yet implemented, so when you choose an encoding format, you have to stick to it for all the supplied parameters.

Know more
------

If you want to read more about the power of concat, test.cpp is waiting for you :)

Build
------
It is just a header file! Just copy it to your include path and maybe rename the namespace to something more convenient than my nickname, and start using it.

----

Please **"Star"** the project on GitHub to help it to survive! Thanks!