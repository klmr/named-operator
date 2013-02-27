# Named operators

Named operators are (user-defined) operators which have names rather than symbols. Here’s Haskell:

```haskell
x = a `div` b
```

and [here’s R](http://stat.ethz.ch/R-manual/R-patched/library/base/html/match.html):

```R
yup <- 4 %in% c(1, 2, 3, 4, 5)
```

In fact, [C++ also has named operators](http://gcc.gnu.org/onlinedocs/cpp/C_002b_002b-Named-Operators.html) – alternative tokens for the primary ones defined, in §2.6.

But those are fixed and not redefinable. Sure, you can `#define` your own names for tokens …

    #define PLUS +

But this has all the usual disadvantages of macros and limits you to the already existing binary operators. Until now.

Take a look at this fully valid, macro-free, compiling and running C++ code:

```c++
int x = 42;
int y = 23;
auto z = x <divmod> y; // calculates { x / y, x % y }
```

You want assignment operators? Not a problem:

```c++
vector<int> vec{ 1, 2, 3 };
vec <append>= 4;
// same as:
vec = vec <append> 4;
```

## Definition

Operators can be defined for any binary function-like object by calling `make_named_operator`:

```c++
auto divmod = make_named_operator(divmod_f);
```

where

```c++
pair<int, int> divmod_f(int x, int y) {
    return { x / y, x % y };
}
```

Or, if you prefer functors (and yes, templates work just fine):

```c++
auto append = make_named_operator(append_t());
```

with

```c++
struct append_t {
    template <typename T>
    vector<T> operator ()(vector<T> const& vs, T const& v) const {
        auto copy(vs);
        copy.push_back(v);
        return copy;
    }
};
```

And of course lambdas work as well:

```c++
auto in = make_named_operator(
    [](int i, vector<int> const& x) {
        return find(begin(x), end(x), i) != end(x);
    });

// …

bool result = 24 <in> vec;
```

## Design rationale & implementation

Overloading operators with unconventional semantics generally frowned upon because it violates the user’s expectations (although it has [variously](http://www.boost.org/doc/libs/1_53_0/libs/assign/doc/index.html) been used [to great effect](http://boost-spirit.com)).

Furthermore, the set of operators that can be created in this fashion is limited to a subset of the built-in operators.

On the other hand, using infix notation instead of function calls can undeniably make code more readable, especially when nesting lots of operations. Compare

```c++
auto result = contains(set_minus(set_minus(A, B), C), x);
```

and

```c++
auto result = x <in> (A <set_minus> B <set_minus> C);
```

Other languages have recognised and addressed this problem.

Since C++ allows overloading operators for custom types, named operators can be implemented by simply sticking a place-holder object between two overloaded operators (which can be entirely arbitrary):

```c++
struct some_tag {} op;
struct op_temporary {};
op_temporary operator <(int lhs, some_tag rhs);
int operator >(op_temporary lhs, int rhs);
```

These declarations are enough to make the following syntax valid:

```c++
int x, y, z;
z = x <op> y;
```

Of course, what the compiler really sees is

```c++
z = operator>(operator<(x, op), y);
```

This already highlights a problem: operator precedence. In effect, `op` will have the precedence of its surrounding operators (and woe if those don’t match!). In particular, the precedence of `<` and `>` is very low. However, I don’t believe that this is too big a problem: somebody once remarked to me,

> C++ really only has two precedence rules: (1) [BODMAS](http://en.wikipedia.org/wiki/Order_of_operations#Mnemonics); (2) for everything else use parentheses.

While [I don’t quite agree with this](http://stackoverflow.com/questions/4968033/why-does-casting-int-to-bool-gives-a-warning/4968060#comment5545079_4968060), I think it’s the right attitude in the case of named “operators” that are added via what is effectively a language hack: be on the safe side, use parentheses.

The implementation itself is straightforward: the first operator caches the left-hand side of the expression in `named_operator_lhs`, and the second operator performs the given operation on the cached value and the right-hand side. As we want the operators to be freely configurable, we cache it as well. This way we can adapt any callable object with two parameters into a binary named operator.

## Background

The idea for user-defined names operators comes from [an answer posted by Stack Overflow user Yakk](http://stackoverflow.com/a/15090751/1968). His proposal uses configurable delimiters for the operator name, allowing for operators such as `-diff-` and `*cross*`, thus taking the respective operator precedence from their delimiting operators.
