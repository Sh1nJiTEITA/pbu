#ifndef PBU_H
#define PBU_H

#include <iostream>
#ifdef __linux__
#include <sys/ioctl.h>
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif

void test_progress_bar(int);

#include <allocator.hpp>

template <class T>
class PBUBar
{
   T m_n;
   T m_total;
   int m_bar_width;
   char* m_desc;

public:
   PBUBar() {}

#ifdef __linux__
   static winsize termSize()
   {
      winsize w;
      if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
      {
         std::cerr << "PBU: Error during getting window size" << std::endl;
      }
      return w;
   }

   static int termRows() { return PBUBar::termSize().ws_row; }
   static int termCols() { return PBUBar::termSize().ws_col; }
#elif _WIN32
   static int termRows()
   {
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
      if (console != INVALID_HANDLE_VALUE &&
          GetConsoleScreenBufferInfo(console, &csbi))
      {
         return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
      }
      return -1;
   }

   static int termCols()
   {
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
      if (console != INVALID_HANDLE_VALUE &&
          GetConsoleScreenBufferInfo(console, &csbi))
      {
         return csbi.srWindow.Right - csbi.srWindow.Left + 1;
      }
      return -1;
   }
#endif  // _WIN32
   void setN(T n) { m_n = n; }
   void setTotal(T t) { m_total = t; }
   void setWidth(int w) { m_bar_width = w; }
   void display()
   {
      std::cout << "\r[";
      float percent = static_cast<float>(m_n) / static_cast<float>(m_total);
      // std::cout << m_n << "/" << m_total << "\n";
      int pos = static_cast<int>(m_bar_width * percent);
      // std::cout << "pos: " << pos << "\n";
      for (int i = 0; i < m_bar_width; ++i)
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
      std::cout << "]" << percent * 100 << "%";
      std::cout.flush();
   }
   void start() {}
   void end() {}
};

#endif
