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

// Tests for <wheels/concurrency/locker_box.h++>

#include <wheels/concurrency/locker_box.h++>

#if WHEELS_HAS_FEATURE(CXX_THREADS)

#include <wheels/unit_test.h++>
#include <wheels/with.h++>

#include <cassert>

namespace {
    struct test_mutex {
        void lock() {
            assert(!locked);
            locked = true;
        }
        void unlock() {
            assert(locked);
            locked = false;
        }

        bool locked = false;
    };
} // namespace

TEST_CASE("concurrency/locker_box", "locker_box tests") {
    using test_box = wheels::locker_box<int, test_mutex>;

    test_box box { 0 };

    SECTION("loop", "range-based loop") {
        WITH(auto&& x : box.open()) {
            CHECK(box.mutex().locked);
            CHECK(x == 0);
            x = 42;
            break;
            CHECK(false);
        }
        CHECK_FALSE(box.mutex().locked);

        const auto& const_box = box;
        WITH(auto&& x : const_box.open()) {
            CHECK(box.mutex().locked);
            CHECK(x == 42);
        }
        CHECK_FALSE(box.mutex().locked);
    }

    SECTION("apply", "apply function") {
        SECTION("noret", "noreturn") {
            apply([&](int& x) {
                CHECK(box.mutex().locked);
                CHECK(x == 0);
                x = 42;
            }, box);
            CHECK_FALSE(box.mutex().locked);
            auto const& const_box = box;
            apply([&](int const& x) {
                CHECK(box.mutex().locked);
                CHECK(x == 42);
            }, const_box);
            CHECK_FALSE(box.mutex().locked);
        }

        SECTION("ret", "with return value") {
            auto r = apply([&](int& x) -> int {
                CHECK(box.mutex().locked);
                CHECK(x == 0);
                return 42;
            }, box);
            CHECK_FALSE(box.mutex().locked);
            CHECK(r == 42);
        }

        SECTION("exception", "exceptional exit") {
            try {
                apply([&](int& x) {
                    CHECK(box.mutex().locked);
                    CHECK(x == 0);
                    throw -1;
                }, box);
            } catch(...) {
                CHECK_FALSE(box.mutex().locked);
            }
        }
    }
}

#endif // WHEELS_HAS_FEATURE(CXX_THREADS)

