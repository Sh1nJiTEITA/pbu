#ifndef PBUVEC_H
#define PBUVEC_H

#include <allocator.hpp>

namespace pbu
{
template <class T, class Allocator = pbu::Allocator<T>>
class Vec
{
   T* m_data = nullptr;
   T* m_last = nullptr;
   T* m_max  = nullptr;

   pbu::Allocator<T> m_allocator;

public:
   // Vec(size_t size) {
   //
   // };

   void reserve(size_t n)
   {
      if (m_data == nullptr)
      {
         m_data = m_allocator.allocate(n);
      }
      else { 
         
      }
      m_max = m_data + n;
   };
};
};  // namespace pbu

#endif
