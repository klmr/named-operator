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

// Named parameter tools

#ifndef WHEELS_NAMED_PARAM_HPP
#define WHEELS_NAMED_PARAM_HPP

#include "meta.h++"

#define WHEELS_DEFINE_NAMED_PARAMETER(NAME) \
    struct NAME##_name : ::wheels::named_parameter_detail::name<NAME##_name> { \
        using ::wheels::named_parameter_detail::name<NAME##_name>::operator=; \
    } constexpr NAME {}; \
    template <typename T> \
    using NAME##_parameter = ::wheels::named_parameter<NAME##_name, T>; \
    WHEELS_EXPECT_SEMICOLON

namespace wheels {
    template <typename Name, typename T>
    struct named_parameter {
    public:
        static_assert(std::is_reference<T>::value, "T must be a reference type");
        static_assert(is_bare<Name>::value, "Name must be a bare type");

        using name = Name;
        using type = T;

        constexpr named_parameter(T t) : value(std::forward<T>(t)) {}

        named_parameter& operator=(named_parameter const&) = delete;

        T forward() const { return std::forward<T>(value); }

    private:
        T value;
    };

    template <typename Param>
    using ParameterName = typename Param::name;
    template <typename Param>
    using ParameterType = typename Param::type;

    template <typename Name, typename... T>
    struct has_parameter : Any<std::is_same<ParameterName<T>, Name>...> {};

    template <typename Name, typename... T>
    struct get_parameter {};
    template <typename Name, typename Head, typename... Tail>
    struct get_parameter<Name, Head, Tail...>
    : Conditional<
        std::is_same<ParameterName<Head>, Name>,
        identity<Head>,
        get_parameter<Name, Tail...>
    > {};
    template <typename Name, typename... T>
    using GetParameter = Invoke<get_parameter<Name, T...>>;
    template <typename Name, typename... T>
    using GetParameterType = ParameterType<GetParameter<Name, T...>>;

    namespace named_parameter_detail {
        template <typename Name>
        struct name {
            constexpr name() = default;
            template <typename T>
            constexpr named_parameter<Name, T&&> operator=(T&& t) const {
                return { std::forward<T>(t) };
            }
            name(name const&) = delete;
            name& operator=(name const&) = delete;
        };
    } // namespace named_parameter_detail

    template <typename T>
    struct is_named_parameter : is_specialization_of<T, named_parameter> {};

    template <typename... T>
    struct no_named_parameters : Not<Any<is_named_parameter<T>...>> {};

    template <typename... T>
    struct named_parameter_count : count<is_named_parameter<T>...> {};

    template <typename Name, typename Head, typename... Tail>
    Head forward_named(Name const&, named_parameter<Name, Head> const& param, Tail const&...) {
        return param.forward();
    }

    template <typename Name, typename Head, typename... Tail>
    auto forward_named(Name const& name, Head const&, Tail const&... tail)
    -> decltype(forward_named(name, tail...)) {
        return forward_named(name, tail...);
    }
} // namespace wheels

#endif // WHEELS_NAMED_PARAM_HPP

