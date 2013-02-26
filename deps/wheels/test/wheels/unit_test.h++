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

// Unit testing tools

#ifndef WHEELS_UNIT_TEST_HPP
#define WHEELS_UNIT_TEST_HPP

#include <wheels/expand.h++>

#include <catch.h++>

#define STATIC_ASSERT(...) do { static_assert(__VA_ARGS__, #__VA_ARGS__); CHECK(true); } while(false)

#if WHEELS_HAS_ISSUE(BROKEN_CONCEPT_CHECK)
    #define CONCEPT_CHECK(...)
#else
    #define CONCEPT_CHECK(...) do { using namespace wheels::concepts; ::wheels::concepts::check<__VA_ARGS__>(); CHECK(true); } while(false)
#endif

#define UNUSED_VARIABLE(NAME) do { (void)reinterpret_cast<char&>(NAME); } while(false)

namespace wheels {
    //! A simple tracer to be used with CRTP
    template <typename T>
    struct tracer {
        tracer() noexcept {
            ++created();
        }

        tracer(tracer const&) noexcept {
            ++created();
            ++copied();
        }
        tracer(tracer&&) noexcept {
            ++created();
            ++moved();
        }
        tracer& operator=(tracer const&) noexcept {
            ++copy_assigned();
            return *this;
        }
        tracer& operator=(tracer&&) noexcept {
            ++move_assigned();
            return *this;
        }

        ~tracer() noexcept {
            ++destroyed();
        }

        static void reset() noexcept {
            created() = 0;
            copied() = 0;
            moved() = 0;
            copy_assigned() = 0;
            move_assigned() = 0;
            cloned() = 0;
            destroyed() = 0;
            called() = 0;
        }

        static int& created() noexcept { static int value = 0; return value; }
        static int& copied() noexcept { static int value = 0; return value; }
        static int& moved() noexcept { static int value = 0; return value; }
        static int& copy_assigned() noexcept { static int value = 0; return value; }
        static int& move_assigned() noexcept { static int value = 0; return value; }
        static int& cloned() noexcept { static int value = 0; return value; }
        static int& destroyed() noexcept { static int value = 0; return value; }
        static int& called() noexcept { static int value = 0; return value; }
    };

    //! Reset all the tracers
    template <typename... T>
    void reset_tracers() {
        WHEELS_EXPAND_SIDE_EFFECTS(tracer<T>::reset());
    }
} // namespace wheels

#endif // WHEELS_UNIT_TEST_HPP

