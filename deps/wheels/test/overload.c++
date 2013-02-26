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

// Tests for <wheels/overload.h++>

#include <wheels/overload.h++>

#include <wheels/unit_test.h++>

#include <wheels/concept.h++>

#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>

#include <string>

int fun0(int n) {
    return n * 2;
}

int fun1(double n) {
    return static_cast<int>(n);
}

int fun2(std::string const& n) {
    return n.size();
}

int overloaded(int n) {
    return n * 2;
}
int overloaded(double n) {
    return static_cast<int>(n);
}
int overloaded(std::string const& n) {
    return n.size();
}

TEST_CASE("overload", "overloaded function wrapper") {
    SECTION("move", "move semantics") {
        struct funobj {
            int operator()(int n) const { return p? *p + n : -1; }
            std::unique_ptr<int> p;
        } f { std::unique_ptr<int>(new int(17)) };
        auto overload = wheels::make_overload(std::move(f));
        auto moved = std::move(overload);
        CONCEPT_CHECK(decltype(overload),
                      MoveSemantics,
                      with_signature<int(int)>::Callable
                     );
        CHECK(overload(23) == -1);
        CHECK(moved(23) == 17+23);
    }
    SECTION("ref", "ref wrapper") {
        struct foo {
            foo() = default;
            foo(foo const&) = delete;

            const foo* operator()() const { return this; }
        } no_copy;
        auto overload = wheels::make_overload(std::ref(no_copy));
        CHECK(overload() == &no_copy);
    }
}

TEST_CASE("overload/single", "overload with a single function") {
    SECTION("funobj", "function object") {
        struct funobj {
            int operator()(int n) const { return 17 + n; }
        } f;
        auto overload = wheels::make_overload(f);
        CONCEPT_CHECK(decltype(overload),
                      DefaultConstructible,
                      CopySemantics,
                      with_signature<void(int)>::Callable,
                      with_signature<int(int)>::Callable
                     );
        CHECK(overload(23) == 17+23);
    }
    SECTION("funptr", "function pointer") {
        auto overload = wheels::make_overload(fun0);
        CONCEPT_CHECK(decltype(overload),
                      CopySemantics,
                      with_signature<void(int)>::Callable,
                      with_signature<int(int)>::Callable
                     );
        CHECK(overload(42) == 42*2);
    }
    SECTION("memfun", "member function pointer") {
        struct foo {
            int f(int n) { return n + delta; }
            int delta;
        } x { 3 };
        auto overload = wheels::make_overload(&foo::f);
        CONCEPT_CHECK(decltype(overload),
                      CopySemantics,
                      with_signature<void(foo&,int)>::Callable,
                      with_signature<int(foo&,int)>::Callable
                     );
        CHECK(overload(x, 23) == 23+x.delta);
    }
}

TEST_CASE("overload/multiple", "overload with multiple functions") {
    SECTION("funobj", "function object") {
        struct funobj0 {
            int operator()(int n) const { return 17 + n; }
            double operator()(double d) const { return 23 + d; }
        } f0;
        struct funobj1 {
            int operator()(std::string const& s) const { return 42 + s.size(); }
        } f1;
        auto overload = wheels::make_overload(f0, f1);
        CONCEPT_CHECK(decltype(overload),
                      DefaultConstructible,
                      CopySemantics,
                      with_signature<int(int)>::Callable,
                      with_signature<int(double)>::Callable,
                      with_signature<double(double)>::Callable,
                      with_signature<int(std::string const&)>::Callable
                     );
        CHECK(overload(5) == 17+5);
        CHECK(overload(0.5) == 23 + 0.5);
        CHECK(overload("foo") == 42+3);
    }
    SECTION("funptr", "function pointer") {
        auto overload = wheels::make_overload(fun0, fun1, fun2);
        CONCEPT_CHECK(decltype(overload),
                      CopySemantics,
                      with_signature<int(int)>::Callable,
                      with_signature<int(double)>::Callable,
                      with_signature<double(double)>::Callable,
                      with_signature<int(std::string const&)>::Callable
                     );
        CHECK(overload(42) == 42*2);
        CHECK(overload(23.5) == 23);
        CHECK(overload("foo") == 3);
    }
    SECTION("memfun", "member function pointer") {
        struct foo {
            int f(int n) { return n + delta; }
            double g(double d) { return d + delta*0.1; }
            int h(std::string const& s) { return s.size() + delta; }
            int delta;
        } x { 3 };
        auto overload = wheels::make_overload(&foo::f, &foo::g, &foo::h);
        CONCEPT_CHECK(decltype(overload),
                      CopySemantics,
                      with_signature<void(foo&,int)>::Callable,
                      with_signature<int(foo&,int)>::Callable
                     );
        CHECK(overload(x, 23) == 23+x.delta);
        CHECK(overload(x, 17.0) == 17.0+x.delta*0.1);
        CHECK(overload(x, "foo") == 3+x.delta);
    }
    SECTION("mixed", "mixed callable kinds") {
        struct funobj0 {
            int operator()(double n) const { return 17.6 + n; }
            int operator()(std::string const& s) const { return 42 + s.size(); }
        } f0;
        struct foo {
            int f(std::string const& s) { return s.size() + delta; }
            int g(std::u16string const& s) { return s.size() + delta*2; }
            int delta;
        } x { 3 };
        auto overload = wheels::make_overload(fun0, f0, &foo::f, &foo::g);
        CONCEPT_CHECK(decltype(overload),
                      CopySemantics,
                      with_signature<int(int)>::Callable,
                      with_signature<int(double)>::Callable,
                      with_signature<int(std::string const&)>::Callable,
                      with_signature<int(foo&, std::string const&)>::Callable,
                      with_signature<int(foo&, std::u16string const&)>::Callable
                     );
        CHECK(overload(42) == 42*2);
        CHECK(overload(0.5) == 18);
        CHECK(overload("foo") == 42+3);
        CHECK(overload(x, "foo") == 3+x.delta);
        CHECK(overload(x, u"foo") == 3+x.delta*2);
    }
#if !WHEELS_HAS_ISSUE(BROKEN_OVERLOAD_MACRO)
    SECTION("overloaded", "overloaded names") {
        SECTION("variadic", "all types listed") {
            auto overload = wheels::make_overload<int(int), int(double), int(std::string const&)>(WHEELS_OVERLOAD(overloaded));
            CONCEPT_CHECK(decltype(overload),
                          CopySemantics,
                          with_signature<int(int)>::Callable,
                          with_signature<int(double)>::Callable,
                          with_signature<int(std::string const&)>::Callable
                         );
            CHECK(overload(42) == 42*2);
            CHECK(overload(17.5) == 17);
            CHECK(overload("foobar") == 6);
        }
    }
#endif
}

TEST_CASE("overload/visitor", "make_visitor tests") {
    int called = 0;
    auto fun1 = [&called] (int) { called = 1; };
    auto fun2 = [&called] (std::string const&) { called = 2; };

    auto visitor = wheels::make_visitor(fun1, fun2);
    CONCEPT_CHECK(decltype(visitor),
                  CopyConstructible,
                  with_signature<void(int)>::Callable,
                  with_signature<void(std::string const&)>::Callable
                 );

    using test_variant = boost::variant<int, std::string>;
    test_variant var { 42 };

    CHECK(called == 0);
    boost::apply_visitor(visitor, var);
    CHECK(called == 1);

    called = 0;

    var = "foo";
    boost::apply_visitor(visitor, var);
    CHECK(called == 2);
}
