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

// Clang feature detection and workarounds

#ifndef WHEELS_DETAIL_CLANG_HPP
#define WHEELS_DETAIL_CLANG_HPP

// Clang feature set can be (mostly) computed :)
#define WHEELS_COMPILER_FEATURES (\
    (__has_feature(cxx_access_control_sfinae)          << WHEELS_CXX_ACCESS_CONTROL_SFINAE)            | \
    (__has_feature(cxx_alias_templates)                << WHEELS_CXX_ALIAS_TEMPLATES)                  | \
    (__has_feature(cxx_alignas)                        << WHEELS_CXX_ALIGNAS)                          | \
    (__has_feature(cxx_attributes)                     << WHEELS_CXX_ATTRIBUTES)                       | \
    (__has_feature(cxx_constexpr)                      << WHEELS_CXX_CONSTEXPR)                        | \
    (__has_feature(cxx_decltype)                       << WHEELS_CXX_DECLTYPE)                         | \
    (__has_feature(cxx_default_function_template_args) << WHEELS_CXX_DEFAULT_FUNCTION_TEMPLATE_ARGS)   | \
    (__has_feature(cxx_defaulted_functions)            << WHEELS_CXX_DEFAULTED_FUNCTIONS)              | \
    (__has_feature(cxx_delegating_constructors)        << WHEELS_CXX_DELEGATING_CONSTRUCTORS)          | \
    (__has_feature(cxx_deleted_functions)              << WHEELS_CXX_DELETED_FUNCTIONS)                | \
    (__has_feature(cxx_explicit_conversions)           << WHEELS_CXX_EXPLICIT_CONVERSIONS)             | \
    (__has_feature(cxx_generalized_initializers)       << WHEELS_CXX_GENERALIZED_INITIALIZERS)         | \
    (__has_feature(cxx_implicit_moves)                 << WHEELS_CXX_IMPLICIT_MOVES)                   | \
    (__has_feature(cxx_inheriting_constructors)        << WHEELS_CXX_INHERITING_CONSTRUCTORS)          | \
    (__has_feature(cxx_inline_namespaces)              << WHEELS_CXX_INLINE_NAMESPACES)                | \
    (__has_feature(cxx_lambdas)                        << WHEELS_CXX_LAMBDAS)                          | \
    (__has_feature(cxx_local_type_template_args)       << WHEELS_CXX_LOCAL_TYPE_TEMPLATE_ARGS)         | \
    (__has_feature(cxx_noexcept)                       << WHEELS_CXX_NOEXCEPT)                         | \
    (__has_feature(cxx_nonstatic_member_init)          << WHEELS_CXX_NONSTATIC_MEMBER_INIT)            | \
    (__has_feature(cxx_nullptr)                        << WHEELS_CXX_NULLPTR)                          | \
    (__has_feature(cxx_override_control)               << WHEELS_CXX_OVERRIDE_CONTROL)                 | \
    (__has_feature(cxx_reference_qualified_functions)  << WHEELS_CXX_REFERENCE_QUALIFIED_FUNCTIONS)    | \
    (__has_feature(cxx_range_for)                      << WHEELS_CXX_RANGE_FOR)                        | \
    (__has_feature(cxx_raw_string_literals)            << WHEELS_CXX_RAW_STRING_LITERALS)              | \
    (__has_feature(cxx_rvalue_references)              << WHEELS_CXX_RVALUE_REFERENCES)                | \
    (__has_feature(cxx_static_assert)                  << WHEELS_CXX_STATIC_ASSERT)                    | \
    (__has_feature(cxx_auto_type)                      << WHEELS_CXX_AUTO_TYPE)                        | \
    (__has_feature(cxx_strong_enums)                   << WHEELS_CXX_STRONG_ENUMS)                     | \
    (__has_feature(cxx_trailing_return)                << WHEELS_CXX_TRAILING_RETURN)                  | \
    (__has_feature(cxx_unicode_literals)               << WHEELS_CXX_UNICODE_LITERALS)                 | \
    (__has_feature(cxx_unrestricted_unions)            << WHEELS_CXX_UNRESTRICTED_UNIONS)              | \
    (__has_feature(cxx_user_literals)                  << WHEELS_CXX_USER_LITERALS)                    | \
    (__has_feature(cxx_variadic_templates)             << WHEELS_CXX_VARIADIC_TEMPLATES)               | \
0)

#define WHEELS_UNREACHABLE do { __builtin_unreachable(); } while(false)

#define WHEELS_WARNING(MESSAGE) \
    WHEELS_PRAGMA(message #MESSAGE) \
    WHEELS_EXPECT_SEMICOLON

#define WHEELS_ISSUE_BROKEN_CONCEPT_CHECK
#define WHEELS_ISSUE_BROKEN_OVERLOAD_MACRO

#endif // WHEELS_DETAIL_CLANG_HPP

