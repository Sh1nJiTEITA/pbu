#include <catch2/catch_test_macros.hpp>
#include <chrono>

#include "pbu.h"
// #include <sys/ioctl.h>
#include <thread>
//

// #include <catch2/catch_all.hpp>

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

TEST_CASE("Str")
{
   REQUIRE(1 == 1);
   // test_simple_progress_bar();
   const char* b = "1234567";
   
   const char* c[] = {
      "a",
      "b",
      "c"
   };


   const pbu::Str a(b);

   const pbu::Str multi_s(c, 3, "_");

   // multi_s.printWithNull();
   // std::cout << "\n";
   // multi_s.printSharps();
   
   const pbu::Str* concat_strs = new pbu::Str[2]{
      a,
      multi_s
   };

   pbu::Str concat_s = pbu::concat(concat_strs, 2, "|");
   std::cout << concat_s << "\n" ;

}
