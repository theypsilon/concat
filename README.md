concat.hpp [![Build Status](https://travis-ci.org/theypsilon/concat.svg?branch=master)](https://travis-ci.org/theypsilon/concat)
======

Because string concatenation deserves one-liners in C++11 too.

```cpp
std::cout << concat("aa", "bb") << std::endl;
/* output: "aabb" */
```
    
    
That simple. **concat** also works with containers and other scalar types.

```cpp
std::vector<int> v{1,2,3,4,5};
std::cout << concat(v) << std::endl;
/* output: "12345" */
```

    
You may use separators in two ways:

```cpp
std::cout << concat(separator(" + "), 1,2,3,4,5) << std::endl;
/* output: "1 + 2 + 3 + 4 + 5" */

std::cout << concat<' '>('a','b','c') << std::endl;
/* output: "a b c" */
```



It is possible to mix between different parameter types, because under the hood we are using a ``std::ostringstream``.

```cpp
std::cout << concat<' '>("hello", "world", std::make_tuple(1,2,3), '!', v) << std::endl;
/* output: "hello world 1 2 3 ! 1 2 3 4 5" */
```



Yeah, it also accepts tuples (even nested ones). You may also introduce modifiers.

```cpp
std::cout << concat<' '>(std::setprecision(2), 4.0/3.0, std::setprecision(3), 1.0/3.0) << std::endl;
/* output: "1.3 0.333" */
```



And if you want fine-grained control of the underlying ``std::stringstream``, you may also supply it. Just make sure that you pass it as the first parameter (second, if there is also a separator parameter).

```cpp
std::stringstream s;
concat<' '>(s, "it", "just", "works!");
std::cout << s.str() << endl;
/* output: "it just works!" */
```



If you supply the std:stringstream as the second or any other parameter, it just gonna be converted to ``std::string``, so you are not writing on it.

Supplying the ``std::stringstream`` can be useful.

```cpp
std::ostringstream s1, s2;

read_file(s2, "test.txt"); // this might cause s2.setstate(std::ios::failbit);

try {
    s1.exceptions(std::ios::failbit);
    concat(s1, s2); // s1 gets the output of reading s2
} catch(std::ios::failure& e) {
    std::cout << e.what() << std::endl;
}

/* output could be like this: "ios_base::clear: unspecified iostream_category error" */
```


    
You can work with unicode, by specifing the char type as template parameter.

```cpp
assert((concat<char16_t>(                u"uni", u"code") == u"unicode") &&
       (concat<char32_t>(separator(U""), U"Uni", U"code") == U"Unicode"));
           
/* that's true! */
```
    

By the way, the only way to specify a separator with UTF parameters is that one.

String type conversion between different UTF charsets is not yet implemented, so when you choose an encoding format, you have to stick to it for all the supplied parameters.

Why not just use std::stringstream?
------

``std::stringstream`` is good enough in many situations, but sometimes I wish it could cover more use cases. Unfortunately with ``std::stringstream`` you can not print arrays, containers, tuples or other stringstreams in a uniform and concise manner. 

Furthemore there are some tricky things about streams, that can make them feel a little unsafe. I.e.: when you attempt to add a ```(const char*)nullptr``` to a stream, it silently fails (unless you configured exceptions), and following operations with ``<<`` would totally be ignored. This would never happen with **concat** unless you decide to inject your own ``std::stringstream`` as shown above.

Separators, and the function syntax, are also some nice additions that could help to produce a more terse and readable code. It is convenient to remember that in most code we write, we should always think in trading off a little performance penalty for a better readability.

Know more
------

If you want to read more about the power of **concat**, you can learn all you need to know just by reading ``test.cpp``.

Build
------
It is just a header file! Just copy ``concat.hpp`` to your include path, maybe rename the namespace to something more convenient than my nickname, and start using it. 

Of course, also make sure your compiler is set to C++11 and that you are linking a standard library implementation in your project, because there is no other dependency. 

----

Please **"Star"** the project on GitHub to help it to survive! Thanks!
