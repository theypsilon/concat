#ifndef THEYPSILON_CONCAT
#define THEYPSILON_CONCAT

#include <sstream>

namespace theypsilon {

    namespace separator {
        constexpr char none [] = "";
        constexpr char space[] = " ";
        constexpr char endl [] = "\n";
        constexpr char coma [] = ", ";
        constexpr char plus [] = " + ";
    }

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
            has_begin_end<T>::end_value> {};

        template<typename T> 
        struct is_stream : std::integral_constant<bool, 
            std::is_same<T, std::istringstream>::value ||
            std::is_same<T, std::ostringstream>::value ||
            std::is_same<T, std::stringstream>::value> {};
    }

    namespace { // concat_intern

        template <typename W, typename S, typename T,
        typename std::enable_if<is_container<T>::value == false && is_stream<T>::value == false, T>::type* = nullptr>
        bool concat_intern_recursion(W& writter, const S& separator, const T& head) {
            writter << head;
            return true;
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
            typename T::const_iterator it = std::begin(seq), et = std::end(seq);
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
        bool concat_intern_recursion(W& writter, const S& separator, const T& head, Args&&... tail) {
            if (concat_intern_recursion(writter, separator, head)) 
                writter << separator;
            return concat_intern_recursion(writter, separator, tail...);
        }

        template <typename W>
        std::string concat_to_string(const W& writter) {
            return writter ? writter.str() : "";
        }

        template <typename S, typename... Args>
        std::string concat_intern(const S& separator, std::stringstream& writter, const Args&... seq) {
            concat_intern_recursion(writter, separator, seq...);
            return concat_to_string(writter);
        }

        template <typename S, typename... Args>
        std::string concat_intern(const S& separator, std::ostringstream& writter, const Args&... seq) {
            concat_intern_recursion(writter, separator, seq...);
            return concat_to_string(writter);
        }

        template <typename S, typename... Args>
        std::string concat_intern(const S& separator, const Args&... seq) {
            std::ostringstream writter;
            return concat_intern(separator, writter, seq...);
        }
    }

    template <char separator, typename... Args>
    std::string concat(Args&&... seq) {
        return concat_intern(separator, std::forward<Args>(seq)...);
    }

    template <const char* separator = separator::none, typename... Args>
    std::string concat(Args&&... seq) {
        return concat_intern(separator, std::forward<Args>(seq)...);
    }

    template <std::ostream& (*separator) (std::ostream&), typename... Args>
    std::string concat(Args&&... seq) {
        return concat_intern(separator, std::forward<Args>(seq)...);
    }

}

#endif