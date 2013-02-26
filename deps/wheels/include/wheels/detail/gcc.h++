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

// GCC feature detection and workarounds

#ifndef WHEELS_DETAIL_GCC_HPP
#define WHEELS_DETAIL_GCC_HPP

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

// GCC feature set has to be hardcoded per version
#if GCC_VERSION >= 40700
    #define WHEELS_COMPILER_FEATURES 0x3FFBFBFE6
#else
    #error "GCC >=4.7 is required"
#endif

#define WHEELS_UNREACHABLE do { __builtin_unreachable(); } while(false)

#define WHEELS_WARNING(MESSAGE) \
    WHEELS_PRAGMA(message #MESSAGE) \
    WHEELS_EXPECT_SEMICOLON

// alignas emulation
namespace wheels {
    namespace emulation_detail {
        using size_t = decltype(sizeof(0));

        constexpr size_t max(size_t n) { return n; }

        template <typename... Tail>
        constexpr size_t max(size_t a, size_t b, Tail... tail) {
            return max(a > b? a : b, tail...);
        }

        template <typename... T>
        constexpr size_t max_alignment() {
            return max_alignment<alignof(T)...>();
        }

        template <size_t... N>
        constexpr size_t max_alignment() {
            return max(N...);
        }
    }
}
#define alignas(...) \
    __attribute__((__aligned__(::wheels::emulation_detail::max_alignment<__VA_ARGS__>())))
#define WHEELS_HAS_FEATURE_CXX_ALIGNAS

#endif // WHEELS_DETAIL_GCC_HPP

