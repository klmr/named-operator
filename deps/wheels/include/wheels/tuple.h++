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

// Tuple utilities

#ifndef WHEELS_TUPLE_HPP
#define WHEELS_TUPLE_HPP

#include "meta.h++"

#include <cstddef>
#include <initializer_list>
#include <tuple>
#include <utility>

namespace wheels {
    // TODO: use a simple typelist instead of std::tuple for tag dispatching?

    //! Pack of indices
    template <int... Indices>
    struct indices {
        using next = indices<Indices..., sizeof...(Indices)>;
    };

    //! Indices pack construction
    template <int Size>
    struct build_indices : identity<typename build_indices<Size-1>::type::next> {};
    template <>
    struct build_indices<0> : identity<indices<>> {};
    template <int Size>
    using BuildIndices = Invoke<build_indices<Size>>;

    //! Indices pack for a given tuple
    template <typename Tuple>
    using IndicesFor = BuildIndices<std::tuple_size<Bare<Tuple>>::value>;

    template <std::size_t I, typename T>
    using TupleElement = Invoke<std::tuple_element<I, T>>;

    template <typename T>
    using TupleSize = std::tuple_size<Bare<T>>;

#if defined(WHEELS_STD_TUPLE_LAYOUT_OPTIMAL) || defined(WHEELS_STD_TUPLE_LAYOUT_UNKNOWN)

    template <typename... T>
    using tuple = std::tuple<T...>;
    using std::make_tuple;
    using std::forward_as_tuple;
    using std::tie;
    using std::tuple_cat;
    using std::get;

#else
    template <typename... T>
    struct tuple;

    namespace tuple_detail {
        template <typename T, typename Tuple>
        struct cons_tuple;
        template <typename Head, typename... Tail>
        struct cons_tuple<Head, std::tuple<Tail...>> : identity<std::tuple<Head, Tail...>> {};
        template <typename T, typename Tuple>
        using ConsTuple = typename cons_tuple<T, Tuple>::type;

        template <typename T>
        struct member { T _; }; // need this to get "alignment of a reference"
        template <typename T, typename U>
        struct layout_compare_impl : Bool<
            std::is_empty<T>::value
            || (!std::is_empty<U>::value && alignof(member<T>) > alignof(member<U>))
        > {};
        template <typename T, typename U>
        struct layout_compare
    #if defined(WHEELS_STD_TUPLE_LAYOUT_STRAIGHT)
        : layout_compare_impl<T,U> {};
    #elif defined(WHEELS_STD_TUPLE_LAYOUT_REVERSED)
        : layout_compare_impl<U,T> {};
    #endif

        template <template <typename...> class Template, typename... T>
        struct apply_invoke : identity<Template<Invoke<T>...>> {};

        template <typename T, std::size_t I>
        struct indexed : identity<indexed<T,I>> {
            using t = T;
            static constexpr std::size_t i = I;
        };

        template <typename T, typename Tuple>
        struct insert_sorted
        : identity<std::tuple<T>> {};
        template <typename T, std::size_t I, typename THead, std::size_t IHead, typename... TTail, std::size_t... ITail>
        struct insert_sorted<indexed<T, I>, std::tuple<indexed<THead, IHead>, indexed<TTail, ITail>...>>
        : Conditional<
            layout_compare<T, THead>,
            identity<std::tuple<indexed<T, I>, indexed<THead, IHead>, indexed<TTail, ITail>...>>,
            apply_invoke<ConsTuple, indexed<THead, IHead>, insert_sorted<indexed<T, I>, std::tuple<indexed<TTail, ITail>...>>>
        > {};
        template <typename T, typename Tuple>
        using InsertSorted = Invoke<insert_sorted<T, Tuple>>;

        template <typename Acc, typename... T>
        struct with_indices_impl : identity<Acc> {};
        template <typename... Acc, typename Head, typename... Tail>
        struct with_indices_impl<std::tuple<Acc...>, Head, Tail...> : with_indices_impl<std::tuple<Acc..., indexed<Head, sizeof...(Acc)>>, Tail...> {};
        template <typename... T>
        struct with_indices : with_indices_impl<std::tuple<>, T...> {};
        template <typename... T>
        using WithIndices = Invoke<with_indices<T...>>;
         
        template <std::size_t Target, typename Tuple>
        struct find_index;
        template <std::size_t Target, typename THead, std::size_t From, std::size_t To, typename... Tail>
        struct find_index<Target, std::tuple<indexed<indexed<THead, To>,From>, Tail...>>
        : Conditional<
            Bool<Target == To>,
            std::integral_constant<std::size_t, From>,
            find_index<Target, std::tuple<Tail...>>
        > {};

        template <typename Tuple>
        struct reverse_layout_impl;
        template <typename... T, std::size_t... From, std::size_t... To>
        struct reverse_layout_impl<std::tuple<indexed<indexed<T,To>,From>...>>
        : identity<std::tuple<
            indexed<T, find_index<From, std::tuple<indexed<indexed<T,To>,From>...>>::value>...
        >> {};
        template <typename Tuple>
        struct reverse_layout;
        template <typename... T>
        struct reverse_layout<std::tuple<T...>> : reverse_layout_impl<WithIndices<T...>> {};
        template <typename Tuple>
        using ReverseLayout = Invoke<reverse_layout<Tuple>>;

        template <typename Tuple>
        struct split_impl;
        template <typename... T, std::size_t... I>
        struct split_impl<std::tuple<indexed<T, I>...>> {
            using tuple = std::tuple<T...>;
            using map = std::tuple<std::integral_constant<std::size_t, I>...>;
        };
        template <typename Tuple>
        struct split {
            using tuple = typename split_impl<Tuple>::tuple;
            using map = typename split_impl<Tuple>::map;
            using inverse_map = typename split_impl<ReverseLayout<Tuple>>::map;
        };

        template <typename Acc, typename T>
        struct optimal_layout_impl : split<Acc> {};
         
        template <typename Acc, typename Head, typename... Tail>
        struct optimal_layout_impl<Acc, std::tuple<Head, Tail...>>
        : optimal_layout_impl<InsertSorted<Head, Acc>, std::tuple<Tail...>> {};

        template <typename... T>
        struct optimal_layout : optimal_layout_impl<std::tuple<>, WithIndices<T...>> {};

        template <typename... T>
        using OptimalLayoutTuple = typename optimal_layout<T...>::tuple;
        template <typename... T>
        using OptimalLayoutMap = typename optimal_layout<T...>::map;
        template <typename... T>
        using OptimalLayoutInverseMap = typename optimal_layout<T...>::inverse_map;

        template <typename Acc, typename From, typename To>
        struct convert_layout_map_impl : identity<Acc> {};
        template <typename... Acc, typename From, typename THead, typename... TTail>
        struct convert_layout_map_impl<std::tuple<Acc...>, From, std::tuple<THead, TTail...>>
        : convert_layout_map_impl<std::tuple<Acc..., TupleElement<THead::value, From>>, From, std::tuple<TTail...>> {};
        template <typename From, typename To>
        struct convert_layout_map : convert_layout_map_impl<std::tuple<>, From, To> {};
        template <typename From, typename To>
        using ConvertLayoutMap = Invoke<convert_layout_map<From, To>>;

        template <typename... T, typename... Indices>
        std::tuple<TupleElement<Indices::value, std::tuple<T...>>...> forward_mapped_tuple(std::tuple<Indices...>, std::tuple<T...> t) {
            return std::forward_as_tuple(std::forward<TupleElement<Indices::value, std::tuple<T...>>>(std::get<Indices::value>(t))...);
        }

        template <typename... T, typename... Indices>
        std::tuple<TupleElement<Indices::value, std::tuple<T...>>...> forward_mapped(std::tuple<Indices...> i, T&&... t) {
            return forward_mapped_tuple(i, std::forward_as_tuple(std::forward<T>(t)...));
        }

        template <typename... N>
        struct sum : std::integral_constant<std::size_t, 0> {};
        template <typename Head, typename... Tail>
        struct sum<Head, Tail...> : std::integral_constant<std::size_t, Head::value + sum<Tail...>::value> {};

        template <std::size_t X, std::size_t Y>
        struct position : identity<position<X, Y>> {
            static constexpr std::size_t x = X;
            static constexpr std::size_t y = Y;
        };
        template <std::size_t I, std::size_t X, std::size_t Acc, typename... Tuples>
        struct position_of_impl;
        template <std::size_t I, std::size_t X, std::size_t Acc, typename Head, typename... Tail>
        struct position_of_impl<I, X, Acc, Head, Tail...>
        : Conditional<
            Bool<(std::tuple_size<Bare<Head>>::value + Acc > I)>,
            position<X, I - Acc>,
            position_of_impl<I, X+1, Acc + std::tuple_size<Bare<Head>>::value, Tail...>
        > {};
        template <std::size_t I, typename... Tuples>
        struct position_of : position_of_impl<I, 0, 0, Tuples...> {};
        template <std::size_t I, typename... Tuples>
        using PositionOf = Invoke<position_of<I, Tuples...>>;

        template <typename Tuple, typename Indices = IndicesFor<Tuple>>
        struct to_std_tuple;
        template <typename Tuple, int... Indices>
        struct to_std_tuple<Tuple, indices<Indices...>> : identity<std::tuple<TupleElement<Indices, Tuple>...>> {};
        template <typename Tuple>
        using ToStdTuple = Invoke<to_std_tuple<Tuple>>;

        template <typename Acc, typename... Tuples>
        struct concat_tuples_impl : identity<Acc> {};
        template <typename... Acc, typename... Heads, typename... Tail>
        struct concat_tuples_impl<tuple<Acc...>, std::tuple<Heads...>, Tail...>
        : concat_tuples_impl<tuple<Acc..., Heads...>, Tail...> {};
        template <typename... Tuples>
        struct concat_tuples : concat_tuples_impl<tuple<>, ToStdTuple<Bare<Tuples>>...> {};
        template <typename... Tuples>
        using ConcatTuples = Invoke<concat_tuples<Tuples...>>;

        template <typename P, typename Tuple,
                  typename Inner = TupleElement<P::x, Bare<Tuple>>,
                  typename Outer = TupleElement<P::y, Bare<Inner>>,
                  typename Result = WithQualificationsOf<Inner, Outer>>
        Result get_position(Tuple t) {
            using std::get;
            return get<P::y>(get<P::x>(t));
        }

        template <int... Indices, typename... Tuples>
        auto forward_cat(indices<Indices...>, Tuples&&... t)
        -> decltype(std::forward_as_tuple(
                        get_position<PositionOf<Indices, Tuples...>>(
                            std::forward_as_tuple(std::forward<Tuples>(t)...))...)) {
            return std::forward_as_tuple(
                        get_position<PositionOf<Indices, Tuples...>>(
                            std::forward_as_tuple(std::forward<Tuples>(t)...))...);
        }

        // Goat sacrifice for GCC
        template <typename Ts, typename Us>
        struct pairwise_convertible : Bool<false> {};
        template <>
        struct pairwise_convertible<std::tuple<>, std::tuple<>> : Bool<true> {};
        template <typename THead, typename... TTail, typename UHead, typename... UTail>
        struct pairwise_convertible<std::tuple<THead, TTail...>, std::tuple<UHead, UTail...>>
        : All<std::is_convertible<THead, UHead>, pairwise_convertible<std::tuple<TTail...>, std::tuple<UTail...>>> {};
    } // namespace tuple_detail

    template <typename... T>
    struct tuple {
    private:
        using layout_tuple = tuple_detail::OptimalLayoutTuple<T...>;
        using layout_map = tuple_detail::OptimalLayoutMap<T...>;
        using layout_inverse_map = tuple_detail::OptimalLayoutInverseMap<T...>;

        template <typename Other>
        using LayoutFor = tuple_detail::ConvertLayoutMap<typename Other::layout_map, layout_map>;

    public:
        constexpr tuple() = default;

        explicit tuple(T const&... t)
        : inner(tuple_detail::forward_mapped(layout_map{}, t...)) {
            static_assert(All<std::is_copy_constructible<T>...>::value, "all tuple element types must be copy constructible");
        }
     
        // TODO: get this to compile and remove the hack: EnableIf<std::is_convertible<U, T>...> = _>
        template <typename... U,
                  EnableIf<tuple_detail::pairwise_convertible<std::tuple<U...>, std::tuple<T...>>> = _>
        explicit tuple(U&&... u)
        : inner(tuple_detail::forward_mapped(layout_map{}, std::forward<U>(u)...)) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
        }

        tuple(tuple const&) = default;
        tuple(tuple&&) = default;

        template <typename... U,
                  EnableIf<std::is_constructible<T, U const&>...> = _>
        tuple(tuple<U...> const& t)
        : inner(tuple_detail::forward_mapped_tuple(LayoutFor<tuple<U...>>{}, t.inner)) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
        }
        template <typename... U,
                  EnableIf<std::is_constructible<T, U&&>...> = _>
        tuple(tuple<U...>&& t)
        : inner(tuple_detail::forward_mapped_tuple(LayoutFor<tuple<U...>>{}, std::move(t.inner))) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
        }

        template <typename U1, typename U2,
                  EnableIf<std::is_convertible<U1 const&, TupleElement<0, std::tuple<T...>>>,
                           std::is_convertible<U2 const&, TupleElement<1, std::tuple<T...>>>> = _>
        tuple(std::pair<U1, U2> const& pair)
        : tuple(pair.first, pair.second) {
            static_assert(sizeof...(T) == 2, "tuple size must be 2");
        }
        template <typename U1, typename U2,
                  EnableIf<std::is_convertible<U1, TupleElement<0, std::tuple<T...>>>,
                           std::is_convertible<U2, TupleElement<1, std::tuple<T...>>>> = _>
        tuple(std::pair<U1, U2>&& pair)
        : tuple(std::forward<U1>(pair.first), std::forward<U2>(pair.second)) {
            static_assert(sizeof...(T) == 2, "tuple size must be 2");
        }

        template <typename... U,
                  EnableIf<std::is_constructible<T, U const&>...> = _>
        tuple(std::tuple<U...> const& that)
        : inner(tuple_detail::forward_mapped_tuple(layout_map{}, that)) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
        }
        template <typename... U,
                  EnableIf<std::is_constructible<T, U&&>...> = _>
        tuple(std::tuple<U...>&& that)
        : inner(tuple_detail::forward_mapped_tuple(layout_map{}, std::move(that))) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
        }

        template <typename Alloc>
        tuple(std::allocator_arg_t tag, Alloc const& a)
        : inner(tag, a) {}

        template <typename Alloc>
        explicit tuple(std::allocator_arg_t tag, Alloc const& a, T const&... t)
        : inner(tag, a, tuple_detail::forward_mapped(layout_map{}, t...)) {
            static_assert(All<std::is_copy_constructible<T>...>::value, "all tuple element types must be copy constructible");
        }
     
        template <typename Alloc, typename... U,
                  EnableIf<tuple_detail::pairwise_convertible<std::tuple<U...>, std::tuple<T...>>> = _>
        explicit tuple(std::allocator_arg_t tag, Alloc const& a, U&&... u)
        : inner(tag, a, tuple_detail::forward_mapped(layout_map{}, std::forward<U>(u)...)) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
        }

        template <typename Alloc>
        tuple(std::allocator_arg_t tag, Alloc const& a, tuple const& that)
        : inner(tag, a, that.inner) {}

        template <typename Alloc>
        tuple(std::allocator_arg_t tag, Alloc const& a, tuple&& that)
        : inner(tag, a, std::move(that.inner)) {}

        template <typename Alloc, typename... U,
                  EnableIf<std::is_constructible<T, U const&>...> = _>
        tuple(std::allocator_arg_t tag, Alloc const& a, tuple<U...> const& t)
        : inner(tag, a, tuple_detail::forward_mapped_tuple(LayoutFor<tuple<U...>>{}, t.inner)) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
        }
        template <typename Alloc, typename... U,
                  EnableIf<std::is_constructible<T, U&&>...> = _>
        tuple(std::allocator_arg_t tag, Alloc const& a, tuple<U...>&& t)
        : inner(tag, a, tuple_detail::forward_mapped_tuple(LayoutFor<tuple<U...>>{}, std::move(t.inner))) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
        }

        template <typename Alloc, typename U1, typename U2,
                  EnableIf<std::is_convertible<U1 const&, TupleElement<0, std::tuple<T...>>>,
                           std::is_convertible<U2 const&, TupleElement<1, std::tuple<T...>>>> = _>
        tuple(std::allocator_arg_t tag, Alloc const& a, std::pair<U1, U2> const& pair)
        : tuple(tag, a, pair.first, pair.second) {
            static_assert(sizeof...(T) == 2, "tuple size must be 2");
        }
        template <typename Alloc, typename U1, typename U2,
                  EnableIf<std::is_convertible<U1, TupleElement<0, std::tuple<T...>>>,
                           std::is_convertible<U2, TupleElement<1, std::tuple<T...>>>> = _>
        tuple(std::allocator_arg_t tag, Alloc const& a, std::pair<U1, U2>&& pair)
        : tuple(tag, a, std::forward<U1>(pair.first), std::forward<U2>(pair.second)) {
            static_assert(sizeof...(T) == 2, "tuple size must be 2");
        }

        template <typename Alloc, typename... U,
                  EnableIf<std::is_constructible<T, U const&>...> = _>
        tuple(std::allocator_arg_t tag, Alloc const& a, std::tuple<U...> const& that)
        : inner(tag, a, tuple_detail::forward_mapped_tuple(layout_map{}, that)) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
        }
        template <typename Alloc, typename... U,
                  EnableIf<std::is_constructible<T, U&&>...> = _>
        tuple(std::allocator_arg_t tag, Alloc const& a, std::tuple<U...>&& that)
        : inner(tag, a, tuple_detail::forward_mapped_tuple(layout_map{}, std::move(that))) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
        }

        tuple& operator=(tuple const&) = default;
        tuple& operator=(tuple&&) = default;

        template <typename... U>
        tuple& operator=(tuple<U...> const& t) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
            static_assert(All<std::is_assignable<T&, U const&>...>::value,
                          "all arguments must be assignable to the corresponding element");

            inner = tuple_detail::forward_mapped_tuple(LayoutFor<tuple<U...>>{}, t.inner);
            return *this;
        }
        template <typename... U>
        tuple& operator=(tuple<U...>&& t) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
            static_assert(All<std::is_assignable<T&, U&&>...>::value,
                          "all arguments must be assignable to the corresponding element");

            inner = tuple_detail::forward_mapped_tuple(LayoutFor<tuple<U...>>{}, std::move(t.inner));
            return *this;
        }
     
        template <typename U1, typename U2>
        tuple& operator=(std::pair<U1, U2> const& pair) {
            static_assert(sizeof...(T) == 2, "tuple size must be 2");
            static_assert(std::is_assignable<TupleElement<0, std::tuple<T...>>&, U1 const&>::value
                          && std::is_assignable<TupleElement<1, std::tuple<T...>>&, U2 const&>::value,
                          "pair elements must be assignable to the corresponding element");
            inner = tuple_detail::forward_mapped(layout_map{}, pair.first, pair.second);
            return *this;
        }
        template <typename U1, typename U2>
        tuple& operator=(std::pair<U1, U2>&& pair) {
            static_assert(sizeof...(T) == 2, "tuple size must be 2");
            static_assert(std::is_assignable<TupleElement<0, std::tuple<T...>>&, U1&&>::value
                          && std::is_assignable<TupleElement<1, std::tuple<T...>>&, U2&&>::value,
                          "pair elements must be assignable to the corresponding element");
            inner = tuple_detail::forward_mapped(layout_map{}, std::forward<U1>(pair.first), std::forward<U2>(pair.second));
            return *this;
        }

        template <typename... U>
        tuple& operator=(std::tuple<U...> const& that) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
            static_assert(All<std::is_assignable<T&, U const&>...>::value,
                          "all arguments must be assignable to the corresponding element");
            inner = tuple_detail::forward_mapped_tuple(layout_map{}, that);
            return *this;
        }
        template <typename... U>
        tuple& operator=(std::tuple<U...>&& that) {
            static_assert(sizeof...(T) == sizeof...(U), "number of constructor parameters must match tuple size");
            static_assert(All<std::is_assignable<T&, U&&>...>::value,
                          "all arguments must be assignable to the corresponding element");
            inner = tuple_detail::forward_mapped_tuple(layout_map{}, std::move(that));
            return *this;
        }

        void swap(tuple& that)
        noexcept(noexcept(std::declval<layout_tuple>().swap(std::declval<layout_tuple>()))) {
            inner.swap(that.inner);
        }

        template <std::size_t I, typename... T1>
        friend TupleElement<I, std::tuple<T1...>>& get(tuple<T1...>& t) noexcept;
        template <std::size_t I, typename... T1>
        friend TupleElement<I, std::tuple<T1...>>&& get(tuple<T1...>&& t) noexcept;
        template <std::size_t I, typename... T1>
        friend TupleElement<I, std::tuple<T1...>> const& get(tuple<T1...> const& t) noexcept;
        template <typename... L, typename... R>
        friend bool operator==(tuple<L...> const& t, tuple<R...> const& u);
        template <typename... L, typename... R>
        friend bool operator<(tuple<L...> const& t, tuple<R...> const& u);
        template <typename...>
        friend class tuple;
     
    private:
        layout_tuple inner;
    };

    template <typename... T>
    tuple<UnwrapReference<Decay<T>>...> make_tuple(T&&... t) {
        return tuple<UnwrapReference<Decay<T>>...>(std::forward<T>(t)...);
    }

    template <typename... T>
    tuple<T&&...> forward_as_tuple(T&&... t) noexcept {
        return tuple<T&&...>(std::forward<T>(t)...);
    }

    template <typename... T>
    tuple<T&...> tie(T&... t) noexcept {
        return tuple<T&...>(t...);
    }

    template <typename... Tuples,
              typename Concatenated = tuple_detail::ConcatTuples<Tuples...>>
    Concatenated tuple_cat(Tuples&&... t) {
        return Concatenated {
                tuple_detail::forward_cat(
                    BuildIndices<tuple_detail::sum<std::tuple_size<Bare<Tuples>>...>::value>{},
                    std::forward<Tuples>(t)...)
        };
    }

    template <std::size_t I, typename... T>
    TupleElement<I, std::tuple<T...>>& get(tuple<T...>& t) noexcept {
        return std::get<TupleElement<I, typename tuple<T...>::layout_inverse_map>::value>(t.inner);
    }
    template <std::size_t I, typename... T>
    TupleElement<I, std::tuple<T...>>&& get(tuple<T...>&& t) noexcept {
        return std::get<TupleElement<I, typename tuple<T...>::layout_inverse_map>::value>(std::move(t.inner));
    }
    template <std::size_t I, typename... T>
    TupleElement<I, std::tuple<T...>> const& get(tuple<T...> const& t) noexcept {
        return std::get<TupleElement<I, typename tuple<T...>::layout_inverse_map>::value>(t.inner);
    }

    template <typename... T>
    void swap(tuple<T...>& x, tuple<T...>& y) noexcept(std::declval<tuple<T...>&>().swap(std::declval<tuple<T...>&>())) {
        x.swap(y);
    }

    template <typename... T, typename... U>
    bool operator==(tuple<T...> const& t, tuple<U...> const& u) {
        return t.inner == u.inner;
    }
    template <typename... T, typename... U>
    bool operator<(tuple<T...> const& t, tuple<U...> const& u) {
        return t.inner < u.inner;
    }
    template <typename... T, typename... U>
    bool operator!=(tuple<T...> const& t, tuple<U...> const& u) {
        return !(t == u);
    }
    template <typename... T, typename... U>
    bool operator>(tuple<T...> const& t, tuple<U...> const& u) {
        return u < t;
    }
    template <typename... T, typename... U>
    bool operator<=(tuple<T...> const& t, tuple<U...> const& u) {
        return !(u < t);
    }
    template <typename... T, typename... U>
    bool operator>=(tuple<T...> const& t, tuple<U...> const& u) {
        return !(t < u);
    }
#endif // defined(WHEELS_STD_TUPLE_LAYOUT_OPTIMAL) || defined(WHEELS_STD_TUPLE_LAYOUT_UNKNOWN)
} // namespace wheels

namespace std {
    template <typename... T>
    struct tuple_size< ::wheels::tuple<T...>> : tuple_size<tuple<T...>> {};

    template <size_t I, typename... T>
    struct tuple_element<I, ::wheels::tuple<T...>> : tuple_element<I, tuple<T...>> {};

    template <typename... T, typename Alloc>
    struct uses_allocator< ::wheels::tuple<T...>, Alloc> : true_type {};
} // namespace std

#endif // WHEELS_TUPLE_HPP

