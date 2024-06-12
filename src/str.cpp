#include "str.h"

namespace pbu
{

void Str::_alloc(size_t max_size)
{
   m_data = (char *)malloc(max_size);
   if (m_data == NULL)
   {
      perror("Str::_alloc: error during allocation");
      exit(EXIT_FAILURE);
   }
}

size_t Str::countChars(const char *str)
{
   size_t i = 0;
   while (str[i] != '\0')
   {
      i++;
   }
   return i;
}

void Str::_copy(const char **str, size_t num, const char *sep)
{
   char *ptr   = m_data;
   bool is_sep = sep[0] != '\0';
   for (size_t i = 0; i < num; ++i)
   {
      while (*(str[i]) != '\0')
      {
         *ptr = *str[i];
         str[i]++;
         ptr++;
      }
      if (is_sep && i < num - 1)
      {
         const char *sep_b = sep;
         while (*sep_b != '\0')
         {
            *ptr = *sep_b;
            ptr++;
            sep_b++;
         }
         sep_b = sep;
      }
   }
   *ptr = '\0';
}

Str::Str(const char *d)
    : m_len{countChars(d)}
{
   _alloc(sizeof(char) * m_len);
   _copy(&d, 1, "");
}

Str::Str(const char **d, size_t num, const char *sep)
    : m_len{0}
{
   for (size_t i = 0; i < num; ++i)
   {
      m_len += Str::countChars(d[i]);
   }
   size_t sep_len;
   if (sep[0] == '\0')
   {
      sep_len = 0;
   }
   else
   {
      sep_len = Str::countChars(sep) * (num - 1);
   }
   m_len += sep_len + 1; // +1 -> +'\0'
   _alloc(sizeof(char) * m_len);
   _copy(d, num, sep);
}

Str::~Str() { free(m_data); }

void Str::printWithNull() const
{
   int i = 0;
    while (1) {
        if (m_data[i] == '\0') {
            printf("\\0");
            break;
        } else {
            printf("%c", m_data[i]);
        }
        i++;
    }
    printf("\n");
}


void Str::printSharps() const 
{
   for (size_t i = 0; i < m_len; ++i) { 
      printf("#");
   }
}
std::ostream &operator<<(std::ostream &os, const Str &s)
{
   os << s.m_data;
   return os;
}

// Str operator+(const Str &s) {}

Str concat(const Str *strs, size_t num, const char* sep)
{
   const char** d = (const char**)malloc(sizeof(const char*) * num);
   for (size_t i = 0; i < num; ++i) { 
      d[i] = strs[i].m_data;
   }
   Str out(d, num, sep);
   free(d);
   return out;
}

}  // namespace pbu
