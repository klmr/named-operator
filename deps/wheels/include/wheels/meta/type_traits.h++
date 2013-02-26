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

// Type traits

#ifndef WHEELS_TYPE_TRAITS_HPP
#define WHEELS_TYPE_TRAITS_HPP

#include "../detail/config.h++"
#include "core.h++"

#include <type_traits>
#include <functional>
#include <utility>

#if !WHEELS_HAS_FEATURE(CXX_ALIGNED_UNION)

    // aligned_union emulation
    namespace std {
        template <decltype(sizeof(0)) Len, typename... T>
        struct aligned_union;
    } // namespace std

#endif // !WHEELS_HAS_FEATURE(CXX_ALIGNED_UNION)

namespace wheels {
    // const-volatile modifications
    //! Removes top-level const
    template <typename T>
    using RemoveConst = Invoke<std::remove_const<T>>;
    //! Removes top-level volatile
    template <typename T>
    using RemoveVolatile = Invoke<std::remove_volatile<T>>;
    //! Removes top-level cv-qualifiers
    template <typename T>
    using RemoveCv = Invoke<std::remove_cv<T>>;
    //! Adds top-level const
    template <typename T>
    using AddConst = Invoke<std::add_const<T>>;
    //! Adds top-level volatile
    template <typename T>
    using AddVolatile = Invoke<std::add_volatile<T>>;
    //! Adds top-level const and volatile
    template <typename T>
    using AddCv = Invoke<std::add_cv<T>>;

    // reference modifications
    //! Removes reference qualifiers
    template <typename T>
    using RemoveReference = Invoke<std::remove_reference<T>>;
    //! Adds an lvalue reference qualifier
    template <typename T>
    using AddLvalueReference = Invoke<std::add_lvalue_reference<T>>;
    //! Adds an rvalue reference qualifier
    /*! Reference collapsing rules apply */
    template <typename T>
    using AddRvalueReference = Invoke<std::add_rvalue_reference<T>>;

    // sign modifications
    //! Obtains the signed counterpart of a given type
    template <typename T>
    using MakeSigned = Invoke<std::make_signed<T>>;
    //! Obtains the unsigned counterpart of a given type
    template <typename T>
    using MakeUnsigned = Invoke<std::make_unsigned<T>>;

    // array modifications
    //! Removes an array type's top-level extent
    template <typename T>
    using RemoveExtent = Invoke<std::remove_extent<T>>;
    //! Removes all the extents of an array type
    template <typename T>
    using RemoveAllExtents = Invoke<std::remove_all_extents<T>>;

    // pointer modifications
    //! Removes a pointer qualifier
    template <typename T>
    using RemovePointer = Invoke<std::remove_pointer<T>>;
    //! Adds a pointer qualifier
    template <typename T>
    using AddPointer = Invoke<std::add_pointer<T>>;

    namespace traits_detail {
        template <typename>
        struct extract_alignment;
        template <std::size_t Len, std::size_t Align>
        struct extract_alignment<std::aligned_storage<Len, Align>> : std::integral_constant<std::size_t, Align> {};

        template <std::size_t Len>
        struct default_alignment : extract_alignment<std::aligned_storage<Len>> {};
    } // namespace traits_detail

    // other transformations
    //! Provides storage with the given length and alignment
    template <std::size_t Len, std::size_t Align = traits_detail::default_alignment<Len>::value>
    using AlignedStorage = Invoke<std::aligned_storage<Len, Align>>;
    //! Provides storage properly aligned for the given types
    template <std::size_t Len, typename... T>
    using AlignedUnion = Invoke<std::aligned_union<Len, T...>>;
    //! Emulates pass-by-value semantics
    template <typename T>
    using Decay = Invoke<std::decay<T>>;
    //! Removes all reference and cv qualifiers
    template <typename T>
    using Bare = RemoveCv<RemoveReference<T>>;

    //! ???
    template <typename... T>
    using CommonType = Invoke<std::common_type<T...>>;
    //! Obtains the underlying type of an enum
    template <typename T>
    using UnderlyingType = Invoke<std::underlying_type<T>>;

    // propagations
    //! Produces Destination as const if Source is const
    template <typename Source, typename Destination>
    using WithConstOf = Conditional<std::is_const<Source>, AddConst<Destination>, Destination>;
    //! Produces Destination as volatile if Source is volatile
    template <typename Source, typename Destination>
    using WithVolatileOf = Conditional<std::is_volatile<Source>, AddVolatile<Destination>, Destination>;
    //! Produces Destination as const if Source is const, and volatile if Source is volatile
    template <typename Source, typename Destination>
    using WithCvOf = WithConstOf<Source, WithVolatileOf<Source, Destination>>;

    //! Produces Destination with the same value category of Source
    template <typename Source, typename Destination>
    using WithValueCategoryOf =
        Conditional<std::is_lvalue_reference<Source>,
            AddLvalueReference<Destination>,
            Conditional<std::is_rvalue_reference<Source>,
                AddRvalueReference<Destination>,
                Destination
            >
        >;

    //! Produces Destination with the same reference and cv-qualifiers of Source
    template <typename Source, typename Destination>
    using WithQualificationsOf = WithValueCategoryOf<Source, WithCvOf<RemoveReference<Source>, Destination>>;

    // other traits
    //! Tests if two types are the same after stripping all qualifiers
    template <typename T, typename U>
    using is_related = std::is_same<Bare<T>, Bare<U>>;

    //! Obtains the value_type nested type of T
    template <typename T>
    struct value_type { using type = typename T::value_type; };

    //! Template alias to obtain the value_type nested type of T
    template <typename T>
    using ValueType = Invoke<value_type<T>>;

    //! Tests if T is a specialization of Template
    template <typename T, template <typename...> class Template>
    struct is_specialization_of : Bool<false> {};
    template <template <typename...> class Template, typename... Args>
    struct is_specialization_of<Template<Args...>, Template> : Bool<true> {};

    //! Provides storage properly aligned for any given T
    template <typename... T>
    struct storage_for : std::aligned_union<1, T...> {};
    template <typename T>
    struct storage_for<T> : std::aligned_storage<sizeof(T), alignof(T)> {};
    template <typename... T>
    using StorageFor = Invoke<storage_for<T...>>;

    //! Obtains the class of a member function pointer
    template <typename T>
    struct class_of : identity<T> {};
    template <typename Signature, typename Class>
    struct class_of<Signature Class::*> : identity<Class> {};
    template <typename T>
    using ClassOf = Invoke<class_of<T>>;
    //! Obtains the function type of a member function pointer
    template <typename T>
    struct signature_of : identity<T> {};
    template <typename Signature, typename Class>
    struct signature_of<Signature Class::*> : identity<Signature> {};
    template <typename T>
    using SignatureOf = Invoke<signature_of<T>>;

    namespace invoke_detail {
        template <typename Fun, typename Obj, typename... Args,
                  EnableIf<std::is_member_function_pointer<Bare<Fun>>,
                           std::is_base_of<ClassOf<Bare<Fun>>, Bare<Obj>>
                          > = _>
        auto invoke(Fun&& fun, Obj&& obj, Args&&... args)
        -> decltype((std::declval<Obj>().*std::declval<Fun>())(std::declval<Args>()...)) {
            return (std::forward<Obj>(obj).*std::forward<Fun>(fun))(std::forward<Args>(args)...);
        }

        template <typename Fun, typename Obj, typename... Args,
                  EnableIf<std::is_member_function_pointer<Bare<Fun>>,
                           Not<std::is_base_of<ClassOf<Bare<Fun>>, Bare<Obj>>>
                          > = _>
        auto invoke(Fun&& fun, Obj&& obj, Args&&... args)
        -> decltype(((*std::declval<Obj>()).*std::declval<Fun>())(std::declval<Args>()...)) {
            return ((*std::forward<Obj>(obj)).*std::forward<Fun>(fun))(std::forward<Args>(args)...);
        }

        template <typename Fun, typename Obj,
                  EnableIf<std::is_member_object_pointer<Bare<Fun>>,
                           std::is_base_of<ClassOf<Bare<Fun>>, Bare<Obj>>
                          > = _>
        auto invoke(Fun&& fun, Obj&& obj)
        -> decltype(std::declval<Obj>().*std::declval<Fun>()) {
            return std::forward<Obj>(obj).*std::forward<Fun>(fun);
        }

        template <typename Fun, typename Obj,
                  EnableIf<std::is_member_object_pointer<Bare<Fun>>,
                           Not<std::is_base_of<ClassOf<Bare<Fun>>, Bare<Obj>>>
                          > = _>
        auto invoke(Fun&& fun, Obj&& obj)
        -> decltype((*std::declval<Obj>()).*std::declval<Fun>()) {
            return (*std::forward<Obj>(obj)).*std::forward<Fun>(fun);
        }

        template <typename Fun, typename... Args,
                  DisableIf<std::is_member_pointer<Bare<Fun>>> = _>
        auto invoke(Fun&& fun, Args&&... args)
        -> decltype(std::declval<Fun>()(std::declval<Args>()...)) {
            return std::forward<Fun>(fun)(std::forward<Args>(args)...);
        }
    } // namespace invoke_detail

    //! Tests if a type is deduced
    /*! Works in conjunction with the type `deduced` */
    template <typename T>
    using is_deduced = std::is_same<T, deduced>;

    //! INVOKE semantics from [func.require]
    template <typename Result = deduced, typename... T,
              EnableIf<is_deduced<Result>> = _>
    auto invoke(T&&... t)
    -> decltype(invoke_detail::invoke(std::declval<T>()...)) {
        return invoke_detail::invoke(std::forward<T>(t)...);
    }

    template <typename Result = deduced, typename... T,
              EnableIf<Not<is_deduced<Result>>,
                       Bool<true, decltype(invoke_detail::invoke(std::declval<T>()...))>,
                       std::is_void<Result>
                      > = _>
    void invoke(T&&... t) {
        invoke_detail::invoke(std::forward<T>(t)...);
    }

    template <typename Result = deduced, typename... T,
              EnableIf<Not<is_deduced<Result>>,
                       Not<std::is_void<Result>>,
                       std::is_convertible<decltype(invoke_detail::invoke(std::declval<T>()...)), Result>
                      > = _>
    Result invoke(T&&... t) {
        return invoke_detail::invoke(std::forward<T>(t)...);
    }

    //! Tests if T is callable with the given Signature
    template <typename T, typename Signature, typename = void>
    struct is_callable : Bool<false> {
        static_assert(std::is_function<Signature>::value, "Signature must be a function type");
    };
    template <typename Fun, typename Ret, typename... Args>
    struct is_callable<Fun, Ret(Args...),
                       Void<decltype(invoke<Ret>(std::declval<Fun>(), std::declval<Args>()...))>>
    : Bool<true> {};

    namespace result_of_detail {
        template <typename T, bool Callable>
        struct impl {};
        template <typename Fun, typename... Args>
        struct impl<Fun(Args...), true> : identity<decltype(invoke(std::declval<Fun>(), std::declval<Args>()...))> {};
    } // namespace result_of_detail
    //! Computes the result of a functor given a set of arguments
    /*! Contrary to std::result_of, it SFINAEs */
    template <typename T>
    struct result_of; // { static_assert(Bool<false, T>::value, "Parameter must be a signature"); };
    template <typename Fun, typename... Args>
    struct result_of<Fun(Args...)> : result_of_detail::impl<Fun(Args...), is_callable<Fun, void(Args...)>::value> {};
    template <typename T>
    using ResultOf = Invoke<result_of<T>>;

    //! Tests if T is a function object type ([function.objects])
    template <typename T, typename Signature>
    struct is_function_object : All<std::is_object<T>, Not<std::is_member_pointer<T>>, is_callable<T, Signature>> {};

    //! Tests if T is a pointer to a function
    template <typename T>
    struct is_function_pointer : All<std::is_pointer<T>, std::is_function<RemovePointer<T>>> {};

    //! Obtains the first element of a parameter pack
    template <typename... T>
    struct pack_head {};
    template <typename Head, typename... Tail>
    struct pack_head<Head, Tail...> : identity<Head> {};
    template <typename... T>
    using PackHead = Invoke<pack_head<T...>>;

    //! Removes function qualifiers from a function or pointer to member function type
    template <typename T>
    struct remove_function_qualifiers : identity<T> {};
    template <typename Class, typename Signature>
    struct remove_function_qualifiers<Signature Class::*> : identity<Invoke<remove_function_qualifiers<Signature>> Class::*> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...)> : identity<Ret(Args...)> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...) const> : identity<Ret(Args...)> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...) volatile> : identity<Ret(Args...)> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...) const volatile> : identity<Ret(Args...)> {};
#if WHEELS_HAS_FEATURE(CXX_REFERENCE_QUALIFIED_FUNCTIONS)
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...)&> : identity<Ret(Args...)> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...) const&> : identity<Ret(Args...)> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...) volatile&> : identity<Ret(Args...)> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...) const volatile&> : identity<Ret(Args...)> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...)&&> : identity<Ret(Args...)> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...) const&&> : identity<Ret(Args...)> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...) volatile&&> : identity<Ret(Args...)> {};
    template <typename Ret, typename... Args>
    struct remove_function_qualifiers<Ret(Args...) const volatile&&> : identity<Ret(Args...)> {};
#endif // WHEELS_HAS_FEATURE(CXX_REFERENCE_QUALIFIED_FUNCTIONS)
    template <typename T>
    using RemoveFunctionQualifiers = Invoke<remove_function_qualifiers<T>>;

    namespace qualifier_detail {
        struct carrier;
    } // namespace qualifier_detail

    //! Converts function qualifiers to object qualifiers
    template <typename T>
    struct from_function_qualifiers : identity<T> {};
    template <typename Class, typename Signature>
    struct from_function_qualifiers<Signature Class::*> : identity<from_function_qualifiers<Signature>>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...)> : identity<qualifier_detail::carrier>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...) const> : identity<qualifier_detail::carrier const>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...) volatile> : identity<qualifier_detail::carrier volatile>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...) const volatile> : identity<qualifier_detail::carrier const volatile>  {};
#if WHEELS_HAS_FEATURE(CXX_REFERENCE_QUALIFIED_FUNCTIONS)
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...)&> : identity<qualifier_detail::carrier>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...) const&> : identity<qualifier_detail::carrier const>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...) volatile&> : identity<qualifier_detail::carrier volatile>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...) const volatile&> : identity<qualifier_detail::carrier const volatile>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...)&&> : identity<qualifier_detail::carrier&&>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...) const&&> : identity<qualifier_detail::carrier const&&>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...) volatile&&> : identity<qualifier_detail::carrier volatile&&>  {};
    template <typename Ret, typename... Args>
    struct from_function_qualifiers<Ret(Args...) const volatile&&> : identity<qualifier_detail::carrier const volatile&&>  {};
#endif // WHEELS_HAS_FEATURE(CXX_REFERENCE_QUALIFIED_FUNCTIONS)
    template <typename Fun>
    using FromFunctionQualifiers = Invoke<from_function_qualifiers<Fun>>;

    //! Converts object qualifiers to function qualifiers
    template <typename Q>
    struct to_function_qualifiers : identity<qualifier_detail::carrier()> {};
    template <typename Q>
    struct to_function_qualifiers<Q const> : identity<qualifier_detail::carrier() const>  {};
    template <typename Q>
    struct to_function_qualifiers<Q volatile> : identity<qualifier_detail::carrier() volatile>  {};
    template <typename Q>
    struct to_function_qualifiers<Q const volatile> : identity<qualifier_detail::carrier() const volatile>  {};
#if WHEELS_HAS_FEATURE(CXX_REFERENCE_QUALIFIED_FUNCTIONS)
    template <typename Q>
    struct to_function_qualifiers<Q&> : identity<qualifier_detail::carrier()&> {};
    template <typename Q>
    struct to_function_qualifiers<Q const&> : identity<qualifier_detail::carrier() const&>  {};
    template <typename Q>
    struct to_function_qualifiers<Q volatile&> : identity<qualifier_detail::carrier() volatile&>  {};
    template <typename Q>
    struct to_function_qualifiers<Q const volatile&> : identity<qualifier_detail::carrier() const volatile&>  {};
    template <typename Q>
    struct to_function_qualifiers<Q&&> : identity<qualifier_detail::carrier()&&> {};
    template <typename Q>
    struct to_function_qualifiers<Q const&&> : identity<qualifier_detail::carrier() const&&>  {};
    template <typename Q>
    struct to_function_qualifiers<Q volatile&&> : identity<qualifier_detail::carrier() volatile&&>  {};
    template <typename Q>
    struct to_function_qualifiers<Q const volatile&&> : identity<qualifier_detail::carrier() const volatile&&>  {};
#endif // WHEELS_HAS_FEATURE(CXX_REFERENCE_QUALIFIED_FUNCTIONS)
    template <typename Q>
    using ToFunctionQualifiers = Invoke<to_function_qualifiers<Q>>;

    //! Unwraps a reference_wrapper type
    template <typename T>
    struct unwrap_reference : identity<T> {};
    template <typename T>
    struct unwrap_reference<std::reference_wrapper<T>> : identity<T&> {};
    template <typename T>
    using UnwrapReference = Invoke<unwrap_reference<T>>;
    //! Unwraps a reference_wrapper
    template <typename T>
    T&& unref(T&& t) { return std::forward<T>(t); }
    template <typename T>
    T& unref(std::reference_wrapper<T> t) { return t.get(); }

    //! Tests if a type is not a reference and has no cv-qualifiers
    template <typename T>
    struct is_bare : std::is_same<T, Bare<T>> {};

    template <typename Tester, typename... T>
    using TraitOf = decltype(Tester::template test<T...>(0));
} // namespace wheels

#if !WHEELS_HAS_FEATURE(CXX_ALIGNED_UNION)

    // aligned_union emulation
    namespace std {
        template <decltype(sizeof(0)) Len, typename... T>
        struct aligned_union
        : ::std::aligned_storage<
              ::wheels::max< ::std::integral_constant<decltype(sizeof(0)), Len>, ::std::integral_constant<decltype(sizeof(0)), sizeof(T)>...>::value,
              ::wheels::max< ::std::integral_constant<decltype(sizeof(0)), alignof(T)>...>::value> {
            static constexpr size_t alignment_value = alignof(::wheels::Invoke<aligned_union>);
        };
    } // namespace std
    #define WHEELS_HAS_FEATURE_CXX_ALIGNED_UNION

#endif // !WHEELS_HAS_FEATURE(CXX_ALIGNED_UNION)

#endif // WHEELS_TYPE_TRAITS_HPP

