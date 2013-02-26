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

// Concept checking tools

#ifndef WHEELS_CONCEPT_HPP
#define WHEELS_CONCEPT_HPP

#include "meta.h++"

#include <boost/concept_check.hpp>
#include <boost/concept_archetype.hpp>

#include <climits>
#include <cstdint>

#include <chrono>
#include <initializer_list>
#include <istream>
#include <memory>
#include <ostream>
#include <ratio>
#include <string>

#if WHEELS_HAS_ISSUE(BROKEN_CONCEPT_CHECK)
    WHEELS_WARNING("Concept checks do not work right in this compiler");
#endif

#define WHEELS_ASSERT_TYPE(TYPE, ...) \
    static_assert(::std::is_same<decltype((__VA_ARGS__)), TYPE>::value, "Wrong type: " #__VA_ARGS__ " is not " #TYPE)

namespace wheels {
    namespace concepts {
        namespace concept_detail {
            template <typename... T>
            void ignore(T const&...);

            template <typename T>
            T rvalue();

            template <typename... Concepts>
            struct tester : Concepts... {};
        } // namespace concept_detail

        //! Concept checker
        template <typename T, template <typename...> class... Concepts>
        constexpr bool check() {
            BOOST_CONCEPT_ASSERT((concept_detail::tester<Concepts<T>...>));
            return true;
        }

        using boost::DefaultConstructible;

        using boost::EqualityComparable;
        using boost::LessThanComparable;

        using boost::InputIterator;
        using boost::OutputIterator;
        using boost::ForwardIterator;
        using boost::BidirectionalIterator;
        using boost::RandomAccessIterator;

        template <typename T>
        struct ContextuallyConvertibleToBool {
        private:
            T value;

        public:
            BOOST_CONCEPT_USAGE(ContextuallyConvertibleToBool) {
                do {} while(value);
            }
        };

        template <typename T>
        struct MoveConstructible {
            static_assert(std::is_move_constructible<T>::value, "type must be move constructible");
        };

        template <typename T>
        struct CopyConstructible : boost::CopyConstructible<T>, MoveConstructible<T> {};

        template <typename T>
        struct Destructible {
        public:
            static_assert(std::is_destructible<T>::value, "type must be destructible" );
        };

        template <typename T>
        struct MoveAssignable {
        private:
            T assigned;
            static T assignee();
            using return_type = decltype((assigned = assignee()));

        public:
            static_assert(std::is_convertible<return_type, T&>::value, "type must be move assignable" );

            BOOST_CONCEPT_USAGE(MoveAssignable) {
                assigned = assignee();
            }
        };

        template <typename T>
        struct CopyAssignable : boost::Assignable<T>, MoveAssignable<T> {};

        template <typename T>
        struct MoveSemantics : MoveConstructible<T>, MoveAssignable<T> {};

        template<typename T>
        struct CopySemantics : CopyConstructible<T>, CopyAssignable<T> {};

        template<typename T, typename Signature>
        struct Callable {
            static_assert(is_callable<T, Signature>::value, "type must be invokable for this signature");
        };

        template<typename T, typename... U>
        struct Constructible {
            static_assert(std::is_constructible<T, U...>::value, "type must be constructible from these types");
        };

        template<typename Signature>
        struct with_signature;

        template<typename Ret, typename... Args>
        struct with_signature<Ret(Args...)> {
            template<typename T>
            using Constructible = concepts::Constructible<T, Args...>;

            template<typename T>
            using Callable = concepts::Callable<T, Ret(Args...)>;
        };

        template <typename T, typename U = T>
        struct LvalueSwappable {
        public:
            BOOST_CONCEPT_USAGE(LvalueSwappable) {
                using std::swap;
                swap(t, u);
                swap(u, t);
            }
        private:
            T t;
            U u;
        };

        template <typename T, typename U = T>
        struct RvalueSwappable {
        public:
            BOOST_CONCEPT_USAGE(RvalueSwappable) {
                using std::swap;
                swap(concept_detail::rvalue<T>(), concept_detail::rvalue<U>());
                swap(concept_detail::rvalue<U>(), concept_detail::rvalue<T>());
            }
        };

        template <typename T, typename U = T>
        struct Swappable : LvalueSwappable<T, U>
                         , RvalueSwappable<T, U> {};

        template <typename T, typename U = T>
        struct ValueSwappable : Swappable<decltype(*std::declval<T&>())>
                              , Swappable<decltype(*std::declval<T&&>())> {};

        template <typename P>
        struct NullablePointer : EqualityComparable<P>
                               , DefaultConstructible<P>
                               , CopySemantics<P>
                               , Destructible<P>
                               , LvalueSwappable<P> {
        public:
            BOOST_CONCEPT_USAGE(NullablePointer) {
                std::nullptr_t const npc = nullptr;
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<P>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(a != b)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(ac != b)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(a != bc)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(ac != bc)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(a == np)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(ac == np)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(a == npc)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(ac == npc)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(np == a)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(npc == a)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(np == bc)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(npc == bc)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(a != np)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(ac != np)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(a != npc)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(ac != npc)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(np != a)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(npc != a)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(np != bc)>));
                BOOST_CONCEPT_ASSERT((ContextuallyConvertibleToBool<decltype(npc != bc)>));
                P u(np);
                P uc(npc);
                WHEELS_ASSERT_TYPE(P&, t = np);
                WHEELS_ASSERT_TYPE(P&, t = npc);

                concept_detail::ignore(u, uc);
            }
        private:
            P t;
            P a, b;
            P const ac, bc;
            std::nullptr_t np;
        };

        template <typename H, typename K>
        struct Hash : CopyConstructible<H>, Destructible<H> {
        private:
            struct U { operator K const(); };

        public:
            static_assert(is_function_object<H, std::size_t(K)>::value, "H must be a function object type");
            static_assert(is_function_object<H, std::size_t(U)>::value, "H must be a function object type");

            BOOST_CONCEPT_USAGE(Hash) {
                WHEELS_ASSERT_TYPE(std::size_t, h(k));
                WHEELS_ASSERT_TYPE(std::size_t, h(u));
            }
        private:
            H h;
            K k;
            U u;
        };

        template <typename K>
        struct for_key {
            template <typename T>
            using Hash = concepts::Hash<T, K>;
        };

        template <typename X>
        struct Allocator {
        public:
            using XX = std::allocator_traits<X>;
            using T = typename X::value_type;
            using pointer = typename XX::pointer;
            using const_pointer = typename XX::const_pointer;
            using void_pointer = typename XX::void_pointer;
            using const_void_pointer = typename XX::const_void_pointer;
            using size_type = typename XX::size_type;
            using difference_type = typename XX::difference_type;

            static_assert(std::is_convertible<pointer,const_pointer>::value,
                          "X::pointer must be convertible to X::const_pointer");
            static_assert(std::is_convertible<pointer,void_pointer>::value,
                          "X::pointer must be convertible to X::void_pointer");
            static_assert(std::is_convertible<pointer,const_void_pointer>::value,
                          "X::pointer must be convertible to X::const_void_pointer");
            static_assert(std::is_convertible<const_pointer,const_void_pointer>::value,
                          "X::const_pointer must be convertible to X::const_void_pointer");
            static_assert(std::is_convertible<void_pointer,const_void_pointer>::value,
                          "X::void_pointer must be convertible to X::const_void_pointer");
            static_assert(std::is_unsigned<size_type>::value && std::is_integral<size_type>::value,
                          "X::size_type must be an unsigned integer type");
            static_assert(std::is_unsigned<size_type>::value && std::is_integral<size_type>::value,
                          "X::size_type must be an unsigned integer type");
            static_assert(std::is_signed<difference_type>::value && std::is_integral<difference_type>::value,
                          "X::difference_type must be an signed integer type");
            static_assert(std::is_class<X>::value, "Allocators must be of class type");

            struct C { C(int x, int y, int z); };
            BOOST_CONCEPT_USAGE(Allocator) {
                BOOST_CONCEPT_ASSERT((NullablePointer<pointer>));
                BOOST_CONCEPT_ASSERT((NullablePointer<const_pointer>));
                BOOST_CONCEPT_ASSERT((NullablePointer<void_pointer>));
                BOOST_CONCEPT_ASSERT((NullablePointer<const_void_pointer>));
                T* x;
                WHEELS_ASSERT_TYPE(T&, *p);
                WHEELS_ASSERT_TYPE(T const&, *q);
                WHEELS_ASSERT_TYPE(pointer, static_cast<pointer>(w));
                WHEELS_ASSERT_TYPE(const_pointer, static_cast<const_pointer>(z));
                WHEELS_ASSERT_TYPE(pointer, a.allocate(n));
                WHEELS_ASSERT_TYPE(pointer, XX::allocate(a, n, u));
                a.deallocate(p, n);
                WHEELS_ASSERT_TYPE(size_type, XX::max_size(a));
                WHEELS_ASSERT_TYPE(bool, a1 == a2);
                WHEELS_ASSERT_TYPE(bool, a1 != a2);
                X aa1(a);
                X aa2(std::move(a));
                XX::construct(a, c, 17, 23, 42);
                XX::destroy(a, c);
                WHEELS_ASSERT_TYPE(X, XX::select_on_container_copy_construction(a));
                bool pocca = XX::propagate_on_container_copy_assignment::value;
                bool pocma = XX::propagate_on_container_move_assignment::value;
                bool pocs = XX::propagate_on_container_swap::value;

                concept_detail::ignore(x, aa1, aa2, pocca, pocs, pocma);
            }
        private:
            X& a;
            X& a1;
            X& a2;
            pointer p;
            const_pointer q;
            void_pointer w;
            const_void_pointer z;
            size_type n;
            const_pointer u;
            C* c;
        };

        namespace concept_detail {
            struct numeric_assign_byref_tester {
                template <typename T, T& (T::*)(T const&) = &T::operator= > class assignment {};

                template <typename T>
                static Bool<true> test(assignment<T>*);
                template <typename>
                static Bool<false> test(...);
            };
            struct numeric_assign_byval_tester {
                template <typename T, T& (T::*)(T) = &T::operator= > class assignment {};

                template <typename T>
                static Bool<true> test(assignment<T>*);
                template <typename>
                static Bool<false> test(...);
            };
            template <typename T>
            struct has_numeric_assign : Any<decltype(numeric_assign_byref_tester::test<T>(nullptr)),
                                            decltype(numeric_assign_byval_tester::test<T>(nullptr))> {};

            template <typename T>
            void operator&(T) = delete;

            template <typename T, typename = void>
            struct has_overloaded_addressof : Bool<false> {};
            template <typename T>
            struct has_overloaded_addressof<T, Void<decltype(std::declval<T>().operator&())>>
            : Bool<true> {};
            template <typename T>
            struct has_overloaded_addressof<T, Void<decltype(operator&(std::declval<T>()))>>
            : Bool<true> {};
        } // namespace concept_detail

        template <typename N>
        struct Numeric {
        public:
            static_assert(!std::is_abstract<N>::value,
                          "N cannot be an abstract class");
            static_assert(!std::is_reference<N>::value,
                          "N cannot be a reference type");
            static_assert(!std::is_const<N>::value && !std::is_volatile<N>::value,
                          "N cannot be cv-qualified");
            static_assert(!std::is_class<N>::value || std::is_default_constructible<N>::value,
                          "if N is a class is must have a public default constructor");
            static_assert(!std::is_class<N>::value || std::is_constructible<N, N const&>::value,
                          "if N is a class is must have a public copy constructor");
            static_assert(!std::is_class<N>::value || std::is_destructible<N>::value,
                          "if N is a class is must have a public destructor");
            static_assert(!std::is_class<N>::value || concept_detail::has_numeric_assign<N>::value,
                          "if N is a class is must have a public assignment operator with signature N& N::operator=(N const&) or N& N::operator=(N)");
            static_assert(!std::is_class<N>::value || !concept_detail::has_overloaded_addressof<N>::value,
                          "if N is a class is must not overload unary operator&");
        };

        namespace concept_detail {
            template <typename T>
            struct is_ratio : Bool<false> {};
            template <std::intmax_t Num, std::intmax_t Den>
            struct is_ratio<std::ratio<Num, Den>> : Bool<true> {};
        } // namespace concept_detail

        template <typename C>
        struct Clock {
        public:
            using rep = typename C::rep;
            using period = typename C::period;
            using duration = typename C::duration;
            using time_point = typename C::time_point;

            static_assert(std::is_same<duration,std::chrono::duration<rep,period>>::value,
                          "C::duration must be std::chrono::duration<C::rep, C::period>");
            static_assert(concept_detail::is_ratio<period>::value,
                          "C::period must be a specialization of std::ratio");

            BOOST_CONCEPT_USAGE(Clock) {
                WHEELS_ASSERT_TYPE(bool const, C::is_steady);
                WHEELS_ASSERT_TYPE(time_point, C::now());
            }
        };

        template <typename TC>
        struct TrivialClock : Clock<TC> {
        public:
            using rep = typename Clock<TC>::rep;
            using period = typename Clock<TC>::period;
            using duration = typename Clock<TC>::duration;
            using time_point = typename Clock<TC>::time_point;

            static_assert(noexcept(TC::now()), "TC::now() cannot throw exceptions");

            BOOST_CONCEPT_USAGE(TrivialClock) {
                BOOST_CONCEPT_ASSERT((EqualityComparable<rep>));
                BOOST_CONCEPT_ASSERT((LessThanComparable<rep>));
                BOOST_CONCEPT_ASSERT((DefaultConstructible<rep>));
                BOOST_CONCEPT_ASSERT((CopyConstructible<rep>));
                BOOST_CONCEPT_ASSERT((CopyAssignable<rep>));
                BOOST_CONCEPT_ASSERT((Destructible<rep>));
                BOOST_CONCEPT_ASSERT((Numeric<rep>));
                BOOST_CONCEPT_ASSERT((LvalueSwappable<rep>));

                BOOST_CONCEPT_ASSERT((EqualityComparable<duration>));
                BOOST_CONCEPT_ASSERT((LessThanComparable<duration>));
                BOOST_CONCEPT_ASSERT((DefaultConstructible<duration>));
                BOOST_CONCEPT_ASSERT((CopyConstructible<duration>));
                BOOST_CONCEPT_ASSERT((CopyAssignable<duration>));
                BOOST_CONCEPT_ASSERT((Destructible<duration>));
                BOOST_CONCEPT_ASSERT((Numeric<duration>));
                BOOST_CONCEPT_ASSERT((LvalueSwappable<duration>));

                BOOST_CONCEPT_ASSERT((EqualityComparable<time_point>));
                BOOST_CONCEPT_ASSERT((LessThanComparable<time_point>));
                BOOST_CONCEPT_ASSERT((DefaultConstructible<time_point>));
                BOOST_CONCEPT_ASSERT((CopyConstructible<time_point>));
                BOOST_CONCEPT_ASSERT((CopyAssignable<time_point>));
                BOOST_CONCEPT_ASSERT((Destructible<time_point>));
                BOOST_CONCEPT_ASSERT((Numeric<time_point>));
                BOOST_CONCEPT_ASSERT((LvalueSwappable<time_point>));

                BOOST_CONCEPT_ASSERT((TrivialClock<typename time_point::clock>));
            }
        };

        template <typename T>
        struct CharTraits {
        public:
            using char_type = typename T::char_type;
            using int_type = typename T::int_type;
            using off_type = typename T::off_type;
            using pos_type = typename T::pos_type;
            using state_type = typename T::state_type;

            BOOST_CONCEPT_USAGE(CharTraits) {
                BOOST_CONCEPT_ASSERT((CopySemantics<state_type>));
                BOOST_CONCEPT_ASSERT((DefaultConstructible<state_type>));

                WHEELS_ASSERT_TYPE(bool, T::eq(c, d));
                static_assert(noexcept(T::eq(c, d)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(bool, T::lt(c, d));
                static_assert(noexcept(T::lt(c, d)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(int, T::compare(p, q, n));
                // TODO: Why the heck doesn't GCC noexcept all of these?
                //static_assert(noexcept(T::compare(p, q, n)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(std::size_t, T::length(p));
                //static_assert(noexcept(T::length(p)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(char_type const*, T::find(p, n, c));
                //static_assert(noexcept(T::find(p, n, c)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(char_type*, T::move(s, p, n));
                //static_assert(noexcept(T::move(s, p, n)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(char_type*, T::copy(s, p, n));
                //static_assert(noexcept(T::copy(s, p, n)), "Operations on CharTraits must not throw exceptions");
                static_assert(noexcept(T::assign(r, d)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(char_type*, T::assign(s, n, c));
                //static_assert(noexcept(T::assign(s, n, c)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(int_type, T::not_eof(e));
                static_assert(noexcept(T::not_eof(e)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(char_type, T::to_char_type(e));
                static_assert(noexcept(T::to_char_type(e)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(int_type, T::to_int_type(c));
                static_assert(noexcept(T::to_int_type(c)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(bool, T::eq_int_type(e, f));
                static_assert(noexcept(T::eq_int_type(e, f)), "Operations on CharTraits must not throw exceptions");
                WHEELS_ASSERT_TYPE(int_type, T::eof());
                static_assert(noexcept(T::eof()), "Operations on CharTraits must not throw exceptions");
            }

        private:
            char_type c, d;
            char_type const *p, *q;
            char_type* s;
            std::size_t i, j, n;
            int_type e, f;
            pos_type pos;
            state_type state;
            char_type r;
        };

        // TODO: containers

        template <typename S>
        struct SeedSequence {
        public:
            using result_type = typename S::result_type;

            static_assert(std::is_unsigned<result_type>::value && std::is_integral<result_type>::value && sizeof(result_type) * CHAR_BIT >= 32,
                          "result_type must be an unsigned integer type of at least 32 bits");

            BOOST_CONCEPT_USAGE(SeedSequence) {
                S();
                S(ib, ie);
                S(il);
                WHEELS_ASSERT_TYPE(void, q.generate(rb, re));
                WHEELS_ASSERT_TYPE(std::size_t, r.size());
                WHEELS_ASSERT_TYPE(void, r.param(ob));
            }

        private:
            S q;
            S const r;
            boost::input_iterator_archetype<std::uint32_t> ib, ie;
            boost::mutable_random_access_iterator_archetype<std::uint32_t> rb, re;
            boost::output_iterator_archetype<std::uint32_t> ob;
            std::initializer_list<result_type> il;
        };

        struct seed_sequence_archetype {
            using result_type = std::uint32_t;

            seed_sequence_archetype() {}
            template <typename It>
            seed_sequence_archetype(It, It) {}
            seed_sequence_archetype(std::initializer_list<result_type>) {}
            template <typename It>
            void generate(It, It) {}
            std::size_t size() const { return 0; }
            template <typename It>
            void param(It) {}
        };

        template <typename G>
        struct UniformRandomNumberGenerator {
        public:
            using result_type = typename G::result_type;

            static_assert(std::is_unsigned<result_type>::value && std::is_integral<result_type>::value,
                          "result_type must be an unsigned integer type");

            BOOST_CONCEPT_USAGE(UniformRandomNumberGenerator) {
                WHEELS_ASSERT_TYPE(result_type, g());
                WHEELS_ASSERT_TYPE(result_type, G::min());
                WHEELS_ASSERT_TYPE(result_type, G::max());
                static_assert(G::min() < G::max(), "G::min() must be less than G::max()");
            }

        private:
            G g;
        };

        struct uniform_random_number_generator_archetype {
            using result_type = unsigned long long;
            result_type operator()() { return 0; }
            static constexpr result_type min() { return 0; }
            static constexpr result_type max() { return 42; }
        };

        template <typename E>
        struct RandomNumberEngine : UniformRandomNumberGenerator<E>
                                  , CopySemantics<E> {
        public:
            using result_type = typename E::result_type;

            BOOST_CONCEPT_USAGE(RandomNumberEngine) {
                E();
                E(x);
                E(s);
                E(q);
                WHEELS_ASSERT_TYPE(void, e.seed());
                WHEELS_ASSERT_TYPE(void, e.seed(s));
                WHEELS_ASSERT_TYPE(void, e.seed(q));
                WHEELS_ASSERT_TYPE(result_type, e());
                WHEELS_ASSERT_TYPE(void, e.discard(z));
                WHEELS_ASSERT_TYPE(bool, x == y);
                WHEELS_ASSERT_TYPE(bool, x != y);
                WHEELS_ASSERT_TYPE(bool, x != y);
                WHEELS_ASSERT_TYPE(decltype(os)&, os << x);
                WHEELS_ASSERT_TYPE(decltype(is)&, is >> v);
            }

        private:
            E e, v;
            E const x, y;
            result_type s;
            seed_sequence_archetype q;
            unsigned long long z;
            std::ostream os;
            std::istream is;
        };

        template <typename D>
        struct RandomNumberDistribution : CopySemantics<D> {
        public:
            using result_type = typename D::result_type;
            using param_type = typename D::param_type;

            static_assert(is_function_object<D, result_type(uniform_random_number_generator_archetype)>::value,
                          "a distribution must be a function object");
            static_assert(std::is_arithmetic<result_type>::value,
                          "result_type must be an arithmetic type");
            static_assert(std::is_same<typename param_type::distribution_type, D>::value,
                          "param_type::distribution_type must be this distribution type");

            BOOST_CONCEPT_USAGE(RandomNumberDistribution) {
                BOOST_CONCEPT_ASSERT((CopySemantics<param_type>));
                BOOST_CONCEPT_ASSERT((EqualityComparable<param_type>));

                D();
                D(p);
                WHEELS_ASSERT_TYPE(void, d.reset());
                WHEELS_ASSERT_TYPE(param_type, x.param());
                WHEELS_ASSERT_TYPE(void, x.param(p));
                WHEELS_ASSERT_TYPE(result_type, d(g));
                WHEELS_ASSERT_TYPE(result_type, d(g, p));
                WHEELS_ASSERT_TYPE(result_type, x.min());
                WHEELS_ASSERT_TYPE(result_type, x.max());
                WHEELS_ASSERT_TYPE(bool, x == y);
                WHEELS_ASSERT_TYPE(bool, x != y);
                WHEELS_ASSERT_TYPE(decltype(os)&, os << x);
                WHEELS_ASSERT_TYPE(decltype(is)&, is >> d);
            }

        private:
            D d;
            D const x, y;
            param_type const p;
            uniform_random_number_generator_archetype g, g1, g2;
            std::ostream os;
            std::istream is;
        };

        template <typename T>
        struct RegexTraits {
        public:
            using char_type = typename T::char_type;
            using string_type = typename T::string_type;
            using locale_type = typename T::locale_type;
            using char_class_type = typename T::char_class_type;

            static_assert(std::is_same<string_type, std::basic_string<char_type>>::value, "string_type must be std::basic_string<char_type>");
            static_assert(std::is_copy_constructible<locale_type>::value, "locale_type must be copy constructible");

            BOOST_CONCEPT_USAGE(RegexTraits) {
                WHEELS_ASSERT_TYPE(std::size_t, T::length(p));
                WHEELS_ASSERT_TYPE(char_type, v.translate(c));
                WHEELS_ASSERT_TYPE(char_type, v.translate_nocase(c));
                WHEELS_ASSERT_TYPE(string_type, v.transform(F1, F2));
                WHEELS_ASSERT_TYPE(string_type, v.transform_primary(F1, F2));
                WHEELS_ASSERT_TYPE(string_type, v.lookup_collatename(F1, F2));
                WHEELS_ASSERT_TYPE(char_class_type, v.lookup_classname(F1, F2, b));
                WHEELS_ASSERT_TYPE(bool, v.isctype(c, cl));
                WHEELS_ASSERT_TYPE(int, v.value(c, I));
                WHEELS_ASSERT_TYPE(locale_type, u.imbue(loc));
                WHEELS_ASSERT_TYPE(locale_type, v.getloc());
            }

        private:
            T u;
            T const v;
            char_type const* p;
            boost::input_iterator_archetype<std::uint32_t> I1, I2;
            boost::forward_iterator_archetype<std::uint32_t> F1, F2;
            char_type const c;
            string_type s;
            string_type const cs;
            bool b;
            int I;
            char_class_type cl;
            locale_type loc;
        };

        template <typename L>
        struct BasicLockable {
        public:
            BOOST_CONCEPT_USAGE(BasicLockable) {
                m.lock();
                m.unlock();
                static_assert(noexcept(m.unlock()), "unlock() must not throw exceptions");
            }

        private:
            L m;
        };

        template <typename L>
        struct Lockable : BasicLockable<L> {
        public:
            BOOST_CONCEPT_USAGE(Lockable) {
                WHEELS_ASSERT_TYPE(bool, m.try_lock());
            }

        private:
            L m;
        };

        template <typename L>
        struct TimedLockable : Lockable<L> {
        public:
            BOOST_CONCEPT_USAGE(TimedLockable) {
                WHEELS_ASSERT_TYPE(bool, m.try_lock_for(rel_time));
                WHEELS_ASSERT_TYPE(bool, m.try_lock_until(abs_time));
            }

        private:
            L m;
            std::chrono::nanoseconds rel_time;
            std::chrono::system_clock::time_point abs_time;
        };

        template <typename M>
        struct Mutex : Lockable<M>
                     , DefaultConstructible<M>
                     , Destructible<M> {
        public:
            BOOST_CONCEPT_USAGE(Mutex) {
                static_assert(!std::is_copy_constructible<M>::value && !std::is_move_constructible<M>::value
                           && !std::is_copy_assignable<M>::value && !std::is_move_assignable<M>::value,
                              "Mutex types shall not be copyable or movable");
            }

        private:
            M m;
        };
    } // namespace concepts
} // namespace wheels

#undef WHEELS_ASSERT_TYPE

#endif // WHEELS_CONCEPT_HPP
 
