#ifndef PBUVEC_H
#define PBUVEC_H

#include <allocator.hpp>

// #define PBU_ALLOCATOR_INFO_ENABLE
#define PBU_VEC_DEBUG

namespace pbu
{

template <class T, class Allocator = pbu::Allocator<T>>
class Vec
{
   T* __data = nullptr;
   T* __last = nullptr;
   T* __max  = nullptr;

   pbu::Allocator<T> __allocator;

public:
   Vec(size_t size, pbu::Allocator<T> a = pbu::Allocator<T>())
       : __allocator{a}
   {
      reserve(size);
   };

   ~Vec()
   {
      if (__data != nullptr)
      {
         for (T* b = __data; b != __max; ++b)
         {
            __allocator.destroy(b);
         }
         __allocator.deallocate(__data, capacity());
      }
   }
   size_t capacity() const noexcept { return (size_t)(__max - __data); }
   size_t size() const noexcept { return (size_t)(__last - __data); }
   const Allocator& allocator() { return __allocator; }

   void reserve(size_t new_size)
   {
      size_t s = size();
      if (__data == nullptr)
      {
         __data = __allocator.allocate(new_size);
      }
      else
      {
         __data = __allocator.reallocate(__data, s, new_size);
         __last = __data;
      }
      __last = __data + s;
      __max  = __data + new_size;
   };

   void add(T v)
   {
      __allocator.construct(++__last, v);
      // ++__last;
   }

   T& operator[](size_t i) { return __data[i]; }

#ifdef PBU_VEC_DEBUG
   PresentAllocationInfo rprInfo() { 
      PresentAllocationInfo msg(capacity() * 2);
       

   }
#endif
};

};  // namespace pbu

#endif
