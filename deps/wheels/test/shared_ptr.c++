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

// Tests for <wheels/smart_ptr/shared_ptr.h++>

#include <wheels/smart_ptr/shared_ptr.h++>

#include <wheels/unit_test.h++>

#include <algorithm>
#include <new>
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

TEST_CASE("smart_ptr/make_shared", "shared_ptr factory tests") {
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
        auto ptr = wheels::make_shared<foo>(42, true, "foo");
        STATIC_ASSERT(std::is_same<decltype(ptr), std::shared_ptr<foo>>::value);
        CHECK(&*ptr == instance);
    }
    SECTION("raw", "Raw pointer") {
        auto* raw0 = new int { 42 };
        auto p0 = wheels::make_shared(wheels::raw = raw0);
        STATIC_ASSERT(std::is_same<decltype(p0), std::shared_ptr<int>>::value);
        CHECK(&*p0 == raw0);

        struct base { virtual ~base() = default; };
        struct derived : base {};
        auto* raw1 = new derived;
        auto p1 = wheels::make_shared<base>(wheels::raw = raw1);
        STATIC_ASSERT(std::is_same<decltype(p1), std::shared_ptr<base>>::value);
        CHECK(&*p1 == static_cast<base*>(raw1));
    }
    SECTION("deleter", "Value initialized; custom deleter") {
        auto ptr = wheels::make_shared<int>(wheels::deleter = test_deleter { 17 });
        STATIC_ASSERT(std::is_same<decltype(ptr), std::shared_ptr<int>>::value);
        CHECK(*ptr == 0);
        REQUIRE(std::get_deleter<test_deleter>(ptr));
        CHECK(std::get_deleter<test_deleter>(ptr)->tag == 17);
    }
    SECTION("tuple+deleter", "Forward as tuple; custom deleter") {
        auto ptr = wheels::make_shared<foo>(std::forward_as_tuple(42, true, "foo"), wheels::deleter = test_deleter { 17 });
        STATIC_ASSERT(std::is_same<decltype(ptr), std::shared_ptr<foo>>::value);
        CHECK(&*ptr == instance);
        REQUIRE(std::get_deleter<test_deleter>(ptr));
        CHECK(std::get_deleter<test_deleter>(ptr)->tag == 17);
    }
    SECTION("raw+deleter", "Raw pointer; custom deleter") {
        auto* raw = new int;
        auto ptr = wheels::make_shared<int>(wheels::raw = raw, wheels::deleter = test_deleter { 17 });
        STATIC_ASSERT(std::is_same<decltype(ptr), std::shared_ptr<int>>::value);
        CHECK(&*ptr == raw);
        REQUIRE(std::get_deleter<test_deleter>(ptr));
        CHECK(std::get_deleter<test_deleter>(ptr)->tag == 17);
    }
}

