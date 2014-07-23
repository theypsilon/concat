/*
 *  CONCAT 
 *  Version: 2014-23-07
 *  ----------------------------------------------------------
 *  Copyright (c) 2014 José Manuel Barroso Galindo. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef THEYPSILON_CONCAT
#define THEYPSILON_CONCAT

#include <sstream>
#include <deque>

namespace $ {

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
        constexpr char coma [] = ", ";
        constexpr char plus [] = " + ";
    };

    namespace { // is_container, is_stream
        template<typename T>
        struct has_const_iterator {
        private:
            typedef char                      yes;
            typedef struct { char array[2]; } no;

            template<typename C> static yes test(typename C::const_iterator*);
            template<typename C> static no  test(...);
        public:
            static const bool value = sizeof(test<T>(0)) == sizeof(yes);
            typedef T type;
        };

        template <typename T>
        struct has_begin_end {
            template<typename C> static char (&f(typename std::enable_if<
            std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
            typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

            template<typename C> static char (&f(...))[2];

            template<typename C> static char (&g(typename std::enable_if<
            std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
            typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

            template<typename C> static char (&g(...))[2];

            static bool const beg_value = sizeof(f<T>(0)) == 1;
            static bool const end_value = sizeof(g<T>(0)) == 1;
        };

        template<typename T, typename CharT>
        constexpr bool is_writable_stream() {
            return std::is_same<T, std::basic_ostringstream<CharT>>::value || std::is_same<T, std::basic_stringstream<CharT>>::value;
        }

        template<typename T, typename CharT = char> 
        constexpr bool is_stream() {
            return std::is_same<T, std::basic_istringstream<CharT>>::value || is_writable_stream<T, CharT>();
        }

        template<typename T> 
        constexpr bool is_array() {
            return std::is_array<T>::value;
        }

        template<typename T> 
        constexpr bool is_char_type() {
            return  std::is_same<typename std::decay<T>::type, char    >::value || 
                    std::is_same<typename std::decay<T>::type, wchar_t >::value || 
                    std::is_same<typename std::decay<T>::type, char16_t>::value || 
                    std::is_same<typename std::decay<T>::type ,char32_t>::value;
        }

        template<typename T, typename CharT = char>
        constexpr bool is_c_str() { 
            return  std::is_same<typename std::decay<T>::type, CharT const *>::value ||
                    std::is_same<typename std::decay<T>::type, CharT       *>::value;
        }

        template<typename T> 
        constexpr bool is_char_sequence() {
            return  is_c_str<T,     char>() ||
                    is_c_str<T,  wchar_t>() ||
                    is_c_str<T, char16_t>() ||
                    is_c_str<T, char32_t>();
        }

        template<typename T> 
        constexpr bool is_container() {
            return (has_const_iterator<T>::value && 
                    has_begin_end<T>::beg_value  && 
                    has_begin_end<T>::end_value  &&
                    !std::is_same<T, std::string>::value &&
                    !is_stream<T>()) 
            || (std::is_array<T>::value && !is_char_sequence<T*>());
        }

        template<typename T> 
        constexpr bool can_be_false() {
            return (is_stream<T>() || is_char_sequence<T>()) && !std::is_array<T>::value;
        }

        template <typename T>
        constexpr bool is_string() {
            return  std::is_same<T,std::string   >::value ||
                    std::is_same<T,std::wstring  >::value ||
                    std::is_same<T,std::u16string>::value ||
                    std::is_same<T,std::u32string>::value;
        }

        template <typename T>
        constexpr bool is_basic_type() {
            return std::is_scalar<T>::value || is_string<T>();
        }

        template <typename T>
        constexpr bool is_modifier() {
            return  !is_container    <T>() && !is_stream    <T>() && 
                    !is_char_sequence<T>() && !is_basic_type<T>() &&
                    !std::is_array<T>::value;
        }
    }

    namespace { // concat_intern

        template <typename CharT, char head, char... tail>
        std::basic_string<CharT> get_separator() { return {head, tail...}; }

        template <typename W, typename S>
        void separate(W& writter, const S* separator) { 
            if (separator) writter << separator;
        }

        template <typename W, typename S>
        void separate(W& writter, const S& separator) { 
            writter << separator;
        }

        template <typename CharT, typename W>
        std::basic_string<CharT> concat_to_string(const W& writter) {
            return writter ? writter.str() : std::basic_string<CharT>();
        }

        namespace {

            template <typename T,
                typename std::enable_if<!can_be_false<T>() && !is_container<T>(), T>::type* = nullptr>
            void must_write(std::deque<bool>& b, const T& v) {
                b.push_back(is_basic_type<T>());
            }

            template <typename T,
                typename std::enable_if<can_be_false<T>(), T>::type* = nullptr>
            void must_write(std::deque<bool>& b, const T& v) {
                b.push_back(((bool)v) == true);
            }

            template <typename T,
                typename std::enable_if<is_container<T>(), T>::type* = nullptr>
            void must_write(std::deque<bool>& b, const T& container) {
                for(const auto& element : container) {
                    must_write(b, element);
                    if (b.back())
                        return b.push_back(true);
                }
                b.push_back(false);
            }

            template <typename T, typename... Args>
            void must_write(std::deque<bool>& b, const T& head, const Args&... tail) {
                must_write(b, head);
                must_write(b, tail...);
            }

            template <typename... Args>
            std::deque<bool> get_write_deque(const Args&... seq) {
                std::deque<bool> b;
                must_write(b, seq...);
                return b;
            }

            bool any_writable(const std::deque<bool>& b) {
                for (bool e: b) if (e) return true;
                return false;
            }

            template <typename CharT, typename W, typename S, typename T>
            void do_base_write(W& writter, const S& separator, std::deque<bool>& b, const T& v);

            template <typename CharT, typename W, typename S, typename T,
                typename std::enable_if<is_char_sequence<T*>(), T>::type* = nullptr>
            void do_write(W& writter, const S& separator, std::deque<bool>& b, const T* v) {
                if (v) writter << v;
            }

            template <typename CharT, typename W, typename S, typename T,
                typename std::enable_if<(!is_container<T>() && !is_stream<T>() && !is_char_sequence<T>()) || is_modifier<T>(), T>::type* = nullptr>
            void do_write(W& writter, const S& separator, std::deque<bool>& b, const T& v) {
                writter << v;
            }

            template <typename CharT, typename W, typename S, typename T,
                typename std::enable_if<is_stream<T>(), T>::type* = nullptr>
            void do_write(W& writter, const S& separator, std::deque<bool>& b, const T& v) {
                if (v) writter << concat_to_string<CharT>(v);
            }

            template <typename CharT, typename W, typename S, typename T,
                typename std::enable_if<is_container<T>(), T>::type* = nullptr>
            void do_write(W& writter, const S& separator, std::deque<bool>&, const T& container) {
                std::deque<bool> b;
                for (const auto& element: container)
                    must_write(b, element);
                for (const auto& element: container) {
                    do_base_write<CharT>(writter, separator, b, element);
                }
            }

            template <typename CharT, typename W, typename S, typename T, typename... Args>
            void do_write(W& writter, const S& separator, std::deque<bool>& b, const T& head, const Args&... tail) {
                do_base_write<CharT>(writter, separator, b, head);
                do_write<CharT>(writter, separator, b, tail...);
            }

            template <typename CharT, typename W, typename S, typename T>
            inline void do_base_write(W& writter, const S& separator, std::deque<bool>& b, const T& v) {
                if (is_modifier<T>())
                    do_write<CharT>(writter, separator, b, v);
                
                bool must = b.front();
                b.pop_front();
                if (!must) return;
                do_write<CharT>(writter, separator, b, v);
                if (any_writable(b)) separate(writter, separator);
            }

            template <typename CharT, typename S, typename T, typename... Args,
                typename std::enable_if<is_writable_stream<T, CharT>() == true, T>::type* = nullptr>
            std::basic_string<CharT> concat_intern(const S& separator, T& writter, const Args&... seq) {
                auto b = get_write_deque(seq...);
                do_write<CharT>(writter, separator, b, seq...);
                return concat_to_string<CharT>(writter);
            }
        }

        namespace unused {

            template <typename W, typename S, typename T,
                typename std::enable_if<!is_container<T>() && !is_stream<T>(), T>::type* = nullptr>
            bool concat_intern_recursion(W& writter, const S& separator, const T& head) {
                writter << head;
                return std::is_scalar<T>::value ||
                       std::is_same<T,std::basic_string<char>>::value;
            }

            template <typename W, typename S>
            bool concat_intern_recursion(W& writter, const S& separator, const char* head) {
                if (!head) return false;
                writter << head;
                return true;
            }

            template <typename W, typename S, typename T, 
                typename std::enable_if<is_stream<T>() == true, T>::type* = nullptr>
            bool concat_intern_recursion(W& writter, const S& separator, const T& head) {
                if (!head) return false;
                writter << head.str();
                return true;
            }

            template <typename W, typename S, typename T,
                typename std::enable_if<is_container<T>() == true, T>::type* = nullptr>
            bool concat_intern_recursion(W& writter, const S& separator, const T& seq) {
                bool writting = false;
                auto it = std::begin(seq), et = std::end(seq);
                while (it != et) {
                    bool b = concat_intern_recursion(writter, separator, *it);
                    ++it;
                    if (!b) continue;
                    writting = true;
                    if (it != et) separate(writter, separator);
                }
                return writting;
            }

            template <typename W, typename S, typename T, typename... Args>
            bool concat_intern_recursion(W& writter, const S& separator, const T& head, const Args&... tail) {            
                if (concat_intern_recursion(writter, separator, head)) 
                    separate(writter, separator);
                return concat_intern_recursion(writter, separator, tail...);
            }

            template <typename S, typename T, typename... Args,
                typename std::enable_if<is_writable_stream<T, char>() == true, T>::type* = nullptr>
            std::basic_string<char> concat_intern(const S& separator, T& writter, const Args&... seq) {
                concat_intern_recursion(writter, separator, seq...);
                return concat_to_string(writter);
            }
        }

        template <typename CharT, typename S, typename... Args>
        std::basic_string<CharT> concat_intern(const S& separator, const Args&... seq) {
            std::basic_ostringstream<CharT> writter;
            return concat_intern<CharT>(separator, writter, seq...);
        }
    }

    template <typename CharT = char, typename... Args>
    std::basic_string<CharT> concat(const separator_t<CharT>& sep, Args&&... seq) {
        return concat_intern<CharT>(sep.sep, std::forward<Args>(seq)...);
    }

    template <char head, char... tail, typename F, typename... Args,
        typename = typename std::enable_if<std::is_same<F, separator_t<char>>::value == false, F>::type>
    std::basic_string<char> concat(F&& first, Args&&... rest) {
        return concat_intern<char>(get_separator<char, head, tail...>(), std::forward<F>(first), std::forward<Args>(rest)...);
    }

    template <const char* sep, typename F, typename... Args,
        typename = typename std::enable_if<std::is_same<F, separator_t<char>>::value == false, F>::type>
    std::basic_string<char> concat(F&& first, Args&&... rest) {
        return concat_intern<char>(sep, std::forward<F>(first), std::forward<Args>(rest)...);
    }

    template <typename CharT = char, typename F, typename... Args,
        typename = typename std::enable_if<std::is_same<F, separator_t<CharT>>::value == false, F>::type>
    std::basic_string<CharT> concat(F&& first, Args&&... rest) {
        return concat_intern<CharT>((const char*)nullptr, std::forward<F>(first), std::forward<Args>(rest)...);
    }

    template <std::ostream& sep (std::ostream&), typename F, typename... Args,
        typename = typename std::enable_if<std::is_same<F, separator_t<char>>::value == false, F>::type>
    std::basic_string<char> concat(F&& first, Args&&... rest) {
        return concat_intern<char>(sep, std::forward<F>(first), std::forward<Args>(rest)...);
    }

}

#endif