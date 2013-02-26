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

// Tests for <wheels/chrono.h++>

#include <wheels/chrono.h++>

#include <wheels/unit_test.h++>

#include <cstdint>

#include <ratio>

TEST_CASE("chrono/conversions", "Duration conversion shortcuts") {
    using namespace wheels::literal::chrono;

    using picoseconds = std::chrono::duration<std::intmax_t, std::pico>;

    CHECK(wheels::to_nanoseconds(picoseconds(42500)) == 42);
    CHECK(wheels::to_nanoseconds<wheels::FractionalDuration>(picoseconds(42500)) == 42.5);
    CHECK(wheels::to_nanoseconds(42.5_mus) == 42500);
    CHECK(wheels::to_nanoseconds<wheels::FractionalDuration>(42.5005_mus) == 42500.5);

    CHECK(wheels::to_microseconds(42500_ns) == 42);
    CHECK(wheels::to_microseconds<wheels::FractionalDuration>(42500_ns) == 42.5);
    CHECK(wheels::to_microseconds(42.5_ms) == 42500);
    CHECK(wheels::to_microseconds<wheels::FractionalDuration>(42.5005_ms) == 42500.5);

    CHECK(wheels::to_milliseconds(42500_mus) == 42);
    CHECK(wheels::to_milliseconds<wheels::FractionalDuration>(42500_mus) == 42.5);
    CHECK(wheels::to_milliseconds(42.5_s) == 42500);
    CHECK(wheels::to_milliseconds<wheels::FractionalDuration>(42.5005_s) == 42500.5);

    CHECK(wheels::to_seconds(42500_ms) == 42);
    CHECK(wheels::to_seconds<wheels::FractionalDuration>(42500_ms) == 42.5);
    CHECK(wheels::to_seconds(70.5_min) == 4230);
    CHECK(wheels::to_seconds<wheels::FractionalDuration>(70.125_min) == 4207.5);

    CHECK(wheels::to_minutes(4230_s) == 70);
    CHECK(wheels::to_minutes<wheels::FractionalDuration>(4230_s) == 70.5);
    CHECK(wheels::to_minutes(70.5_hr) == 4230);
    CHECK(wheels::to_minutes<wheels::FractionalDuration>(70.125_hr) == 4207.5);

    using tens_of_hours = std::chrono::duration<long double, std::ratio<36000>>;

    CHECK(wheels::to_hours(4230_min) == 70);
    CHECK(wheels::to_hours<wheels::FractionalDuration>(4230_min) == 70.5);
    CHECK(wheels::to_hours(tens_of_hours(42.5)) == 425);
    CHECK(wheels::to_hours<wheels::FractionalDuration>(tens_of_hours(42.25)) == 422.5);
}

