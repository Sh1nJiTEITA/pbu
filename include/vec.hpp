/*

   TODO: 1. add rem() method;
         2. add rem(const T* begin, const T* end);
         3. add rem(int i) method;
         4. add rem_b(int i) method;

*/

#ifndef PBUVEC_H
#define PBUVEC_H

// #include <concurrencysal.h>

#include <allocator.hpp>
#include <iostream>

// #define PBU_ALLOCATOR_INFO_ENABLE
#define PBU_VEC_DEBUG

#define PBU_COLOR_OUTPUT

namespace pbu
{

template <class T, class Allocator = pbu::Allocator<T>>
class Vec
{
protected:
   T* __data = nullptr;
   T* __last = nullptr;
   T* __max  = nullptr;

   Allocator __allocator;

   void __ensure_capacity()
   {
      if (!__data)
      {
         reserve(1);
      }
      else if (__last == __max)
      {
         reserve(capacity() * 2);
      }
   }

public:
   Vec()
       : __allocator(pbu::Allocator<T>())
   {
      reserve(1);
   }
   Vec(size_t size, pbu::Allocator<T> a = pbu::Allocator<T>())
       : __allocator{a}
   {
      reserve(size);
   };

   Vec(const T* b, const T* e)
   {
      if (b == nullptr || e == nullptr || e - b < 0)
      {
         abort();
      }
      reserve(e - b);
      for (const T* it = b; it != e; ++it)
      {
         __allocator.construct(__last++, *it);
      }
   }

   Vec(const Vec& v)
   {
      // std::cout << "Copy!\n";
      reserve(v.capacity());
      for (size_t i = 0; i < v.size(); ++i)
      {
         __allocator.construct(__last++, *(v.__data + i));
      }
   }

   Vec(Vec&& v)
       // : __data{PBU_MOV(v.__data)}
       // , __last{PBU_MOV(v.__last)}
       // , __max{PBU_MOV(v.__max)}
       // , __allocator{v.__allocator}
       : __data{v.__data}, __last{v.__last}, __max{v.__max}
   // , __allocator{v.__allocator}
   {
      v.__data = v.__last = v.__max = nullptr;
   }

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

   const T* data() const noexcept { return __data; }

   T* begin() noexcept { return __data; }
   T* end() noexcept { return __last; }
   T* rbegin() noexcept { return __last - 1; }
   T* rend() noexcept { return __last - 1; }
   const T* begin() const noexcept { return __data; }
   const T* end() const noexcept { return __last; }
   const T* rbegin() const noexcept { return __last - 1; }
   const T* rend() const noexcept { return __last - 1; }

   const Allocator& allocator() noexcept { return __allocator; }
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

   void clear()
   {
      while (__last != __data)
      {
         __allocator.destroy(--__last);
      }
   }

   void shrink()
   {
      if (__last != __max)
      {
         reserve(size());
      }
   }

   virtual void add(const T& v)
   {
      __ensure_capacity();
      __allocator.construct(__last++, v);
   }
   virtual void add(const T&& v)
   {
      __ensure_capacity();
      __allocator.construct(__last++, PBU_MOV(v));
   }
   template <typename... Args>
   void emp(Args&&... args)
   {
      __ensure_capacity();
      __allocator.construct(__last++, PBU_FWD(args)...);
   }

   T& operator[](size_t i) noexcept { return __data[i]; }
   Vec& operator=(const Vec& v)
   {
      if (this != &v)
      {
         clear();
         __allocator = v.__allocator;
         reserve(v.capacity());
         for (const T* it = v.begin(); it != v.end(); ++it)
         {
            __allocator.construct(__last++, *it);
         }
      }
      return *this;
   }
   Vec& operator=(Vec&& v)
   {
      if (this != &v)
      {
         clear();
         if (__data != nullptr)
         {
            __allocator.deallocate(__data, capacity());
         }
         __data      = v.__data;
         __last      = v.__last;
         __max       = v.__max;
         __allocator = v.__allocator;

         v.__data = v.__last = v.__max = nullptr;
      }
      return *this;
   }

#ifdef PBU_VEC_DEBUG
   size_t allocationCount() const noexcept
   {
      return __allocator.allocationCount();
   }
   size_t reallocationCount() const noexcept
   {
      return __allocator.reallocationCount();
   }
   size_t deallocationCount() const noexcept
   {
      return __allocator.deallocationCount();
   }
   size_t constructionCount() const noexcept
   {
      return __allocator.constructionCount();
   }
   size_t destructionCount() const noexcept
   {
      return __allocator.destructionCount();
   }
   PresentAllocationInfo rprInfo() const
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

      if (__data == nullptr)
      {
         msg << " no data";
      }
      else if (__max == nullptr)
      {
         msg << " no max ALLOCATION ERROR";
      }
      else if (__last == nullptr)
      {
         msg << " nothing allocated yet";
      }
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
            msg << " ";
            // sizeof(T) == 1 && *it == '\0' && it == __last - 1 ? msg << "\\0"
            //                                                   : msg << *it;
            //
            // IF T == char
            if (sizeof(T) == 1)
            {
               if (*it == '\0' && it == __last - 1)
               {
                  msg << "\\0";
               }
               else
               {
                  char str[2];
                  str[0] = *it;
                  str[1] = '\0';
                  msg << str;
               }
            }
            else
            {
               msg << *it;
            }
         }
         if (it == __last)
         {
            msg << "\t<- last";
         }
      }
      return msg;
   }
#endif
};

};  // namespace pbu

#endif
