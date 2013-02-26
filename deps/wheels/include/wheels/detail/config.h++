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

// Configuration and compiler workarounds

#ifndef WHEELS_DETAIL_CONFIG_HPP
#define WHEELS_DETAIL_CONFIG_HPP

//! Feature test macro
#define WHEELS_HAS_FEATURE(X) (defined(WHEELS_HAS_FEATURE_##X) || ((WHEELS_COMPILER_FEATURES & (1 << WHEELS_##X)) != 0))
//! Bug test macro
#define WHEELS_HAS_ISSUE(X) (defined(WHEELS_ISSUE_##X))

// Bugs currently considered:
// INCOMPLETE_IS_CONVERTIBLE    : std::is_convertible is not complete in libstdc++
// DURATION_NO_CONSTEXPR        : std::duration has no constexpr members in libc++
// BROKEN_CONCEPT_CHECK         : clang doesn't seem to play nice with Boost.ConceptCheck
// BROKEN_OVERLOAD_MACRO        : clang doesn't seem to play nice with the use of Boost.Preprocessor in WHEELS_OVERLOAD

//! _Pragma helper that stringizes arguments
#define WHEELS_PRAGMA(...) _Pragma(#__VA_ARGS__)

#define WHEELS_EXPECT_SEMICOLON static_assert(true, "")

// Compiler dependent macros defined by this header:
// WHEELS_UNREACHABLE       : marks code that is not reachable
// WHEELS_WARNING(MESSAGE)  : generates a warning message with the given string literal

// Standard library dependent macros defined by this header:
// WHEELS_STD_TUPLE_LAYOUT_OPTIMAL  : if std::tuple already has optimal layout
// WHEELS_STD_TUPLE_LAYOUT_STRAIGHT : if std::tuple lays fields in order of template parameters
// WHEELS_STD_TUPLE_LAYOUT_REVERSED : if std::tuple lays fields in reverse order of template parameters
// WHEELS_STD_TUPLE_LAYOUT_UNKNOWN  : if std::tuple layout is unknown, or not one of the above

// C++11 features
//  from clang's list
#define WHEELS_CXX_ACCESS_CONTROL_SFINAE            1
#define WHEELS_CXX_ALIAS_TEMPLATES                  2
#define WHEELS_CXX_ALIGNAS                          3
#define WHEELS_CXX_ATTRIBUTES                       4
#define WHEELS_CXX_CONSTEXPR                        5
#define WHEELS_CXX_DECLTYPE                         6
#define WHEELS_CXX_DEFAULT_FUNCTION_TEMPLATE_ARGS   7
#define WHEELS_CXX_DEFAULTED_FUNCTIONS              8
#define WHEELS_CXX_DELEGATING_CONSTRUCTORS          9
#define WHEELS_CXX_DELETED_FUNCTIONS               10
#define WHEELS_CXX_EXPLICIT_CONVERSIONS            11
#define WHEELS_CXX_GENERALIZED_INITIALIZERS        12
#define WHEELS_CXX_IMPLICIT_MOVES                  13
#define WHEELS_CXX_INHERITING_CONSTRUCTORS         14
#define WHEELS_CXX_INLINE_NAMESPACES               15
#define WHEELS_CXX_LAMBDAS                         16
#define WHEELS_CXX_LOCAL_TYPE_TEMPLATE_ARGS        17
#define WHEELS_CXX_NOEXCEPT                        18
#define WHEELS_CXX_NONSTATIC_MEMBER_INIT           19
#define WHEELS_CXX_NULLPTR                         20
#define WHEELS_CXX_OVERRIDE_CONTROL                21
#define WHEELS_CXX_REFERENCE_QUALIFIED_FUNCTIONS   22
#define WHEELS_CXX_RANGE_FOR                       23
#define WHEELS_CXX_RAW_STRING_LITERALS             24
#define WHEELS_CXX_RVALUE_REFERENCES               25
#define WHEELS_CXX_STATIC_ASSERT                   26
#define WHEELS_CXX_AUTO_TYPE                       27
#define WHEELS_CXX_STRONG_ENUMS                    28
#define WHEELS_CXX_TRAILING_RETURN                 29
#define WHEELS_CXX_UNICODE_LITERALS                30
#define WHEELS_CXX_UNRESTRICTED_UNIONS             31
#define WHEELS_CXX_USER_LITERALS                   32
#define WHEELS_CXX_VARIADIC_TEMPLATES              33

// some standard libraries don't support aligned_union yet
#define WHEELS_CXX_ALIGNED_UNION                    0
// some standard libraries don't support threads yet
#define WHEELS_CXX_THREADS                          0

// Compiler detection
#if defined(__clang__)
    #include "clang.h++"
#elif defined(__GNUC__)
    #include "gcc.h++"
#else
    #include "compiler-generic.h++"
#endif

// Standard library detection
// Needs to include a stdlib header for detection :(
#include <utility>
#if defined(__GLIBCXX__)
    #include "libstdc++.h++"
#elif defined(_LIBCPP_VERSION)
    #include "libc++.h++"
#else
    #include "stdlib-generic.h++"
#endif

#endif // WHEELS_DETAIL_CONFIG_HPP

