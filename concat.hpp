#ifndef THEYPSILON_CONCAT
#define THEYPSILON_CONCAT

#include <sstream>
#include <deque>

namespace theypsilon {

    struct separator {
        const char* sep;
        constexpr explicit separator(const char* s) noexcept: sep{s} {} 

        const static char none [];
        const static char space[];
        const static char endl [];
        const static char coma [];
        const static char plus [];
    };

    const char separator::none [] = "";
    const char separator::space[] = " ";
    const char separator::endl [] = "\n";
    const char separator::coma [] = ", ";
    const char separator::plus [] = " + ";

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

        template<typename T> 
        struct is_container : std::integral_constant<bool, 
            has_const_iterator<T>::value && 
            has_begin_end<T>::beg_value  && 
            has_begin_end<T>::end_value  &&
            !std::is_same<T, std::string>::value> {};

        template<typename T> 
        struct is_writable_stream : std::integral_constant<bool, 
            std::is_same<T, std::ostringstream>::value ||
            std::is_same<T, std::stringstream>::value> {};

        template<typename T> 
        struct is_stream : std::integral_constant<bool, 
            std::is_same<T, std::istringstream>::value ||
            is_writable_stream<T>::value> {};

        template<typename T> 
        using char_sequence_type = typename std::remove_pointer<typename std::decay<T>::type>::type;

        template<typename T> 
        struct is_char_sequence : std::integral_constant<bool, 
            std::is_same<char_sequence_type<T>, char    >::value || 
            std::is_same<char_sequence_type<T>, wchar_t >::value || 
            std::is_same<char_sequence_type<T>, char16_t>::value || 
            std::is_same<char_sequence_type<T> ,char32_t>::value> {};

        template<typename T> 
        struct can_be_false : std::integral_constant<bool, 
            is_stream<T>::value || 
            is_char_sequence<T>::value> {};

      template<typename T> 
        struct is_basic_type : std::integral_constant<bool, 
            std::is_scalar<T>::value ||
            std::is_same<T,std::string>::value> {};
    }

    namespace { // concat_intern

        template <typename W>
        std::string concat_to_string(const W& writter) {
            return writter ? writter.str() : "";
        }

        template <char head, char... tail>
        std::string get_separator() { return {head, tail...}; }

        namespace {

            template <typename T,
                typename std::enable_if<!can_be_false<T>::value && !is_container<T>::value, T>::type* = nullptr>
            void must_write(std::deque<bool>& b, const T& v) {
                b.push_back(std::is_scalar<T>::value ||
                            std::is_same<T,std::string>::value);
            }

            template <typename T,
                typename std::enable_if<can_be_false<T>::value && !is_container<T>::value, T>::type* = nullptr>
            void must_write(std::deque<bool>& b, const T& v) {
                b.push_back(((bool)v) == true);
            }

            template <typename T,
                typename std::enable_if<!can_be_false<T>::value && is_container<T>::value, T>::type* = nullptr>
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

            template <typename W, typename S, typename T>
            void do_base_write(W& writter, const S& separator, std::deque<bool>& b, const T& v);

            template <typename W, typename S, typename T,
                typename std::enable_if<!is_container<T>::value && !can_be_false<T>::value && !is_basic_type<T>::value, T>::type* = nullptr>
            void pre_write(W& writter, const S& separator, std::deque<bool>& b, const T& v) {
                writter << v;
            }

            template <typename W, typename S, typename T,
                typename std::enable_if<is_container<T>::value || can_be_false<T>::value || is_basic_type<T>::value, T>::type* = nullptr>
            void pre_write(W& writter, const S& separator, std::deque<bool>& b, const T& v) {
            }

            template <typename W, typename S, typename T,
                typename std::enable_if<!is_container<T>::value && !is_stream<T>::value && is_char_sequence<T>::value, T>::type* = nullptr>
            void do_write(W& writter, const S& separator, std::deque<bool>& b, const T& v) {
                if (v) writter << v;
            }

            template <typename W, typename S, typename T,
                typename std::enable_if<!is_container<T>::value && !is_stream<T>::value && is_char_sequence<T>::value, T>::type* = nullptr>
            void do_write(W& writter, const S& separator, std::deque<bool>& b, const T* v) {
                if (v) writter << v;
            }

            template <typename W, typename S, typename T,
                typename std::enable_if<!is_container<T>::value && !is_stream<T>::value && !is_char_sequence<T>::value, T>::type* = nullptr>
            void do_write(W& writter, const S& separator, std::deque<bool>& b, const T& v) {
                writter << v;
            }

            template <typename W, typename S, typename T,
                typename std::enable_if<!is_container<T>::value && is_stream<T>::value && !is_char_sequence<T>::value, T>::type* = nullptr>
            void do_write(W& writter, const S& separator, std::deque<bool>& b, const T& v) {
                if (v) writter << v.str();
            }

            template <typename W, typename S, typename T,
                typename std::enable_if<is_container<T>::value && !is_stream<T>::value && !is_char_sequence<T>::value, T>::type* = nullptr>
            void do_write(W& writter, const S& separator, std::deque<bool>&, const T& container) {
                std::deque<bool> b;
                for (const auto& element: container)
                    must_write(b, element);
                for (const auto& element: container) {
                    do_base_write(writter, separator, b, element);
                }
            }

            template <typename W, typename S, typename T, typename... Args>
            void do_write(W& writter, const S& separator, std::deque<bool>& b, const T& head, const Args&... tail) {
                do_base_write(writter, separator, b, head);
                do_write(writter, separator, b, tail...);
            }

            template <typename W, typename S, typename T>
            inline void do_base_write(W& writter, const S& separator, std::deque<bool>& b, const T& v) {
                pre_write(writter, separator, b, v);
                bool must = b.front();
                b.pop_front();
                if (must) {
                    do_write(writter, separator, b, v);
                    if (any_writable(b)) writter << separator;
                }
            }

            template <typename S, typename T, typename... Args,
                typename std::enable_if<is_writable_stream<T>::value == true, T>::type* = nullptr>
            std::string concat_intern(const S& separator, T& writter, const Args&... seq) {
                auto b = get_write_deque(seq...);
                do_write(writter, separator, b, seq...);
                return concat_to_string(writter);
            }
        }

        namespace unused {

            template <typename W, typename S, typename T,
                typename std::enable_if<!is_container<T>::value && !is_stream<T>::value, T>::type* = nullptr>
            bool concat_intern_recursion(W& writter, const S& separator, const T& head) {
                writter << head;
                return std::is_scalar<T>::value ||
                       std::is_same<T,std::string>::value;
            }

            template <typename W, typename S>
            bool concat_intern_recursion(W& writter, const S& separator, const char* head) {
                if (!head) return false;
                writter << head;
                return true;
            }

            template <typename W, typename S, typename T, 
                typename std::enable_if<is_stream<T>::value == true, T>::type* = nullptr>
            bool concat_intern_recursion(W& writter, const S& separator, const T& head) {
                if (!head) return false;
                writter << head.str();
                return true;
            }

            template <typename W, typename S, typename T,
                typename std::enable_if<is_container<T>::value == true, T>::type* = nullptr>
            bool concat_intern_recursion(W& writter, const S& separator, const T& seq) {
                bool writting = false;
                auto it = std::begin(seq), et = std::end(seq);
                while (it != et) {
                    bool b = concat_intern_recursion(writter, separator, *it);
                    ++it;
                    if (!b) continue;
                    writting = true;
                    if (it != et) writter << separator;
                }
                return writting;
            }

            template <typename W, typename S, typename T, typename... Args>
            bool concat_intern_recursion(W& writter, const S& separator, const T& head, const Args&... tail) {            
                if (concat_intern_recursion(writter, separator, head)) 
                    writter << separator;
                return concat_intern_recursion(writter, separator, tail...);
            }

            template <typename S, typename T, typename... Args,
                typename std::enable_if<is_writable_stream<T>::value == true, T>::type* = nullptr>
            std::string concat_intern(const S& separator, T& writter, const Args&... seq) {
                concat_intern_recursion(writter, separator, seq...);
                return concat_to_string(writter);
            }
        }

        template <typename S, typename... Args>
        std::string concat_intern(const S& separator, const Args&... seq) {
            std::ostringstream writter;
            return concat_intern(separator, writter, seq...);
        }
    }

    template <typename... Args>
    std::string concat(const separator& sep, Args&&... seq) {
        return concat_intern(sep.sep, std::forward<Args>(seq)...);
    }

    template <char head, char... tail, typename F, typename... Args,
        typename = typename std::enable_if<std::is_same<F, separator>::value == false, F>::type>
    std::string concat(F&& first, Args&&... rest) {
        return concat_intern(get_separator<head, tail...>(), std::forward<F>(first), std::forward<Args>(rest)...);
    }

    template <const char* sep = separator::none, typename F, typename... Args,
        typename = typename std::enable_if<std::is_same<F, separator>::value == false, F>::type>
    std::string concat(F&& first, Args&&... rest) {
        return concat_intern(sep, std::forward<F>(first), std::forward<Args>(rest)...);
    }

    template <std::ostream& (*sep) (std::ostream&), typename F, typename... Args,
        typename = typename std::enable_if<std::is_same<F, separator>::value == false, F>::type>
    std::string concat(F&& first, Args&&... rest) {
        return concat_intern(sep, std::forward<F>(first), std::forward<Args>(rest)...);
    }

}

#endif