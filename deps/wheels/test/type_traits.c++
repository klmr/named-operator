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

// Tests for <wheels/type_traits.h++>

#include <wheels/meta/type_traits.h++>

#include <wheels/unit_test.h++>

TEST_CASE("traits/Bare", "wheels::Bare type transformation") {
    STATIC_ASSERT(std::is_same<wheels::Bare<int>, int>::value);
    STATIC_ASSERT(std::is_same<wheels::Bare<int const>, int>::value);
    STATIC_ASSERT(std::is_same<wheels::Bare<int volatile>, int>::value);
    STATIC_ASSERT(std::is_same<wheels::Bare<int const volatile>, int>::value);

    STATIC_ASSERT(std::is_same<wheels::Bare<int&>, int>::value);
    STATIC_ASSERT(std::is_same<wheels::Bare<int const&>, int>::value);
    STATIC_ASSERT(std::is_same<wheels::Bare<int volatile&>, int>::value);
    STATIC_ASSERT(std::is_same<wheels::Bare<int const volatile&>, int>::value);

    STATIC_ASSERT(std::is_same<wheels::Bare<int&&>, int>::value);
    STATIC_ASSERT(std::is_same<wheels::Bare<int const&&>, int>::value);
    STATIC_ASSERT(std::is_same<wheels::Bare<int volatile&&>, int>::value);
    STATIC_ASSERT(std::is_same<wheels::Bare<int const volatile&&>, int>::value);
}

TEST_CASE("traits/WithConstOf", "wheels::WithConstOf type transformation") {
    STATIC_ASSERT(std::is_same<wheels::WithConstOf<int, double>, double>::value);
    STATIC_ASSERT(std::is_same<wheels::WithConstOf<int, double const>, double const>::value);

    STATIC_ASSERT(std::is_same<wheels::WithConstOf<int const, double>, double const>::value);
    STATIC_ASSERT(std::is_same<wheels::WithConstOf<int const, double const>, double const>::value);
}

TEST_CASE("traits/WithVolatileOf", "wheels::WithVolatileOf type transformation") {
    STATIC_ASSERT(std::is_same<wheels::WithVolatileOf<int, double>, double>::value);
    STATIC_ASSERT(std::is_same<wheels::WithVolatileOf<int, double volatile>, double volatile>::value);

    STATIC_ASSERT(std::is_same<wheels::WithVolatileOf<int volatile, double>, double volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithVolatileOf<int volatile, double volatile>, double volatile>::value);
}

TEST_CASE("traits/WithCvOf", "wheels::WithCvOf type transformation") {
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int, double>, double>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int, double const>, double const>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int, double volatile>, double volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int, double const volatile>, double const volatile>::value);

    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int const, double>, double const>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int const, double const>, double const>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int const, double volatile>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int const, double const volatile>, double const volatile>::value);

    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int volatile, double>, double volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int volatile, double const>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int volatile, double volatile>, double volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int volatile, double const volatile>, double const volatile>::value);

    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int const volatile, double>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int const volatile, double const>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int const volatile, double volatile>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithCvOf<int const volatile, double const volatile>, double const volatile>::value);
}

TEST_CASE("traits/WithValueCategoryOf", "wheels::WithValueCategoryOf type transformation") {
    STATIC_ASSERT(std::is_same<wheels::WithValueCategoryOf<int, double>, double>::value);
    STATIC_ASSERT(std::is_same<wheels::WithValueCategoryOf<int, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithValueCategoryOf<int, double&&>, double&&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithValueCategoryOf<int&, double>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithValueCategoryOf<int&, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithValueCategoryOf<int&, double&&>, double&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithValueCategoryOf<int&&, double>, double&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithValueCategoryOf<int&&, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithValueCategoryOf<int&&, double&&>, double&&>::value);
}

TEST_CASE("traits/WithQualificationsOf", "wheels::WithQualificationsOf type transformation") {
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double>, double>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double const>, double const>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double volatile>, double volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double const volatile>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double&&>, double&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double const&&>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double volatile&&>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int, double const volatile&&>, double const volatile&&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double>, double const>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double const>, double const>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double volatile>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double const volatile>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double&&>, double&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double const&&>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double volatile&&>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const, double const volatile&&>, double const volatile&&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double>, double volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double const>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double volatile>, double volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double const volatile>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double&&>, double&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double const&&>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double volatile&&>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile, double const volatile&&>, double const volatile&&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double const>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double volatile>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double const volatile>, double const volatile>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double&&>, double&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double const&&>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double volatile&&>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile, double const volatile&&>, double const volatile&&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double const>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double volatile>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double const volatile>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double&&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double const&&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double volatile&&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&, double const volatile&&>, double const volatile&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double const>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double volatile>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double const volatile>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double&&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double const&&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double volatile&&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&, double const volatile&&>, double const volatile&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double const>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double volatile>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double const volatile>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double&&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double const&&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double volatile&&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&, double const volatile&&>, double const volatile&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double const>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double volatile>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double const volatile>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double&&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double const&&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double volatile&&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&, double const volatile&&>, double const volatile&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double>, double&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double const>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double volatile>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double const volatile>, double const volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double&&>, double&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double const&&>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double volatile&&>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int&&, double const volatile&&>, double const volatile&&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double const>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double volatile>, double const volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double const volatile>, double const volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double&&>, double&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double const&&>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double volatile&&>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const&&, double const volatile&&>, double const volatile&&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double const>, double const volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double volatile>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double const volatile>, double const volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double&&>, double&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double const&&>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double volatile&&>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int volatile&&, double const volatile&&>, double const volatile&&>::value);

    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double>, double const volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double const>, double const volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double volatile>, double const volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double const volatile>, double const volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double&>, double&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double const&>, double const&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double volatile&>, double volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double const volatile&>, double const volatile&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double&&>, double&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double const&&>, double const&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double volatile&&>, double volatile&&>::value);
    STATIC_ASSERT(std::is_same<wheels::WithQualificationsOf<int const volatile&&, double const volatile&&>, double const volatile&&>::value);
}

TEST_CASE("traits/related", "wheels::is_related trait") {
    STATIC_ASSERT(wheels::is_related<int, int const volatile&>::value);
    STATIC_ASSERT(wheels::is_related<int const, int>::value);
    STATIC_ASSERT(wheels::is_related<int volatile, int const volatile&>::value);
    STATIC_ASSERT(wheels::is_related<int const volatile, int>::value);
    STATIC_ASSERT(wheels::is_related<int&, int>::value);
    STATIC_ASSERT(wheels::is_related<int const&, int>::value);
    STATIC_ASSERT(wheels::is_related<int volatile&, int const volatile&>::value);
    STATIC_ASSERT(wheels::is_related<int const volatile&, int>::value);
    STATIC_ASSERT(wheels::is_related<int&&, int>::value);
    STATIC_ASSERT(wheels::is_related<int const&&, int>::value);
    STATIC_ASSERT(wheels::is_related<int volatile&&, int>::value);
    STATIC_ASSERT(wheels::is_related<int const volatile&&, int const volatile&>::value);

    STATIC_ASSERT(!wheels::is_related<int, double const volatile&>::value);
    STATIC_ASSERT(!wheels::is_related<int const, double>::value);
    STATIC_ASSERT(!wheels::is_related<int volatile, double const volatile&>::value);
    STATIC_ASSERT(!wheels::is_related<int const volatile, double>::value);
    STATIC_ASSERT(!wheels::is_related<int&, double>::value);
    STATIC_ASSERT(!wheels::is_related<int const&, double>::value);
    STATIC_ASSERT(!wheels::is_related<int volatile&, double const volatile&>::value);
    STATIC_ASSERT(!wheels::is_related<int const volatile&, double>::value);
    STATIC_ASSERT(!wheels::is_related<int&&, double>::value);
    STATIC_ASSERT(!wheels::is_related<int const&&, double>::value);
    STATIC_ASSERT(!wheels::is_related<int volatile&&, double>::value);
    STATIC_ASSERT(!wheels::is_related<int const volatile&&, double const volatile&>::value);
}

struct foo {};
struct bar { using value_type = foo; };

TEST_CASE("traits/ValueType", "wheels::ValueType trait") {
    STATIC_ASSERT(std::is_same<wheels::ValueType<bar>, foo>::value);
}

template <typename... T>
struct qux {};

template <typename... T>
struct fred {};

TEST_CASE("traits/is_specialization", "wheels::is_specialization trait") {
    STATIC_ASSERT(wheels::is_specialization_of<qux<int>, qux>::value);
    STATIC_ASSERT(wheels::is_specialization_of<fred<int, double>, fred>::value);
    STATIC_ASSERT(!wheels::is_specialization_of<fred<int>, qux>::value);
    STATIC_ASSERT(!wheels::is_specialization_of<int, qux>::value);
}

struct alignas(16) non_pod { virtual ~non_pod(); };

TEST_CASE("traits/StorageFor", "wheels::StorageFor type transformation") {
    STATIC_ASSERT(sizeof(wheels::StorageFor<int>) >= sizeof(int));
    STATIC_ASSERT(alignof(wheels::StorageFor<int>) >= alignof(int));
    STATIC_ASSERT(std::is_pod<wheels::StorageFor<int>>::value);

    STATIC_ASSERT(sizeof(wheels::StorageFor<int, double, non_pod>) >= sizeof(int)
               && sizeof(wheels::StorageFor<int, double, non_pod>) >= sizeof(double)
               && sizeof(wheels::StorageFor<int, double, non_pod>) >= sizeof(non_pod));
    STATIC_ASSERT(alignof(wheels::StorageFor<int, double, non_pod>) >= alignof(int)
               && alignof(wheels::StorageFor<int, double, non_pod>) >= alignof(double)
               && alignof(wheels::StorageFor<int, double, non_pod>) >= alignof(non_pod));
    STATIC_ASSERT(std::is_pod<wheels::StorageFor<int, double, non_pod>>::value);
}

TEST_CASE("traits/is_callable", "wheels::is_callable trait") {
    STATIC_ASSERT(!wheels::is_callable<int, void()>::value);

    auto functor = [](int) { };
    STATIC_ASSERT(wheels::is_callable<decltype(functor), void(int)>::value);
    STATIC_ASSERT(!wheels::is_callable<decltype(functor), void()>::value);
    STATIC_ASSERT(wheels::is_callable<decltype(functor), void(long)>::value);
    STATIC_ASSERT(!wheels::is_callable<decltype(functor), double(int)>::value);

    struct local {
        int* operator()() {
            return nullptr;
        }

        int operator()() const {
            return 0;
        }
    };

    STATIC_ASSERT(wheels::is_callable<local, void()>::value);
    STATIC_ASSERT(wheels::is_callable<local, int*()>::value);
    STATIC_ASSERT(wheels::is_callable<local, void*()>::value);
    STATIC_ASSERT(!wheels::is_callable<local, int()>::value);

    STATIC_ASSERT(wheels::is_callable<const local, void()>::value);
    STATIC_ASSERT(!wheels::is_callable<const local, int*()>::value);
    STATIC_ASSERT(wheels::is_callable<const local, int()>::value);
    STATIC_ASSERT(wheels::is_callable<const local, long()>::value);

    using pmo = int local::*;
    STATIC_ASSERT(wheels::is_callable<pmo, int&(local&)>::value);
    STATIC_ASSERT(wheels::is_callable<pmo, int const&(local const&)>::value);
    STATIC_ASSERT(wheels::is_callable<pmo, int&&(local&&)>::value);
    STATIC_ASSERT(wheels::is_callable<pmo, int const&&(local const&&)>::value);
    STATIC_ASSERT(wheels::is_callable<pmo, double(local&)>::value);
    STATIC_ASSERT(!wheels::is_callable<pmo, int&(local&, int)>::value);

    using pmf = float (local::*)(double);
    using pcmf = int* (local::*)() const;
    STATIC_ASSERT(wheels::is_callable<pmf, float(local&, double)>::value);
    STATIC_ASSERT(wheels::is_callable<pmf, float(local*, double)>::value);
    STATIC_ASSERT(wheels::is_callable<pmf, double(local&, double)>::value);
    STATIC_ASSERT(wheels::is_callable<pmf, double(local*, double)>::value);

    STATIC_ASSERT(wheels::is_callable<pcmf, int* (local&)>::value);
    STATIC_ASSERT(wheels::is_callable<pcmf, int* (local*)>::value);
    STATIC_ASSERT(wheels::is_callable<pcmf, void* (local&)>::value);
    STATIC_ASSERT(wheels::is_callable<pcmf, void* (local*)>::value);
    STATIC_ASSERT(wheels::is_callable<pcmf, int* (local const&)>::value);
    STATIC_ASSERT(wheels::is_callable<pcmf, int* (local const*)>::value);
    STATIC_ASSERT(wheels::is_callable<pcmf, void* (local const&)>::value);
    STATIC_ASSERT(wheels::is_callable<pcmf, void* (local const*)>::value);
}

TEST_CASE("traits/is_function_object", "wheels::is_function_object trait") {
    using function = void(double);
    auto lambda = [](int) { };
    using lambda_t = decltype(lambda);
    struct local { int operator()(void*); };
    using pmo = int local::*;
    using pmf = float (local::*)(double);

    STATIC_ASSERT(!wheels::is_function_object<int, void()>::value);
    STATIC_ASSERT(!wheels::is_function_object<function, void(double)>::value);
    STATIC_ASSERT(wheels::is_function_object<function*, void(double)>::value);
    STATIC_ASSERT(!wheels::is_function_object<function&, void(double)>::value);
    STATIC_ASSERT(wheels::is_function_object<lambda_t, void(int)>::value);
    STATIC_ASSERT(!wheels::is_function_object<lambda_t*, void(int)>::value);
    STATIC_ASSERT(!wheels::is_function_object<lambda_t&, void(int)>::value);
    STATIC_ASSERT(wheels::is_function_object<local, int(void*)>::value);
    STATIC_ASSERT(!wheels::is_function_object<local const, int(void*)>::value);
    STATIC_ASSERT(!wheels::is_function_object<pmo, int&(local&)>::value);
    STATIC_ASSERT(!wheels::is_function_object<pmo, int&(local*)>::value);
    STATIC_ASSERT(!wheels::is_function_object<pmf, float(local&, double)>::value);
    STATIC_ASSERT(!wheels::is_function_object<pmf, float(local*, double)>::value);
}

TEST_CASE("invoke", "INVOKE semantics") {
    struct local {
        int i;
    } l;
    auto functor = &local::i;
    CHECK(&wheels::invoke(functor, l) == &l.i);
}

namespace {
    template <typename T, typename = void>
    struct result_of_sfinae : wheels::Bool<false> {};

    template <typename Fun, typename... Args>
    struct result_of_sfinae<Fun(Args...),
                            wheels::Void<wheels::ResultOf<Fun(Args...)>>>
    : wheels::Bool<true> {};
} // namespace
TEST_CASE("result_of", "wheels::result_of trait") {
    struct funobj {
        double operator()(int) const { return 0.0; }
        float operator()(int, int) const { return 0.0f; }
    };
#if 0
    STATIC_ASSERT(!result_of_sfinae_tester<funobj()>::value);
    STATIC_ASSERT(result_of_sfinae_tester<funobj(int)>::value);
    STATIC_ASSERT(result_of_sfinae_tester<funobj(int, int)>::value);
#endif
    STATIC_ASSERT(std::is_same<wheels::ResultOf<funobj(int)>, double>::value);
    STATIC_ASSERT(std::is_same<wheels::ResultOf<funobj(int, int)>, float>::value);
}

