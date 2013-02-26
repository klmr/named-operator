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

// make_shared factory with named parameters for the deleter

#ifndef WHEELS_SMART_PTR_SHARED_PTR_HPP
#define WHEELS_SMART_PTR_SHARED_PTR_HPP

#include "../allocator.h++"
#include "../meta.h++"
#include "../named_param.h++"
#include "../detail/pointer_param.h++"
#include "../tuple.h++"

#include <memory>

namespace wheels {
    //! Factory for shared_ptr
    //! Classic make_shared: just arguments to forward, default deleter
    template <typename T, typename... Args,
              EnableIf<no_named_parameters<Args...>> = _,
              typename Pointer = std::shared_ptr<T>>
    Pointer make_shared(Args&&... args) {
        return Pointer { new T(std::forward<Args>(args)...) };
    }

    //! make_shared from a raw pointer (like `shared_ptr<T>(p)` but with deduction)
    template <typename T = deduced,
              typename Raw,
              typename Pointee = Conditional<is_deduced<T>, RemovePointer<Bare<Raw>>, T>,
              typename Pointer = std::shared_ptr<Pointee>>
    Pointer make_shared(names::raw_parameter<Raw> const& raw) {
        return Pointer { raw.forward() };
    }
    //! make_shared with value-initialization and a custom deleter
    template <typename T, typename Deleter,
              typename Pointer = std::shared_ptr<T>>
    Pointer make_shared(names::deleter_parameter<Deleter> const& deleter) {
        return Pointer { new T{}, deleter.forward() };
    }

    namespace shared_detail {
        //! make_shared from a tuple, with custom deleter (indices backend)
        template <typename T, typename Tuple, typename Deleter,
                  int... I,
                  typename Pointer = std::shared_ptr<T>>
        Pointer make(Tuple&& tuple, Deleter&& deleter, indices<I...>) {
            using std::get;
            return Pointer { new T(get<I>(tuple)...), std::forward<Deleter>(deleter) };
        }
    } // namespace shared_detail

    //! make_shared from a tuple, with custom deleter
    template <typename T, typename Tuple, typename Deleter,
              DisableIf<is_named_parameter<Tuple>> = _,
              typename Pointer = std::shared_ptr<T>>
    Pointer make_shared(Tuple&& tuple, names::deleter_parameter<Deleter> const& deleter) {
        return shared_detail::make<T>(std::forward<Tuple>(tuple), deleter.forward(), IndicesFor<Tuple>{});
    }
    //! make_shared from a raw pointer, with custom deleter
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
              typename Pointer = std::shared_ptr<Pointee>>
    Pointer make_shared(Arg0 const& arg0, Arg1 const& arg1) {
        return Pointer { forward_named(names::raw, arg0, arg1), forward_named(names::deleter, arg0, arg1) };
    }
}

#endif // WHEELS_SMART_PTR_SHARED_PTR_HPP

