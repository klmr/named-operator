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

// Tests for <wheels/scope.h++>

#include <wheels/scope.h++>

#include <wheels/unit_test.h++>

TEST_CASE("scope", "scope guard tests") {
    SECTION("finally", "scope guard that always runs") {
        int called { 0 };
        {
            auto guard = wheels::finally([&]{ ++called; });
            CHECK(called == 0);
        }
        CHECK(called == 1);
    }
}

