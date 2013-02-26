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

// Smart pointer with value semantics

#ifndef WHEELS_SMART_PTR_VALUE_PTR_HPP
#define WHEELS_SMART_PTR_VALUE_PTR_HPP

#include "../allocator.h++"
#include "../meta.h++"
#include "../named_param.h++"
#include "../detail/pointer_param.h++"
#include "../tuple.h++"

#include <boost/operators.hpp>

#include <cstddef>

#include <functional>
#include <memory>
#include <new>
#include <tuple>
#include <utility>

namespace wheels {
    //! Cloning policy that invokes the copy constructor
    template <typename T>
    struct copy_constructor_cloner {
        T* operator()(T* ptr) const {
            return new T(*ptr);
        }
    };
    //! Cloning policy that calls a clone() member function
    template <typename T>
    struct clone_function_cloner {
        constexpr clone_function_cloner() noexcept = default;

        template <typename U,
                  EnableIf<std::is_base_of<T*, U*>> = _>
        clone_function_cloner(clone_function_cloner<U> const&) noexcept {}

        T* operator()(T* ptr) const {
            return ptr->clone();
        }
    };

    namespace value_ptr_detail {
        template <typename T, T* (T::*)() const = &T::clone>
        class cloneable;
    } // namespace value_ptr_detail

    template <typename T, typename = void>
    struct is_cloneable : Bool<false> {};
    template <typename T>
    struct is_cloneable<T, Void<decltype(static_cast<T*>(std::declval<T const&>().clone()))>>
    : Bool<true> {};

    //! Default cloning policy
    /*! Uses clone() function if type is polymorphic and function is available; copy ctor otherwise */
    template <typename T>
    using DefaultCloner = Conditional<
                            All<std::is_polymorphic<T>, is_cloneable<T>>
                            , clone_function_cloner<T>
                            , copy_constructor_cloner<T>
                        >;

    //! A smart pointer that treats the pointee as if contained by-value
    template <typename T, typename Cloner = DefaultCloner<T>, typename Deleter = std::default_delete<T>>
    class value_ptr
    : boost::totally_ordered<value_ptr<T>,
      boost::equality_comparable<value_ptr<T>, std::nullptr_t>
    > {
    public:
        using element_type = T;
        using cloner_type = Cloner;
        using deleter_type = Deleter;
        using pointer = T*;
        using reference = T&;

        //! Constructs a null pointer
        constexpr value_ptr() noexcept : storage(nullptr, cloner_type{}, deleter_type{}) {}
        //! Constructs a null pointer
        constexpr value_ptr(std::nullptr_t) noexcept : value_ptr() {}

        //! Takes ownership of an existing pointer
        template <typename U>
        explicit value_ptr(U* ptr) noexcept
        : storage(ptr, cloner_type{}, deleter_type{}) {
            static_assert(std::is_convertible<U*, pointer>::value, "pointee type must be compatible");
            static_assert(!std::is_polymorphic<U>::value || !std::is_same<cloner_type, copy_constructor_cloner<T>>::value,
                          "conversion cannot cause slicing");
        }

        template <typename U, typename C, typename D,
                  EnableIf<std::is_convertible<U*, pointer>,
                           std::is_convertible<C, cloner_type>,
                           std::is_convertible<D, deleter_type>
                          > = _>
        value_ptr(U* ptr, C&& cloner, D&& deleter)
        : storage(ptr, std::forward<C>(cloner), std::forward<D>(deleter)) {}

        //! Makes a copy, according to the cloning policy
        value_ptr(value_ptr const& that)
        : storage(clone(that.get_ptr()), that.get_cloner(), that.get_deleter()) {}

        //! Makes a copy, according to the cloning policy
        template <typename U, typename C, typename D,
                  EnableIf<std::is_convertible<U*, pointer>> = _>
        value_ptr(value_ptr<U, C, D> const& that)
        : storage(clone(that.get_ptr()), that.get_cloner(), that.get_deleter()) {}

        //! Moves from another value_ptr
        value_ptr(value_ptr&& that) noexcept
        : storage(that.release(), std::move(that.get_cloner()), std::move(that.get_deleter())) {}

        //! Moves from another value_ptr
        template <typename U, typename C, typename D,
                  EnableIf<std::is_convertible<U*, pointer>> = _>
        value_ptr(value_ptr<U, C, D>&& that) noexcept
        : storage(that.release(), std::move(that.get_cloner()), std::move(that.get_deleter())) {}

        //! Destroys the value_ptr
        ~value_ptr() noexcept(noexcept(std::declval<value_ptr>().reset())) {
            reset();
        }

        //! Makes this pointer null
        value_ptr& operator=(std::nullptr_t) noexcept {
            reset();
            return *this;
        }

        //! Assigns another pointer to this one
        value_ptr& operator=(value_ptr that) noexcept {
            swap(that);
            return *this;
        }

        //! Dereferences the pointer
        reference operator*() const noexcept {
            return *get();
        }
        //! Forwards member access to the underlying pointer
        pointer operator->() const noexcept {
            return get();
        }
        //! Obtains the underlying pointer
        pointer get() const noexcept {
            return get_ptr();
        }

        //! Tests if the pointer is null
        explicit operator bool() const noexcept {
            return get();
        }

        //! Releases ownership of the underlying pointer
        pointer release() noexcept {
            pointer old = get_ptr();
            get_ptr() = nullptr;
            return old;
        }

        //! Resets the underlying pointer
        void reset(pointer p = {}) noexcept(noexcept(std::declval<deleter_type&>()(p))) {
            std::swap(get_ptr(), p);
            get_deleter()(p);
        }

        void swap(value_ptr& that) {
            using std::swap;
            swap(get_ptr(), that.get_ptr());
            swap(get_cloner(), that.get_cloner());
            swap(get_deleter(), that.get_deleter());
        }

        cloner_type& get_cloner() { return std::get<1>(storage); }
        cloner_type const& get_cloner() const { return std::get<1>(storage); }

        deleter_type& get_deleter() { return std::get<2>(storage); }
        deleter_type const& get_deleter() const { return std::get<2>(storage); }

    private:
        std::tuple<pointer, cloner_type, deleter_type> storage;

        pointer& get_ptr() { return std::get<0>(storage); }
        pointer const& get_ptr() const { return std::get<0>(storage); }

        template <typename Ptr>
        pointer clone(Ptr p) {
            return p? get_cloner()(p) : nullptr;
        }
    };

    template <typename T, typename Cloner, typename Deleter>
    void swap(value_ptr<T, Cloner, Deleter>& x, value_ptr<T, Cloner, Deleter>& y) {
        x.swap(y);
    }

    template <typename T, typename Cloner, typename Deleter>
    bool operator==(value_ptr<T, Cloner, Deleter> const& x, value_ptr<T, Cloner, Deleter> const& y) {
        return x.get() == y.get();
    }
    template <typename T, typename Cloner, typename Deleter>
    bool operator<(value_ptr<T, Cloner, Deleter> const& x, value_ptr<T, Cloner, Deleter> const& y) {
        return std::less<T>()(x.get(), y.get());
    }
    template <typename T, typename Cloner, typename Deleter>
    bool operator==(value_ptr<T, Cloner, Deleter> const& x, std::nullptr_t) {
        return x.get() == nullptr;
    }

    //! Factory for value_ptr
    //! Classic make_value: just arguments to forward, default cloner and deleter
    template <typename T, typename... Args,
              EnableIf<no_named_parameters<Args...>> = _,
              typename Pointer = value_ptr<T>>
    Pointer make_value(Args&&... args) {
        return Pointer { new T(std::forward<Args>(args)...) };
    }
    //! make_value from a raw pointer (like `value_ptr<T>(p)` but with deduction)
    template <typename T = deduced,
              typename Raw,
              typename Pointee = Conditional<is_deduced<T>, RemovePointer<Bare<Raw>>, T>,
              typename Pointer = value_ptr<Pointee>>
    Pointer make_value(names::raw_parameter<Raw> const& raw) {
        return Pointer { raw.forward() };
    }

    //! make_value with value-initialization and a custom cloner
    template <typename T, typename Cloner,
              typename Pointer = value_ptr<T, Decay<Cloner>>>
    Pointer make_value(names::cloner_parameter<Cloner> const& cloner) {
        return Pointer { new T{}, cloner.forward(), std::default_delete<T> {} };
    }

    //! make_value with value-initialization and a custom deleter
    template <typename T, typename Deleter,
              typename Pointer = value_ptr<T, DefaultCloner<T>, Decay<Deleter>>>
    Pointer make_value(names::deleter_parameter<Deleter> const& deleter) {
        return Pointer { new T{}, DefaultCloner<T> {}, deleter.forward() };
    }

    //! make_value with value-initialization, custom cloner and custom deleter
    template <typename T, typename Arg0, typename Arg1,
              EnableIf<is_named_parameter<Arg0>, is_named_parameter<Arg1>> = _,
              typename Cloner = ParameterType<GetParameter<names::cloner_name, Arg0, Arg1>>,
              typename Deleter = ParameterType<GetParameter<names::deleter_name, Arg0, Arg1>>,
              typename Pointer = value_ptr<T, Decay<Cloner>, Decay<Deleter>>>
    Pointer make_value(Arg0 const& arg0, Arg1 const& arg1) {
        return Pointer { new T{}, forward_named(names::cloner, arg0, arg1), forward_named(names::deleter, arg0, arg1) };
    }

    namespace value_detail {
        //! make_value from a tuple, with custom cloner and custom deleter (indices backend)
        template <typename T, typename Tuple, typename Cloner, typename Deleter,
                  int... Indices,
                  typename Pointer = value_ptr<T, Decay<Cloner>, Decay<Deleter>>>
        Pointer make(Tuple&& tuple, Cloner&& cloner, Deleter&& deleter, indices<Indices...>) {
            using std::get;
            return Pointer { new T(get<Indices>(tuple)...), std::forward<Cloner>(cloner), std::forward<Deleter>(deleter) };
        }
    } // namespace value_detail

    //! make_value from a tuple, with custom cloner
    template <typename T, typename Tuple, typename Cloner,
              DisableIf<is_named_parameter<Tuple>> = _,
              typename Pointer = value_ptr<T, Decay<Cloner>>>
    Pointer make_value(Tuple&& tuple, names::cloner_parameter<Cloner> const& cloner) {
        return value_detail::make<T>(std::forward<Tuple>(tuple), cloner.forward(), std::default_delete<T> {}, IndicesFor<Tuple>{});
    }
    //! make_value from a tuple, with custom deleter
    template <typename T, typename Tuple, typename Deleter,
              DisableIf<is_named_parameter<Tuple>> = _,
              typename Pointer = value_ptr<T, DefaultCloner<T>, Decay<Deleter>>>
    Pointer make_value(Tuple&& tuple, names::deleter_parameter<Deleter> const& deleter) {
        return value_detail::make<T>(std::forward<Tuple>(tuple), DefaultCloner<T> {}, deleter.forward(), IndicesFor<Tuple>{});
    }
    //! make_value from a tuple, with custom cloner and custom deleter
    template <typename T, typename Tuple, typename Arg0, typename Arg1,
              EnableIf<Not<is_named_parameter<Tuple>>, is_named_parameter<Arg0>, is_named_parameter<Arg1>> = _,
              typename Cloner = ParameterType<GetParameter<names::cloner_name, Arg0, Arg1>>,
              typename Deleter = ParameterType<GetParameter<names::deleter_name, Arg0, Arg1>>,
              typename Pointer = value_ptr<T, Decay<Cloner>, Decay<Deleter>>>
    Pointer make_value(Tuple&& tuple, Arg0 const& arg0, Arg1 const& arg1) {
        return value_detail::make<T>(std::forward<Tuple>(tuple),
                                     forward_named(names::cloner, arg0, arg1),
                                     forward_named(names::deleter, arg0, arg1),
                                     IndicesFor<Tuple>{});
    }

    //! make_value from a raw pointer, with custom cloner
    template <typename T = deduced,
              typename Arg0, typename Arg1,
              EnableIf<is_named_parameter<Arg0>,
                       is_named_parameter<Arg1>,
                       has_parameter<names::raw_name, Arg0, Arg1>,
                       has_parameter<names::cloner_name, Arg0, Arg1>> = _,
              typename Raw = GetParameterType<names::raw_name, Arg0, Arg1>,
              typename Cloner = GetParameterType<names::cloner_name, Arg0, Arg1>,
              typename Pointee = Conditional<is_deduced<T>, RemovePointer<Bare<Raw>>, T>,
              typename Pointer = value_ptr<Pointee, Decay<Cloner>>>
    Pointer make_value(Arg0 const& arg0, Arg1 const& arg1) {
        return Pointer { forward_named(names::raw, arg0, arg1), forward_named(names::cloner, arg0, arg1), std::default_delete<Pointee> {} };
    }
    //! make_value from a raw pointer, with custom deleter
    template <typename T = deduced,
              typename Arg0, typename Arg1,
              EnableIf<is_named_parameter<Arg0>,
                       is_named_parameter<Arg1>,
                       has_parameter<names::raw_name, Arg0, Arg1>,
                       has_parameter<names::deleter_name, Arg0, Arg1>> = _,
              typename Raw = GetParameterType<names::raw_name, Arg0, Arg1>,
              typename Deleter = GetParameterType<names::deleter_name, Arg0, Arg1>,
              typename Pointee = Conditional<is_deduced<T>, RemovePointer<Bare<Raw>>, T>,
              typename Pointer = value_ptr<Pointee, DefaultCloner<Pointee>, Decay<Deleter>>>
    Pointer make_value(Arg0 const& arg0, Arg1 const& arg1) {
        return Pointer { forward_named(names::raw, arg0, arg1), DefaultCloner<Pointee> {}, forward_named(names::deleter, arg0, arg1) };
    }
    //! make_value from a raw pointer, with custom cloner, with custom deleter
    template <typename T = deduced,
              typename Arg0, typename Arg1, typename Arg2,
              EnableIf<is_named_parameter<Arg0>,
                       is_named_parameter<Arg1>,
                       is_named_parameter<Arg2>,
                       has_parameter<names::raw_name, Arg0, Arg1, Arg2>,
                       has_parameter<names::cloner_name, Arg0, Arg1, Arg2>,
                       has_parameter<names::deleter_name, Arg0, Arg1, Arg2>> = _,
              typename Raw = GetParameterType<names::raw_name, Arg0, Arg1, Arg2>,
              typename Cloner = GetParameterType<names::cloner_name, Arg0, Arg1, Arg2>,
              typename Deleter = GetParameterType<names::deleter_name, Arg0, Arg1, Arg2>,
              typename Pointee = Conditional<is_deduced<T>, RemovePointer<Bare<Raw>>, T>,
              typename Pointer = value_ptr<Pointee, Decay<Cloner>, Decay<Deleter>>>
    Pointer make_value(Arg0 const& arg0, Arg1 const& arg1, Arg2 const& arg2) {
        return Pointer { forward_named(names::raw, arg0, arg1, arg2),
                         forward_named(names::cloner, arg0, arg1, arg2),
                         forward_named(names::deleter, arg0, arg1, arg2) };
    }

    //! Allocator-based factory for value_ptr
    template <typename T, typename A, typename... Args,
              typename Cloner = allocator_cloner<T, A>,
              typename Deleter = allocator_delete<T, A>,
              typename Pointer = value_ptr<T, Cloner, Deleter>>
    Pointer allocate_value(A const& a, Args&&... args) {
        A a2 { a };
        auto holder = allocator_detail::allocate(a2, 1);
        ::new(holder.get()) T(std::forward<Args>(args)...);
        return Pointer { holder.release(), Cloner { a }, Deleter { std::move(a2) } };
        // TODO: get the cloner and deleter to share the allocator
    }
} // namespace wheels

#endif // WHEELS_SMART_PTR_VALUE_PTR_HPP

