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
   size_t _countChars(const char* s)
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

   void _ensure_capacity(size_t s)
   {
      if (!_data)
      {
         reserve(s);
      }
      else if (_last == _max || size() + s > capacity())
      {
         reserve(2 * (size() > s ? size() + 1 : s + 1));
      }
   }

   void _kill_null_term()
   {
      if (*(_last - 1) != '\0') abort();
      if (size() != 0)
      {
         _allocator.destroy(_last - 1);
         _last--;
      }
   }
   void _add_null_term()
   {
      if (*(_last - 1) == '\0') abort();
      _allocator.construct(_last++, '\0');
   }

public:
   Str()
       : Vec<char>()
   {
      _allocator.construct(_last++, '\0');
   }

   Str(const char* s)
       : Vec<char>{_countChars(s) + 1}
   {
      const char* ptr = s;
      while (*ptr != '\0')
      {
         _allocator.construct(_last++, *ptr);
         ptr++;
      }
      _add_null_term();
   }
   Str(char s)
       : Vec<char>{2}
   {
      _allocator.construct(_last++, s);
      _add_null_term();
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
      _ensure_capacity(1);
      _kill_null_term();
      _allocator.construct(_last++, v);
      _add_null_term();
   }
   void add(const char&& v) override
   {
      _ensure_capacity(1);
      _kill_null_term();
      _allocator.construct(_last++, PBU_MOV(v));
      _add_null_term();
   }
   void add(const char* s)
   {
      _ensure_capacity(_countChars(s));
      _kill_null_term();
      const char* ptr = s;
      while (*ptr != '\0')
      {
         _allocator.construct(_last++, *ptr);
         ptr++;
      }
      _add_null_term();
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
         _allocator = v._allocator;
         reserve(v.capacity());
         for (const char* it = v.begin(); it != v.end(); ++it)
         {
            _allocator.construct(_last++, *it);
         }
      }
      return *this;
   }
   Str& operator=(Str&& v)
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
   size_t count(const Str& s)
   {
      char* m_ptr    = _data;
      char* s_ptr    = s._data;
      size_t counter = 0;
      bool is_count  = false;
      do
      {
         if (*s_ptr == '\0' && is_count)
         {
            counter++;
            s_ptr = s._data;
         }
         if (*m_ptr == *s_ptr)
         {
            is_count = true;
            s_ptr++;
         }
         else
         {
            is_count = false;
            s_ptr    = s._data;
         }
      } while (*m_ptr++ != '\0');
      return counter;
   }
   bool is_inside(const Str& s)
   {
      char* m_ptr   = _data;
      char* s_ptr   = s._data;
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
            s_ptr    = s._data;
         }
      } while (*m_ptr++ != '\0');
      return false;
   }
   Vec<Str> split(const Str& s)
   {
      Vec<Str> v(size() / s.size());
      char* m_ptr   = _data;
      char* m_ptr_b = _data;
      char* s_ptr   = s._data;
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
            s_ptr    = s._data;
         }
      } while (*m_ptr++ != '\0');
   }
};

};  // namespace pbu

#endif
