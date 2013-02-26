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

// Tests for <wheels/stopwatch.h++>

#include <wheels/stopwatch.h++>
#include <wheels/chrono.h++>

#include <wheels/unit_test.h++>

namespace {
    struct test_clock {
    public:
        using duration = std::chrono::nanoseconds;
        using rep = duration::rep;
        using period = duration::period;
        using time_point = std::chrono::time_point<test_clock, duration>;

        static constexpr bool is_steady = true;

        static time_point now() {
            return current();
        }

        static void elapse(duration d) {
            current() += d;
        }

    private:
        static time_point& current() {
            static time_point value {};
            return value;
        }
    };
} // namespace

TEST_CASE("chrono/stopwatch", "Stopwatch tests") {
    using test_stopwatch = wheels::basic_stopwatch<test_clock>;
    using namespace wheels::literal::chrono;

    test_stopwatch sw;
    CHECK_FALSE(sw.is_running());
    CHECK(sw.elapsed() == 0_ns);

    sw.start();
    CHECK(sw.is_running());
    CHECK(sw.elapsed() == 0_ns);

    test_clock::elapse(10_ns);
    CHECK(sw.is_running());
    CHECK(sw.elapsed() == 10_ns);

    test_clock::elapse(10_ns);
    CHECK(sw.is_running());
    CHECK(sw.elapsed() == 20_ns);

    sw.restart();
    CHECK(sw.is_running());
    CHECK(sw.elapsed() == 0_ns);

    test_clock::elapse(10_ns);
    CHECK(sw.is_running());
    CHECK(sw.elapsed() == 10_ns);

    sw.reset();
    CHECK_FALSE(sw.is_running());
    CHECK(sw.elapsed() == 0_ns);

    sw.start();
    CHECK(sw.is_running());
    CHECK(sw.elapsed() == 0_ns);

    test_clock::elapse(10_ns);
    CHECK(sw.is_running());
    CHECK(sw.elapsed() == 10_ns);

    auto l0 = sw.lap();
    CHECK(sw.elapsed() == 10_ns);
    CHECK(l0 == 10_ns);

    test_clock::elapse(20_ns);
    auto l1 = sw.lap();
    CHECK(sw.elapsed() == 30_ns);
    CHECK(l1 == 20_ns);

    test_clock::elapse(30_ns);
    auto l2 = sw.lap();
    CHECK(sw.elapsed() == 60_ns);
    CHECK(l2 == 30_ns);

    sw.stop();
    CHECK_FALSE(sw.is_running());
    CHECK(sw.elapsed() == 60_ns);

    auto sw2 = wheels::stopwatch::start_new();
    CHECK(sw2.is_running());
}

TEST_CASE("chrono/timing", "Time execution tests") {
    using namespace wheels::literal::chrono;

    auto elapsed = wheels::time_execution<test_clock>([] {
        test_clock::elapse(10_ns);
    });
    CHECK(elapsed == 10_ns);
}

