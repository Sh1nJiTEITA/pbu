#include "str.h"

#include <cstring>

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
    : m_len{countChars(d) + 1}
{
   _alloc(sizeof(char) * (m_len));
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
   m_len += sep_len + 1;  // +1 -> +'\0'
   _alloc(sizeof(char) * m_len);
   _copy(d, num, sep);
}

Str::~Str() { free(m_data); }

const char *Str::c_str() const { return m_data; }

void Str::printWithNull() const
{
   int i = 0;
   while (1)
   {
      if (m_data[i] == '\0')
      {
         printf("\\0");
         break;
      }
      else
      {
         printf("%c", m_data[i]);
      }
      i++;
   }
   printf("\n");
}

void Str::printSharps() const
{
   for (size_t i = 0; i < m_len; ++i)
   {
      printf("#");
   }
}
std::ostream &operator<<(std::ostream &os, const Str &s)
{
   os << s.m_data;
   return os;
}

// Str operator+(const Str &s) {}

Str concat(const Str *strs, size_t num, const char *sep)
{
   const char **d = (const char **)malloc(sizeof(const char *) * num);
   for (size_t i = 0; i < num; ++i)
   {
      d[i] = strs[i].m_data;
   }
   Str out(d, num, sep);
   free(d);
   return out;
}

int split(const Str &str, const char *sep, Str *&arr, size_t *count)
{
   int sep_len = Str::countChars(sep);
   if (sep_len == 0) return 0;

   char **str_arr = (char **)malloc(sizeof(char *) * (str.m_len / sep_len + 1));
   int sub_str_count = 0;
   const char *ptr   = str.m_data;
   const char *last  = str.m_data;

   while (*ptr != '\0')
   {
      if (strncmp(ptr, sep, sep_len) == 0)
      {
         int substring_len      = ptr - last;
         str_arr[sub_str_count] = (char *)malloc(substring_len + 1);
         strncpy(str_arr[sub_str_count], last, substring_len);
         str_arr[sub_str_count][substring_len] = '\0';

         sub_str_count++;
         ptr += sep_len;
         last = ptr;
      }
      else
      {
         ptr++;
      }
   }

   if (last != ptr)
   {
      int substring_len      = ptr - last;
      str_arr[sub_str_count] = (char *)malloc(substring_len + 1);
      strncpy(str_arr[sub_str_count], last, substring_len);
      str_arr[sub_str_count][substring_len] = '\0';

      sub_str_count++;
   }

   if (arr == nullptr)
   {
      arr = (Str *)malloc(sizeof(Str) * sub_str_count);
   }

   for (int i = 0; i < sub_str_count; ++i)
   {
      arr[i] = Str(str_arr[i]);
   }

   for (int i = 0; i < sub_str_count; ++i)
   {
      free(str_arr[i]);
   }
   free(str_arr);
   if (count != nullptr)
   {
      *count = sub_str_count;
   }

   return 1;
}

}  // namespace pbu
