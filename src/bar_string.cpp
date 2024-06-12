#include "bar_string.h"

#include <cstdio>

void BarString::_alloc_data(int str_count)
{
   if (m_data == nullptr)
   {
      perror("BarString: Trying to allocate already allocated string");
   }
   m_data = (char**)malloc(str_count * sizeof(char*));
   if (m_data == NULL)
   {
      perror("BarString: Error during allocation string array");
      exit(EXIT_FAILURE);
   }
   for (int i = 0; i < str_count; ++i)
   {
      m_data[i] = nullptr;
      // m_data[i] = (char*)malloc((str_len + 1) * sizeof(char));
      // if (m_data[i] == NULL)
      // {
      //    perror("BarString: Error during allocation strings");
      //    exit(EXIT_FAILURE);
      // }
   }
   m_count = str_count;
}

void BarString::_free()
{
   for (int i = 0; i < m_count; ++i)
   {
      if (m_data[i] != nullptr)
      {
         std::free(m_data[i]);
      }
   }
   std::free(m_data);
}

void BarString::add(char* str)
{
   if (m_data == nullptr)
   {
      _alloc_data(1);
   }
   else
   {
      char** old = m_data;
      m_data     = nullptr;
      _alloc_data(m_count + 1);
      for (int i = 0; i < m_count - 1; ++i)
      {
         m_data[i] = old[i];
      }
   }
}
