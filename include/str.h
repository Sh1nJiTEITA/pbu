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
   void _copy(const char** str, size_t num, const char* sep);

public:
   Str(const char*);
   Str(const char**, size_t num, const char* sep = "");
   ~Str();
   const char* c_str() const;
   void printWithNull() const;
   void printSharps() const;

   static size_t countChars(const char* str);

   friend std::ostream& operator<<(std::ostream& os, const Str& s);
   friend Str concat(const Str* strs, size_t num, const char* sep);
   // Str operator+(const Str& s);
};

Str concat(const Str* strs, size_t num, const char* sep);

}  // namespace pbu

#endif
