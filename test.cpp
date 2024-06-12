#include <chrono>

#include "pbu.h"
// #include <sys/ioctl.h>
#include <thread>
//

#include "str.h"

void test_simple_progress_bar()
{
   PBUBar<int> bar;
   bar.setTotal(100);
   // bar.setN(0);
   // bar.display();
   bar.setWidth(PBUBar<int>::termCols() - 10);
   // bar.setWidth(50);

   for (int i = 0; i <= 100; ++i)
   {
      bar.setN(i);
      bar.display();
      // test_progress_bar(i);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }
}

int main()
{
   // test_simple_progress_bar();
   const char* b = "1234567";
   pbu::Str a(b);
   std::cout << a << "\n";  // << a._countChars(a.m_data) << std::endl;

   // std::cout << "cols: " << PBUBar<int>::termCols() << "\n";
   // std::cout << "rows: " << PBUBar<int>::termRows() << "\n";
   // std::cout << "cols: " << ws.w << "\n";
   // std::cout << "cols: " << ws.ws_col << "\n";

   return 0;
}
