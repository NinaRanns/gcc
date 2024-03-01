// <basic_variant> -*- C++ -*-

// Copyright (C) 2016-2022 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file basic_variant
 *  This is the `<basic_variant>` C++ Library header.
 */

//TODO : should basic_variant have it's own version of never valueless ?

#ifndef _GLIBCXX_BASICVARIANT
#define _GLIBCXX_BASICVARIANT 1

#pragma GCC system_header

#if __cplusplus >= 201703L

#include <variant>
#include <memory>
#if __cplusplus >= 202002L
# include <compare>
#endif


namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<typename... _Types> class tuple;
  
  template<typename _Alloc, typename... _Types> class basic_variant;


  template<typename _Alloc, typename... _Types>
    struct variant_size<basic_variant<_Alloc, _Types...>>
    : std::integral_constant<size_t, sizeof...(_Types)> {};

  
  template<typename _Alloc, typename... _Types>
    inline constexpr size_t
    variant_size_v<basic_variant<_Alloc, _Types...>> = sizeof...(_Types);

  template<typename _Alloc, typename... _Types>
    inline constexpr size_t
    variant_size_v<const basic_variant<_Alloc, _Types...>> = sizeof...(_Types);

   template<size_t _Np, typename _Alloc,  typename... _Types>
    struct variant_alternative<_Np, basic_variant<_Alloc, _Types...>>
    {
      static_assert(_Np < sizeof...(_Types));

      using type = typename _Nth_type<_Np, _Types...>::type;
    };

  template<size_t _Np, typename _Alloc, typename... _Types>
    constexpr variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>>&
    get(basic_variant<_Alloc, _Types...>&);

  template<size_t _Np, typename _Alloc, typename... _Types>
    constexpr variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>>&&
    get(basic_variant<_Alloc, _Types...>&&);

  template<size_t _Np, typename _Alloc, typename... _Types>
    constexpr variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>> const&
    get(const basic_variant<_Alloc, _Types...>&);

  template<size_t _Np, typename _Alloc, typename... _Types>
    constexpr variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>> const&&
    get(const basic_variant<_Alloc, _Types...>&&);


  template < typename _Alloc, typename... _Types, typename _Tp>
    _GLIBCXX20_CONSTEXPR
    decltype(auto)
    __basic_variant_cast(_Tp&& __rhs)
    {
      if constexpr (is_lvalue_reference_v<_Tp>)
    {
      if constexpr (is_const_v<remove_reference_t<_Tp>>)
        return static_cast<const basic_variant<_Alloc, _Types...>&>(__rhs);
      else
        return static_cast<basic_variant<_Alloc, _Types...>&>(__rhs);
    }
      else
        return static_cast<basic_variant<_Alloc, _Types...>&&>(__rhs);
    }


namespace __detail
{
namespace __variant
{

  template<typename _Type>
      struct _AA_Uninitialized
      {
        template<typename _Alloc, typename ... _Args>
        constexpr
        _AA_Uninitialized(const _Alloc& _a, _Args&&... __args):
        _M_storage(std::make_obj_using_allocator<_Type>(_a,
            std::forward<_Args>(__args)...))
        {
        }

        constexpr ~_AA_Uninitialized() requires (__has_trivial_destructor(_Type))
            = default;

        // if alive, should be destroyed by the variant
        constexpr ~_AA_Uninitialized() {};

        _AA_Uninitialized(const _AA_Uninitialized&) = default;
        _AA_Uninitialized(_AA_Uninitialized&&) = default;
        _AA_Uninitialized& operator=(const _AA_Uninitialized&) = default;
        _AA_Uninitialized& operator=(_AA_Uninitialized&&) = default;

        constexpr const _Type& _M_get() const & noexcept
        { return _M_storage; }

        constexpr _Type& _M_get() & noexcept
        { return _M_storage; }

        constexpr const _Type&& _M_get() const && noexcept
        { return std::move(_M_storage); }

        constexpr _Type&& _M_get() && noexcept
        { return std::move(_M_storage); }

        struct _Empty_byte { };

        union {
            _Empty_byte _M_empty;
            _Type _M_storage;
        };
      };

template<typename... _Types>
    union _AA_Variadic_union
    {
      _AA_Variadic_union() = default;

      template<size_t _Np, typename... _Args>
    _AA_Variadic_union(in_place_index_t<_Np>, _Args&&...) = delete;
    };

  // Defines members and ctors.
  template<typename _First, typename... _Rest>
    union _AA_Variadic_union<_First, _Rest...>
    {
      constexpr _AA_Variadic_union() : _M_rest() { }

      template<typename... _Args>
    constexpr
    _AA_Variadic_union(in_place_index_t<0>, _Args&&... __args)
    : _M_first(std::forward<_Args>(__args)...)
    { }

      template<size_t _Np, typename... _Args>
    constexpr
    _AA_Variadic_union(in_place_index_t<_Np>, _Args&&... __args)
    : _M_rest(in_place_index<_Np-1>, std::forward<_Args>(__args)...)
    { }

#if __cpp_lib_variant >= 202106L
      _AA_Variadic_union(const _AA_Variadic_union&) = default;
      _AA_Variadic_union(_AA_Variadic_union&&) = default;
      _AA_Variadic_union& operator=(const _AA_Variadic_union&) = default;
      _AA_Variadic_union& operator=(_AA_Variadic_union&&) = default;

      ~_AA_Variadic_union() = default;

      constexpr ~_AA_Variadic_union()
    requires (!__has_trivial_destructor(_First))
          || (!__has_trivial_destructor(_AA_Variadic_union<_Rest...>))
      { }
#endif

      _AA_Uninitialized<_First> _M_first;
      _AA_Variadic_union<_Rest...> _M_rest;
    };
  template <typename _Alloc>
  struct _Variant_alloc
  {
      using allocator_traits = std::allocator_traits<_Alloc>;

      constexpr _Variant_alloc() = default;
      constexpr _Variant_alloc(const _Variant_alloc&) requires(allocator_traits::is_always_equal::value) = default;
      constexpr _Variant_alloc(_Variant_alloc&&) = default;
      constexpr _Variant_alloc& operator=(const _Variant_alloc& __rhs) requires(allocator_traits::is_always_equal::value) = default;
      constexpr _Variant_alloc& operator=(_Variant_alloc&& __rhs) requires(allocator_traits::is_always_equal::value
                         || allocator_traits::propagate_on_container_move_assignment::value) = default;

      constexpr _Variant_alloc(const _Variant_alloc&  __rhs) :
        _M_alloc(allocator_traits::select_on_container_copy_construction(__rhs._M_alloc)) {};

      constexpr _Variant_alloc& operator=(const _Variant_alloc& __rhs)
      {
          _M_alloc = __rhs._M_alloc;
          return *this;
      }

      _Variant_alloc(const _Alloc _a) : _M_alloc(_a){};

      template <typename _AllocU>
      constexpr _Variant_alloc(const _Variant_alloc<_AllocU>&  __rhs) :
        _M_alloc(allocator_traits::select_on_container_copy_construction(__rhs._M_alloc)) {};

      template <typename _AllocU>
      constexpr _Variant_alloc(_Variant_alloc<_AllocU>&&  __rhs) :
        _M_alloc(__rhs._M_alloc) {};

      template <typename _AllocU>
      constexpr _Variant_alloc& operator=(const _Variant_alloc<_AllocU>& __rhs)
      {
          _M_alloc = __rhs._M_alloc;
          return *this;
      }


      _Alloc       _M_alloc;
  };



  template<typename _Alloc,typename... _Types>
    struct _Basic_Variant_storage
    {
      using allocator_traits = std::allocator_traits<_Alloc>;
//special member functions
      // defaults
      constexpr _Basic_Variant_storage(const _Basic_Variant_storage&) requires(_Traits<_Types...>::_S_trivial_copy_ctor) = default;
      constexpr _Basic_Variant_storage(_Basic_Variant_storage&&) requires(_Traits<_Types...>::_S_trivial_move_ctor) = default;
      constexpr _Basic_Variant_storage& operator=(const _Basic_Variant_storage& __rhs) requires(allocator_traits::is_always_equal::value
          && _Traits<_Types...>::_S_trivial_copy_assign) = default;
      constexpr _Basic_Variant_storage& operator=(_Basic_Variant_storage&& __rhs) requires(
          (allocator_traits::is_always_equal::value
              || allocator_traits::propagate_on_container_move_assignment::value)
          && _Traits<_Types...>::_S_trivial_move_assign) = default;

      constexpr ~_Basic_Variant_storage() requires(_Traits<_Types...>::_S_trivial_dtor) = default;

      // non defaults
      constexpr _Basic_Variant_storage() noexcept(_Traits<_Types...>:: _S_nothrow_default_ctor
          && ( allocator_traits::is_always_equal::value  ||
                !uses_allocator_v<remove_cv_t<typename _Nth_type<0, _Types...>::type>, _Alloc>))
      : _M_alloc_wrapper(),
        _M_u(in_place_index<0>,_M_alloc_wrapper._M_alloc),
        _M_index(0)
      { }

      constexpr _Basic_Variant_storage(const _Basic_Variant_storage& __rhs)
      noexcept(_Traits<_Types...>:: _S_nothrow_copy_ctor)
      : _M_alloc_wrapper(__rhs._M_alloc_wrapper)
      {

        std::__detail::__variant::__raw_idx_visit(
          [this](auto&& __rhs_mem, auto __rhs_index) mutable
          {
            constexpr size_t __j = __rhs_index;
            if constexpr (__j != variant_npos)
                std::_Construct(std::__addressof(this->_M_u),
                   in_place_index<__j>,
                   _M_alloc_wrapper._M_alloc,__rhs_mem);

          }, __basic_variant_cast<_Alloc, _Types...>(__rhs));
          this->_M_index = __rhs._M_index;
       }

      constexpr _Basic_Variant_storage(_Basic_Variant_storage&& __rhs)
       noexcept(_Traits<_Types...>:: _S_nothrow_move_ctor)
       : _M_alloc_wrapper(std::move(__rhs._M_alloc_wrapper))
       {

        std::__detail::__variant::__raw_idx_visit(
           [this](auto&& __rhs_mem, auto __rhs_index) mutable
           {
             constexpr size_t __j = __rhs_index;
             if constexpr (__j != variant_npos)
                 std::_Construct(std::__addressof(this->_M_u),
                    in_place_index<__j>,
                    _M_alloc_wrapper._M_alloc,std::move(__rhs_mem));

           }, __basic_variant_cast<_Alloc, _Types...>(__rhs));
           this->_M_index = __rhs._M_index;
        }

      template<typename _AllocU>
      constexpr _Basic_Variant_storage(const _Basic_Variant_storage<_AllocU, _Types... >& __rhs)
      noexcept(_Traits<_Types...>:: _S_nothrow_copy_ctor)
      : _M_alloc_wrapper(__rhs._M_alloc_wrapper)
      {

        std::__detail::__variant::__raw_idx_visit(
          [this](auto&& __rhs_mem, auto __rhs_index) mutable
          {
            constexpr size_t __j = __rhs_index;
            if constexpr (__j != variant_npos)
                std::_Construct(std::__addressof(this->_M_u),
                   in_place_index<__j>,
                   _M_alloc_wrapper._M_alloc,__rhs_mem);

          }, __basic_variant_cast<_AllocU, _Types...>(__rhs));
          this->_M_index = __rhs._M_index;
       }

      template<typename _AllocU>
      constexpr _Basic_Variant_storage( _Basic_Variant_storage<_AllocU, _Types... >&& __rhs)
       noexcept(_Traits<_Types...>:: _S_nothrow_move_ctor)
       : _M_alloc_wrapper(std::move(__rhs._M_alloc_wrapper))
       {

        std::__detail::__variant::__raw_idx_visit(
           [this](auto&& __rhs_mem, auto __rhs_index) mutable
           {
             constexpr size_t __j = __rhs_index;
             if constexpr (__j != variant_npos)
                 std::_Construct(std::__addressof(this->_M_u),
                    in_place_index<__j>,
                    _M_alloc_wrapper._M_alloc,std::move(__rhs_mem));

           }, __basic_variant_cast<_AllocU, _Types...>(__rhs));
           this->_M_index = __rhs._M_index;
        }
      constexpr _Basic_Variant_storage(allocator_arg_t, const _Alloc& __a)
      :  _M_alloc_wrapper(__a),
        _M_u(in_place_index<0>,_M_alloc_wrapper._M_alloc),
        _M_index(0)
      { }

      constexpr _Basic_Variant_storage(allocator_arg_t, const _Alloc& __a,
                                 const _Basic_Variant_storage& __rhs)
      : _M_alloc_wrapper(__a)
      {

        std::__detail::__variant::__raw_idx_visit(
          [this](auto&& __rhs_mem, auto __rhs_index) mutable
          {
              constexpr size_t __j = __rhs_index;
              if constexpr (__j != variant_npos)
                  std::_Construct(std::__addressof(this->_M_u),
                     in_place_index<__j>,
                     _M_alloc_wrapper._M_alloc,__rhs_mem);
          }, __basic_variant_cast<_Alloc, _Types...>(__rhs));
          this->_M_index = __rhs._M_index;
       }


      constexpr _Basic_Variant_storage(allocator_arg_t, const _Alloc& __a,
                                 _Basic_Variant_storage&& __rhs)
      : _M_alloc_wrapper(__a)
      {

        std::__detail::__variant::__raw_idx_visit(
          [this](auto&& __rhs_mem, auto __rhs_index) mutable
          {
              constexpr size_t __j = __rhs_index;
              if constexpr (__j != variant_npos)
                  std::_Construct(std::__addressof(this->_M_u),
                     in_place_index<__j>,
                     _M_alloc_wrapper._M_alloc,std::move(__rhs_mem));
          }, __basic_variant_cast<_Alloc, _Types...>(__rhs));
          this->_M_index = __rhs._M_index;
       }


      _GLIBCXX20_CONSTEXPR
      ~_Basic_Variant_storage()
      { _M_reset(); }


      template<size_t _Np, typename... _Args>
    constexpr
    _Basic_Variant_storage(in_place_index_t<_Np>, _Args&&... __args)
    : _M_alloc_wrapper(),
      _M_u(in_place_index<_Np>,
           _M_alloc_wrapper._M_alloc, std::forward<_Args>(__args)...),
       _M_index{_Np}
    { }

      template<size_t _Np, typename... _Args>
    constexpr
    _Basic_Variant_storage(allocator_arg_t, const _Alloc& __a, in_place_index_t<_Np>, _Args&&... __args)
    : _M_alloc_wrapper(__a),
      _M_u(in_place_index<_Np>,
           _M_alloc_wrapper._M_alloc, std::forward<_Args>(__args)...),
       _M_index{_Np}
    { }

    constexpr
    _Basic_Variant_storage&
    operator=(const _Basic_Variant_storage& __rhs)
    noexcept(_Traits<_Types...>::_S_nothrow_copy_assign && allocator_traits::is_always_equal::value )
    {
      if constexpr (allocator_traits::propagate_on_container_copy_assignment::value)
        _M_alloc_wrapper = __rhs._M_alloc_wrapper;

      std::__detail::__variant::__raw_idx_visit(
          [this](auto&& __rhs_mem, auto __rhs_index) mutable
          {
            constexpr size_t __j = __rhs_index;
            if constexpr (__j == variant_npos)
              this->_M_reset(); // Make *this valueless.
            else if (this->_M_index == __j)
              __variant::__get<__j>(*this) = __rhs_mem;
            else
              {
            using _Tj = typename _Nth_type<__j, _Types...>::type;
            if constexpr (is_nothrow_copy_constructible_v<_Tj>
                      || !is_nothrow_move_constructible_v<_Tj>)

            {
              __emplace<__j>( __rhs_mem);
            }
            else
              {
                using _Basic_Variant = basic_variant<_Alloc, _Types...>;
                _Basic_Variant& __self = __basic_variant_cast<_Alloc, _Types...>(*this);
                __self = _Basic_Variant(allocator_arg, get_allocator(), in_place_index<__j>, __rhs_mem);
              }
              }
          }, __basic_variant_cast<_Alloc, _Types...>(__rhs));
        return *this;
          }


    constexpr
    _Basic_Variant_storage&
    operator=(_Basic_Variant_storage&& __rhs)
    noexcept(_Traits<_Types...>::_S_nothrow_move_assign
        && ( allocator_traits::is_always_equal::value  ||
             allocator_traits::propagate_on_container_move_assignment::value))
    {
      if constexpr (allocator_traits::propagate_on_container_move_assignment::value)
        _M_alloc_wrapper = __rhs._M_alloc_wrapper;

      std::__detail::__variant::__raw_idx_visit(
          [this](auto&& __rhs_mem, auto __rhs_index) mutable
          {
            constexpr size_t __j = __rhs_index;
            if constexpr (__j != variant_npos)
              {
            if (this->_M_index == __j)
              __variant::__get<__j>(*this) = std::move(__rhs_mem);
            else
              {
                using _Tj = typename _Nth_type<__j, _Types...>::type;
                if constexpr (is_nothrow_move_constructible_v<_Tj> &&
                    allocator_traits::is_always_equal::value  ||
                    uses_allocator_v<remove_cv_t<_Tj>, _Alloc>)
                    __emplace<__j>(std::move(__rhs_mem));
                else
                  {
                  using _Basic_Variant = basic_variant<_Alloc, _Types...>;
                  _Basic_Variant& __self = __basic_variant_cast<_Alloc, _Types...>(*this);
                __self.template emplace<__j>(std::move(__rhs_mem));
                  }
              }
              }
            else
              this->_M_reset();
          }, __basic_variant_cast<_Alloc, _Types...>(__rhs));
        return *this;
          }


    // Implementation of v.emplace<N>(args...).
    template<size_t _Np, typename... _Args>
      constexpr
      inline void
      __emplace( _Args&&... __args)
      {
        _M_reset();
        std::uninitialized_construct_using_allocator
            (std::__addressof(__variant::__get_n<_Np>(_M_u)),
                _M_alloc_wrapper._M_alloc,
                std::forward<_Args>(__args)...);
        // Construction didn't throw, so can set the new index now:
        _M_index = _Np;
      }


      constexpr void
      _M_reset()
      {
          if (!_M_valid()) [[unlikely]]
                              return;

          std::__do_visit<void>([](auto&& __this_mem) mutable
          {
            std::_Destroy(std::__addressof(__this_mem));
          }, __basic_variant_cast<_Alloc, _Types...>(*this));
          _M_index = variant_npos;
      }



      constexpr bool
      _M_valid() const noexcept
      {
            if constexpr (__variant::__never_valueless<_Types...>())
              return true;
            return this->_M_index != variant_npos;
      }

       constexpr const _AA_Variadic_union<_Types...>& _M_get_u() const & noexcept
      { return _M_u; }

      constexpr _AA_Variadic_union<_Types...>& _M_get_u() & noexcept
      { return _M_u; }

      constexpr const _AA_Variadic_union<_Types...>&& _M_get_u() const && noexcept
      { return std::move(_M_u); }

      constexpr _AA_Variadic_union<_Types...>&& _M_get_u() && noexcept
      { return std::move(_M_u); }


      _Variant_alloc<_Alloc>     _M_alloc_wrapper;
      _AA_Variadic_union<_Types...> _M_u;
      size_t _M_index;

      constexpr _Alloc get_allocator() const {
         return _M_alloc_wrapper._M_alloc;
      }


    };


  template<typename _Tp, typename _Alloc,  typename... _Ti, size_t... _Ind>
    struct _Build_FUNs<_Tp, basic_variant<_Alloc, _Ti...>, index_sequence<_Ind...>>
    : _Build_FUN<_Ind, _Tp, _Ti>...
    {
      using _Build_FUN<_Ind, _Tp, _Ti>::_S_fun...;
    };






  template<typename _Alloc, typename... _Types, size_t... __indices>
    struct _Variant_hash_base<basic_variant<_Alloc, _Types...>,
                  std::index_sequence<__indices...>>
    : _Base_dedup<__indices, __poison_hash<remove_const_t<_Types>>>... { };

  // Equivalent to decltype(get<_Np>(as-variant(declval<_Variant>())))
  template<size_t _Np, typename _Variant,
      typename _AsV = decltype(__variant::__as(std::declval<_Variant>())),
      typename _Tp = variant_alternative_t<_Np, remove_reference_t<_AsV>>>
    using __get_t
      = __conditional_t<is_lvalue_reference_v<_Variant>, _Tp&, _Tp&&>;


} // namespace __variant
} // namespace __detail

  template<typename _Tp, typename _Alloc, typename... _Types>
    constexpr bool
    holds_alternative(const basic_variant<_Alloc, _Types...>& __v) noexcept
    {
      static_assert(__detail::__variant::__exactly_once<_Tp, _Types...>,
            "T must occur exactly once in alternatives");
      return __v.index() == std::__find_uniq_type_in_pack<_Tp, _Types...>();
    }

  template<typename _Tp, typename _Alloc, typename... _Types>
    constexpr _Tp&
    get(basic_variant<_Alloc, _Types...>& __v)
    {
      static_assert(__detail::__variant::__exactly_once<_Tp, _Types...>,
            "T must occur exactly once in alternatives");
      static_assert(!is_void_v<_Tp>, "_Tp must not be void");
      constexpr size_t __n = std::__find_uniq_type_in_pack<_Tp, _Types...>();
      return std::get<__n>(__v);
    }

  template<typename _Tp, typename _Alloc, typename... _Types>
    constexpr _Tp&&
    get(basic_variant<_Alloc, _Types...>&& __v)
    {
      static_assert(__detail::__variant::__exactly_once<_Tp, _Types...>,
            "T must occur exactly once in alternatives");
      static_assert(!is_void_v<_Tp>, "_Tp must not be void");
      constexpr size_t __n = std::__find_uniq_type_in_pack<_Tp, _Types...>();
      return std::get<__n>(std::move(__v));
    }

  template<typename _Tp, typename _Alloc, typename... _Types>
    constexpr const _Tp&
    get(const basic_variant<_Alloc, _Types...>& __v)
    {
      static_assert(__detail::__variant::__exactly_once<_Tp, _Types...>,
            "T must occur exactly once in alternatives");
      static_assert(!is_void_v<_Tp>, "_Tp must not be void");
      constexpr size_t __n = std::__find_uniq_type_in_pack<_Tp, _Types...>();
      return std::get<__n>(__v);
    }

  template<typename _Tp, typename _Alloc, typename... _Types>
    constexpr const _Tp&&
    get(const basic_variant<_Alloc, _Types...>&& __v)
    {
      static_assert(__detail::__variant::__exactly_once<_Tp, _Types...>,
            "T must occur exactly once in alternatives");
      static_assert(!is_void_v<_Tp>, "_Tp must not be void");
      constexpr size_t __n = std::__find_uniq_type_in_pack<_Tp, _Types...>();
      return std::get<__n>(std::move(__v));
    }

  template<size_t _Np, typename _Alloc, typename... _Types>
    constexpr add_pointer_t<variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>>>
    get_if(basic_variant<_Alloc, _Types...>* __ptr) noexcept
    {
      using _Alternative_type = variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>>;
      static_assert(_Np < sizeof...(_Types),
            "The index must be in [0, number of alternatives)");
      static_assert(!is_void_v<_Alternative_type>, "_Tp must not be void");
      if (__ptr && __ptr->index() == _Np)
    return std::addressof(__detail::__variant::__get<_Np>(*__ptr));
      return nullptr;
    }

  template<size_t _Np, typename _Alloc, typename... _Types>
    constexpr
    add_pointer_t<const variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>>>
    get_if(const basic_variant<_Alloc, _Types...>* __ptr) noexcept
    {
      using _Alternative_type = variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>>;
      static_assert(_Np < sizeof...(_Types),
            "The index must be in [0, number of alternatives)");
      static_assert(!is_void_v<_Alternative_type>, "_Tp must not be void");
      if (__ptr && __ptr->index() == _Np)
    return std::addressof(__detail::__variant::__get<_Np>(*__ptr));
      return nullptr;
    }

  template<typename _Tp, typename _Alloc, typename... _Types>
    constexpr add_pointer_t<_Tp>
    get_if(basic_variant<_Alloc, _Types...>* __ptr) noexcept
    {
      static_assert(__detail::__variant::__exactly_once<_Tp, _Types...>,
            "T must occur exactly once in alternatives");
      static_assert(!is_void_v<_Tp>, "_Tp must not be void");
      constexpr size_t __n = std::__find_uniq_type_in_pack<_Tp, _Types...>();
      return std::get_if<__n>(__ptr);
    }

  template<typename _Tp, typename _Alloc, typename... _Types>
    constexpr add_pointer_t<const _Tp>
    get_if(const basic_variant<_Alloc, _Types...>* __ptr) noexcept
    {
      static_assert(__detail::__variant::__exactly_once<_Tp, _Types...>,
            "T must occur exactly once in alternatives");
      static_assert(!is_void_v<_Tp>, "_Tp must not be void");
      constexpr size_t __n = std::__find_uniq_type_in_pack<_Tp, _Types...>();
      return std::get_if<__n>(__ptr);
    }

#define _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(__OP, __NAME) \
  template<typename _AllocLhs, typename _AllocRhs, typename... _Types> \
    constexpr bool operator __OP(const basic_variant<_AllocLhs, _Types...>& __lhs, \
                 const basic_variant<_AllocRhs, _Types...>& __rhs) \
    { \
      bool __ret = true; \
      __detail::__variant::__raw_idx_visit( \
        [&__ret, &__lhs] (auto&& __rhs_mem, auto __rhs_index) mutable \
        { \
      if constexpr (__rhs_index != variant_npos) \
        { \
          if (__lhs.index() == __rhs_index) \
            { \
          auto& __this_mem = std::get<__rhs_index>(__lhs);  \
                  __ret = __this_mem __OP __rhs_mem; \
                } \
          else \
        __ret = (__lhs.index() + 1) __OP (__rhs_index + 1); \
            } \
          else \
            __ret = (__lhs.index() + 1) __OP (__rhs_index + 1); \
    }, __rhs); \
      return __ret; \
    }

  _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(<, less)
  _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(<=, less_equal)
  _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(==, equal)
  _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(!=, not_equal)
  _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(>=, greater_equal)
  _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(>, greater)

#undef _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE

//  not needed, exists for std::Variant
//  constexpr bool operator==(monostate, monostate) noexcept { return true; }

#ifdef __cpp_lib_three_way_comparison
  template<typename _AllocV, typename _AllocW, typename... _Types>
    requires (three_way_comparable<_Types> && ...)
    constexpr
    common_comparison_category_t<compare_three_way_result_t<_Types>...>
    operator<=>(const basic_variant<_AllocV,_Types...>& __v, const basic_variant<_AllocW, _Types...>& __w)
    {
      common_comparison_category_t<compare_three_way_result_t<_Types>...> __ret
    = strong_ordering::equal;

      __detail::__variant::__raw_idx_visit(
    [&__ret, &__v] (auto&& __w_mem, auto __w_index) mutable
    {
      if constexpr (__w_index != variant_npos)
        {
          if (__v.index() == __w_index)
        {
          auto& __this_mem = std::get<__w_index>(__v);
          __ret = __this_mem <=> __w_mem;
          return;
        }
        }
      __ret = (__v.index() + 1) <=> (__w_index + 1);
    }, __w);
      return __ret;
    }

//  constexpr strong_ordering
//  operator<=>(monostate, monostate) noexcept { return strong_ordering::equal; }
#else
//  constexpr bool operator!=(monostate, monostate) noexcept { return false; }
//  constexpr bool operator<(monostate, monostate) noexcept { return false; }
//  constexpr bool operator>(monostate, monostate) noexcept { return false; }
//  constexpr bool operator<=(monostate, monostate) noexcept { return true; }
//  constexpr bool operator>=(monostate, monostate) noexcept { return true; }
#endif

  template<typename _Visitor, typename... _Variants>
    constexpr __detail::__variant::__visit_result_t<_Visitor, _Variants...>
    visit(_Visitor&&, _Variants&&...);

  template<typename _Alloc, typename... _Types>
    _GLIBCXX20_CONSTEXPR
    inline enable_if_t<(is_move_constructible_v<_Types> && ...)
            && (is_swappable_v<_Types> && ...)>
    swap(basic_variant<_Alloc,_Types...>& __lhs, basic_variant<_Alloc,_Types...>& __rhs)
    noexcept(noexcept(__lhs.swap(__rhs)))
    { __lhs.swap(__rhs); }

  template<typename _Alloc, typename... _Types>
    enable_if_t<!((is_move_constructible_v<_Types> && ...)
           && (is_swappable_v<_Types> && ...))>
    swap(basic_variant<_Alloc,_Types...>&, basic_variant<_Alloc,_Types...>&) = delete;


  template<typename _Alloc, typename... _Types>
    class basic_variant
    : private __detail::__variant::_Basic_Variant_storage<_Alloc, _Types...>
    {
    private:
      template <typename _UAlloc, typename... _UTypes, typename _Tp>
    friend _GLIBCXX20_CONSTEXPR decltype(auto)
    __basic_variant_cast(_Tp&&);

      static_assert(sizeof...(_Types) > 0,
            "basic_variant must have at least one alternative");
      static_assert(!(std::is_reference_v<_Types> || ...),
            "basic_variant must have no reference alternative");
      static_assert(!(std::is_void_v<_Types> || ...),
            "basic_variant must have no void alternative");

      using _Storage_Base = __detail::__variant::_Basic_Variant_storage<_Alloc, _Types...>;


      template<typename _Tp>
    static constexpr bool __not_self
      = !is_same_v<__remove_cvref_t<_Tp>, basic_variant>;

      template<typename _Tp>
    static constexpr bool
    __exactly_once = __detail::__variant::__exactly_once<_Tp, _Types...>;

      template<typename _Tp>
    static constexpr size_t __accepted_index
      = __detail::__variant::__accepted_index<_Tp, basic_variant>::value;

      template<size_t _Np, typename = enable_if_t<(_Np < sizeof...(_Types))>>
    using __to_type = typename _Nth_type<_Np, _Types...>::type;

      template<typename _Tp, typename = enable_if_t<__not_self<_Tp>>>
    using __accepted_type = __to_type<__accepted_index<_Tp>>;

      template<typename _Tp>
    static constexpr size_t __index_of
      = std::__find_uniq_type_in_pack<_Tp, _Types...>();

      using _Traits = __detail::__variant::_Traits<_Types...>;

      template<typename _Tp>
    struct __is_in_place_tag : false_type { };
      template<typename _Tp>
    struct __is_in_place_tag<in_place_type_t<_Tp>> : true_type { };
      template<size_t _Np>
    struct __is_in_place_tag<in_place_index_t<_Np>> : true_type { };

      template<typename _Tp>
    static constexpr bool __not_in_place_tag
      = !__is_in_place_tag<__remove_cvref_t<_Tp>>::value;

    public:
      using allocator_traits = std::allocator_traits<_Alloc>;
      using allocator_type = _Alloc;

      //defaults
      constexpr basic_variant()
      requires(__detail::__variant::_Traits<_Types...>::_S_default_ctor)
        = default;

      basic_variant(const basic_variant& __rhs)
      requires(__detail::__variant::_Traits<_Types...>::_S_copy_ctor) = default;
      basic_variant(basic_variant&&)
      requires(__detail::__variant::_Traits<_Types...>::_S_move_ctor) = default;
      basic_variant& operator=(const basic_variant&)
      requires(__detail::__variant::_Traits<_Types...>::_S_copy_assign) = default;
      basic_variant& operator=(basic_variant&&)
      requires(__detail::__variant::_Traits<_Types...>::_S_move_assign) = default;
      _GLIBCXX20_CONSTEXPR ~basic_variant() = default;

      template<typename _Tp,
                 typename = enable_if_t<sizeof...(_Types) != 0>,
                 typename = enable_if_t<__not_in_place_tag<_Tp>>,
                 typename _Tj = __accepted_type<_Tp&&>,
                 typename = enable_if_t<__exactly_once<_Tj>
                            && is_constructible_v<_Tj, _Tp>>>
      constexpr
      basic_variant(_Tp&& __t)
      noexcept(is_nothrow_constructible_v<_Tj, _Tp> &&
          (!uses_allocator_v<remove_cv_t<_Tj>, allocator_type> ||
          allocator_traits::is_always_equal::value))
      : _Storage_Base(in_place_index<__accepted_index<_Tp>>,
            std::forward<_Tp>(__t))
      { }


      template<typename _AllocU>
      constexpr
      basic_variant(const basic_variant<_AllocU, _Types...>& w)
      requires (__detail::__variant::_Traits<_Types...>::_S_copy_ctor
                  && is_convertible_v<const _AllocU&, allocator_type>
                  && !is_same_v<_AllocU, _Alloc>)
      : _Storage_Base(static_cast<const __detail::__variant::_Basic_Variant_storage<_AllocU, _Types...> &>(w))
      { }

      template<typename _AllocU>
      constexpr
      basic_variant(const basic_variant<_AllocU, _Types...>& w)
      requires (__detail::__variant::_Traits<_Types...>::_S_move_ctor
                  && !is_convertible_v<const _AllocU&, allocator_type>
                  && !is_same_v<_AllocU, _Alloc>) = delete;

      template<typename _AllocU>
      constexpr
      basic_variant(basic_variant<_AllocU, _Types...>&& w)
      requires (__detail::__variant::_Traits<_Types...>::_S_move_ctor
                  && is_convertible_v<const _AllocU&, allocator_type>
                  && !is_same_v<_AllocU, _Alloc>)
      : _Storage_Base(std::move(static_cast<__detail::__variant::_Basic_Variant_storage<_AllocU, _Types...>&&>(w)))
      { }

      template<typename _AllocU>
      constexpr
      basic_variant(basic_variant<_AllocU, _Types...>& w)
      requires (__detail::__variant::_Traits<_Types...>::_S_copy_ctor
                &&  !is_convertible_v<const _AllocU&, allocator_type>
                && !is_same_v<_AllocU, _Alloc>) = delete;


      template<typename _Tp, typename... _Args,
           typename = enable_if_t<__exactly_once<_Tp>
                      && is_constructible_v<_Tp, _Args...>>>
      constexpr explicit
      basic_variant(in_place_type_t<_Tp>, _Args&&... __args)
      : _Storage_Base(in_place_index<__index_of<_Tp>>,
            std::forward<_Args>(__args)...)
      { }

      template<typename _Tp, typename _Up, typename... _Args,
                 typename = enable_if_t<__exactly_once<_Tp>
                            && is_constructible_v<_Tp,
                             initializer_list<_Up>&, _Args...>>>
      constexpr explicit
      basic_variant(in_place_type_t<_Tp>, initializer_list<_Up> __il,
          _Args&&... __args)
      : _Storage_Base(in_place_index<__index_of<_Tp>>, __il,
            std::forward<_Args>(__args)...)
      { }

      template<size_t _Np, typename... _Args,
           typename _Tp = __to_type<_Np>,
           typename = enable_if_t<is_constructible_v<_Tp, _Args...>>>
      constexpr explicit
      basic_variant(in_place_index_t<_Np>, _Args&&... __args)
      : _Storage_Base(in_place_index<_Np>, std::forward<_Args>(__args)...)
      { }

      template<size_t _Np, typename _Up, typename... _Args,
           typename _Tp = __to_type<_Np>,
           typename = enable_if_t<is_constructible_v<_Tp,
                             initializer_list<_Up>&,
                             _Args...>>>
      constexpr explicit
      basic_variant(in_place_index_t<_Np>, initializer_list<_Up> __il,
          _Args&&... __args)
      : _Storage_Base(in_place_index<_Np>, __il, std::forward<_Args>(__args)...)
      { }

      // allocator extended constructors
      constexpr basic_variant(std::allocator_arg_t, const allocator_type & __a)
      requires(__detail::__variant::_Traits<_Types...>::_S_default_ctor)
      : _Storage_Base(std::allocator_arg, __a) {}

      constexpr basic_variant(std::allocator_arg_t, const allocator_type & __a,
                              const basic_variant& _other)
      requires(__detail::__variant::_Traits<_Types...>::_S_copy_ctor)
      : _Storage_Base(std::allocator_arg, __a, _other) {}

      constexpr basic_variant(std::allocator_arg_t, const allocator_type & __a,
                              basic_variant&& _other)
      requires(__detail::__variant::_Traits<_Types...>::_S_move_ctor)
      : _Storage_Base(std::allocator_arg, __a, std::move(_other)) {}
     
      template<typename _Tp,
                 typename = enable_if_t<sizeof...(_Types) != 0>,
                 typename = enable_if_t<__not_in_place_tag<_Tp>>,
                 typename _Tj = __accepted_type<_Tp&&>,
                 typename = enable_if_t<__exactly_once<_Tj>
                            && is_constructible_v<_Tj, _Tp>>>
      constexpr
      basic_variant(std::allocator_arg_t, const allocator_type & __a, _Tp&& __t)
      noexcept(is_nothrow_constructible_v<_Tj, _Tp>)
      : _Storage_Base(std::allocator_arg, __a,
            in_place_index<__accepted_index<_Tp>>,
            std::forward<_Tp>(__t))
      { }

      template<typename _Tp, typename... _Args,
           typename = enable_if_t<__exactly_once<_Tp>
                      && is_constructible_v<_Tp, _Args...>>>
      constexpr explicit
      basic_variant(std::allocator_arg_t, const allocator_type & __a,
          in_place_type_t<_Tp>, _Args&&... __args)
      : _Storage_Base(std::allocator_arg, __a,
          in_place_index<__index_of<_Tp>>,
            std::forward<_Args>(__args)...)
      { }

      template<typename _Tp, typename _Up, typename... _Args,
                 typename = enable_if_t<__exactly_once<_Tp>
                            && is_constructible_v<_Tp,
                             initializer_list<_Up>&, _Args...>>>
      constexpr explicit
      basic_variant(std::allocator_arg_t, const allocator_type & __a,
          in_place_type_t<_Tp>, initializer_list<_Up> __il,
          _Args&&... __args)
      : _Storage_Base(std::allocator_arg, __a,
          in_place_index<__index_of<_Tp>>, __il,
            std::forward<_Args>(__args)...)
      { }

      template<size_t _Np, typename... _Args,
           typename _Tp = __to_type<_Np>,
           typename = enable_if_t<is_constructible_v<_Tp, _Args...>>>
      constexpr explicit
      basic_variant(std::allocator_arg_t, const allocator_type & __a,
          in_place_index_t<_Np>, _Args&&... __args)
      : _Storage_Base(std::allocator_arg, __a,
          in_place_index<_Np>, std::forward<_Args>(__args)...)
      { }

      template<size_t _Np, typename _Up, typename... _Args,
           typename _Tp = __to_type<_Np>,
           typename = enable_if_t<is_constructible_v<_Tp,
                             initializer_list<_Up>&,
                             _Args...>>>
      constexpr explicit
      basic_variant(std::allocator_arg_t, const allocator_type & __a,
          in_place_index_t<_Np>, initializer_list<_Up> __il,
          _Args&&... __args)
      : _Storage_Base(std::allocator_arg, __a,
          in_place_index<_Np>, __il, std::forward<_Args>(__args)...)
      { }


      template<typename _Tp>
          _GLIBCXX20_CONSTEXPR
          enable_if_t<__exactly_once<__accepted_type<_Tp&&>>
                  && is_constructible_v<__accepted_type<_Tp&&>, _Tp>
                  && is_assignable_v<__accepted_type<_Tp&&>&, _Tp>,
                  basic_variant&>
          operator=(_Tp&& __rhs)
          noexcept(is_nothrow_assignable_v<__accepted_type<_Tp&&>&, _Tp>
               && is_nothrow_constructible_v<__accepted_type<_Tp&&>, _Tp> &&
              (!uses_allocator_v<remove_cvref_t<__accepted_type<_Tp&&>>, allocator_type>
                || allocator_traits::is_always_equal::value))
          {
            constexpr auto __index = __accepted_index<_Tp>;
            if (index() == __index)
              std::get<__index>(*this) = std::forward<_Tp>(__rhs);
            else
              {
                using _Tj = __accepted_type<_Tp&&>;
                if constexpr (is_nothrow_constructible_v<_Tj, _Tp>
                      || !is_nothrow_move_constructible_v<_Tj>)
              this->emplace<__index>(std::forward<_Tp>(__rhs));
                else
              operator=(basic_variant(allocator_arg, this->get_allocator(), std::forward<_Tp>(__rhs)));
              }
            return *this;
          }

      template<typename _AllocU>
      constexpr
          operator=(const basic_variant<_AllocU, _Types...>& __rhs)
          noexcept(__detail::__variant::_Traits<_Types...>::_S_nothrow_copy_assign && allocator_traits::is_always_equal::value )
          requires (__detail::__variant::_Traits<_Types...>::_S_copy_assign
                          && is_convertible_v<const _AllocU&, allocator_type>)
     {
       if constexpr (allocator_traits::propagate_on_container_copy_assignment::value)
            this->_M_alloc_wrapper._M_alloc = __rhs.get_allocator;

       std::__detail::__variant::__raw_idx_visit(
       [this](auto&& __rhs_mem, auto __rhs_index) mutable
       {
         constexpr size_t __j = __rhs_index;
         if constexpr (__j == variant_npos)
           this->_M_reset(); // Make *this valueless.
         else if (this->_M_index == __j)
           std::get<__j>(*this) = __rhs_mem;
         else
           {
         using _Tj = typename _Nth_type<__j, _Types...>::type;
         if constexpr (is_nothrow_copy_constructible_v<_Tj>
                   || !is_nothrow_move_constructible_v<_Tj>)

         {
           this-> template __emplace<__j>( __rhs_mem);
         }
         else
           {
             *this = _Basic_Variant(allocator_arg, get_allocator(), in_place_index<__j>, __rhs_mem);
           }
           }
         }, __rhs);
       return *this;
     }

      template<typename _AllocU>
      constexpr
          operator=(const basic_variant<_AllocU, _Types...>& __rhs)
                  requires (__detail::__variant::_Traits<_Types...>::_S_copy_assign
                  && !is_convertible_v<const _AllocU&, allocator_type>) = delete;

      template<typename _AllocU>
      constexpr
          operator=(basic_variant<_AllocU, _Types...>&& __rhs)
          noexcept(__detail::__variant::_Traits<_Types...>::_S_nothrow_move_assign
                  && ( allocator_traits::is_always_equal::value  ||
                       allocator_traits::propagate_on_container_move_assignment::value))
          requires (__detail::__variant::_Traits<_Types...>::_S_move_assign
                       && is_convertible_v<const _AllocU&, allocator_type>)
      {
        if constexpr (allocator_traits::propagate_on_container_move_assignment::value)
              this->_M_alloc_wrapper._M_alloc = __rhs.get_allocator;

        std::__detail::__variant::__raw_idx_visit(
            [this](auto&& __rhs_mem, auto __rhs_index) mutable
            {
              constexpr size_t __j = __rhs_index;
              if constexpr (__j != variant_npos)
                {
              if (this->_M_index == __j)
                  std::get<__j>(*this) = std::move(__rhs_mem);
              else
                {
                  using _Tj = typename _Nth_type<__j, _Types...>::type;
                  if constexpr (is_nothrow_move_constructible_v<_Tj> &&
                      allocator_traits::is_always_equal::value  ||
                      uses_allocator_v<remove_cv_t<_Tj>, _Alloc>)
                       this->template __emplace<__j>(std::move(__rhs_mem));
                  else
                    {

                       emplace<__j>(std::move(__rhs_mem));
                    }
                }
                }
              else
                this->_M_reset();
            }, __rhs);
          return *this;
      }


      template<typename _AllocU>
      constexpr
          operator=(basic_variant<_AllocU, _Types...>&& __rhs)
                  requires (__detail::__variant::_Traits<_Types...>::_S_move_assign
                  && !is_convertible_v<const _AllocU&, allocator_type>) = delete;

      constexpr bool valueless_by_exception() const noexcept
      { return !this->_M_valid(); }


      template<typename _Tp, typename... _Args>
    _GLIBCXX20_CONSTEXPR
    enable_if_t<is_constructible_v<_Tp, _Args...> && __exactly_once<_Tp>,
            _Tp&>
    emplace(_Args&&... __args)
    {
      constexpr size_t __index = __index_of<_Tp>;
      return this->emplace<__index>(std::forward<_Args>(__args)...);
    }

      template<typename _Tp, typename _Up, typename... _Args>
    _GLIBCXX20_CONSTEXPR
    enable_if_t<is_constructible_v<_Tp, initializer_list<_Up>&, _Args...>
            && __exactly_once<_Tp>,
            _Tp&>
    emplace(initializer_list<_Up> __il, _Args&&... __args)
    {
      constexpr size_t __index = __index_of<_Tp>;
      return this->emplace<__index>(__il, std::forward<_Args>(__args)...);
    }

      template<size_t _Np, typename... _Args>
    _GLIBCXX20_CONSTEXPR
    enable_if_t<is_constructible_v<__to_type<_Np>, _Args...>,
            __to_type<_Np>&>
    emplace(_Args&&... __args)
    {
        namespace __variant = std::__detail::__variant;
      using type = typename _Nth_type<_Np, _Types...>::type;
       // Provide the strong exception-safety guarantee when possible,
       // to avoid becoming valueless.
       if constexpr (is_nothrow_constructible_v<type, _Args...>)
         {
           this-> template __emplace<_Np>(std::forward<_Args>(__args)...);
         }
       else if constexpr (is_scalar_v<type>)
         {
           // This might invoke a potentially-throwing conversion operator:
           const type __tmp(std::forward<_Args>(__args)...);
           // But this won't throw:
           this-> template __emplace<_Np>(__tmp);
         }
       else if constexpr (__variant::_Never_valueless_alt<type>()
           && _Traits::_S_move_assign)
         {
           // This construction might throw:
           basic_variant __tmp(allocator_arg, this->get_allocator(),in_place_index<_Np>,
                 std::forward<_Args>(__args)...);
           // But _Never_valueless_alt<type> means this won't:
           *this = std::move(__tmp);
         }
       else
         {
           // This case only provides the basic exception-safety guarantee,
           // i.e. the variant can become valueless.
         this-> template __emplace<_Np>(std::forward<_Args>(__args)...);
         }
      return std::get<_Np>(*this);
    }

      template<size_t _Np, typename _Up, typename... _Args>
    _GLIBCXX20_CONSTEXPR
    enable_if_t<is_constructible_v<__to_type<_Np>,
                       initializer_list<_Up>&, _Args...>,
            __to_type<_Np>&>
    emplace(initializer_list<_Up> __il, _Args&&... __args)
    {
      namespace __variant = std::__detail::__variant;
      using type = typename _Nth_type<_Np, _Types...>::type;
      // Provide the strong exception-safety guarantee when possible,
      // to avoid becoming valueless.
      if constexpr (is_nothrow_constructible_v<type,
                                 initializer_list<_Up>&,
                                 _Args...>)
        {
          this-> template __emplace<_Np>(__il,
                    std::forward<_Args>(__args)...);
        }
      else if constexpr (__variant::_Never_valueless_alt<type>()
          && _Traits::_S_move_assign)
        {
          // This construction might throw:
          basic_variant __tmp(allocator_arg, this->get_allocator(), in_place_index<_Np>, __il,
                std::forward<_Args>(__args)...);
          // But _Never_valueless_alt<type> means this won't:
          *this = std::move(__tmp);
        }
      else
        {
          // This case only provides the basic exception-safety guarantee,
          // i.e. the variant can become valueless.
          this-> template __emplace<_Np>( __il,
                    std::forward<_Args>(__args)...);
        }
      return std::get<_Np>(*this);
    }

      template<size_t _Np, typename... _Args>
    enable_if_t<!(_Np < sizeof...(_Types))> emplace(_Args&&...) = delete;

      template<typename _Tp, typename... _Args>
    enable_if_t<!__exactly_once<_Tp>> emplace(_Args&&...) = delete;

      constexpr size_t index() const noexcept
      {
         return this->_M_index;
      }

      constexpr allocator_type get_allocator() const
      {
        return _Storage_Base::get_allocator();
      }

      _GLIBCXX20_CONSTEXPR
      void
      swap(basic_variant& __rhs)
      noexcept((__is_nothrow_swappable<_Types>::value && ...)
           && is_nothrow_move_constructible_v<basic_variant>)
      {
        static_assert((is_move_constructible_v<_Types> && ...));

        if constexpr (allocator_traits::propagate_on_container_swap::value)
              std::swap(this->_M_alloc_wrapper._M_alloc, __rhs._M_alloc_wrapper._M_alloc);

        __detail::__variant::__raw_idx_visit(
            [this, &__rhs](auto&& __rhs_mem, auto __rhs_index) mutable
            {
              if constexpr (__rhs_index != variant_npos)
              {
                if (this->index() == __rhs_index)
                {
                    auto& __this_mem =
                        std::get<__rhs_index>(*this);
                    using std::swap;
                    swap(__this_mem, __rhs_mem);
                }
                else
                {
                  if (!this->valueless_by_exception()) [[__likely__]]
                  {
                     auto __tmp(std::move(__rhs_mem));
                     __detail::__variant::__raw_idx_visit(
                     [&__rhs](auto&& __this_mem, auto __this_index) mutable
                     {
                       constexpr size_t __k = __this_index;
                       if constexpr (__k != variant_npos)
                           __rhs. template emplace<__k>(std::move(__this_mem));
                     }, *this);
                     this->template __emplace<__rhs_index>(std::move(__tmp));
                  }
                  else
                  {
                    this->template __emplace<__rhs_index>(std::move(__rhs_mem));
                    __rhs._M_reset();
                  }
                }
              }
              else
              {
                if (!this->valueless_by_exception()) [[__likely__]]
                {
                  __detail::__variant::__raw_idx_visit(
                      [&__rhs](auto&& __this_mem, auto __this_index) mutable
                   {
                     constexpr size_t __k = __this_index;
                     if constexpr (__k != variant_npos)
                         __rhs. template emplace<__k>(std::move(__this_mem));
                   }, *this);
                    this->_M_reset();
                }
               }
            }, __rhs);
      }

#if defined(__clang__) && __clang_major__ <= 7
    public:
      using _Base::_M_u; // See https://bugs.llvm.org/show_bug.cgi?id=31852
#endif


    private:
      template<size_t _Np, typename _Vp>
    friend constexpr decltype(auto)
    __detail::__variant::__get(_Vp&& __v) noexcept;

      template<typename _AllocU, typename... _UTypes>
      friend class basic_variant;

#define _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(__OP) \
    template<typename _AllocLhs, typename _AllocRhs, typename... _Ts> \
    friend constexpr bool \
    operator __OP(const basic_variant<_AllocLhs, _Ts...>& __lhs, \
              const basic_variant<_AllocRhs, _Ts...>& __rhs);

      _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(<)
      _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(<=)
      _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(==)
      _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(!=)
      _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(>=)
      _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE(>)

#undef _BASIC_VARIANT_RELATION_FUNCTION_TEMPLATE
    };

  template<size_t _Np, typename _Alloc, typename... _Types>
    constexpr variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>>&
    get(basic_variant<_Alloc, _Types...>& __v)
    {
      static_assert(_Np < sizeof...(_Types),
            "The index must be in [0, number of alternatives)");
      if (__v.index() != _Np)
    __throw_bad_variant_access(__v.valueless_by_exception());
      return __detail::__variant::__get<_Np>(__v);
    }

  template<size_t _Np, typename _Alloc,  typename... _Types>
    constexpr variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>>&&
    get(basic_variant<_Alloc, _Types...>&& __v)
    {
      static_assert(_Np < sizeof...(_Types),
            "The index must be in [0, number of alternatives)");
      if (__v.index() != _Np)
    __throw_bad_variant_access(__v.valueless_by_exception());
      return __detail::__variant::__get<_Np>(std::move(__v));
    }

  template<size_t _Np, typename _Alloc,  typename... _Types>
    constexpr const variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>>&
    get(const basic_variant<_Alloc, _Types...>& __v)
    {
      static_assert(_Np < sizeof...(_Types),
            "The index must be in [0, number of alternatives)");
      if (__v.index() != _Np)
    __throw_bad_variant_access(__v.valueless_by_exception());
      return __detail::__variant::__get<_Np>(__v);
    }

  template<size_t _Np, typename _Alloc,  typename... _Types>
    constexpr const variant_alternative_t<_Np, basic_variant<_Alloc, _Types...>>&&
    get(const basic_variant<_Alloc, _Types...>&& __v)
    {
      static_assert(_Np < sizeof...(_Types),
            "The index must be in [0, number of alternatives)");
      if (__v.index() != _Np)
    __throw_bad_variant_access(__v.valueless_by_exception());
      return __detail::__variant::__get<_Np>(std::move(__v));
    }


  /// @cond undocumented
  template<bool, typename _Alloc, typename... _Types>
    struct __basic_variant_hash_call_base_impl
    {
      size_t
      operator()(const basic_variant<_Alloc,_Types...>& __t) const
      noexcept((is_nothrow_invocable_v<hash<decay_t<_Types>>, _Types> && ...))
      {
    size_t __ret;
    __detail::__variant::__raw_visit(
      [&__t, &__ret](auto&& __t_mem) mutable
      {
        using _Type = __remove_cvref_t<decltype(__t_mem)>;
        if constexpr (!is_same_v<_Type,
                         __detail::__variant::__variant_cookie>)
          __ret = std::hash<size_t>{}(__t.index())
              + std::hash<_Type>{}(__t_mem);
        else
          __ret = std::hash<size_t>{}(__t.index());
      }, __t);
    return __ret;
      }
    };

  template<typename _Alloc, typename... _Types>
    struct __basic_variant_hash_call_base_impl<false, _Alloc, _Types...> {};

  template<typename _Alloc, typename... _Types>
    using __basic_variant_hash_call_base =
    __basic_variant_hash_call_base_impl<(__poison_hash<remove_const_t<_Types>>::
                   __enable_hash_call &&...), _Alloc, _Types...>;
  /// @endcond

  template<typename _Alloc, typename... _Types>
    struct hash<basic_variant<_Alloc,_Types...>>
    : private __detail::__variant::_Variant_hash_base<
      basic_variant<_Alloc,_Types...>, std::index_sequence_for<_Types...>>,
      public __basic_variant_hash_call_base<_Alloc, _Types...>
    {
      using result_type [[__deprecated__]] = size_t;
      using argument_type [[__deprecated__]] = basic_variant<_Alloc, _Types...>;
    };


  template<typename _Alloc, typename... _Types>
    struct __is_fast_hash<hash<basic_variant<_Alloc,_Types...>>>
    : bool_constant<(__is_fast_hash<_Types>::value && ...)>
    { };

  namespace pmr{
     template<typename... Types>
     using variant = basic_variant<polymorphic_allocator<char>, Types...>;
  }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif // C++17

#endif // _GLIBCXX_VARIANT
