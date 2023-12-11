// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 } }

#include <testsuite_hooks.h>
#include "../../../../include/std/basic_variant.h"
#include "memory_resource"

using namespace std;
template<typename _Tp = void,
         bool SOCCC = true,
         bool POCCA = false,
         bool POCMA = false,
         bool POCS = false,
         bool IAE = false>
struct MyAlloc{
   /// The allocator type
    using allocator_type = MyAlloc<_Tp, SOCCC, POCCA, POCMA, POCS, IAE>;

    /// The allocated type
    using value_type = _Tp;

    /// The allocator's pointer type.
    using pointer = _Tp*;

    /// The allocator's const pointer type.
    using const_pointer = const _Tp*;

    /// The allocator's void pointer type.
    using void_pointer = void*;

    /// The allocator's const void pointer type.
    using const_void_pointer = const void*;

    /// The allocator's difference type
    using difference_type = std::ptrdiff_t;

    /// The allocator's size type
    using size_type = std::size_t;

    using propagate_on_container_copy_assignment = integral_constant<bool, POCCA>;
    using propagate_on_container_move_assignment = integral_constant<bool, POCMA>;
    using propagate_on_container_swap = integral_constant<bool, POCS>;

    constexpr allocator_type
    select_on_container_copy_construction() const noexcept
    { if constexpr(SOCCC == true)  return *this;
      else return MyAlloc<_Tp,SOCCC, POCCA, POCMA, POCS, IAE>{};
    }
    /// @}

    /// Whether all instances of the allocator type compare equal.
    using is_always_equal = integral_constant<bool, IAE>;

    constexpr MyAlloc() = default;

    constexpr MyAlloc(int _id) : id ( _id){ }
    template<typename U>
    constexpr MyAlloc(const MyAlloc<U, SOCCC, POCCA, POCMA, POCS, IAE>& rhs) : id ( rhs.id){ }

    int id = 0;

};
struct Throws{
    Throws(){};
    Throws(const Throws&){ throw 1;}
    Throws& operator=(const Throws&) = default;
};

template<bool SOCCC = false,
         bool POCCA = false,
         bool POCMA = false,
         bool POCS = false,
         bool IAE = false>
struct X {
  using allocator_type = MyAlloc<X, SOCCC, POCCA, POCMA, POCS, IAE>;
  using allocator_traits = std::allocator_traits<allocator_type>;
  constexpr X() = default;
  constexpr X(std::allocator_arg_t, const allocator_type& a) : alloc(a) {};

  constexpr X(int i) : value(i){};
  constexpr X(std::allocator_arg_t, const allocator_type& a, int i):
          value(i), alloc(a) {};

  constexpr X(const X& other) : value(other.value), alloc(allocator_traits::select_on_container_copy_construction(other.alloc)){};
  constexpr X(std::allocator_arg_t, const allocator_type& a, const X& other):
          value(other.value), alloc(a) {};


  X& operator=(const X& other){
    if constexpr (allocator_traits::propagate_on_container_copy_assignment::value)
        alloc = other.alloc ;
    value = other.value;
    return *this;
  }

  X& operator=(X&& other){
    if constexpr (allocator_traits::propagate_on_container_move_assignment::value)
        alloc = other.alloc ;
    value = other.value;
    return *this;
  }

  int value = 0;
  allocator_type alloc = allocator_type{-1};

};
template<bool SOCCC = true,
         bool POCCA = false,
         bool POCMA = false,
         bool POCS = false,
         bool IAE = false>
void checkPropagationCopy(int A_VAL, int B_VAL){

  using myX = X<SOCCC, POCCA, POCMA, POCS, IAE>;
  using AllocX = MyAlloc<myX, SOCCC, POCCA, POCMA, POCS, IAE>;
  using Allocint = MyAlloc<int,SOCCC, POCCA, POCMA, POCS, IAE>;
  using variantX = std::basic_variant<AllocX, int, myX, Throws>;
  using variantint = std::basic_variant<Allocint, int, myX, Throws>;


  {
    variantX obj{std::allocator_arg, AllocX{5}, myX{42}};
    variantX copy1(obj);
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == A_VAL);
    VERIFY(get<myX>(copy1).alloc.id == A_VAL);
  }
  {
    variantX obj{std::allocator_arg, AllocX{5}, myX{42}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    variantX copy1(obj);
    VERIFY(copy1.get_allocator().id == A_VAL);
    VERIFY(copy1.valueless_by_exception());
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    variantX copy1(obj);
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == B_VAL);
    VERIFY(get<myX>(copy1).alloc.id == B_VAL);
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    variantX copy1(obj);
    VERIFY(copy1.get_allocator().id == B_VAL);
    VERIFY(copy1.valueless_by_exception());
  }
  {
    variantX obj{std::allocator_arg, AllocX{5}, myX{42}};
    variantX copy1(std::allocator_arg, Allocint{6}, obj);
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(get<myX>(copy1).alloc.id == 6);
  }
  {
    variantX obj{std::allocator_arg, AllocX{5}, myX{42}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    variantX copy1(std::allocator_arg, Allocint{6}, obj);
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(copy1.valueless_by_exception());
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    variantX copy1(std::allocator_arg, Allocint{6}, obj);
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(get<myX>(copy1).alloc.id == 6);
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    variantX copy1(std::allocator_arg, Allocint{6}, obj);
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(copy1.valueless_by_exception());
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    variantX copy1(std::allocator_arg, AllocX{6}, obj);
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(get<myX>(copy1).alloc.id == 6);
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    variantX copy1(std::allocator_arg, AllocX{6}, obj);
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(copy1.valueless_by_exception());
  }
}
template<bool SOCCC = true,
         bool POCCA = false,
         bool POCMA = false,
         bool POCS = false,
         bool IAE = false>
void checkPropagationMove(){

  using myX = X<SOCCC, POCCA, POCMA, POCS, IAE>;
  using AllocX = MyAlloc<myX, SOCCC, POCCA, POCMA, POCS, IAE>;
  using Allocint = MyAlloc<int,SOCCC, POCCA, POCMA, POCS, IAE>;
  using variantX = std::basic_variant<AllocX, int, myX, Throws>;
  using variantint = std::basic_variant<Allocint, int, myX, Throws>;


  {
    variantX obj{std::allocator_arg, AllocX{5}, myX{42}};
    variantX copy1(std::move(obj));
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 5);
    VERIFY(get<myX>(copy1).alloc.id == 5);
  }
  {
    variantX obj{std::allocator_arg, AllocX{5}, myX{42}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    variantX copy1(std::move(obj));
    VERIFY(copy1.get_allocator().id == 5);
    VERIFY(copy1.valueless_by_exception());
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    variantX copy1(std::move(obj));
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 5);
    VERIFY(get<myX>(copy1).alloc.id == 5);
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    variantX copy1(std::move(obj));
    VERIFY(copy1.get_allocator().id == 5);
    VERIFY(copy1.valueless_by_exception());
  }
  {
    variantX obj{std::allocator_arg, AllocX{5}, myX{42}};
    variantX copy1(std::allocator_arg, Allocint{6}, std::move(obj));
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(get<myX>(copy1).alloc.id == 6);
  }
  {
    variantX obj{std::allocator_arg, AllocX{5}, myX{42}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    variantX copy1(std::allocator_arg, Allocint{6}, std::move(obj));
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(copy1.valueless_by_exception());
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    variantX copy1(std::allocator_arg, Allocint{6}, std::move(obj));
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(get<myX>(copy1).alloc.id == 6);
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    variantX copy1(std::allocator_arg, Allocint{6}, std::move(obj));
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(copy1.valueless_by_exception());
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    variantX copy1(std::allocator_arg, AllocX{6}, std::move(obj));
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(get<myX>(copy1).alloc.id == 6);
  }
  {
    variantint obj{std::allocator_arg, Allocint{5}, myX{42}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    variantX copy1(std::allocator_arg, AllocX{6}, std::move(obj));
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(copy1.valueless_by_exception());
  }
}

template<bool SOCCC = true,
         bool POCCA = false,
         bool POCMA = false,
         bool POCS = false,
         bool IAE = false>
void checkPropagationCopyAssign(int A_VAL, int B_VAL){

  using myX = X<SOCCC, POCCA, POCMA, POCS, IAE>;
  using AllocX = MyAlloc<myX, SOCCC, POCCA, POCMA, POCS, IAE>;
  using Allocint = MyAlloc<int,SOCCC, POCCA, POCMA, POCS, IAE>;
  using variantX = std::basic_variant<AllocX, int, myX, Throws>;
  using variantint = std::basic_variant<Allocint, int, myX, Throws>;


  {
    variantX obj1{std::allocator_arg, AllocX{5}, myX{42}};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};
    obj1 = obj2;
    VERIFY(get<myX>(obj1).value == 20);
    VERIFY(obj1.get_allocator().id == A_VAL);
    VERIFY(get<myX>(obj1).alloc.id == A_VAL);
  }
  {
    variantX obj1{std::allocator_arg, AllocX{5}, 42};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};
    obj1 = obj2;
    VERIFY(get<myX>(obj1).value == 20);
    VERIFY(obj1.get_allocator().id == A_VAL);
    VERIFY(get<myX>(obj1).alloc.id == A_VAL);
  }
  {
    variantX obj1{std::allocator_arg, AllocX{5}, 42};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};
    Throws t;
    try{ obj2.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj2.valueless_by_exception());
    obj1 = obj2;
    VERIFY(obj1.valueless_by_exception());
    VERIFY(obj1.get_allocator().id == A_VAL);
   }
  {
      variantX obj1{std::allocator_arg, AllocX{5}, 42};
      variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};
      Throws t;
      try{ obj1.template emplace<Throws>(t); }       catch(...){}
      VERIFY(obj1.valueless_by_exception());
      obj1 = obj2;
      VERIFY(get<myX>(obj1).value == 20);
      VERIFY(obj1.get_allocator().id == A_VAL);
      VERIFY(get<myX>(obj1).alloc.id == A_VAL);
   }
  {
    variantX obj1{std::allocator_arg, AllocX{5}, 42};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};
    Throws t;
    try{ obj2.template emplace<Throws>(t); }       catch(...){}
    try{ obj1.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj2.valueless_by_exception());
    VERIFY(obj1.valueless_by_exception());
    obj1 = obj2;
    VERIFY(obj1.valueless_by_exception());
    VERIFY(obj1.get_allocator().id == A_VAL);
  }
}
template<bool SOCCC = true,
         bool POCCA = false,
         bool POCMA = false,
         bool POCS = false,
         bool IAE = false>
void checkPropagationMoveAssign(int A_VAL, int B_VAL){

  using myX = X<SOCCC, POCCA, POCMA, POCS, IAE>;
  using AllocX = MyAlloc<myX, SOCCC, POCCA, POCMA, POCS, IAE>;
  using Allocint = MyAlloc<int,SOCCC, POCCA, POCMA, POCS, IAE>;
  using variantX = std::basic_variant<AllocX, int, myX, Throws>;
  using variantint = std::basic_variant<Allocint, int, myX, Throws>;
  {
    variantX obj1{std::allocator_arg, AllocX{5}, myX{42}};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};
    obj1 = std::move(obj2);
    VERIFY(get<myX>(obj1).value == 20);
    VERIFY(obj1.get_allocator().id == A_VAL);
    VERIFY(get<myX>(obj1).alloc.id == A_VAL);
  }
  {
    variantX obj1{std::allocator_arg, AllocX{5}, 42};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};
    obj1 = std::move(obj2);
    VERIFY(get<myX>(obj1).value == 20);
    VERIFY(obj1.get_allocator().id == A_VAL);
    VERIFY(get<myX>(obj1).alloc.id == A_VAL);
  }
  {
      variantX obj1{std::allocator_arg, AllocX{5}, 42};
      variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};
      Throws t;
      try{ obj2.template emplace<Throws>(t); }       catch(...){}
      VERIFY(obj2.valueless_by_exception());
      obj1 = std::move(obj2);
      VERIFY(obj1.valueless_by_exception());
      VERIFY(obj1.get_allocator().id == A_VAL);
   }
  {
      variantX obj1{std::allocator_arg, AllocX{5}, 42};
      variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};
      Throws t;
      try{ obj1.template emplace<Throws>(t); }       catch(...){}
      VERIFY(obj1.valueless_by_exception());
      obj1 = std::move(obj2);
      VERIFY(get<myX>(obj1).value == 20);
      VERIFY(obj1.get_allocator().id == A_VAL);
      VERIFY(get<myX>(obj1).alloc.id == A_VAL);
   }
    {
      variantX obj1{std::allocator_arg, AllocX{5}, 42};
      variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};
      Throws t;
      try{ obj2.template emplace<Throws>(t); }       catch(...){}
      try{ obj1.template emplace<Throws>(t); }       catch(...){}
      VERIFY(obj2.valueless_by_exception());
      VERIFY(obj1.valueless_by_exception());
      obj1 = std::move(obj2);
      VERIFY(obj1.valueless_by_exception());
      VERIFY(obj1.get_allocator().id == A_VAL);
   }
}

template<bool SOCCC = true,
         bool POCCA = false,
         bool POCMA = false,
         bool POCS = false,
         bool IAE = false>
void checkPropagationValue(){

  using myX = X<SOCCC, POCCA, POCMA, POCS, IAE>;
  using AllocX = MyAlloc<myX, SOCCC, POCCA, POCMA, POCS, IAE>;
  using Allocint = MyAlloc<int,SOCCC, POCCA, POCMA, POCS, IAE>;
  using variantX = std::basic_variant<AllocX, int, myX, Throws>;
  using variantint = std::basic_variant<Allocint, int, myX, Throws>;


  {
    myX value{std::allocator_arg, AllocX{5}, 42};
    variantX copy1(value);
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 0);
    VERIFY(get<myX>(copy1).alloc.id == 0);
  }
  {
    myX value{std::allocator_arg, AllocX{5}, 42};
    variantX copy1(std::move(value));
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 0);
    VERIFY(get<myX>(copy1).alloc.id == 0);
  }
  {
    myX value{std::allocator_arg, AllocX{5}, 42};
    variantX copy1(std::allocator_arg, Allocint{6}, value);
    VERIFY(get<myX>(copy1).value == 42);
    VERIFY(copy1.get_allocator().id == 6);
    VERIFY(get<myX>(copy1).alloc.id == 6);
  }
  {
    myX value{std::allocator_arg, AllocX{5}, 42};
    variantX obj(std::allocator_arg, Allocint{6},std::move(value));
    VERIFY(get<myX>(obj).value == 42);
    VERIFY(obj.get_allocator().id == 6);
    VERIFY(get<myX>(obj).alloc.id == 6);
  }
  {
    myX value{std::allocator_arg, AllocX{5}, 42};
    variantX obj{std::allocator_arg, AllocX{6}, myX{20}};
    obj= value;
    VERIFY(get<myX>(obj).value == 42);
    VERIFY(obj.get_allocator().id == 6);
    // commented out as assignment from value may modify the allocator of the
    // contained alternative, depending on the alternatives behaviour.
    // Ville is happy with that, and pmr allocators don't propagate on
    // assignment
    //VERIFY(get<myX>(obj).alloc.id == 6);
  }
  {
    myX value{std::allocator_arg, AllocX{5}, 42};
    variantX obj{std::allocator_arg, AllocX{6}, 20};
    obj= value;
    VERIFY(get<myX>(obj).value == 42);
    VERIFY(obj.get_allocator().id == 6);
    //VERIFY(get<myX>(obj).alloc.id == 6);
  }
  {
    myX value{std::allocator_arg, AllocX{5}, 42};
    variantX obj{std::allocator_arg, AllocX{6}, myX{20}};
    Throws t;
    try{ obj.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj.valueless_by_exception());
    obj = std::move(value);
    //VERIFY(get<myX>(obj).value == 42);
    VERIFY(obj.get_allocator().id == 6);
   }
   {
     myX value{std::allocator_arg, AllocX{5}, 42};
     variantX obj{std::allocator_arg, AllocX{6}, myX{20}};
     obj = std::move(value);
     VERIFY(get<myX>(obj).value == 42);
     VERIFY(obj.get_allocator().id == 6);
    // VERIFY(get<myX>(obj).alloc.id == 6);
   }
   {
     myX value{std::allocator_arg, AllocX{5}, 42};
     variantX obj{std::allocator_arg, AllocX{6}, 20};
     obj = std::move(value);
     VERIFY(get<myX>(obj).value == 42);
     VERIFY(obj.get_allocator().id == 6);
   //  VERIFY(get<myX>(obj).alloc.id == 6);
   }
   {
     myX value{std::allocator_arg, AllocX{5}, 42};
     variantX obj{std::allocator_arg, AllocX{6}, myX{20}};
     Throws t;
     try{ obj.template emplace<Throws>(t); }       catch(...){}
     VERIFY(obj.valueless_by_exception());
     obj = std::move(value);
     VERIFY(get<myX>(obj).value == 42);
     VERIFY(obj.get_allocator().id == 6);
    }
}
template<bool SOCCC = true,
         bool POCCA = false,
         bool POCMA = false,
         bool POCS = false,
         bool IAE = false>
void checkPropagationSwap(int A_VAL, int B_VAL){

  using myX = X<SOCCC, POCCA, POCMA, POCS, IAE>;
  using AllocX = MyAlloc<myX, SOCCC, POCCA, POCMA, POCS, IAE>;
  using Allocint = MyAlloc<int,SOCCC, POCCA, POCMA, POCS, IAE>;
  using variantX = std::basic_variant<AllocX, int, myX, Throws>;
  using variantint = std::basic_variant<Allocint, int, myX, Throws>;


  {
    variantX obj1{std::allocator_arg, AllocX{5}, myX{42}};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};

    swap(obj1,obj2);
    VERIFY(get<myX>(obj1).value == 20);
    VERIFY(get<myX>(obj2).value == 42);
    VERIFY(obj1.get_allocator().id == A_VAL);
    VERIFY(obj2.get_allocator().id == B_VAL);
  }
  {
    variantX obj1{std::allocator_arg, AllocX{5}, 42};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};

    swap(obj1,obj2);
    VERIFY(get<myX>(obj1).value == 20);
    VERIFY(get<int>(obj2) == 42);
    VERIFY(obj1.get_allocator().id == A_VAL);
    VERIFY(obj2.get_allocator().id == B_VAL);
  }
  {
    variantX obj1{std::allocator_arg, AllocX{5}, myX{42}};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};

    Throws t;
    try{ obj1.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj1.valueless_by_exception());
    swap(obj1,obj2);

    VERIFY(get<myX>(obj1).value == 20);
    VERIFY(obj2.valueless_by_exception());
    VERIFY(obj1.get_allocator().id == A_VAL);
    VERIFY(obj2.get_allocator().id == B_VAL);
  }
  {
    variantX obj1{std::allocator_arg, AllocX{5}, myX{42}};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};

    Throws t;
    try{ obj2.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj2.valueless_by_exception());
    swap(obj1,obj2);

    VERIFY(obj1.valueless_by_exception());
    VERIFY(get<myX>(obj2).value == 42);
    VERIFY(obj1.get_allocator().id == A_VAL);
    VERIFY(obj2.get_allocator().id == B_VAL);
  }
  {
    variantX obj1{std::allocator_arg, AllocX{5}, myX{42}};
    variantX obj2{std::allocator_arg, AllocX{6}, myX{20}};

    Throws t;
    try{ obj1.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj1.valueless_by_exception());
    try{ obj2.template emplace<Throws>(t); }       catch(...){}
    VERIFY(obj2.valueless_by_exception());

    swap(obj1,obj2);

    VERIFY(obj1.valueless_by_exception());
    VERIFY(obj2.valueless_by_exception());
    VERIFY(obj1.get_allocator().id == A_VAL);
    VERIFY(obj2.get_allocator().id == B_VAL);
  }
}
int main()
{

  checkPropagationCopy<false, false, false, false, false>(0, 0);
  checkPropagationCopy<false, false, false, false, true>(5, 0);
  checkPropagationCopy<false, false, false, true, false>(0, 0);
  checkPropagationCopy<false, false, false, true, true>(5, 0);
  checkPropagationCopy<false, false, true, false, false>(0, 0);
  checkPropagationCopy<false, false, true, false, true>(5, 0);
  checkPropagationCopy<false, false, true, true, false>(0, 0);
  checkPropagationCopy<false, false, true, true, true>(5, 0);
  checkPropagationCopy<false, true, false, false, false>(0, 0);
  checkPropagationCopy<false, true, false, false, true>(5, 0);
  checkPropagationCopy<false, true, false, true, false>(0, 0);
  checkPropagationCopy<false, true, false, true, true>(5, 0);
  checkPropagationCopy<false, true, true, false, false>(0, 0);
  checkPropagationCopy<false, true, true, false, true>(5, 0);
  checkPropagationCopy<false, true, true, true, false>(0, 0);
  checkPropagationCopy<false, true, true, true, true>(5, 0);
  checkPropagationCopy<true, false, false, false, false>(5,5);
  checkPropagationCopy<true, false, false, false, true>(5,5);
  checkPropagationCopy<true, false, false, true, false>(5,5);
  checkPropagationCopy<true, false, false, true, true>(5,5);
  checkPropagationCopy<true, false, true, false, false>(5,5);
  checkPropagationCopy<true, false, true, false, true>(5,5);
  checkPropagationCopy<true, false, true, true, false>(5,5);
  checkPropagationCopy<true, false, true, true, true>(5,5);
  checkPropagationCopy<true, true, false, false, false>(5,5);
  checkPropagationCopy<true, true, false, false, true>(5,5);
  checkPropagationCopy<true, true, false, true, false>(5,5);
  checkPropagationCopy<true, true, false, true, true>(5,5);
  checkPropagationCopy<true, true, true, false, false>(5,5);
  checkPropagationCopy<true, true, true, false, true>(5,5);
  checkPropagationCopy<true, true, true, true, false>(5,5);
  checkPropagationCopy<true, true, true, true, true>(5,5);

  checkPropagationMove<false, false, false, false, false>();
  checkPropagationMove<false, false, false, false, true>();
  checkPropagationMove<false, false, false, true, false>();
  checkPropagationMove<false, false, false, true, true>();
  checkPropagationMove<false, false, true, false, false>();
  checkPropagationMove<false, false, true, false, true>();
  checkPropagationMove<false, false, true, true, false>();
  checkPropagationMove<false, false, true, true, true>();
  checkPropagationMove<false, true, false, false, false>();
  checkPropagationMove<false, true, false, false, true>();
  checkPropagationMove<false, true, false, true, false>();
  checkPropagationMove<false, true, false, true, true>();
  checkPropagationMove<false, true, true, false, false>();
  checkPropagationMove<false, true, true, false, true>();
  checkPropagationMove<false, true, true, true, false>();
  checkPropagationMove<false, true, true, true, true>();
  checkPropagationMove<true, false, false, false, false>();
  checkPropagationMove<true, false, false, false, true>();
  checkPropagationMove<true, false, false, true, false>();
  checkPropagationMove<true, false, false, true, true>();
  checkPropagationMove<true, false, true, false, false>();
  checkPropagationMove<true, false, true, false, true>();
  checkPropagationMove<true, false, true, true, false>();
  checkPropagationMove<true, false, true, true, true>();
  checkPropagationMove<true, true, false, false, false>();
  checkPropagationMove<true, true, false, false, true>();
  checkPropagationMove<true, true, false, true, false>();
  checkPropagationMove<true, true, false, true, true>();
  checkPropagationMove<true, true, true, false, false>();
  checkPropagationMove<true, true, true, false, true>();
  checkPropagationMove<true, true, true, true, false>();
  checkPropagationMove<true, true, true, true, true>();

  checkPropagationCopyAssign<false, false, false, false, false>(5,5);
  checkPropagationCopyAssign<false, false, false, false, true>(5,5);
  checkPropagationCopyAssign<false, false, false, true, false>(5,5);
  checkPropagationCopyAssign<false, false, false, true, true>(5,5);
  checkPropagationCopyAssign<false, false, true, false, false>(5,5);
  checkPropagationCopyAssign<false, false, true, false, true>(5,5);
  checkPropagationCopyAssign<false, false, true, true, false>(5,5);
  checkPropagationCopyAssign<false, false, true, true, true>(5,5);
  checkPropagationCopyAssign<false, true, false, false, false>(6,6);
  checkPropagationCopyAssign<false, true, false, false, true>(6,6);
  checkPropagationCopyAssign<false, true, false, true, false>(6,6);
  checkPropagationCopyAssign<false, true, false, true, true>(6,6);
  checkPropagationCopyAssign<false, true, true, false, false>(6,6);
  checkPropagationCopyAssign<false, true, true, false, true>(6,6);
  checkPropagationCopyAssign<false, true, true, true, false>(6,6);
  checkPropagationCopyAssign<false, true, true, true, true>(6,6);
  checkPropagationCopyAssign<true, false, false, false, false>(5,5);
  checkPropagationCopyAssign<true, false, false, false, true>(5,5);
  checkPropagationCopyAssign<true, false, false, true, false>(5,5);
  checkPropagationCopyAssign<true, false, false, true, true>(5,5);
  checkPropagationCopyAssign<true, false, true, false, false>(5,5);
  checkPropagationCopyAssign<true, false, true, false, true>(5,5);
  checkPropagationCopyAssign<true, false, true, true, false>(5,5);
  checkPropagationCopyAssign<true, false, true, true, true>(5,5);
  checkPropagationCopyAssign<true, true, false, false, false>(6,6);
  checkPropagationCopyAssign<true, true, false, false, true>(6,6);
  checkPropagationCopyAssign<true, true, false, true, false>(6,6);
  checkPropagationCopyAssign<true, true, false, true, true>(6,6);
  checkPropagationCopyAssign<true, true, true, false, false>(6,6);
  checkPropagationCopyAssign<true, true, true, false, true>(6,6);
  checkPropagationCopyAssign<true, true, true, true, false>(6,6);
  checkPropagationCopyAssign<true, true, true, true, true>(6,6);


  checkPropagationMoveAssign<false, false, false, false, false>(5,5);
  checkPropagationMoveAssign<false, false, false, false, true>(5,5);
  checkPropagationMoveAssign<false, false, false, true, false>(5,5);
  checkPropagationMoveAssign<false, false, false, true, true>(5,5);
  checkPropagationMoveAssign<false, false, true, false, false>(6,6);
  checkPropagationMoveAssign<false, false, true, false, true>(6,6);
  checkPropagationMoveAssign<false, false, true, true, false>(6,6);
  checkPropagationMoveAssign<false, false, true, true, true>(6,6);
  checkPropagationMoveAssign<false, true, false, false, false>(5,5);
  checkPropagationMoveAssign<false, true, false, false, true>(5,5);
  checkPropagationMoveAssign<false, true, false, true, false>(5,5);
  checkPropagationMoveAssign<false, true, false, true, true>(5,5);
  checkPropagationMoveAssign<false, true, true, false, false>(6,6);
  checkPropagationMoveAssign<false, true, true, false, true>(6,6);
  checkPropagationMoveAssign<false, true, true, true, false>(6,6);
  checkPropagationMoveAssign<false, true, true, true, true>(6,6);
  checkPropagationMoveAssign<true, false, false, false, false>(5,5);
  checkPropagationMoveAssign<true, false, false, false, true>(5,5);
  checkPropagationMoveAssign<true, false, false, true, false>(5,5);
  checkPropagationMoveAssign<true, false, false, true, true>(5,5);
  checkPropagationMoveAssign<true, false, true, false, false>(6,6);
  checkPropagationMoveAssign<true, false, true, false, true>(6,6);
  checkPropagationMoveAssign<true, false, true, true, false>(6,6);
  checkPropagationMoveAssign<true, false, true, true, true>(6,6);
  checkPropagationMoveAssign<true, true, false, false, false>(5,5);
  checkPropagationMoveAssign<true, true, false, false, true>(5,5);
  checkPropagationMoveAssign<true, true, false, true, false>(5,5);
  checkPropagationMoveAssign<true, true, false, true, true>(5,5);
  checkPropagationMoveAssign<true, true, true, false, false>(6,6);
  checkPropagationMoveAssign<true, true, true, false, true>(6,6);
  checkPropagationMoveAssign<true, true, true, true, false>(6,6);
  checkPropagationMoveAssign<true, true, true, true, true>(6,6);

  checkPropagationValue<false, false, false, false, false>();
  checkPropagationValue<false, false, false, false, true>();
  checkPropagationValue<false, false, false, true, false>();
  checkPropagationValue<false, false, false, true, true>();
  checkPropagationValue<false, false, true, false, false>();
  checkPropagationValue<false, false, true, false, true>();
  checkPropagationValue<false, false, true, true, false>();
  checkPropagationValue<false, false, true, true, true>();
  checkPropagationValue<false, true, false, false, false>();
  checkPropagationValue<false, true, false, false, true>();
  checkPropagationValue<false, true, false, true, false>();
  checkPropagationValue<false, true, false, true, true>();
  checkPropagationValue<false, true, true, false, false>();
  checkPropagationValue<false, true, true, false, true>();
  checkPropagationValue<false, true, true, true, false>();
  checkPropagationValue<false, true, true, true, true>();
  checkPropagationValue<true, false, false, false, false>();
  checkPropagationValue<true, false, false, false, true>();
  checkPropagationValue<true, false, false, true, false>();
  checkPropagationValue<true, false, false, true, true>();
  checkPropagationValue<true, false, true, false, false>();
  checkPropagationValue<true, false, true, false, true>();
  checkPropagationValue<true, false, true, true, false>();
  checkPropagationValue<true, false, true, true, true>();
  checkPropagationValue<true, true, false, false, false>();
  checkPropagationValue<true, true, false, false, true>();
  checkPropagationValue<true, true, false, true, false>();
  checkPropagationValue<true, true, false, true, true>();
  checkPropagationValue<true, true, true, false, false>();
  checkPropagationValue<true, true, true, false, true>();
  checkPropagationValue<true, true, true, true, false>();
  checkPropagationValue<true, true, true, true, true>();


  checkPropagationSwap<false, false, false, true, false>(6,5);
  checkPropagationSwap<false, false, false, true, true>(6,5);
  checkPropagationSwap<false, false, true, true, false>(6,5);
  checkPropagationSwap<false, false, true, true, true>(6,5);
  checkPropagationSwap<false, true, false, true, false>(6,5);
  checkPropagationSwap<false, true, false, true, true>(6,5);
  checkPropagationSwap<false, true, true, true, false>(6,5);
  checkPropagationSwap<false, true, true, true, true>(6,5);
  checkPropagationSwap<true, false, false, true, false>(6,5);
  checkPropagationSwap<true, false, false, true, true>(6,5);
  checkPropagationSwap<true, false, true, true, false>(6,5);
  checkPropagationSwap<true, false, true, true, true>(6,5);
  checkPropagationSwap<true, true, false, true, false>(6,5);
  checkPropagationSwap<true, true, false, true, true>(6,5);
  checkPropagationSwap<true, true, true, true, false>(6,5);
  checkPropagationSwap<true, true, true, true, true>(6,5);
}

