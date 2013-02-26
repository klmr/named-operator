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

// Tests for <wheels/tuple.h++>

#include <wheels/tuple.h++>

#include <wheels/unit_test.h++>

#include <array>

namespace {
    template <std::size_t Size, std::size_t Align>
    using Layout = wheels::AlignedStorage<Size, Align>;

    using type1 = Layout<1,1>;
    using type2 = Layout<2,2>;
    using type4 = Layout<4,4>;
    using type8 = Layout<8,8>;

    struct empty0 {};
    struct empty1 {};
    struct empty2 {};

    template <typename T>
    struct movable {
        T value = 0;
        movable(T value) : value(value) {}
        operator T() const { return value; }

        template <typename U>
        movable(movable<U> const& that) : value(that.value) {}
        template <typename U>
        movable(movable<U>&& that) : value(that.value) { that.value = 0; }

        movable() = default;
        movable(movable const&) = default;
        movable(movable&& that) : value(that.value) { that.value = 0; }
        movable& operator=(movable that) {
            std::swap(value, that.value);
            return *this;
        }
    };
} // namespace

template <int I>
using Int = std::integral_constant<int, I>;

TEST_CASE("tuple", "Optimal layout tuple test") {
    SECTION("ebco", "empty base class optimization") {
        STATIC_ASSERT(sizeof(wheels::tuple<empty0, empty1, empty2, type8>) == sizeof(type8));
        STATIC_ASSERT(sizeof(wheels::tuple<type8, empty0, empty1, empty2>) == sizeof(type8));
        STATIC_ASSERT(sizeof(wheels::tuple<type8, empty0, empty1, empty2, type8>) == 2*sizeof(type8));
    }
    SECTION("layout", "optimal layout") {
        STATIC_ASSERT(sizeof(wheels::tuple<empty0, empty1, empty2, type8, type4, type2, type1, type1>)
                      == sizeof(type8) + sizeof(type4) + sizeof(type2) + 2*sizeof(type1));
        STATIC_ASSERT(sizeof(wheels::tuple<type1, type2, empty0, empty1, type1, type8, empty2, type4>)
                      == sizeof(type8) + sizeof(type4) + sizeof(type2) + 2*sizeof(type1));
    }
    SECTION("get", "getting elements") {
        using std::get;
        wheels::tuple<int, double, long> tuple { 1, 2.0, 3L };
        STATIC_ASSERT(std::is_same<int&, decltype(get<0>(tuple))>::value);
        STATIC_ASSERT(std::is_same<double&, decltype(get<1>(tuple))>::value);
        STATIC_ASSERT(std::is_same<long&, decltype(get<2>(tuple))>::value);
        CHECK(get<0>(tuple) == 1);
        CHECK(get<1>(tuple) == 2.0);
        CHECK(get<2>(tuple) == 3L);
    }
    SECTION("convert", "convert from another tuple") {
        using std::get;
        wheels::tuple<movable<short>, movable<float>, movable<int>> source { short(1), 2.0f, 3 };

        wheels::tuple<movable<int>, movable<double>, movable<long>> copy { source };
        CHECK(get<0>(source) == short(1));
        CHECK(get<1>(source) == 2.0f);
        CHECK(get<2>(source) == 3);
        CHECK(get<0>(copy) == 1);
        CHECK(get<1>(copy) == 2.0);
        CHECK(get<2>(copy) == 3L);

        wheels::tuple<movable<int>, movable<double>, movable<long>> move { std::move(source) };
        CHECK(get<0>(source) == short(0));
        CHECK(get<1>(source) == 0.0f);
        CHECK(get<2>(source) == 0);
        CHECK(get<0>(move) == 1);
        CHECK(get<1>(move) == 2.0);
        CHECK(get<2>(move) == 3L);
    }
    SECTION("convert=", "assign-convert from another tuple") {
        using std::get;
        wheels::tuple<movable<short>, movable<float>, movable<int>> source { short(1), 2.0f, 3 };

        wheels::tuple<movable<int>, movable<double>, movable<long>> copy;
        copy = source;
        CHECK(get<0>(source) == short(1));
        CHECK(get<1>(source) == 2.0f);
        CHECK(get<2>(source) == 3);
        CHECK(get<0>(copy) == 1);
        CHECK(get<1>(copy) == 2.0);
        CHECK(get<2>(copy) == 3L);

        wheels::tuple<movable<int>, movable<double>, movable<long>> move;
        move = std::move(source);
        CHECK(get<0>(source) == short(0));
        CHECK(get<1>(source) == 0.0f);
        CHECK(get<2>(source) == 0);
        CHECK(get<0>(move) == 1);
        CHECK(get<1>(move) == 2.0);
        CHECK(get<2>(move) == 3L);
    }
    SECTION("factory", "tuple factories") {
        SECTION("make_tuple values", "make_tuple with all values") {
            auto make = wheels::make_tuple(1, 2.0, 3L);
            STATIC_ASSERT(std::is_same<decltype(make), wheels::tuple<int, double, long>>::value);
        }

        SECTION("make_tuple ref", "make_tuple with reference_wrappers") {
            double d = 2.0;
            long l = 3L;
            auto make_ref = wheels::make_tuple(1, std::ref(d), std::cref(l));
            STATIC_ASSERT(std::is_same<decltype(make_ref), wheels::tuple<int, double&, long const&>>::value);
        }

        SECTION("forward_as_tuple", "forward_as_tuple") {
            int i = 1;
            double d = 2.0;
            long const l = 3L;
            auto forward = wheels::forward_as_tuple(std::move(i), d, l);
            STATIC_ASSERT(std::is_same<decltype(forward), wheels::tuple<int&&, double&, long const&>>::value);
        }

        SECTION("tie", "tie") {
            int i = 1;
            double d = 2.0;
            long l = 3L;
            auto tie = wheels::tie(i, d, l);
            STATIC_ASSERT(std::is_same<decltype(tie), wheels::tuple<int&, double&, long&>>::value);
            wheels::tie(i, std::ignore, l) = std::make_tuple(23, 100.0, 17L);
            CHECK(i == 23);
            CHECK(d == 2.0);
            CHECK(l == 17L);
        }
    }
    SECTION("traits", "tuple trait specializations") {
        using tuple = wheels::tuple<int, empty0, double, long>;
        STATIC_ASSERT(std::tuple_size<tuple>::value == 4);
        STATIC_ASSERT(std::is_same<wheels::TupleElement<0, tuple>, int>::value);
        STATIC_ASSERT(std::is_same<wheels::TupleElement<1, tuple>, empty0>::value);
        STATIC_ASSERT(std::is_same<wheels::TupleElement<2, tuple>, double>::value);
        STATIC_ASSERT(std::is_same<wheels::TupleElement<3, tuple>, long>::value);
    }
    SECTION("tuple_cat", "tuple concatenation") {
        using std::get;

        auto t1 = wheels::make_tuple(1, 2.0, 3L);
        auto t2 = std::make_tuple(4, 5.0, 6L);
        auto t3 = wheels::make_tuple(short(7), 8.0f, 9);
        auto p = std::make_pair(10LL, 'a');
        std::array<int, 3> a { { 12, 13, 14 } };

        auto cat = wheels::tuple_cat(t1, t2, t3, p, a);
        STATIC_ASSERT(std::is_same<decltype(cat),
            wheels::tuple<int, double, long, int, double, long, short, float, int, long long, char, int, int, int>
        >::value);
        CHECK(get<0>(cat) == 1);
        CHECK(get<1>(cat) == 2.0);
        CHECK(get<2>(cat) == 3L);
        CHECK(get<3>(cat) == 4);
        CHECK(get<4>(cat) == 5.0);
        CHECK(get<5>(cat) == 6L);
        CHECK(get<6>(cat) == short(7));
        CHECK(get<7>(cat) == 8.0f);
        CHECK(get<8>(cat) == 9);
        CHECK(get<9>(cat) == 10LL);
        CHECK(get<10>(cat) == 'a');
        CHECK(get<11>(cat) == 12);
        CHECK(get<12>(cat) == 13);
        CHECK(get<13>(cat) == 14);
    }
    SECTION("std::pair", "interop with std::pair") {
        SECTION("ctor", "construction from std::pair") {
            using std::get;
            std::pair<movable<short>, movable<float>> source { short(1), 2.0f };

            wheels::tuple<movable<int>, movable<double>> copy { source };
            CHECK(get<0>(source) == short(1));
            CHECK(get<1>(source) == 2.0f);
            CHECK(get<0>(copy) == 1);
            CHECK(get<1>(copy) == 2.0);

            wheels::tuple<movable<int>, movable<double>> move { std::move(source) };
            CHECK(get<0>(source) == short(0));
            CHECK(get<1>(source) == 0.0f);
            CHECK(get<0>(move) == 1);
            CHECK(get<1>(move) == 2.0);
        }
        SECTION("assign", "assignment of std::pair") {
            using std::get;
            std::pair<movable<short>, movable<float>> source { short(1), 2.0f };

            wheels::tuple<movable<int>, movable<double>> copy;
            copy = source;
            CHECK(get<0>(source) == short(1));
            CHECK(get<1>(source) == 2.0f);
            CHECK(get<0>(copy) == 1);
            CHECK(get<1>(copy) == 2.0);

            wheels::tuple<movable<int>, movable<double>> move;
            move = std::move(source);
            CHECK(get<0>(source) == short(0));
            CHECK(get<1>(source) == 0.0f);
            CHECK(get<0>(move) == 1);
            CHECK(get<1>(move) == 2.0);
        }
    }
    SECTION("std::tuple", "interop with std::tuple") {
        SECTION("ctor", "construction from std::tuple") {
            using std::get;
            std::tuple<movable<short>, movable<float>, movable<int>> source { short(1), 2.0f, 3 };

            wheels::tuple<movable<int>, movable<double>, movable<long>> copy { source };
            CHECK(get<0>(source) == short(1));
            CHECK(get<1>(source) == 2.0f);
            CHECK(get<2>(source) == 3);
            CHECK(get<0>(copy) == 1);
            CHECK(get<1>(copy) == 2.0);
            CHECK(get<2>(copy) == 3L);

            wheels::tuple<movable<int>, movable<double>, movable<long>> move { std::move(source) };
            CHECK(get<0>(source) == short(0));
            CHECK(get<1>(source) == 0.0f);
            CHECK(get<2>(source) == 0);
            CHECK(get<0>(move) == 1);
            CHECK(get<1>(move) == 2.0);
            CHECK(get<2>(copy) == 3L);
        }
        SECTION("assign", "assignment of std::tuple") {
            using std::get;
            std::tuple<movable<short>, movable<float>, movable<int>> source { short(1), 2.0f, 3 };

            wheels::tuple<movable<int>, movable<double>, movable<long>> copy;
            copy = source;
            CHECK(get<0>(source) == short(1));
            CHECK(get<1>(source) == 2.0f);
            CHECK(get<2>(source) == 3);
            CHECK(get<0>(copy) == 1);
            CHECK(get<1>(copy) == 2.0);
            CHECK(get<2>(copy) == 3L);

            wheels::tuple<movable<int>, movable<double>, movable<long>> move;
            move = std::move(source);
            CHECK(get<0>(source) == short(0));
            CHECK(get<1>(source) == 0.0f);
            CHECK(get<2>(source) == 0);
            CHECK(get<0>(move) == 1);
            CHECK(get<1>(move) == 2.0);
            CHECK(get<2>(copy) == 3L);
        }
    }
}

// For GCC bug report. TODO: actually report it.
#if 0
template <typename... T>
struct c {
    template <typename... U,
              typename = typename std::enable_if<std::__and_<std::is_convertible<U, T>...>::value, int>::type>
    static void f(U...) {
        int x = typename std::enable_if<std::__and_<std::is_convertible<U, T>...>::value, int>::type {};
        (void)x;
    }
};

c<int,double,long>::f(1, 2.0, 3L);
#endif

