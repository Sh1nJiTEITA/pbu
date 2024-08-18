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
// #include <cstdint>
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
   T* _data = nullptr;
   T* _last = nullptr;
   T* _max  = nullptr;

   Allocator _allocator;

   void _ensure_capacity()
   {
      if (!_data)
      {
         reserve(1);
      }
      else if (_last == _max)
      {
         reserve(capacity() * 2);
      }
   }

public:
   Vec()
       : _allocator(pbu::Allocator<T>())
   {
      reserve(1);
   }
   Vec(size_t size, pbu::Allocator<T> a = pbu::Allocator<T>())
       : _allocator{a}
   {
      reserve(size);
   };

   Vec(const T* b, const T* e)
   {
      if (b == nullptr || e == nullptr || e - b < 0)
      {
         abort();
      }
      if (b == e)
      {
         reserve(1);
         return;
      }
      else
      {
         reserve((e - b) * 2);
      }
      for (const T* it = b; it != e; ++it)
      {
         _allocator.construct(_last++, *it);
      }
   }

   Vec(const Vec& v)
   {
      reserve(v.capacity());
      for (size_t i = 0; i < v.size(); ++i)
      {
         _allocator.construct(_last++, *(v._data + i));
      }
   }

   Vec(Vec&& v)
       : _data{v._data}, _last{v._last}, _max{v._max}, _allocator{v._allocator}
   {
      v._data = v._last = v._max = nullptr;
   }

   ~Vec()
   {
      if (_data != nullptr)
      {
         for (T* b = _data; b != _max; ++b)
         {
            _allocator.destroy(b);
         }
         _allocator.deallocate(_data, capacity());
      }
   }

   size_t capacity() const noexcept { return (size_t)(_max - _data); }
   size_t size() const noexcept { return (size_t)(_last - _data); }

   const T* data() const noexcept { return _data; }

   T* begin() noexcept { return _data; }
   T* end() noexcept { return _last; }
   T* rbegin() noexcept { return _last - 1; }
   T* rend() noexcept { return _data - 1; }
   const T* begin() const noexcept { return _data; }
   const T* end() const noexcept { return _last; }
   const T* rbegin() const noexcept { return _last - 1; }
   const T* rend() const noexcept { return _data - 1; }

   const Allocator& allocator() noexcept { return _allocator; }

   void reserve(size_t new_size)
   {
      size_t s = size();
      if (_data == nullptr)
      {
         _data = _allocator.allocate(new_size);
      }
      else
      {
         _data = _allocator.reallocate(_data, s, new_size);
         _last = _data;
      }
      _last = _data + s;
      _max  = _data + new_size;
   };

   void clear()
   {
      while (_last != _data)
      {
         _allocator.destroy(--_last);
      }
   }

   void shrink()
   {
      if (_last != _max)
      {
         reserve(size());
      }
   }

   virtual void add(const T& v)
   {
      _ensure_capacity();
      _allocator.construct(_last++, v);
   }
   virtual void add(const T&& v)
   {
      _ensure_capacity();
      _allocator.construct(_last++, PBU_MOV(v));
   }
   template <typename... Args>
   void emp(Args&&... args)
   {
      _ensure_capacity();
      _allocator.construct(_last++, PBU_FWD(args)...);
   }
   void rem(size_t n = 1)
   {
      int64_t s  = size();
      int64_t n_ = n;
      if (s - (int64_t)n <= 0)
      {
         clear();
         return;
      }
      for (size_t i = 0; i < n; ++i)
      {
         _allocator.destroy(--_last);
      }
   }
   void rem(const T* b, const T* e)
   {
      // if (e - b <= 0 || b < _data || e > _last - 1)
      // {
      //    return;
      // }
      // for (const T* b_ = b; b_ < e; ++b_)
      // {
      //    _allocator.destroy(b_);
      // }
      // if (e != _last - 1)
      // {
      //    const T* lb = b;
      //    const T* rb = e;
      //    while (<
      //    for (const T* b_ = e + 1; b_ < _last; ++b_) {
      //
      //    }
      // }
   }
   T& operator[](size_t i) noexcept { return _data[i]; }
   Vec& operator=(const Vec& v)
   {
      if (this != &v)
      {
         clear();
         _allocator = v._allocator;
         reserve(v.capacity());
         for (const T* it = v.begin(); it != v.end(); ++it)
         {
            _allocator.construct(_last++, *it);
         }
      }
      return *this;
   }
   Vec& operator=(Vec&& v)
   {
      if (this != &v)
      {
         clear();
         if (_data != nullptr)
         {
            _allocator.deallocate(_data, capacity());
         }
         _data      = v._data;
         _last      = v._last;
         _max       = v._max;
         _allocator = v._allocator;

         v._data = v._last = v._max = nullptr;
      }
      return *this;
   }

#ifdef PBU_VEC_DEBUG
   size_t allocationCount() const noexcept
   {
      return _allocator.allocationCount();
   }
   size_t reallocationCount() const noexcept
   {
      return _allocator.reallocationCount();
   }
   size_t deallocationCount() const noexcept
   {
      return _allocator.deallocationCount();
   }
   size_t constructionCount() const noexcept
   {
      return _allocator.constructionCount();
   }
   size_t destructionCount() const noexcept
   {
      return _allocator.destructionCount();
   }
   PresentAllocationInfo rprInfo() const
   {
      PresentAllocationInfo msg(119 + 37 * capacity());

      msg << "pub::Vec("
             "\n   total_size:\t\t"
          << static_cast<long long int>(sizeof(T) * (_max - _data))
          << "\n   element_size:\t" << static_cast<long long int>(sizeof(T))
          << "\n   max_elements:\t" << capacity() << "\n   current_elements:\t"
          << size() << "\n   allocations:\t\t" << allocationCount()
          << "\n   reallocations:\t" << reallocationCount()
          << "\n   deallocations:\t" << deallocationCount()
          << "\n   constructions:\t" << constructionCount()
          << "\n   destructions:\t" << destructionCount() << "\n):";

      if (_data == nullptr)
      {
         msg << " no data";
      }
      else if (_max == nullptr)
      {
         msg << " no max ALLOCATION ERROR";
      }
      else if (_last == nullptr)
      {
         msg << " nothing allocated yet";
      }
      for (T* it = _data; it != _max; ++it)
      {
         msg << "\n[" << static_cast<long long int>(it - _data) << "]\t";
         if (it < _last)
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
            // sizeof(T) == 1 && *it == '\0' && it == _last - 1 ? msg << "\\0"
            //                                                   : msg << *it;
            //
            // IF T == char
            if (sizeof(T) == 1)
            {
               if (*it == '\0' && it == _last - 1)
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
         if (it == _last)
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
