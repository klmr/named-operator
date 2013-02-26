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

// Tests for <wheels/smart_ptr/value_ptr.h++>

#include <wheels/smart_ptr/value_ptr.h++>

#include <wheels/unit_test.h++>

#include <wheels/concept.h++>

#include <new>
#include <type_traits>

using wheels::tracer;

namespace {
    class simple : tracer<simple> {};

    class can_copy : tracer<can_copy> {
    public:
        can_copy(int value) : value(value) {}
        int value;
    };

    class no_copy {
    public:
        no_copy(int value) : value(value) {}
        no_copy(no_copy&&) = default;
        no_copy& operator=(no_copy&&) = default;
        no_copy(no_copy const&) = delete;
        no_copy& operator=(no_copy const&) = delete;
        int value;
    };
    class with_args0 {
    public:
        with_args0(no_copy&& x, int y) : x(std::move(x)), y(y) {}

        no_copy x;
        int y;
    };
    class with_args1 {
    public:
        with_args1(can_copy x, int y) : x(x), y(y) {}

        can_copy x;
        int y;
    };

    class base {
    public:
        virtual base* clone() const = 0;

        virtual ~base() = default;
    };

    class derived : public base, private tracer<derived> {
    public:
        derived* clone() const /* TODO: check if override is allowed here */ { ++cloned(); return new derived(*this); }
    };

    class with_args2 : public derived {
    public:
        with_args2(int x, bool y, std::string s) {
            CHECK(x == 42);
            CHECK(y == true);
            CHECK(s == "foo");
        }
    };

    class bad_polymorphic : tracer<bad_polymorphic> {
    public:
        virtual ~bad_polymorphic() = default;
    };

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

    template <typename T>
    struct test_cloner {
        T* operator()(T* t) const { return t; }
        int tag;
    };

    struct test_deleter {
        void operator()(void*) {}
        int tag;
    };
} // namespace

TEST_CASE("smart_ptr/value_ptr", "value_ptr tests") {
    wheels::reset_tracers<simple, derived, bad_polymorphic, allocated, deallocated, can_copy>();

    SECTION("concept", "Concepts") {
        CONCEPT_CHECK(wheels::value_ptr<int>, NullablePointer);
    }
    SECTION("copy", "Cloner that uses the copy constructor") {
        wheels::copy_constructor_cloner<simple> cloner;
        simple s;
        auto r = cloner(&s);
        CHECK(r != &s);
        CHECK(tracer<simple>::copied() == 1);
    }
    SECTION("clone", "Cloner that uses the clone() function") {
        wheels::clone_function_cloner<base> cloner;
        derived p;
        auto r = cloner(&p);
        CHECK(r != &p);
        CHECK(tracer<derived>::copied() == 1);
        CHECK(tracer<derived>::cloned() == 1);
    }
    SECTION("default", "Default cloner decision") {
        STATIC_ASSERT(std::is_same<wheels::copy_constructor_cloner<simple>, wheels::DefaultCloner<simple>>::value);
        STATIC_ASSERT(std::is_same<wheels::clone_function_cloner<base>, wheels::DefaultCloner<base>>::value);
        STATIC_ASSERT(std::is_same<wheels::clone_function_cloner<derived>, wheels::DefaultCloner<derived>>::value);
        STATIC_ASSERT(std::is_same<wheels::copy_constructor_cloner<bad_polymorphic>, wheels::DefaultCloner<bad_polymorphic>>::value);
    }
    SECTION("null", "Construction of null pointer") {
        wheels::value_ptr<simple> s0, s1 { nullptr };
        CHECK_FALSE(s0);
        CHECK_FALSE(s1);
        wheels::value_ptr<simple> s2 { new simple };
        s2 = nullptr;
        CHECK_FALSE(s2);
    }
    SECTION("RAII", "Taking ownership and destruction") {
        {
            auto p = new simple;
            wheels::value_ptr<simple> s { p };
            CHECK(tracer<simple>::destroyed() == 0);
            CHECK(s);
            CHECK(s.get() == p);
        }
        CHECK(tracer<simple>::destroyed() == 1);
    }
    SECTION("deleter", "custom deleter") {
        struct custom_deleter : private tracer<custom_deleter> {
            custom_deleter(int mark) : mark(mark) {}
            void operator()(simple*) const { ++called(); }
            int mark;
        };
        {
            custom_deleter deleter { 17 };
            wheels::value_ptr<simple, wheels::DefaultCloner<simple>, custom_deleter> s0 {
                new simple, wheels::DefaultCloner<simple> {}, deleter
            };
            CHECK(tracer<custom_deleter>::called() == 0);
            CHECK(tracer<custom_deleter>::copied() == 1);
            CHECK(s0.get_deleter().mark == 17);
            {
                auto s1 = s0;
                CHECK(tracer<custom_deleter>::called() == 0);
                CHECK(tracer<custom_deleter>::copied() == 2);
                s0.get_deleter().mark = 42;
                CHECK(s1.get_deleter().mark == 17);
            }
            CHECK(tracer<custom_deleter>::called() == 1);
        }
        CHECK(tracer<custom_deleter>::called() == 2);
    }
    SECTION("convert", "Taking ownership from a convertible pointer type") {
        auto p = new derived;
        wheels::value_ptr<base> s { p };
        CHECK(s.get() == p);
    }
    SECTION("copy", "Copy constructor") {
        {
            auto p = new derived;
            wheels::value_ptr<base> s0 { p };
            CHECK(tracer<derived>::copied() == 0);
            {
                auto s1 = s0;
                CHECK(s1);
                CHECK(s1.get() != s0.get());
                CHECK(tracer<derived>::copied() == 1);
                CHECK(tracer<derived>::destroyed() == 0);
            }
            CHECK(tracer<derived>::destroyed() == 1);
        }
        CHECK(tracer<derived>::destroyed() == 2);
    }
    SECTION("cloner", "custom cloner") {
        struct custom_cloner : private tracer<custom_cloner> {
            custom_cloner(int mark) : mark(mark) {}
            base* operator()(base*) const { ++called(); return nullptr; }
            int mark;
        };
        auto p = new derived;
        custom_cloner cloner { 17 };
        wheels::value_ptr<base, custom_cloner> s0 { p, cloner, std::default_delete<derived> {} };
        CHECK(tracer<custom_cloner>::called() == 0);
        CHECK(tracer<custom_cloner>::copied() == 1);
        CHECK(s0.get_cloner().mark == 17);
        auto s1 = s0;
        CHECK(tracer<custom_cloner>::called() == 1);
        CHECK(tracer<custom_cloner>::copied() == 2);
        s0.get_cloner().mark = 42;
        CHECK(s1.get_cloner().mark == 17);
    }
    SECTION("move", "Move constructor") {
        auto p = new derived;
        wheels::value_ptr<base> s0 { p };
        wheels::value_ptr<base> s1 { std::move(s0) };
        CHECK_FALSE(s0);
        CHECK(s1.get() == p);
    }
    SECTION("release", "Releasing ownership") {
        auto p = new simple;
        wheels::value_ptr<simple> s { p };
        auto r = s.release();
        CHECK_FALSE(s);
        CHECK(r == p);
    }
    SECTION("reset", "Resetting the pointer") {
        auto p0 = new simple;
        wheels::value_ptr<simple> s { p0 };
        auto p1 = new simple;
        s.reset(p1);
        CHECK(tracer<simple>::destroyed() == 1);
        CHECK(s);
        CHECK(s.get() == p1);
    }
    SECTION("factory", "make_value") {
        SECTION("forward", "Simple forwarding") {
            auto p0 = wheels::make_value<with_args0>(no_copy { 17 }, 42);
            STATIC_ASSERT(std::is_same<decltype(p0), wheels::value_ptr<with_args0>>::value);
            CHECK(p0->x.value == 17);
            CHECK(p0->y == 42);
        }
        SECTION("allocate", "Allocator-based factory") {
            test_allocator<with_args1> a;
            CHECK(tracer<allocated>::called() == 0);
            {
                auto p0 = wheels::allocate_value<with_args1>(a, can_copy { 17 }, 42);
                CHECK(tracer<can_copy>::moved() == 1);
                CHECK(p0->x.value == 17);
                CHECK(p0->y == 42);
                CHECK(tracer<allocated>::called() == 1);
                {
                    auto p1 = p0;
                    UNUSED_VARIABLE(p1);
                    CHECK(tracer<allocated>::called() == 2);
                }
                CHECK(tracer<deallocated>::called() == 1);
            }
            CHECK(tracer<deallocated>::called() == 2);
        }
        SECTION("raw", "From raw pointer") {
            auto* raw0 = new int { 42 };
            auto p0 = wheels::make_value(wheels::raw = raw0);
            STATIC_ASSERT(std::is_same<decltype(p0), wheels::value_ptr<int>>::value);
            CHECK(&*p0 == raw0);

            auto* raw1 = new derived;
            auto p1 = wheels::make_value<base>(wheels::raw = raw1);
            STATIC_ASSERT(std::is_same<decltype(p1), wheels::value_ptr<base>>::value);
            CHECK(&*p1 == static_cast<base*>(raw1));
        }
        SECTION("cloner", "Value initialized; custom cloner") {
            auto ptr = wheels::make_value<int>(wheels::cloner = test_cloner<int> { 17 });
            STATIC_ASSERT(std::is_same<decltype(ptr), wheels::value_ptr<int, test_cloner<int>>>::value);
            CHECK(*ptr == 0);
            CHECK(ptr.get_cloner().tag == 17);
        }
        SECTION("deleter", "Value initialized; custom deleter") {
            auto ptr = wheels::make_value<int>(wheels::deleter = test_deleter { 17 });
            STATIC_ASSERT(std::is_same<decltype(ptr), wheels::value_ptr<int, wheels::DefaultCloner<int>, test_deleter>>::value);
            CHECK(*ptr == 0);
            CHECK(ptr.get_deleter().tag == 17);
        }
        SECTION("cloner+deleter", "Value initialized; custom cloner; custom deleter") {
            auto ptr = wheels::make_value<int>(wheels::deleter = test_deleter { 17 }, wheels::cloner = test_cloner<int> { 23 });
            STATIC_ASSERT(std::is_same<decltype(ptr), wheels::value_ptr<int, test_cloner<int>, test_deleter>>::value);
            CHECK(*ptr == 0);
            CHECK(ptr.get_cloner().tag == 23);
            CHECK(ptr.get_deleter().tag == 17);
        }
        SECTION("tuple+cloner", "Forward as tuple; custom cloner") {
            auto ptr = wheels::make_value<with_args2>(std::forward_as_tuple(42, true, "foo"), wheels::cloner = test_cloner<with_args2> { 17 });
            STATIC_ASSERT(std::is_same<decltype(ptr), wheels::value_ptr<with_args2, test_cloner<with_args2>>>::value);
            CHECK(ptr.get_cloner().tag == 17);
        }
        SECTION("tuple+deleter", "Forward as tuple; custom deleter") {
            auto ptr = wheels::make_value<with_args2>(std::forward_as_tuple(42, true, "foo"), wheels::deleter = test_deleter { 17 });
            STATIC_ASSERT(std::is_same<decltype(ptr), wheels::value_ptr<with_args2, wheels::DefaultCloner<with_args2>, test_deleter>>::value);
            CHECK(ptr.get_deleter().tag == 17);
        }
        SECTION("tuple+cloner+deleter", "Forward as tuple; custom cloner; custom deleter") {
            auto ptr = wheels::make_value<with_args2>(std::forward_as_tuple(42, true, "foo"),
                                                      wheels::deleter = test_deleter { 17 },
                                                      wheels::cloner = test_cloner<with_args2> { 23 });
            STATIC_ASSERT(std::is_same<decltype(ptr), wheels::value_ptr<with_args2, test_cloner<with_args2>, test_deleter>>::value);
            CHECK(ptr.get_cloner().tag == 23);
            CHECK(ptr.get_deleter().tag == 17);
        }
        SECTION("raw+cloner", "Raw pointer; custom cloner") {
            auto* raw = new int;
            auto ptr = wheels::make_value<int>(wheels::raw = raw, wheels::cloner = test_cloner<int> { 17 });
            STATIC_ASSERT(std::is_same<decltype(ptr), wheels::value_ptr<int, test_cloner<int>>>::value);
            CHECK(&*ptr == raw);
            CHECK(ptr.get_cloner().tag == 17);
        }
        SECTION("raw+deleter", "Raw pointer; custom deleter") {
            auto* raw = new int;
            auto ptr = wheels::make_value<int>(wheels::raw = raw, wheels::deleter = test_deleter { 17 });
            STATIC_ASSERT(std::is_same<decltype(ptr), wheels::value_ptr<int, wheels::DefaultCloner<int>, test_deleter>>::value);
            CHECK(&*ptr == raw);
            CHECK(ptr.get_deleter().tag == 17);
        }
        SECTION("raw+cloner+deleter", "Raw pointer; custom cloner; custom deleter") {
            auto* raw = new int;
            auto ptr = wheels::make_value<int>(wheels::deleter = test_deleter { 17 },
                                               wheels::raw = raw,
                                               wheels::cloner = test_cloner<int> { 23 });
            STATIC_ASSERT(std::is_same<decltype(ptr), wheels::value_ptr<int, test_cloner<int>, test_deleter>>::value);
            CHECK(&*ptr == raw);
            CHECK(ptr.get_cloner().tag == 23);
            CHECK(ptr.get_deleter().tag == 17);
        }
    }
}
