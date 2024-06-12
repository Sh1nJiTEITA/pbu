#ifndef PBUSTR_H
#define PBUSTR_H

#include <cstdio>
#include <cstdlib>
#include <ostream>

namespace pbu
{

class Str
{
   char* m_data;
   size_t m_len;

   void _alloc(size_t max_size);
   size_t _countChars(const char* str);
   void _copy(const char* str);

public:
   Str(const char*);
   ~Str();

   friend std::ostream& operator<<(std::ostream& os, const Str& s);
   Str operator+(const Str& s);
};

Str concat(...);

}  // namespace pbu

#endif
