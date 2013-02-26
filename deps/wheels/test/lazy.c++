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

#include <wheels/lazy.h++>

#include <wheels/unit_test.h++>

#include <functional>

TEST_CASE("lazy", "Lazy/eager evaluation trait") {
    SECTION("lazy", "Lazy evaluation") {
        SECTION("nonfunction", "Non-function types") {
            bool called = false;
            auto lazy_int = [&]() -> char {
                called = true;
                return 42;
            };
            auto r = wheels::lazy<int>(lazy_int);
            CHECK(called);
            CHECK(r == 42);
        }
#if !WHEELS_HAS_ISSUE(INCOMPLETE_IS_CONVERTIBLE)
        // TODO: when will GCC fix std::is_convertible?
        SECTION("function", "Function types") {
            bool outer_called = false;
            bool inner_called = false;
            auto lazy_fun = [&]() -> std::function<char()> {
                outer_called = true;
                return [&]() -> char {
                    inner_called = true;
                    return 42;
                };
            };
            auto r = wheels::lazy<std::function<int()>>(lazy_fun);
            CHECK(outer_called);
            CHECK_FALSE(inner_called);
            auto s = r();
            CHECK(inner_called);
            CHECK(s == 42);
        }
#endif
    }
    SECTION("eager", "Eager evaluation") {
        SECTION("nonfunction", "Non-function types") {
            auto r = wheels::lazy<int>(42);
            CHECK(r == 42);
        }
        SECTION("function", "Function types") {
            bool called = false;
            auto fun = [&]() -> char {
                called = true;
                return 42;
            };
            auto r = wheels::lazy<std::function<int()>>(fun);
            CHECK_FALSE(called);
            auto s = r();
            CHECK(called);
            CHECK(s == 42);
        }
    }
}

