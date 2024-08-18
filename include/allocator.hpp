#ifndef PBUALLOCATOR_H
#define PBUALLOCATOR_H

#include <cstddef>
#include <new>
#include <type_traits>

// #define PBU_ALLOCATOR_INFO_ENABLE
#define PBU_COLOR_OUTPUT

#ifdef PBU_COLOR_OUTPUT
#define PBU_CLOG_RESET "\033[0m"
#define PBU_CLOG_RED "\033[31m"          /* Red */
#define PBU_CLOG_GREEN "\033[32m"        /* Green */
#define PBU_CLOG_YELLOW "\033[33m"       /* Yellow */
#define PBU_CLOG_BLUE "\033[34m"         /* Blue */
#define PBU_CLOG_MAGENTA "\033[35m"      /* Magenta */
#define PBU_CLOG_CYAN "\033[36m"         /* Cyan */
#define PBU_CLOG_WHITE "\033[37m"        /* White */
#define PBU_CLOG_ORANGE "\033[38;5;214m" /* orange */
#else
#include <iostream>
#define PBU_CLOG_RESET ""
#define PBU_CLOG_RED ""     /* Red */
#define PBU_CLOG_GREEN ""   /* Green */
#define PBU_CLOG_YELLOW ""  /* Yellow */
#define PBU_CLOG_BLUE ""    /* Blue */
#define PBU_CLOG_MAGENTA "" /* Magenta */
#define PBU_CLOG_CYAN ""    /* Cyan */
#define PBU_CLOG_WHITE ""   /* White */
#define PBU_CLOG_ORANGE ""  /* orange */
#endif

/* SHORT IMPLEMENTATION OF std::move */
#define PBU_MOV(...) \
   static_cast<std::remove_reference_t<decltype(__VA_ARGS__)>&&>(__VA_ARGS__)
// static_cast<decltype(_VA_ARGS__)&&>(__VA_ARGS__)

/* SHORT IMPLEMENTATION OF std::forward */
#define PBU_FWD(...) static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)

namespace pbu
{
template <class T>
class Allocator
{
public:
   using value_type      = T;
   using pointer         = T*;
   using const_pointer   = const T*;
   using reference       = T&;
   using const_reference = const T&;
   using size_type       = std::size_t;
   using difference_type = std::ptrdiff_t;

   Allocator() = default;
   // Allocator() {}
   // template <class U>
   // Allocator(Allocator<U>&& o) noexcept
   // {
   //    _construction_count = o._construction_count;
   //    _destruction_count  = o._destruction_count;
   //    _allocation_count   = o._allocation_count;
   //    _reallocation_count = o._reallocation_count;
   //    _deallocation_count = o._deallocation_count;
   // }
   //
   // Allocator(Allocator&& o) noexcept
   //     : _allocation_count{o._allocation_count}
   //     , _reallocation_count{o._reallocation_count}
   //     , _deallocation_count{o._deallocation_count}
   //     , _construction_count{o._construction_count}
   //     , _destruction_count{o._destruction_count}
   // {
   //    o._allocation_count   = 0;
   //    o._reallocation_count = 0;
   //    o._deallocation_count = 0;
   //    o._construction_count = 0;
   //    o._destruction_count  = 0;
   // }
   //
   // template <class U>
   // constexpr Allocator(const Allocator<U>& o) noexcept
   //     : _allocation_count{o._allocation_count}
   //     , _reallocation_count{o._reallocation_count}
   //     , _deallocation_count{o._deallocation_count}
   //     , _construction_count{o._construction_count}
   //     , _destruction_count{o._destruction_count}
   // {
   // }
   //
   ~Allocator() {}

   size_type max_size() const noexcept
   {
      return static_cast<size_t>(-1) / sizeof(T);
   }

   [[nodiscard]] pointer allocate(size_type n, const void* = 0)
   {
      if (n == 0)
      {
         return nullptr;
      }
      if (n > max_size())
      {
         throw std::bad_alloc();
      }
      pointer ptr = static_cast<pointer>(::operator new(n * sizeof(T)));

#ifdef PBU_ALLOCATOR_INFO_ENABLE
      std::clog << PBU_CLOG_GREEN << "[A] Allocating pointer:\t\t|"
                << static_cast<void*>(ptr) << "|\t"
                << "bytes: " << sizeof(T) * n << "\t|" << PBU_CLOG_RESET
                << "\n";
#endif
      _allocation_count++;
      return ptr;
   }

   void deallocate(pointer p, size_type s) noexcept
   {
#ifdef PBU_ALLOCATOR_INFO_ENABLE
      std::clog << PBU_CLOG_MAGENTA << "[D] Deallocating pointer:\t|"
                << static_cast<void*>(p) << "|\tbytes: " << s << "\t|"
                << PBU_CLOG_RESET << "\n";
#endif
      if (p)
      {
         ::operator delete(p);
         _deallocation_count++;
      }
   }

   [[nodiscard]] pointer reallocate(pointer p, size_t old_size, size_t new_size)
   /*
      pointer ptr = allocate(new_size);
      ptr         = pbu_mov(p);
      deallocate(p, 0);
      return ptr;
   */
   {
#ifdef PBU_ALLOCATOR_INFO_ENABLE
      std::clog << PBU_CLOG_BLUE << "[R] Reallocating pointer:\t|"
                << static_cast<void*>(p) << "|\tbytes: " << old_size << " -> "
                << new_size << "\t|" << PBU_CLOG_RESET << "\n";
#endif
      pointer new_ptr = allocate(new_size);
      size_type m     = old_size < new_size ? old_size : new_size;
      for (size_type i = 0; i < m; ++i)
      {
         construct(&new_ptr[i], PBU_MOV(p[i]));
         destroy(&p[i]);
      }
      deallocate(p, old_size);
      _reallocation_count++;
      return new_ptr;
   }

   template <typename... Args>
   void construct(pointer p, Args&&... args)
   {
#ifdef PBU_ALLOCATOR_INFO_ENABLE
      std::clog << PBU_CLOG_ORANGE << "[C] Constructing pointer:\t|"
                << static_cast<void*>(p) << "|" << "\tbytes: " << sizeof(T)
                << "\t|" << PBU_CLOG_RESET << "\n";
#endif
      new (static_cast<void*>(p)) T(PBU_FWD(args)...);
      _construction_count++;
   }

   void destroy(pointer p)
   {
      ++_destruction_count;
#ifdef PBU_ALLOCATOR_INFO_ENABLE
      std::clog << PBU_CLOG_BLUE << "[D] Destroying pointer:\t\t|"
                << static_cast<void*>(p) << "|\t" << "bytes: " << sizeof(T)
                << "\t|" << PBU_CLOG_RESET << "\n";
#endif
      p->~T();
   }

   size_type allocationCount() const noexcept { return _allocation_count; }
   size_type reallocationCount() const noexcept { return _reallocation_count; }
   size_type deallocationCount() const noexcept { return _deallocation_count; }
   size_type constructionCount() const noexcept { return _construction_count; }
   size_type destructionCount() const noexcept { return _destruction_count; }

private:
   size_type _allocation_count   = 0;
   size_type _reallocation_count = 0;
   size_type _deallocation_count = 0;
   size_type _construction_count = 0;
   size_type _destruction_count  = 0;
};
template <class T, class U>
bool operator==(const Allocator<T>&, const Allocator<U>&)
{
   return true;
}

template <class T, class U>
bool operator!=(const Allocator<T>&, const Allocator<U>&)
{
   return false;
}

class PresentAllocationInfo
{
   char* _data = nullptr;
   char* _max  = nullptr;
   char* _last = nullptr;

   int _num_base    = 10;
   int _decimal_acc = 7;

   bool _is_total_precision = false;
   pbu::Allocator<char> _allocator;

public:
   // UTILS -~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-
   static size_t calcCharCount(const char* str)
   {
      size_t chars    = 0;
      const char* ptr = str;
      while (*ptr != '\0')
      {
         chars++;
         ptr++;
      }
      return chars;
   }

   const char* get() { return _data; }

   // CONSTRUCTION -~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
   PresentAllocationInfo(size_t chars_count)
   {
      if (chars_count != 0)
      {
         _data = _allocator.allocate(chars_count);
         _max  = _data + chars_count;
         _last = _data;
      }
   }

   PresentAllocationInfo(const char* message)
       : PresentAllocationInfo(PresentAllocationInfo::calcCharCount(message))

   {
      *this << message;
   }

   ~PresentAllocationInfo()
   {
      char* ch = _data;
      _allocator.deallocate(_data, _max - _data);
   }

   char& operator[](size_t i) { return _data[i]; }

   // SETTERS -~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-
   PresentAllocationInfo& setIsTotalPrecision(bool i)
   {
      _is_total_precision = i;
      return *this;
   }

   PresentAllocationInfo& setBase(int b)
   {
      _num_base = b;
      return *this;
   }

   PresentAllocationInfo& setPrecision(int p)
   {
      _decimal_acc = p;
      return *this;
   }

   // ADDING -~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
   // TODO: NOT INPLEMENTED
   PresentAllocationInfo& operator<<(const PresentAllocationInfo&)
   {
      return *this;
   }

   PresentAllocationInfo& operator<<(long long int v)
   {
      char* result = _allocator.allocate(50);
      if (_num_base < 2 || _num_base > 36)
      {
         *this << "INVALID_INT";
         return *this;
      }
      char *ptr = result, *ptr1 = ptr, tmp_char;
      long long int tmp_v;
      do
      {
         tmp_v = v;
         v /= _num_base;
         *ptr++ =
             /*
             "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"
                         */
             "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRST"
             "UVWXYZ"[35 + (tmp_v - v * _num_base)];

      } while (v);
      if (tmp_v < 0)
      {
         *ptr++ = '-';
      }
      *ptr-- = '\0';
      while (ptr1 < ptr)
      {
         tmp_char = *ptr;
         *ptr--   = *ptr1;
         *ptr1++  = tmp_char;
      }
      *this << result;
      _allocator.deallocate(result, 50);
      return *this;
   }

   PresentAllocationInfo& operator<<(double v)
   {
      if (_num_base < 2 || _num_base > 36)
      {
         *this << "INVALID_DOUBLE";
         return *this;
      }
      char* decimal_string       = _allocator.allocate(_decimal_acc + 1);
      long long int integer_part = static_cast<long long int>(v);
      double decimal_part;
      if (integer_part < 0)
      {
         decimal_part = -1 * v - -1 * integer_part;
      }
      else
      {
         decimal_part = v - integer_part;
      }

      size_t precision = _decimal_acc;

      while (precision > 0 && ((decimal_part != 0.0f) || _is_total_precision))
      {
         decimal_part *= _num_base;
         size_t intPart = static_cast<size_t>(decimal_part);
         _allocator.construct(decimal_string + _decimal_acc - precision,
                               "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[intPart]);
         decimal_part -= intPart;
         --precision;
      }
      _allocator.construct(decimal_string + _decimal_acc - precision, '\0');
      *this << integer_part << "." << decimal_string;
      _allocator.deallocate(decimal_string, _decimal_acc + 1);
      return *this;
   }

   PresentAllocationInfo& operator<<(float v)
   {
      *this << (double)v;
      return *this;
   }

   PresentAllocationInfo& operator<<(int v)
   {
      *this << (long long int)v;
      return *this;
   }
   PresentAllocationInfo& operator<<(size_t v)
   {
      *this << (long long int)v;
      return *this;
   }
   PresentAllocationInfo& operator<<(const char* message)
   {
      if (_last != _data)
      {
         _allocator.destroy(_last);
         _last--;
      }
      const char* ptr = message;
      size_t chars    = 0;
      while (*ptr != '\0')
      {
         chars++;
         ptr++;
      }

      size_t old_size       = _max - _data;
      size_t old_ready_size = _last - _data;
      if (old_size - old_ready_size < chars)
      {
         size_t new_size = old_size * 2;
         _data          = _allocator.reallocate(_data, old_size, new_size);
         _last          = _data + old_ready_size;
         _max           = _data + new_size;
      }
      ptr = message;
      while (*ptr != '\0')
      {
         _allocator.construct(_last, *ptr);
         _last++;
         ptr++;
      }
      _allocator.construct(_last, '\0');
      _last++;
      return *this;
   }
#ifdef PBU_ALLOCATOR_INFO_ENABLE
   friend std::ostream& operator<<(std::ostream& os,
                                   const PresentAllocationInfo& message)
   {
      for (char* ch = message._data; ch != message._last; ++ch)
      {
         os << *ch;
      }
      // os << message._data;
      return os;
   }
#endif
};

}  // namespace pbu

#endif
