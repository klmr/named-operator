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

// Tests for <wheels/enums.h++>

#include <wheels/enums.h++>

#include <wheels/unit_test.h++>

#include <type_traits>

namespace {
    enum class foo : unsigned {
        b0000 = 0,
        b0001 = 1,
        b0010 = 2,
        b0011 = 3,
        b0100 = 4,
        b0101 = 5,
        b0110 = 6,
        b0111 = 7,
        b1000 = 8,
        b1001 = 9,
        b1010 = 10,
        b1011 = 11,
        b1100 = 12,
        b1101 = 13,
        b1110 = 14,
        b1111 = 15,
        n0011 = ~3u
    };

    enum class bar : int {
        n1 = -1,
        p0 = 0,
        p1 = 1,
        p2 = 2,
        p3 = 3,
        p4 = 4,
        p5 = 5,
        p6 = 6,
        p7 = 7,
        p8 = 8,
        p9 = 9
    };
} // namespace

namespace wheels {
    template <> struct is_flags<foo> : std::true_type {};
    template <> struct is_arithmetic<bar> : std::true_type {};
} // namespace wheels

TEST_CASE("enums/flags", "Flags enum operators") {
    SECTION("or", "Bitwise OR") {
        foo r = foo::b0011 | foo::b0101;
        CHECK(r == foo::b0111);
    }
    SECTION("or-assign", "Bitwise OR assignment") {
        foo r = foo::b0011;
        r |= foo::b0101;
        CHECK(r == foo::b0111);
    }
    SECTION("and", "Bitwise AND") {
        foo r = foo::b0011 & foo::b0101;
        CHECK(r == foo::b0001);
    }
    SECTION("and-assign", "Bitwise AND assignment") {
        foo r = foo::b0011;
        r &= foo::b0101;
        CHECK(r == foo::b0001);
    }
    SECTION("xor", "Bitwise XOR") {
        foo r = foo::b0011 ^ foo::b0101;
        CHECK(r == foo::b0110);
    }
    SECTION("xor-assign", "Bitwise XOR assignment") {
        foo r = foo::b0011;
        r ^= foo::b0101;
        CHECK(r == foo::b0110);
    }
    SECTION("compl", "Bitwise complement") {
        foo r = ~foo::b0011;
        CHECK(r == foo::n0011);
    }
    SECTION("has_flag", "Flag test") {
        foo r = foo::b0011;
        CHECK(wheels::has_flag(r, foo::b0010));
        CHECK(wheels::has_flag(r, foo::b0001));
        CHECK_FALSE(wheels::has_flag(r, foo::b1000));
        CHECK_FALSE(wheels::has_flag(r, foo::b0100));
    }
    SECTION("set_flag", "Flag setting") {
        foo r = foo::b0011;

        SECTION("clear", "Setting a flag that is not set") {
            foo r1 = wheels::set_flag(r, foo::b0100);
            CHECK(r1 == foo::b0111);
        }

        SECTION("set", "Setting a flag that is already set") {
            foo r1 = wheels::set_flag(r, foo::b0010);
            CHECK(r1 == foo::b0011);
        }

        CHECK(r == foo::b0011);
    }
    SECTION("clear_flag", "Flag clearing") {
        foo r = foo::b0011;

        SECTION("set", "Clearing a flag that is set") {
            foo r1 = wheels::clear_flag(r, foo::b0010);
            CHECK(r1 == foo::b0001);
        }

        SECTION("clear", "Clearing a flag that is already cleared") {
            foo r1 = wheels::clear_flag(r, foo::b0100);
            CHECK(r1 == foo::b0011);
        }

        CHECK(r == foo::b0011);
    }
    SECTION("toggle_flag", "Flag toggling") {
        foo r = foo::b0011;

        SECTION("set", "Toggling a flag that is set") {
            foo r1 = wheels::toggle_flag(r, foo::b0010);
            CHECK(r1 == foo::b0001);
        }

        SECTION("clear", "Toggling a flag that is not set") {
            foo r1 = wheels::toggle_flag(r, foo::b0100);
            CHECK(r1 == foo::b0111);
        }

        CHECK(r == foo::b0011);
    }
    SECTION("constexpr", "constant expression tests") {
        STATIC_ASSERT((foo::b0010 | foo::b0001) == foo::b0011);
        STATIC_ASSERT((foo::b0110 & foo::b0011) == foo::b0010);
        STATIC_ASSERT((foo::b0110 ^ foo::b0011) == foo::b0101);
        STATIC_ASSERT(~foo::b0011 == foo::n0011);
        STATIC_ASSERT(wheels::set_flag(foo::b0011, foo::b0100) == foo::b0111);
        STATIC_ASSERT(wheels::clear_flag(foo::b0011, foo::b0001) == foo::b0010);
        STATIC_ASSERT(wheels::toggle_flag(foo::b0011, foo::b0110) == foo::b0101);
    }
}

TEST_CASE("enums/arithmetic", "Arithmetic enum operators") {
    SECTION("plus", "Addition") {
        bar r = bar::p3 + bar::p5;
        CHECK(r == bar::p8);
    }
    SECTION("plus-assign", "Addition assignment") {
        bar r = bar::p3;
        r += bar::p5;
        CHECK(r == bar::p8);
    }
    SECTION("minus", "Subtraction") {
        bar r = bar::p7 - bar::p2;
        CHECK(r == bar::p5);
    }
    SECTION("minus-assign", "Subtraction assignment") {
        bar r = bar::p7;
        r -= bar::p2;
        CHECK(r == bar::p5);
    }
    SECTION("times", "Multiplication") {
        bar r = bar::p2 * bar::p4;
        CHECK(r == bar::p8);
    }
    SECTION("times-assign", "Multiplication assignment") {
        bar r = bar::p2;
        r *= bar::p4;
        CHECK(r == bar::p8);
    }
    SECTION("divide", "Division") {
        bar r = bar::p7 / bar::p3;
        CHECK(r == bar::p2);
    }
    SECTION("divide-assign", "Division assignment") {
        bar r = bar::p7;
        r /= bar::p3;
        CHECK(r == bar::p2);
    }
    SECTION("modulus", "Modulus") {
        bar r = bar::p7 % bar::p3;
        CHECK(r == bar::p1);
    }
    SECTION("modulus-assign", "Modulus assignment") {
        bar r = bar::p7;
        r %= bar::p3;
        CHECK(r == bar::p1);
    }
    SECTION("unary-plus", "Unary plus") {
        bar r = +bar::p6;
        CHECK(r == bar::p6);
    }
    SECTION("unary-minus", "Unary minus") {
        bar r = -bar::p1;
        CHECK(r == bar::n1);
    }
    SECTION("pre-inc", "Pre-increment") {
        bar r = bar::p1;
        bar s = ++r;
        CHECK(s == bar::p2);
        CHECK(r == bar::p2);
    }
    SECTION("pre-dec", "Pre-decrement") {
        bar r = bar::p2;
        bar s = --r;
        CHECK(s == bar::p1);
        CHECK(r == bar::p1);
    }
    SECTION("post-inc", "Post-increment") {
        bar r = bar::p1;
        bar s = r++;
        CHECK(s == bar::p1);
        CHECK(r == bar::p2);
    }
    SECTION("post-dec", "Post-decrement") {
        bar r = bar::p2;
        bar s = r--;
        CHECK(s == bar::p2);
        CHECK(r == bar::p1);
    }
    SECTION("constexpr", "constant expression tests") {
        STATIC_ASSERT(bar::p1 + bar::p2 == bar::p3);
        STATIC_ASSERT(bar::p3 - bar::p1 == bar::p2);
        STATIC_ASSERT(bar::p2 * bar::p3 == bar::p6);
        STATIC_ASSERT(bar::p7 / bar::p2 == bar::p3);
        STATIC_ASSERT(bar::p7 % bar::p2 == bar::p1);
        STATIC_ASSERT(+bar::p2 == bar::p2);
        STATIC_ASSERT(-bar::p1 == bar::n1);
    }
}

