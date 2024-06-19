#ifndef PBUSTRV_H
#define PBUSTRV_H

#include <iostream>

#include "allocator.hpp"
#include "vec.hpp"
namespace pbu
{

class Str : public Vec<char>
{
protected:
   size_t __countChars(const char* s)
   {
      const char* ptr = s;
      size_t count    = 0;
      while (*ptr != '\0')
      {
         count++;
         ptr++;
      }
      return count;
   }

   void __ensure_capacity(size_t s)
   {
      if (!__data)
      {
         reserve(s);
      }
      else if (__last == __max || size() + s > capacity())
      {
         reserve(2 * (size() > s ? size() + 1 : s + 1));
      }
   }

   void __kill_null_term()
   {
      if (*(__last - 1) != '\0') abort();
      if (size() != 0)
      {
         __allocator.destroy(__last - 1);
         __last--;
      }
   }
   void __add_null_term()
   {
      if (*(__last - 1) == '\0') abort();
      __allocator.construct(__last++, '\0');
   }

public:
   Str()
       : Vec<char>()
   {
      __allocator.construct(__last++, '\0');
   }

   Str(const char* s)
       : Vec<char>{__countChars(s) + 1}
   {
      const char* ptr = s;
      while (*ptr != '\0')
      {
         __allocator.construct(__last++, *ptr);
         ptr++;
      }
      __add_null_term();
   }
   Str(char s)
       : Vec<char>{2}
   {
      __allocator.construct(__last++, s);
      __add_null_term();
   }

   Str(const Str& o)
       : Vec<char>{o}
   {
   }

   Str(Str&& o) noexcept
       : Vec<char>{PBU_MOV(o)}
   {
      o = Str();
   }

   void add(const char& v) override
   {
      __ensure_capacity(1);
      __kill_null_term();
      __allocator.construct(__last++, v);
      __add_null_term();
   }
   void add(const char&& v) override
   {
      __ensure_capacity(1);
      __kill_null_term();
      __allocator.construct(__last++, PBU_MOV(v));
      __add_null_term();
   }
   void add(const char* s)
   {
      __ensure_capacity(__countChars(s));
      __kill_null_term();
      const char* ptr = s;
      while (*ptr != '\0')
      {
         __allocator.construct(__last++, *ptr);
         ptr++;
      }
      __add_null_term();
   }
   Str operator+(const char* s) const
   {
      Str _s{*this};
      _s.add(s);
      return _s;
   }
   Str& operator+=(const char* s)
   {
      add(s);
      return *this;
   }
   Str& operator=(const Str& v)
   {
      if (this != &v)
      {
         clear();
         __allocator = v.__allocator;
         reserve(v.capacity());
         for (const char* it = v.begin(); it != v.end(); ++it)
         {
            __allocator.construct(__last++, *it);
         }
      }
      return *this;
   }
   Str& operator=(Str&& v)
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
   size_t count(const Str& s)
   {
      char* m_ptr    = __data;
      char* s_ptr    = s.__data;
      size_t counter = 0;
      bool is_count  = false;
      do
      {
         if (*s_ptr == '\0' && is_count)
         {
            counter++;
            s_ptr = s.__data;
         }
         if (*m_ptr == *s_ptr)
         {
            is_count = true;
            s_ptr++;
         }
         else
         {
            is_count = false;
            s_ptr    = s.__data;
         }
      } while (*m_ptr++ != '\0');
      return counter;
   }
   bool is_inside(const Str& s)
   {
      char* m_ptr   = __data;
      char* s_ptr   = s.__data;
      bool is_count = false;
      do
      {
         if (*s_ptr == '\0' && is_count)
         {
            return true;
         }
         if (*m_ptr == *s_ptr)
         {
            is_count = true;
            s_ptr++;
         }
         else
         {
            is_count = false;
            s_ptr    = s.__data;
         }
      } while (*m_ptr++ != '\0');
      return false;
   }
   Vec<Str> split(const Str& s)
   {
      Vec<Str> v(size() / s.size());
      char* m_ptr   = __data;
      char* m_ptr_b = __data;
      char* s_ptr   = s.__data;
      bool is_count = false;
      do
      {
         if (*s_ptr == '\0' && is_count)
         {
            // v.add(
         }
         if (*m_ptr == *s_ptr)
         {
            is_count = true;
            s_ptr++;
         }
         else
         {
            m_ptr_b  = m_ptr;
            is_count = false;
            s_ptr    = s.__data;
         }
      } while (*m_ptr++ != '\0');
   }
};

};  // namespace pbu

#endif
