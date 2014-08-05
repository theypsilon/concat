/*
 *  CONCAT
 *  Version: 2014-07-29
 *  ----------------------------------------------------------
 *  Copyright (c) 2014 José Manuel Barroso Galindo. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef THEYPSILON_CONCAT
#define THEYPSILON_CONCAT

#include <sstream>
#include <iomanip>
#include <tuple>
#include <utility>

namespace theypsilon { // rename this to something that fits your code

	namespace { // separator and delimiter types
		template <typename CharT>
		struct separator_t {
		    const CharT *space;
		    constexpr explicit separator_t(const CharT* s) noexcept : space{s} {}
			friend std::basic_ostream<CharT> & operator<< (std::basic_ostream<CharT> &out, separator_t const &s) {
				out << s.space;
				return out;
			}
		};

		template <typename CharT>
		struct delimiter_t : separator_t<CharT> {
		    const CharT *left, *right;
		    constexpr explicit delimiter_t(const CharT* s, const CharT* l, const CharT* r) noexcept
				: separator_t<CharT>{s}, left{l}, right{r} {}
		};
    }

    // separator api, 3 functions
    template <typename CharT>
    constexpr separator_t<CharT> separator(const CharT* s) {
        return separator_t<CharT>(s);
    }

    template <typename CharT>
    constexpr delimiter_t<CharT> separator(const CharT* s, const CharT* l, const CharT* r) {
        return delimiter_t<CharT>(s, l, r);
    }

    template <typename CharT = char>
    constexpr delimiter_t<CharT> delimiter(const CharT* s = "", const CharT* l = "(", const CharT* r = ")") {
        return delimiter_t<CharT>(s, l, r);
    }

    namespace { // type helpers and traits
        template<typename T, typename CharT>
        struct is_writable_stream : std::integral_constant<bool,
            std::is_same<T, std::basic_ostringstream<CharT>>::value ||
            std::is_same<T, std::basic_stringstream <CharT>>::value ||
            std::is_same<T, std::basic_ostream      <CharT>>::value>{};

        template<typename T, typename CharT = char>
        struct is_stringstream : std::integral_constant<bool,
            std::is_same<T, std::basic_istringstream<CharT>>::value ||
            std::is_same<T, std::basic_ostringstream<CharT>>::value ||
            std::is_same<T, std::basic_stringstream <CharT>>::value>{};

        template<typename T, typename CharT = char>
        struct is_c_str : std::integral_constant<bool,
            std::is_same<typename std::decay<T>::type, CharT const *>::value ||
            std::is_same<typename std::decay<T>::type, CharT       *>::value>{};

        template<typename T>
        struct is_char_sequence : std::integral_constant<bool,
            is_c_str<T,     char>::value ||
            is_c_str<T,  wchar_t>::value ||
            is_c_str<T, char16_t>::value ||
            is_c_str<T, char32_t>::value>{};

        template <typename T>
        struct is_string : std::integral_constant<bool,
            std::is_same<T,std::string   >::value ||
            std::is_same<T,std::wstring  >::value ||
            std::is_same<T,std::u16string>::value ||
            std::is_same<T,std::u32string>::value>{};

        struct can_const_begin_end_impl {
            template<typename T, typename B = decltype(std::begin(std::declval<const T&>())),
                                 typename E = decltype(std::end  (std::declval<const T&>()))>
            static std::true_type  test(int);
            template<typename...>
            static std::false_type test(...);
        };

        template<typename T>
        struct can_const_begin_end : public decltype(can_const_begin_end_impl::test<T>(0)) {};

        template<typename T>
        struct is_iterable : std::integral_constant<bool,
            can_const_begin_end<T>::value &&
            !is_string<T>::value && !is_stringstream<T>::value && !is_char_sequence<T*>::value>{};

        template<typename CharT>
        struct does_overload_ostream_impl {
            template<typename T, typename B = decltype(operator<<( std::declval<std::basic_ostream<CharT>>(),
                                                                   std::declval<const T&>()               ))>
            static std::true_type  test(int);
            template<typename...>
            static std::false_type test(...);
        };

        template<typename CharT, typename T>
        struct does_overload_ostream : public decltype(does_overload_ostream_impl<CharT>::template test<T>(0)) {};

        template <typename CharT, typename T>
        struct is_parametrized_manipulator : std::integral_constant<bool,
            std::is_same<T, decltype(std::setbase      (std::declval<int>()))>::value ||
            std::is_same<T, decltype(std::setprecision (std::declval<int>()))>::value ||
            std::is_same<T, decltype(std::setw         (std::declval<int>()))>::value ||
            std::is_same<T, decltype(std::setfill      (std::declval<CharT>()))>::value ||
            std::is_same<T, decltype(std::setiosflags  (std::declval<std::ios::fmtflags>()))>::value ||
            std::is_same<T, decltype(std::resetiosflags(std::declval<std::ios::fmtflags>()))>::value>{};

        template <typename CharT, typename T>
        struct is_manipulator : std::integral_constant<bool,
            (std::is_function<T>::value || is_parametrized_manipulator<CharT, T>::value)
            && does_overload_ostream<CharT, T>::value>{};

        template <typename T, template <typename...> class Template>
        struct is_specialization_of : std::false_type {};

        template <template <typename...> class Template, typename... Args>
        struct is_specialization_of<Template<Args...>, Template> : std::true_type {};

        template <typename T>
        struct is_separator : std::integral_constant<bool,
            is_specialization_of<T, separator_t>::value ||
            is_specialization_of<T, delimiter_t>::value>{};

        template <bool B, class T = void>
        using enable_if_t = typename std::enable_if<B, T>::type;
    }

    namespace { // concat_impl : stringstream to string helper, separator handlers, and parameter writer functions

        template <typename CharT, typename W>
        std::basic_string<CharT> concat_to_string(const W& writer) {
            return writer.good() ? writer.str() : std::basic_string<CharT>();
        }

        template <typename CharT, char head, char... tail>
        std::basic_string<CharT> get_separator() { return {head, tail...}; }

        template <typename W, typename S>
        void separate(W& writer, const S* separator) {
            if (separator) writer << separator;
        }

        template <typename W, typename S>
        void separate(W& writer, const S& separator) {
            writer << separator;
        }

        template <typename CharT, typename W>
        void open_sequence(W& writer, const delimiter_t<CharT>& del) {
            writer << del.left;
        }

        template <typename CharT, typename W>
        void close_sequence(W& writer, const delimiter_t<CharT>& del) {
            writer << del.right;
        }

        template <typename CharT, typename W, typename S>
        void open_sequence(W& writer, const S& del) {}

        template <typename CharT, typename W, typename S>
        void close_sequence(W& writer, const S& del) {}

        template <typename CharT, typename T, typename W, typename S>
        void concat_impl_write_separator(W& writer, const S& separator) {
            if (!is_manipulator<CharT, T>::value) separate(writer, separator);
        }

        template <typename CharT, typename W, typename S, typename... Args>
        void concat_impl_write_element(W&, const S&, const std::tuple<Args...>&);

        template <typename CharT, typename W, typename S, typename P1, typename P2>
        void concat_impl_write_element(W&, const S&, const std::pair<P1, P2>&);

        // we have 6 base cases, depending of the parameter type:
        // 1. base case any type compatible with << that doesn't require a special handling
        template <typename CharT, typename W, typename S, typename T>
            enable_if_t<!is_iterable<T>::value && !is_stringstream<T>::value,
        void> concat_impl_write_element(W& writer, const S&, const T& element) {
            writer << element;
        }

        // 2. base case for fundamental built-in string types (const CharT* family, a.k.a. cstrings)
        template <typename CharT, typename W, typename S, typename T>
            enable_if_t<is_char_sequence<T*>::value,
        void> concat_impl_write_element(W& writer, const S&, const T* element) {
            if (element) writer << element;
        }

        // 3. base case for std::stringstream types
        template <typename CharT, typename W, typename S, typename T>
            enable_if_t<is_stringstream<T>::value,
        void> concat_impl_write_element(W& writer, const S&, const T& element) {
            if (element.good()) writer << concat_to_string<CharT>(element);
            else writer.setstate(element.rdstate());
        }

        // 4. base case for containers, arrays, and any iterable type EXCEPT the standard string types
        template <typename CharT, typename W, typename S, typename T>
            enable_if_t<is_iterable<T>::value,
        void> concat_impl_write_element(W& writer, const S& separator, const T& container) {
            open_sequence<CharT>(writer, separator);
            auto it = std::begin(container), et = std::end(container);
            while(it != et) {
                concat_impl_write_element<CharT>(writer, separator, *it);
                if (++it != et) concat_impl_write_separator<CharT, T>(writer, separator);
            }
            close_sequence<CharT>(writer, separator);
        }

        // 5. base case for std::tuples
        template<unsigned N, unsigned Last>
        struct tuple_printer {
            template<typename CharT, typename W, typename S, typename T>
            static void print(W& writer, const S& separator, const T& tuple) {
                concat_impl_write_element<CharT>(writer, separator, std::get<N>(tuple));
                concat_impl_write_separator<CharT, T>(writer, separator);
                tuple_printer<N + 1, Last>::template print<CharT>(writer, separator, tuple);
            }
        };

        template<unsigned N>
        struct tuple_printer<N, N> {
            template<typename CharT, typename W, typename S, typename T>
            static void print(W& writer, const S& separator, const T& tuple) {
                concat_impl_write_element<CharT>(writer, separator, std::get<N>(tuple));
            }
        };

        template <typename CharT, typename W, typename S, typename... Args>
        inline void concat_impl_write_element(W& writer, const S& separator, const std::tuple<Args...>& tuple) {
            open_sequence<CharT>(writer, separator);
            tuple_printer<0, sizeof...(Args) - 1>::template print<CharT>(writer, separator, tuple);
            close_sequence<CharT>(writer, separator);
        }

        // 6. base case for std::pairs
        template <typename CharT, typename W, typename S, typename P1, typename P2>
        inline void concat_impl_write_element(W& writer, const S& separator, const std::pair<P1, P2>& pair) {
            concat_impl_write_element<CharT>(writer, separator, std::make_tuple(pair.first, pair.second));
        }

        // the following function is the recursive step that unpacks all the variadic parameters
        template <typename CharT, typename W, typename S, typename T, typename... Args>
        void concat_impl_write_element(W& writer, const S& separator, const T& head, const Args&... tail) {
            concat_impl_write_element<CharT>(writer, separator, head);
            concat_impl_write_separator<CharT, T>(writer, separator);
            concat_impl_write_element<CharT>(writer, separator, tail...);
        }

        // rearranges the parameters in order to prepare the recursive calls
        template <typename CharT, typename S, typename T, typename... Args,
            typename = enable_if_t<is_writable_stream<T, CharT>::value, T>>
        std::basic_string<CharT> concat_impl(const S& separator, T& writer, const Args&... seq) {
            concat_impl_write_element<CharT>(writer, separator, seq...);
            return concat_to_string<CharT>(writer);
        }

        // when the first parameter is not a stringstream non-const reference, this defines the writer stream
        template <typename CharT, typename S, typename... Args>
        std::basic_string<CharT> concat_impl(const S& separator, const Args&... seq) {
            std::basic_ostringstream<CharT> writer;
            return concat_impl<CharT>(separator, writer, seq...);
        }
    }

    // the 3 entry points:
    // 1. entry point,  when received a separator as first element
    template <typename CharT = char, typename F, typename... Args>
        enable_if_t<is_separator<F>::value,
    std::basic_string<CharT>> concat(const F& sep, Args&&... seq) {
        return concat_impl<CharT>(
            sep,
            std::forward<Args>(seq)...
        );
    }

    // 2. entry point,  when the separator es specified via templated char-pack arguments
    template <char head, char... tail, typename F, typename... Args,
        typename = enable_if_t<!is_separator<F>::value, F>>
    std::basic_string<char> concat(F&& first, Args&&... rest) {
        return concat_impl<char>(
            get_separator<char, head, tail...>(),
            std::forward<F>(first),
            std::forward<Args>(rest)...
        );
    }

    // 3. entry point,  when there is no separator.
    template <typename CharT = char, typename F, typename... Args,
        typename = enable_if_t<!is_separator<F>::value, F>>
    std::basic_string<CharT> concat(F&& first, Args&&... rest) {
        return concat_impl<CharT>(
            (const CharT*)nullptr,
            std::forward<F>(first),
            std::forward<Args>(rest)...
        );
    }
}

#endif
