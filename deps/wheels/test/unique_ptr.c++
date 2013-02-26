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

// Tests for <wheels/smart_ptr/unique_ptr.h++>

#include <wheels/smart_ptr/unique_ptr.h++>

#include <wheels/unit_test.h++>

#include <algorithm>
#include <new>
#include <numeric>
#include <string>

using wheels::tracer;

namespace {
    struct allocated;
    struct deallocated;

    template <typename T>
    class test_allocator : private tracer<allocated>, private tracer<deallocated> {
    public:
        using value_type = T;
        test_allocator() = default;
        template <typename U>
        test_allocator(test_allocator<U> const&) {}

        T* allocate(std::size_t n) {
            ++tracer<allocated>::called();
            return static_cast<T*>(::operator new(n*sizeof(T)));
        }
        void deallocate(T* ptr, std::size_t) {
            ++tracer<deallocated>::called();
            ::operator delete(ptr);
        }
    };

    class test_deleter {
    public:
        void operator()(void*) {}
        int tag;
    };
} // namespace

TEST_CASE("smart_ptr/make_unique", "unique_ptr factory tests") {
    struct foo;
    static foo* instance;
    struct foo {
        foo(int x, bool y, std::string s) {
            CHECK(!instance);
            CHECK(x == 42);
            CHECK(y == true);
            CHECK(s == "foo");
            instance = this;
        }
    };
    instance = nullptr;
    wheels::reset_tracers<allocated, deallocated>();

    SECTION("forward", "Forward all arguments; default deleter") {
        auto ptr = wheels::make_unique<foo>(42, true, "foo");
        STATIC_ASSERT(std::is_same<decltype(ptr), std::unique_ptr<foo>>::value);
        CHECK(&*ptr == instance);
    }
    SECTION("allocate", "allocate_unique") {
        test_allocator<foo> a;
        CHECK(tracer<allocated>::called() == 0);
        {
            auto ptr = wheels::allocate_unique<foo>(a, 42, true, "foo");
            STATIC_ASSERT(std::is_same<decltype(ptr), std::unique_ptr<foo, wheels::allocator_delete<foo, test_allocator<foo>>>>::value);
            CHECK(tracer<allocated>::called() == 1);
        }
        CHECK(tracer<deallocated>::called() == 1);
    }
    SECTION("array", "Array version") {
        const int size = 10;
        auto expected { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto p0 = wheels::make_unique<int[size]>();
        STATIC_ASSERT(std::is_same<decltype(p0), std::unique_ptr<int[]>>::value);
        std::iota(p0.get(), p0.get() + size, 0);

        CHECK(std::equal(expected.begin(), expected.end(), p0.get()));

        auto p1 = wheels::make_unique<int[]>(size);
        STATIC_ASSERT(std::is_same<decltype(p1), std::unique_ptr<int[]>>::value);
        std::iota(p1.get(), p1.get() + size, 0);

        CHECK(std::equal(expected.begin(), expected.end(), p1.get()));
    }
    SECTION("raw", "Raw pointer") {
        auto* raw0 = new int { 42 };
        auto p0 = wheels::make_unique(wheels::raw = raw0);
        STATIC_ASSERT(std::is_same<decltype(p0), std::unique_ptr<int>>::value);
        CHECK(&*p0 == raw0);

        struct base { virtual ~base() = default; };
        struct derived : base {};
        auto* raw1 = new derived;
        auto p1 = wheels::make_unique<base>(wheels::raw = raw1);
        STATIC_ASSERT(std::is_same<decltype(p1), std::unique_ptr<base>>::value);
        CHECK(&*p1 == static_cast<base*>(raw1));
    }
    SECTION("deleter", "Value initialized; custom deleter") {
        auto ptr = wheels::make_unique<int>(wheels::deleter = test_deleter { 17 });
        STATIC_ASSERT(std::is_same<decltype(ptr), std::unique_ptr<int, test_deleter>>::value);
        CHECK(*ptr == 0);
        CHECK(ptr.get_deleter().tag == 17);
    }
    SECTION("tuple+deleter", "Forward as tuple; custom deleter") {
        auto ptr = wheels::make_unique<foo>(std::forward_as_tuple(42, true, "foo"), wheels::deleter = test_deleter { 17 });
        STATIC_ASSERT(std::is_same<decltype(ptr), std::unique_ptr<foo, test_deleter>>::value);
        CHECK(&*ptr == instance);
        CHECK(ptr.get_deleter().tag == 17);
    }
    SECTION("raw+deleter", "Raw pointer; custom deleter") {
        auto* raw = new int;
        auto ptr = wheels::make_unique<int>(wheels::raw = raw, wheels::deleter = test_deleter { 17 });
        STATIC_ASSERT(std::is_same<decltype(ptr), std::unique_ptr<int, test_deleter>>::value);
        CHECK(&*ptr == raw);
        CHECK(ptr.get_deleter().tag == 17);
    }
}

