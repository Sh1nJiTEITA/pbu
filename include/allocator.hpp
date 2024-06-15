#ifndef PBUALLOCATOR_H
#define PBUALLOCATOR_H

#include <cstddef>
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
      std::clog << PBU_CLOG_GREEN << "[A] Allocationg pointer:\t|"
                << static_cast<void*>(ptr) << "|" << PBU_CLOG_RESET << "\n";
#endif
      __allocation_count++;
      return ptr;
   }

   void deallocate(pointer p, size_type) noexcept
   {
#ifdef PBU_ALLOCATOR_INFO_ENABLE
      std::clog << PBU_CLOG_MAGENTA << "[D] Deallocating pointer:\t|"
                << static_cast<void*>(p) << "|" << PBU_CLOG_RESET << "\n";
#endif
      if (p)
      {
         ::operator delete(p);
         __deallocation_count++;
      }
   }

   [[nodiscard]] pointer reallocate(pointer p, size_t new_size)
   /*
      pointer ptr = allocate(new_size);
      ptr         = pbu_mov(p);
      deallocate(p, 0);
      return ptr;
   */
   {
      pointer new_ptr = allocate(new_size);  // Выделяем новый блок памяти

      for (size_type i = 0; i < new_size; ++i)
      {
         // construct(&new_ptr[i], std::move(p[i]));
         construct(&new_ptr[i], PBU_MOV(p[i]));
         destroy(&p[i]);
      }

      deallocate(p, 0);

      return new_ptr;
   }

   template <typename... Args>
   void construct(pointer p, Args&&... args)
   {
#ifdef PBU_ALLOCATOR_INFO_ENABLE
      std::clog << PBU_CLOG_ORANGE << "[C] Constructing pointer:\t|"
                << static_cast<void*>(p) << "|" << PBU_CLOG_RESET << "\n";
#endif
      new (static_cast<void*>(p)) T(PBU_FWD(args)...);
      __construction_count++;
   }

   void destroy(pointer p)
   {
      ++__destruction_count;
#ifdef PBU_ALLOCATOR_INFO_ENABLE
      std::clog << PBU_CLOG_BLUE << "[D] Destroying pointer:\t\t|"
                << static_cast<void*>(p) << "|" << PBU_CLOG_RESET << "\n";
#endif
      p->~T();
   }

   size_type allocationCount() const noexcept { return __allocation_count; }
   size_type deallocationCount() const noexcept { return __allocation_count; }
   size_type constructionCount() const noexcept { return __allocation_count; }
   size_type destructionCount() const noexcept { return __destruction_count; }

private:
   size_type __allocation_count   = 0;
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

}  // namespace pbu

#endif
