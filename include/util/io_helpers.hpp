#ifndef UTIL_IO_HELPERS_HPP
#define UTIL_IO_HELPERS_HPP

#include <iostream>
#include <vector>
#include <utility>
#include <wheels/meta.h++>

namespace util {

template <typename C>
struct is_container : std::false_type { };

template <typename T, typename A>
struct is_container<std::vector<T, A>> : std::true_type { };

} // namespace util

template <typename T>
inline std::ostream& operator <<(std::ostream& out, std::pair<T, T> const& rhs) {
    return out << '<' << rhs.first << ", " << rhs.second << '>';
}

template <typename C, wheels::EnableIf<util::is_container<C>>...>
inline std::ostream& operator <<(std::ostream& out, C const& rhs) {
    out << '[';
    bool first = true;
    for (auto&& x : rhs) {
        if (first) first = false;
        else       out << ", ";
        out << x;
    }
    return out << ']';
}

#endif // ndef UTIL_IO_HELPERS_HPP
