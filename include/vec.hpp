#ifndef PBUVEC_H
#define PBUVEC_H

#include <allocator.hpp>

// #define PBU_ALLOCATOR_INFO_ENABLE
#define PBU_VEC_DEBUG

#define PBU_COLOR_OUTPUT

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

   void add(const T& v)
   {
      __allocator.construct(__last, v);
      __last++;
   }
   void add(const T&& v)
   {
      __allocator.construct(__last, PBU_MOV(v));
      __last++;
   }
   template <typename... Args>
   void emp(Args&&... args)
   {
      __allocator.construct(__last, PBU_FWD(args)...);
      __last++;
   }

   T& operator[](size_t i) { return __data[i]; }

#ifdef PBU_VEC_DEBUG
   size_t allocationCount() { return __allocator.allocationCount(); }
   size_t reallocationCount() { return __allocator.reallocationCount(); }
   size_t deallocationCount() { return __allocator.deallocationCount(); }
   size_t constructionCount() { return __allocator.constructionCount(); }
   size_t destructionCount() { return __allocator.destructionCount(); }
   PresentAllocationInfo rprInfo()
   {
      PresentAllocationInfo msg(119 + 37 * capacity());

      msg << "pub::Vec("
             "\n   total_size:\t\t"
          << static_cast<long long int>(sizeof(T) * (__max - __data))
          << "\n   element_size:\t" << static_cast<long long int>(sizeof(T))
          << "\n   max_elements:\t" << capacity() << "\n   current_elements:\t"
          << size() << "\n   allocations:\t\t" << allocationCount()
          << "\n   reallocations:\t" << reallocationCount()
          << "\n   deallocations:\t" << deallocationCount()
          << "\n   constructions:\t" << constructionCount()
          << "\n   destructions:\t" << destructionCount() << "\n):";
      for (T* it = __data; it != __max; ++it)
      {
         msg << "\n[" << static_cast<long long int>(it - __data) << "]\t";
         if (it < __last)
         {
            msg << " -> " << PBU_CLOG_GREEN << "c " << PBU_CLOG_RESET << "= ";
         }
         else
         {
            msg << " -> " << PBU_CLOG_RED << "n " << PBU_CLOG_RESET << "= ";
         }
         if (*it < 0)
         {
            msg << *it;
         }
         else
         {
            msg << " " << *it;
         }
      }
      return msg;
   }
#endif
};

};  // namespace pbu

#endif
