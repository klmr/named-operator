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

// make_unique helper, similar to make_shared

#ifndef WHEELS_SMART_PTR_UNIQUE_PTR_HPP
#define WHEELS_SMART_PTR_UNIQUE_PTR_HPP

#include "../allocator.h++"
#include "../meta.h++"
#include "../named_param.h++"
#include "../detail/pointer_param.h++"
#include "../tuple.h++"

#include <cstddef>
#include <functional>
#include <memory>
#include <new>
#include <tuple>
#include <utility>

namespace wheels {
    //! Factory for unique_ptr
    //! Classic make_unique: just arguments to forward, default deleter
    template <typename T, typename... Args,
              EnableIf<Not<std::is_array<T>>,
                       no_named_parameters<Args...>
                      > = _,
              typename Pointer = std::unique_ptr<T>>
    Pointer make_unique(Args&&... args) {
        return Pointer { new T(std::forward<Args>(args)...) };
    }

    //! make_unique for arrays: both T[] and T[N] supported
    template <typename T,
              EnableIf<std::is_array<T>> = _,
              typename Pointee = RemoveExtent<T>,
              std::ptrdiff_t Extent = std::extent<T>::value,
              typename Pointer = std::unique_ptr<Pointee[]>>
    Pointer make_unique(std::ptrdiff_t size = 0) {
        return Pointer { new Pointee[Extent == 0? size : Extent] };
    }
    //! make_unique from a raw pointer (like `unique_ptr<T>(p)` but with deduction)
    template <typename T = deduced,
              typename Raw,
              typename Pointee = Conditional<is_deduced<T>, RemovePointer<Bare<Raw>>, T>,
              typename Pointer = std::unique_ptr<Pointee>>
    Pointer make_unique(names::raw_parameter<Raw> const& raw) {
        return Pointer { raw.forward() };
    }
    //! make_unique with value-initialization and a custom deleter
    template <typename T, typename Deleter,
              typename Pointer = std::unique_ptr<T, Decay<Deleter>>>
    Pointer make_unique(names::deleter_parameter<Deleter> const& deleter) {
        return Pointer { new T{}, deleter.forward() };
    }

    namespace unique_detail {
        //! make_unique from a tuple, with custom deleter (indices backend)
        template <typename T, typename Tuple, typename Deleter,
                  int... Indices,
                  typename Pointer = std::unique_ptr<T, Decay<Deleter>>>
        Pointer make(Tuple&& tuple, Deleter&& deleter, indices<Indices...>) {
            using std::get;
            return Pointer { new T(get<Indices>(tuple)...), std::forward<Deleter>(deleter) };
        }
    } // namespace unique_detail

    //! make_unique from a tuple, with custom deleter
    template <typename T, typename Tuple, typename Deleter,
              DisableIf<is_named_parameter<Tuple>> = _,
              typename Pointer = std::unique_ptr<T, Decay<Deleter>>>
    Pointer make_unique(Tuple&& tuple, names::deleter_parameter<Deleter> const& deleter) {
        return unique_detail::make<T>(std::forward<Tuple>(tuple), deleter.forward(), IndicesFor<Tuple>{});
    }
    //! make_unique from a raw pointer, with custom deleter
    template <typename T = deduced,
              typename Arg0, typename Arg1,
              EnableIf<is_named_parameter<Arg0>,
                       is_named_parameter<Arg1>,
                       has_parameter<names::raw_name, Arg0, Arg1>,
                       has_parameter<names::deleter_name, Arg0, Arg1>
                       > = _,
              typename Raw = GetParameterType<names::raw_name, Arg0, Arg1>,
              typename Deleter = GetParameterType<names::deleter_name, Arg0, Arg1>,
              typename Pointee = Conditional<is_deduced<T>, RemovePointer<Bare<Raw>>, T>,
              typename Pointer = std::unique_ptr<Pointee, Decay<Deleter>>>
    Pointer make_unique(Arg0 const& arg0, Arg1 const& arg1) {
        return Pointer { forward_named(names::raw, arg0, arg1), forward_named(names::deleter, arg0, arg1) };
    }

    //! Factory for unique_ptr from an allocator
    template <typename T, typename A, typename... Args,
              typename Deleter = allocator_delete<T, A>,
              typename Pointer = std::unique_ptr<T, Deleter>>
    Pointer allocate_unique(A const& a, Args&&... args) {
        A a2 { a };
        auto holder = allocator_detail::allocate(a2, 1);
        ::new(holder.get()) T(std::forward<Args>(args)...);
        return Pointer { holder.release(), Deleter { std::move(a2) } };
    }

    //! unique_ptr with type-erased deleter
    template <typename T>
    using exclusive_ptr = std::unique_ptr<T, std::function<void(T*)>>;
} // namespace wheels

#endif // WHEELS_SMART_PTR_UNIQUE_PTR_HPP

