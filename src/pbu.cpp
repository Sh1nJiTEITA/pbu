#include "pbu.h"

void test_progress_bar(int percent)
{
   std::cout << "\r[";
   int bar_width = 50;
   int pos       = bar_width * percent / 100;
   for (int i = 0; i < bar_width; ++i)
   {
      if (i < pos)
      {
         std::cout << "#";
      }
      else
      {
         std::cout << " ";
      }
   }
   std::cout << "]" << percent << "%";
   std::cout.flush();
}
