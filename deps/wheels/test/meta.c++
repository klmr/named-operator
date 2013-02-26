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

// Tests for <wheels/meta.h++>

#include <wheels/meta.h++>

#include <wheels/unit_test.h++>

#include <type_traits>

TEST_CASE("meta/not", "wheels::Not metafunction") {
    STATIC_ASSERT(!wheels::Not<std::is_pointer<int*>>::value);
    STATIC_ASSERT(wheels::Not<std::is_pointer<int&>>::value);
}

TEST_CASE("meta/any", "wheels::Any metafunction") {
    STATIC_ASSERT(wheels::Any<std::is_pointer<int&>, std::is_pointer<int*>, std::is_pointer<int**>>::value);
    STATIC_ASSERT(wheels::Any<std::is_pointer<int&>, std::is_pointer<int*>, std::is_pointer<int>>::value);
    STATIC_ASSERT(!wheels::Any<std::is_pointer<int&>, std::is_pointer<int[10]>, std::is_pointer<int>>::value);
}

TEST_CASE("meta/all", "wheels::All metafunction") {
    STATIC_ASSERT(wheels::All<std::is_pointer<int*>, std::is_pointer<int**>, std::is_pointer<int***>>::value);
    STATIC_ASSERT(!wheels::All<std::is_pointer<int&>, std::is_pointer<int*>, std::is_pointer<int>>::value);
    STATIC_ASSERT(!wheels::All<std::is_pointer<int&>, std::is_pointer<int[10]>, std::is_pointer<int>>::value);
}

TEST_CASE("meta/min", "wheels::min metafunction") {
    using wheels::Int;
    STATIC_ASSERT(wheels::min<Int<10>>::value == 10);
    STATIC_ASSERT(wheels::min<Int<20>,Int<10>,Int<40>,Int<30>>::value == 10);
    STATIC_ASSERT(wheels::min<Int<20>,Int<10>,Int<40>,Int<40>,Int<10>,Int<30>>::value == 10);
}

TEST_CASE("meta/max", "wheels::max metafunction") {
    using wheels::Int;
    STATIC_ASSERT(wheels::max<Int<40>>::value == 40);
    STATIC_ASSERT(wheels::max<Int<20>,Int<10>,Int<40>,Int<30>>::value == 40);
    STATIC_ASSERT(wheels::max<Int<20>,Int<10>,Int<40>,Int<40>,Int<10>,Int<30>>::value == 40);
}

template <typename T, typename U, wheels::EnableIf<std::is_pointer<T>, std::is_pointer<U>> = wheels::_>
std::true_type f(T, U);

template <typename T, typename U, wheels::DisableIf<std::is_pointer<T>, std::is_pointer<U>> = wheels::_>
std::false_type f(T, U);

TEST_CASE("meta/enable-if", "wheels::EnableIf metafunction") {
    STATIC_ASSERT(!decltype(f(0, 0))::value);
    STATIC_ASSERT(!decltype(f(nullptr, nullptr))::value);
    STATIC_ASSERT(!decltype(f((int*)nullptr, nullptr))::value);
    STATIC_ASSERT(!decltype(f(nullptr, (void*)nullptr))::value);
    STATIC_ASSERT(decltype(f((int*)nullptr, (void*)nullptr))::value);
}

