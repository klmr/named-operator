# Named operators

Named operators are (user-defined) operators which have names rather than symbols. Here’s Haskell:

```haskell
x = a `div` b
```

and [here’s R](http://stat.ethz.ch/R-manual/R-patched/library/base/html/match.html):

```R
yup <- 4 %in% c(1, 2, 3, 4, 5)
```

In fact, [C++ also has named operators](http://gcc.gnu.org/onlinedocs/cpp/C_002b_002b-Named-Operators.html) – alternative tokens for the primary ones defined in §2.6.

But those are fixed and not redefinable. Sure, you can `#define` your own names for tokens …

    #define PLUS +

But this limits you to the already existing binary operators. Until now.

Take a look at this fully valid, compiling and running C++ code:

```c++
int x = 42;
int y = 23;
auto z = x <divmod> y; // calculates { x / y, x % y }
```

You want assignment operators? Not a problem:

```c++
std::vector<int> vec{ 1, 2, 3 };
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
std::pair<int, int> divmod_f(int x, int y) {
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

## Design rationale

…

## What’s the use?

None whatsoever. Increased readability? *Maybe*, at the price of unfamiliar syntax that will trip people up.
