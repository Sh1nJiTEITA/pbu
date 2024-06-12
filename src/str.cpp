#include "str.h"

namespace pbu
{

void Str::_alloc(size_t max_size)
{
   m_data = (char*)malloc(max_size);
   if (m_data == NULL)
   {
      perror("Str::_alloc: error during allocation");
   }
}

size_t Str::_countChars(const char* str)
{
   size_t i = 0;
   while (str[i] != '\0')
   {
      i++;
   }
   return i;
}

void Str::_copy(const char* str)
{
   char* ptr = m_data;
   while (*str != '\0')
   {
      *ptr = *str;
      str++;
      ptr++;
   }
   *ptr = '\0';
}

Str::Str(const char* d)
{
   m_len = _countChars(d);
   _alloc(sizeof(char) * m_len);
   _copy(d);
}

Str::~Str() { free(m_data); }

std::ostream& operator<<(std::ostream& os, const Str& s)
{
   os << s.m_data;
   return os;
}

Str operator+(const Str& s) {}

}  // namespace pbu
