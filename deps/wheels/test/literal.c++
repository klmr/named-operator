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

// Tests for wheels user-defined literals

#include <wheels/chrono.h++>
#include <wheels/literal/string.h++>

#include <wheels/unit_test.h++>

TEST_CASE("literal/chrono", "duration literals") {
    using namespace wheels::literal::chrono;

    auto nanos = 42_ns;
    auto micros = 42_mus;
    auto millis = 42_ms;
    auto secs = 42_s;
    auto mins = 42_min;
    auto hours = 42_hr;

    CHECK(std::chrono::nanoseconds(nanos).count() == 42LL);
    CHECK(std::chrono::nanoseconds(micros).count() == 42LL * 1000);
    CHECK(std::chrono::nanoseconds(millis).count() == 42LL * 1000 * 1000);
    CHECK(std::chrono::nanoseconds(secs).count() == 42LL * 1000 * 1000 * 1000);
    CHECK(std::chrono::nanoseconds(mins).count() == 42LL * 1000 * 1000 * 1000 * 60);
    CHECK(std::chrono::nanoseconds(hours).count() == 42LL * 1000 * 1000 * 1000 * 60 * 60);
}

TEST_CASE("literal/string", "std::string literals") {
    using namespace wheels::literal::string;

    auto test = "Hello world!"_s;
    auto wtest = L"Hello world!"_s;
    auto u8test = u8"Hello, 世界"_s;
    auto u16test = u"Hello, 世界"_s;
    auto u32test = U"Hello, 世界"_s;

    STATIC_ASSERT(std::is_same<std::string, decltype(test)>::value);
    STATIC_ASSERT(std::is_same<std::wstring, decltype(wtest)>::value);
    STATIC_ASSERT(std::is_same<std::string, decltype(u8test)>::value);
    STATIC_ASSERT(std::is_same<std::u16string, decltype(u16test)>::value);
    STATIC_ASSERT(std::is_same<std::u32string, decltype(u32test)>::value);

    CHECK(test == "Hello world!");
    CHECK(test == "Hello world!");
    CHECK(u8test == u8"Hello, 世界");
    CHECK(u16test == u"Hello, 世界");
    CHECK(u32test == U"Hello, 世界");
}

