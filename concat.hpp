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

namespace theypsilon {

    template <typename CharT>
    struct separator_t {
        const CharT* sep;
        constexpr explicit separator_t(const CharT* s) noexcept: sep{s} {}
    };

    template <typename CharT>
    constexpr separator_t<CharT> separator(const CharT* s) {
        return separator_t<CharT>(s);
    }

    namespace sep {
        constexpr char none [] = "";
        constexpr char space[] = " ";
        constexpr char endl [] = "\n";
        constexpr char comma[] = ", ";
        constexpr char plus [] = " + ";
    };

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

        template <bool B, class T = void>
        using enable_if_t = typename std::enable_if<B, T>::type;
    }

    namespace { // concat_intern

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
        std::basic_string<CharT> concat_to_string(const W& writer) {
            return writer.good() ? writer.str() : std::basic_string<CharT>();
        }

        template <typename CharT, typename W, typename S, typename T>
        void concat_intern_write(W&, const S&, bool, const T&);

        template <typename CharT, typename W, typename S, typename T>
            enable_if_t<is_char_sequence<T*>::value,
        void> concat_intern_recursion(W& writer, const S&, const T* v) {
            if (v) writer << v;
        }

        template <typename CharT, typename W, typename S, typename T>
            enable_if_t<(!is_iterable<T>::value && !is_stringstream<T>::value &&
                         !is_char_sequence<T>::value) || is_manipulator<CharT, T>::value,
        void> concat_intern_recursion(W& writer, const S&, const T& v) {
            writer << v;
        }

        template <typename CharT, typename W, typename S, typename T>
            enable_if_t<is_stringstream<T>::value,
        void> concat_intern_recursion(W& writer, const S&, const T& v) {
            if (v.good()) writer << concat_to_string<CharT>(v);
            else writer.setstate(v.rdstate());
        }

        template <typename CharT, typename W, typename S, typename T>
            enable_if_t<is_iterable<T>::value,
        void> concat_intern_recursion(W& writer, const S& separator, const T& container) {
            auto it = std::begin(container), et = std::end(container);
            while(it != et) {
                auto element = *it;
                it++;
                concat_intern_write<CharT>(writer, separator, it != et, element);
            }
        }

        template<unsigned N, unsigned Last>
        struct tuple_printer {
            template<typename CharT, typename W, typename S, typename T>
            static void print(W& writer, const S& separator, const T& v) {
                concat_intern_write<CharT>(writer, separator, true, std::get<N>(v));
                tuple_printer<N + 1, Last>::template print<CharT>(writer, separator, v);
            }
        };

        template<unsigned N>
        struct tuple_printer<N, N> {
            template<typename CharT, typename W, typename S, typename T>
            static void print(W& writer, const S& separator, const T& v) {
                concat_intern_write<CharT>(writer, separator, false, std::get<N>(v));
            }
        };

        template <typename CharT, typename W, typename S, typename... Args>
        void concat_intern_recursion(W& writer, const S& separator, const std::tuple<Args...>& v) {
            tuple_printer<0, sizeof...(Args) - 1>::template print<CharT>(writer, separator, v);
        }

        template <typename CharT, typename W, typename S, typename P1, typename P2>
        void concat_intern_recursion(W& writer, const S& separator, const std::pair<P1, P2>& v) {
            concat_intern_write<CharT>(writer, separator,  true, v.first);
            concat_intern_write<CharT>(writer, separator, false, v.second);
        }

        template <typename CharT, typename W, typename S, typename T, typename... Args>
        void concat_intern_recursion(W& writer, const S& separator, const T& head, const Args&... tail) {
            concat_intern_write<CharT>(writer, separator, true, head);
            concat_intern_recursion<CharT>(writer, separator, tail...);
        }

        template <typename CharT, typename W, typename S, typename T>
        inline void concat_intern_write(W& writer, const S& separator, bool b, const T& v) {
            concat_intern_recursion<CharT>(writer, separator, v);
            if (b && !is_manipulator<CharT, T>::value) separate(writer, separator);
        }

        template <typename CharT, typename S, typename T, typename... Args,
            typename = enable_if_t<is_writable_stream<T, CharT>::value == true, T>>
        std::basic_string<CharT> concat_intern(const S& separator, T& writer, const Args&... seq) {
            concat_intern_recursion<CharT>(writer, separator, seq...);
            return concat_to_string<CharT>(writer);
        }

        template <typename CharT, typename S, typename... Args>
        std::basic_string<CharT> concat_intern(const S& separator, const Args&... seq) {
            std::basic_ostringstream<CharT> writer;
            return concat_intern<CharT>(separator, writer, seq...);
        }
    }

    template <typename CharT = char, typename... Args>
    std::basic_string<CharT> concat(const separator_t<CharT>& sep, Args&&... seq) {
        return concat_intern<CharT>(
            sep.sep,
            std::forward<Args>(seq)...
        );
    }

    template <char head, char... tail, typename F, typename... Args,
        typename = enable_if_t<std::is_same<F, separator_t<char>>::value == false, F>>
    std::basic_string<char> concat(F&& first, Args&&... rest) {
        return concat_intern<char>(
            get_separator<char, head, tail...>(),
            std::forward<F>(first),
            std::forward<Args>(rest)...
        );
    }

    template <const char* sep, typename F, typename... Args,
        typename = enable_if_t<std::is_same<F, separator_t<char>>::value == false, F>>
    std::basic_string<char> concat(F&& first, Args&&... rest) {
        return concat_intern<char>(
            sep,
            std::forward<F>(first),
            std::forward<Args>(rest)...
        );
    }

    template <typename CharT = char, typename F, typename... Args,
        typename = enable_if_t<std::is_same<F, separator_t<CharT>>::value == false, F>>
    std::basic_string<CharT> concat(F&& first, Args&&... rest) {
        return concat_intern<CharT>(
            (const CharT*)nullptr,
            std::forward<F>(first),
            std::forward<Args>(rest)...
        );
    }

    template <std::ostream& sep (std::ostream&), typename CharT = char, typename F, typename... Args,
        typename = enable_if_t<std::is_same<F, separator_t<CharT>>::value == false, F>>
    std::basic_string<CharT> concat(F&& first, Args&&... rest) {
        return concat_intern<CharT>(
            sep,
            std::forward<F>(first),
            std::forward<Args>(rest)...
        );
    }

}

#endif
