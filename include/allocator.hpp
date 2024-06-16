#ifndef PBUALLOCATOR_H
#define PBUALLOCATOR_H

#include <iomanip>
#include <iostream>
#include <limits>

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

   // Allocator() noexcept {}
   // Allocator(const Allocator&) noexcept {}
   Allocator() = default;
   template <class U>
   constexpr Allocator(const Allocator<U>&) noexcept
   {
   }

   ~Allocator() {}

   size_type max_size() const noexcept
   {
      return std::numeric_limits<size_type>::max() / sizeof(T);
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
      __allocation_count++;
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
         __deallocation_count++;
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
      __reallocation_count++;
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
      __construction_count++;
   }

   void destroy(pointer p)
   {
      ++__destruction_count;
#ifdef PBU_ALLOCATOR_INFO_ENABLE
      std::clog << PBU_CLOG_BLUE << "[D] Destroying pointer:\t\t|"
                << static_cast<void*>(p) << "|\t" << "bytes: " << sizeof(T)
                << "\t|" << PBU_CLOG_RESET << "\n";
#endif
      p->~T();
   }

   size_type allocationCount() const noexcept { return __allocation_count; }
   size_type reallocationCount() const noexcept { return __reallocation_count; }
   size_type deallocationCount() const noexcept { return __deallocation_count; }
   size_type constructionCount() const noexcept { return __construction_count; }
   size_type destructionCount() const noexcept { return __destruction_count; }

private:
   size_type __allocation_count   = 0;
   size_type __reallocation_count = 0;
   size_type __deallocation_count = 0;
   size_type __construction_count = 0;
   size_type __destruction_count  = 0;
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
   char* __data = nullptr;
   char* __max  = nullptr;
   char* __last = nullptr;

   int __num_base    = 10;
   int __decimal_acc = 7;

   bool __is_total_precision = false;
   pbu::Allocator<char> __allocator;

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

   const char* get() { return __data; }

   // CONSTRUCTION -~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
   PresentAllocationInfo(size_t chars_count)
   {
      if (chars_count != 0)
      {
         __data = __allocator.allocate(chars_count);
         __max  = __data + chars_count;
         __last = __data;
      }
   }

   PresentAllocationInfo(const char* message)
       : PresentAllocationInfo(PresentAllocationInfo::calcCharCount(message))

   {
      *this << message;
   }

   ~PresentAllocationInfo()
   {
      char* ch = __data;
      __allocator.deallocate(__data, __max - __data);
   }

   char& operator[](size_t i) { return __data[i]; }

   // SETTERS -~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-
   PresentAllocationInfo& setIsTotalPrecision(bool i)
   {
      __is_total_precision = i;
      return *this;
   }

   PresentAllocationInfo& setBase(int b)
   {
      __num_base = b;
      return *this;
   }

   PresentAllocationInfo& setPrecision(int p)
   {
      __decimal_acc = p;
      return *this;
   }

   // ADDING -~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
   PresentAllocationInfo& operator<<(const PresentAllocationInfo&)
   {
      return *this;
   }

   PresentAllocationInfo& operator<<(long long int v)
   {
      char* result = __allocator.allocate(50);
      if (__num_base < 2 || __num_base > 36)
      {
         *this << "INVALID_INT";
         return *this;
      }
      char *ptr = result, *ptr1 = ptr, tmp_char;
      long long int tmp_v;
      do
      {
         tmp_v = v;
         v /= __num_base;
         *ptr++ =
             /*
             "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"
                         */
             "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRST"
             "UVWXYZ"[35 + (tmp_v - v * __num_base)];

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
      __allocator.deallocate(result, 50);
      return *this;
   }

   PresentAllocationInfo& operator<<(double v)
   {
      if (__num_base < 2 || __num_base > 36)
      {
         *this << "INVALID_DOUBLE";
         return *this;
      }
      char* decimal_string       = __allocator.allocate(__decimal_acc + 1);
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

      size_t precision = __decimal_acc;

      while (precision > 0 && ((decimal_part != 0.0f) || __is_total_precision))
      {
         decimal_part *= __num_base;
         size_t intPart = static_cast<size_t>(decimal_part);
         __allocator.construct(decimal_string + __decimal_acc - precision,
                               "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[intPart]);
         decimal_part -= intPart;
         --precision;
      }
      __allocator.construct(decimal_string + __decimal_acc - precision, '\0');
      *this << integer_part << "." << decimal_string;
      __allocator.deallocate(decimal_string, __decimal_acc + 1);
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
      if (__last != __data)
      {
         __allocator.destroy(__last);
         __last--;
      }
      const char* ptr = message;
      size_t chars    = 0;
      while (*ptr != '\0')
      {
         chars++;
         ptr++;
      }

      size_t old_size       = __max - __data;
      size_t old_ready_size = __last - __data;
      if (old_size - old_ready_size < chars)
      {
         size_t new_size = old_size * 2;
         __data          = __allocator.reallocate(__data, old_size, new_size);
         __last          = __data + old_ready_size;
         __max           = __data + new_size;
      }
      ptr = message;
      while (*ptr != '\0')
      {
         __allocator.construct(__last, *ptr);
         __last++;
         ptr++;
      }
      __allocator.construct(__last, '\0');
      __last++;
      return *this;
   }

   friend std::ostream& operator<<(std::ostream& os,
                                   const PresentAllocationInfo& message)
   {
      for (char* ch = message.__data; ch != message.__last; ++ch)
      {
         os << *ch;
      }
      // os << message.__data;
      return os;
   }
};

}  // namespace pbu

#endif
