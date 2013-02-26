// Wheels - various C++ utilities
//
// Written in 2012 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

// std::chrono utilities

#ifndef WHEELS_CHRONO_HPP
#define WHEELS_CHRONO_HPP

#include "meta.h++"

#include <chrono>

namespace wheels {
    //! Makes a duration type with the same period but a fractional representation
    template <typename Duration>
    using FractionalDuration = std::chrono::duration<long double, typename Duration::period>;

#define WHEELS_DURATION_CONVERSION(UNIT) \
    template <template <typename> class Modifier = ::wheels::Identity, typename Duration> \
    typename Modifier< ::std::chrono::UNIT>::rep to_##UNIT(Duration d) { \
        return ::std::chrono::duration_cast<Modifier< ::std::chrono::UNIT>>(d).count(); \
    } \
    WHEELS_EXPECT_SEMICOLON

    WHEELS_DURATION_CONVERSION(nanoseconds);
    WHEELS_DURATION_CONVERSION(microseconds);
    WHEELS_DURATION_CONVERSION(milliseconds);
    WHEELS_DURATION_CONVERSION(seconds);
    WHEELS_DURATION_CONVERSION(minutes);
    WHEELS_DURATION_CONVERSION(hours);

#undef WHEELS_DURATION_CONVERSION

    namespace literal {
        namespace chrono {
#if WHEELS_HAS_ISSUE(DURATION_NO_CONSTEXPR)
    #define WHEELS_DURATION_CONSTEXPR inline
#else
    #define WHEELS_DURATION_CONSTEXPR constexpr
#endif
#define WHEELS_DURATION_LITERAL(ID, TYPE) \
            WHEELS_DURATION_CONSTEXPR ::std::chrono::TYPE operator"" _##ID(unsigned long long value) { \
                return ::std::chrono::TYPE(value); \
            } \
            WHEELS_DURATION_CONSTEXPR ::wheels::FractionalDuration< ::std::chrono::TYPE> operator"" _##ID(long double value) { \
                return ::wheels::FractionalDuration< ::std::chrono::TYPE>(value); \
            } \
            WHEELS_EXPECT_SEMICOLON

            WHEELS_DURATION_LITERAL(ns, nanoseconds);
            WHEELS_DURATION_LITERAL(mus, microseconds);
            WHEELS_DURATION_LITERAL(ms, milliseconds);
            WHEELS_DURATION_LITERAL(s, seconds);
            WHEELS_DURATION_LITERAL(min, minutes);
            WHEELS_DURATION_LITERAL(hr, hours);

#undef WHEELS_DURATION_CONSTEXPR
#undef WHEELS_DURATION_LITERAL
        } // namespace chrono
    } // namespace literal
} // namespace wheels

#endif // WHEELS_CHRONO_HPP

