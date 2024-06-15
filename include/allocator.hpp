#ifndef PBUALLOCATOR_H
#define PBUALLOCATOR_H

#include <iomanip>
#include <iostream>
#include <limits>

// #define PBU_ALLOCATOR_INFO_ENABLE

#ifdef PBU_ALLOCATOR_INFO_ENABLE

#define PBU_CLOG_RESET "\033[0m"
#define PBU_CLOG_RED "\033[31m"          /* Red */
#define PBU_CLOG_GREEN "\033[32m"        /* Green */
#define PBU_CLOG_YELLOW "\033[33m"       /* Yellow */
#define PBU_CLOG_BLUE "\033[34m"         /* Blue */
#define PBU_CLOG_MAGENTA "\033[35m"      /* Magenta */
#define PBU_CLOG_CYAN "\033[36m"         /* Cyan */
#define PBU_CLOG_WHITE "\033[37m"        /* White */
#define PBU_CLOG_ORANGE "\033[38;5;214m" /* orange */

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
   size_type deallocationCount() const noexcept { return __allocation_count; }
   size_type constructionCount() const noexcept { return __allocation_count; }
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

   int __int_base    = 10;
   int __decimal_acc = 7;
   pbu::Allocator<char> __allocator;

public:
   PresentAllocationInfo(size_t chars_count)
   {
      if (chars_count != 0)
      {
         __data = __allocator.allocate(chars_count);
         __max  = __data + chars_count;
         __last = __data;
         // __max[chars_count-1] = '\0';
      }
   }

   ~PresentAllocationInfo()
   {
      char* ch = __data;
      __allocator.deallocate(__data, __max - __data);
   }

   char& operator[](size_t i) { return __data[i]; }

   PresentAllocationInfo& setIntBase(int b)
   {
      __int_base = b;
      return *this;
   }

   PresentAllocationInfo& operator<<(const PresentAllocationInfo&)
   {
      return *this;
   }

   PresentAllocationInfo& operator<<(size_t v)
   {
      char* result = __allocator.allocate(50);
      if (__int_base < 2 || __int_base > 36)
      {
         *this << "INVALID_INT";
         return *this;
      }
      char *ptr = result, *ptr1 = ptr, tmp_char;
      size_t tmp_v;
      do
      {
         tmp_v = v;
         v /= __int_base;
         *ptr++ =
             "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrst"
             "uvwxyz"[35 + (tmp_v - v * __int_base)];

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
   
   /*
      BAD IMPLEMENTATION: do not work for binary or hex numbers... 
   */
   PresentAllocationInfo& operator<<(double v)
   {
      if (__int_base < 2 || __int_base > 36)
      {
         *this << "INVALID_DOUBLE";
         return *this;
      }
      char* decimal_string   = __allocator.allocate(__decimal_acc + 1);
      size_t integer_part = static_cast<int>(v);
      double decimal_part = v - integer_part;
      size_t master       = 10;
      size_t tmp_v;
      for (short int i = 0; i < __decimal_acc; ++i)
      {
         size_t i_v = decimal_part * master;
         tmp_v = i_v;
         i_v /= __int_base;

         __allocator.construct(decimal_string + i,
             "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefgh"
             "ijklmnopq"
             "rst"
             "uvwxyz"[35 + (tmp_v - i_v * __int_base)]);
         master *= 10;
      }
      *this << integer_part << "." << decimal_string;
      __allocator.deallocate(decimal_string, __decimal_acc);
      return *this;
   }

   PresentAllocationInfo& operator<<(int v)
   {
      *this << (size_t)v;
      return *this;
   }
   PresentAllocationInfo& operator<<(const char* message)
   {
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
      return *this;
   }

   void find_null_term(size_t len)
   {
      char* ptr = __data;
      for (size_t i = 0; i < len; ++i)
      {
         if (*ptr != '\0')
         {
            std::cout << *(ptr + i) << "\n";
         }
         else
         {
            std::cout << '\0' << "\n";
         }
      }
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

// std::ostream& operator<<(std::ostream& os, const PresentAllocationInfo&
// message)

}  // namespace pbu

#endif
