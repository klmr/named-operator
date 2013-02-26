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

// Enum manipulation features

#ifndef WHEELS_ENUMS_HPP
#define WHEELS_ENUMS_HPP

#include "detail/config.h++"

#include "meta.h++"

namespace wheels {
    //! Enables the bitwise operators for an enum.
    /*! Use this trait to mark enums that are a collection of flags. */
    template <typename Enum>
    struct is_flags : Bool<false> {};

    //! Enables the arithmetic operators for an enum.
    /*! Use this trait to mark enums that are a collection of named numbers. */
    template <typename Enum>
    struct is_arithmetic : Bool<false> {};

    //! Returns the underlying value of an given enum value
    template <typename Enum>
    constexpr UnderlyingType<Enum> to_underlying(Enum e) {
        return static_cast<UnderlyingType<Enum>>(e);
    }
} // namespace wheels

#define WHEELS_ENUM_UNARY_OPERATOR(OP, KIND) \
    template <typename Enum, \
              ::wheels::EnableIf< ::std::is_enum<Enum>, ::wheels::is_##KIND<Enum>> = ::wheels::_> \
    constexpr Enum operator OP(Enum const& value) { \
        return static_cast<Enum>(OP wheels::to_underlying(value)); \
    }

#define WHEELS_ENUM_BINARY_OPERATOR(OP, KIND) \
    template <typename Enum, \
              ::wheels::EnableIf< ::std::is_enum<Enum>, ::wheels::is_##KIND<Enum>> = ::wheels::_> \
    constexpr Enum operator OP(Enum const& lhs, Enum const& rhs) { \
        return static_cast<Enum>(wheels::to_underlying(lhs) OP wheels::to_underlying(rhs)); \
    } \
    template <typename Enum, \
              ::wheels::EnableIf< ::std::is_enum<Enum>, ::wheels::is_##KIND<Enum>> = ::wheels::_> \
    Enum operator OP##=(Enum& lhs, Enum const& rhs) { \
        return lhs = lhs OP rhs; \
    }

WHEELS_ENUM_BINARY_OPERATOR(|, flags)
WHEELS_ENUM_BINARY_OPERATOR(&, flags)
WHEELS_ENUM_BINARY_OPERATOR(^, flags)
WHEELS_ENUM_UNARY_OPERATOR(~, flags)

namespace wheels {
    //! Tests if an enum value has all the given flags set
    template <typename Enum,
              EnableIf<std::is_enum<Enum>, is_flags<Enum>> = _>
    constexpr bool has_flag(Enum value, Enum flag) {
        return (value & flag) == flag;
    }
    //! Returns enum value, but with the given flags set
    template <typename Enum,
              EnableIf<std::is_enum<Enum>, is_flags<Enum>> = _>
    constexpr Enum set_flag(Enum value, Enum flag) {
        return value | flag;
    }
    //! Returns enum value, but with the given flags cleared
    template <typename Enum,
              EnableIf<std::is_enum<Enum>, is_flags<Enum>> = _>
    constexpr Enum clear_flag(Enum value, Enum flag) {
        return value & ~flag;
    }
    //! Returns enum value, but with the given flags toggled
    template <typename Enum,
              EnableIf<std::is_enum<Enum>, is_flags<Enum>> = _>
    constexpr Enum toggle_flag(Enum value, Enum flag) {
        return value ^ flag;
    }
} // namespace wheels

WHEELS_ENUM_BINARY_OPERATOR(+, arithmetic)
WHEELS_ENUM_BINARY_OPERATOR(-, arithmetic)
WHEELS_ENUM_BINARY_OPERATOR(*, arithmetic)
WHEELS_ENUM_BINARY_OPERATOR(/, arithmetic)
WHEELS_ENUM_BINARY_OPERATOR(%, arithmetic)
WHEELS_ENUM_UNARY_OPERATOR(+, arithmetic)
WHEELS_ENUM_UNARY_OPERATOR(-, arithmetic)

#undef WHEELS_ENUM_UNARY_OPERATOR
#undef WHEELS_ENUM_BINARY_OPERATOR

template <typename Enum,
          wheels::EnableIf<std::is_enum<Enum>, wheels::is_arithmetic<Enum>> = wheels::_>
Enum& operator++(Enum& value) {
    return value = static_cast<Enum>(wheels::to_underlying(value) + 1);
}

template <typename Enum,
          wheels::EnableIf<std::is_enum<Enum>, wheels::is_arithmetic<Enum>> = wheels::_>
Enum& operator--(Enum& value) {
    return value = static_cast<Enum>(wheels::to_underlying(value) - 1);
}

template <typename Enum,
          wheels::EnableIf<std::is_enum<Enum>, wheels::is_arithmetic<Enum>> = wheels::_>
Enum operator++(Enum& value, int) {
    auto temp = value;
    value = static_cast<Enum>(wheels::to_underlying(value) + 1);
    return temp;
}

template <typename Enum,
          wheels::EnableIf<std::is_enum<Enum>, wheels::is_arithmetic<Enum>> = wheels::_>
Enum operator--(Enum& value,int) {
    auto temp = value;
    value = static_cast<Enum>(wheels::to_underlying(value) - 1);
    return temp;
}

#endif // WHEELS_ENUMS_HPP

