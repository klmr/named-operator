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

// Overloaded function object

#ifndef WHEELS_OVERLOAD_HPP
#define WHEELS_OVERLOAD_HPP

#include "meta.h++"

#include "detail/config.h++"

#include <boost/preprocessor/repetition/enum.hpp>

namespace wheels {
    namespace overload_detail {
        template <typename Fun>
        class simple_overload_element : Fun {
        public:
            simple_overload_element() = default;

            template <typename T,
                      DisableIf<is_related<simple_overload_element, T>> = _>
            explicit simple_overload_element(T&& t)
            : Fun(std::forward<T>(t)) {}

            using Fun::operator();
        };

        template <typename Ret, typename... Args>
        class simple_overload_element<Ret(*)(Args...)> {
            using fun_type = Ret(*)(Args...);
        public:
            simple_overload_element() = delete;

            template <typename T,
                      DisableIf<is_related<simple_overload_element, T>> = _>
            explicit simple_overload_element(T&& t)
            : fun_ptr(std::forward<T>(t)) {}

            Ret operator()(Args... args) const {
                return fun_ptr(std::forward<Args>(args)...);
            }

        private:
            fun_type fun_ptr;
        };

        template <typename Class, typename Ret>
        class simple_overload_element<Ret Class::*> {
            using mem_obj_type = Ret Class::*;
        public:
            simple_overload_element() = delete;

            template <typename T,
                      DisableIf<is_related<simple_overload_element, T>> = _>
            explicit simple_overload_element(T&& t)
            : mem_obj_ptr(std::forward<T>(t)) {}

            template <typename C,
                      EnableIf<Any<is_related<Class, C>, std::is_base_of<Class, C>>> = _>
            WithQualificationsOf<C, Ret> operator()(C&& c) const {
                return std::forward<C>(c).*mem_obj_ptr;
            }

            mem_obj_type mem_obj_ptr;
        };

        template <typename T, typename Signature>
        struct mem_fun_overload_element_impl;
        template <typename T>
        struct mem_fun_overload_element {
            using type = mem_fun_overload_element_impl<T, RemoveFunctionQualifiers<T>>;
        };
        template <typename T>
        using overload_element = Invoke<Conditional<
                                    std::is_member_function_pointer<T>,
                                    mem_fun_overload_element<T>,
                                    identity<simple_overload_element<T>>
                                 >>;

        template <typename Class, typename Signature, typename Ret, typename... Args>
        class mem_fun_overload_element_impl<Signature Class::*, Ret (Class::*)(Args...)> {
            using mem_fun_type = Signature Class::*;
            using qualifiers = FromFunctionQualifiers<Signature>;
        public:
            mem_fun_overload_element_impl() = delete;
            template <typename T,
                      DisableIf<is_related<mem_fun_overload_element_impl, T>> = _>
            explicit mem_fun_overload_element_impl(T&& t)
            : mem_fun_ptr(std::forward<T>(t)) {}

            template <typename Qualifiers = qualifiers,
                      DisableIf<std::is_reference<Qualifiers>> = _>
            Ret operator()(WithCvOf<qualifiers, Class>& c, Args&&... args) const {
                return (c.*mem_fun_ptr)(std::forward<Args>(args)...);
            }
            template <typename Qualifiers = qualifiers,
                      DisableIf<std::is_reference<Qualifiers>> = _>
            Ret operator()(WithCvOf<qualifiers, Class>&& c, Args&&... args) const {
                return (std::move(c).*mem_fun_ptr)(std::forward<Args>(args)...);
            }
            template <typename Qualifiers = qualifiers,
                      EnableIf<std::is_reference<Qualifiers>> = _>
            Ret operator()(WithQualificationsOf<qualifiers, Class> c, Args&&... args) const {
                return (std::forward<WithQualificationsOf<qualifiers, Class>>(c).*mem_fun_ptr)(std::forward<Args>(args)...);
            }
            Ret operator()(WithCvOf<RemoveReference<qualifiers>, Class>* c, Args&&... args) const {
                return (c->*mem_fun_ptr)(std::forward<Args>(args)...);
            }
        private:
            mem_fun_type mem_fun_ptr;
        };

        template <typename... Funs>
        class overload_impl {};
        template <typename Fun>
        class overload_impl<Fun> : overload_element<Decay<Fun>> {
            using element_type = overload_element<Decay<Fun>>;
        public:
            overload_impl() = default;

            template <typename T,
                      DisableIf<is_related<overload_impl, T>> = _>
            explicit overload_impl(T&& t)
            : element_type(std::forward<T>(t)) {}

            using element_type::operator();
        };
        template <typename Head, typename... Tail>
        class overload_impl<Head, Tail...> : overload_element<Decay<Head>>, overload_impl<Tail...> {
            using head_type = overload_element<Decay<Head>>;
            using tail_type = overload_impl<Tail...>;
        public:
            overload_impl() = default;

            template <typename H, typename... T,
                      DisableIf<is_related<overload_impl, H>> = _>
            explicit overload_impl(H&& h, T&&... t)
            : head_type(std::forward<H>(h))
            , tail_type(std::forward<T>(t)...) {}

            using head_type::operator();
            using tail_type::operator();
        };
    } // namespace overload_detail

    template <typename... Funs>
    class overload : overload_detail::overload_impl<Funs...> {
        using impl_type = overload_detail::overload_impl<Funs...>;
    public:
        overload() = default;

        template <typename... T,
                  DisableIf<is_related<overload, PackHead<T...>>> = _>
        explicit overload(T&&... t)
        : impl_type(std::forward<T>(t)...) {}

        using impl_type::operator();
    };

    template <typename... Funs, 
              typename Overload = overload<Decay<Funs>...>
             >
    Overload make_overload(Funs&&... funs) {
        return Overload { std::forward<Funs>(funs)... };
    }

#ifndef WHEELS_MAKE_OVERLOAD_MAX_ARITY
# define WHEELS_MAKE_OVERLOAD_MAX_ARITY 17
#endif // WHEELS_MAKE_OVERLOAD_MAX_ARITY

#define WHEELS_OVERLOAD_PASTE(A, B, TEXT) TEXT
#define WHEELS_OVERLOAD(NAME) BOOST_PP_ENUM(WHEELS_MAKE_OVERLOAD_MAX_ARITY, WHEELS_OVERLOAD_PASTE, NAME)

    template <typename Head, typename... Tail,
              EnableIf<Any<is_function_pointer<Decay<Head>>
                         , std::is_member_pointer<Head>>> = _,
              typename Overload = overload<Head, Tail...>>
    Overload make_overload(Decay<Head> head, Decay<Tail>... tail, WHEELS_OVERLOAD(Decay<Head> = nullptr)) {
        return Overload { head, tail... };
    }

    template <typename Result, typename... Funs>
    struct visitor : overload<Funs...> {
        using fun_type = overload<Funs...>;
    public:
        using result_type = Result;

        template <typename... T,
                  DisableIf<is_related<visitor, PackHead<T...>>> = _>
        explicit visitor(T&&... t)
        : fun_type(std::forward<T>(t)...) {}

        using fun_type::operator();
    };

    template <typename Result = void,
              typename... Funs, 
              typename Visitor = visitor<Result, Decay<Funs>...>
             >
    Visitor make_visitor(Funs&&... funs) {
        return Visitor { std::forward<Funs>(funs)... };
    }
} // namespace wheels

#endif // WHEELS_OVERLOAD_HPP

