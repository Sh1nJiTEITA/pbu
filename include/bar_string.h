#ifndef BARSTRING_H
#define BARSTRING_H

#include <cstdlib>

#ifdef __linux__
#include <cstdio>
#endif

class BarString
{
   char** m_data;
   size_t m_count;

   void _alloc_data(int str_count);
   void _free();

public:
   BarString();
   void add(char* str);
};

#endif  // BARSTRING_H
