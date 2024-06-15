#include <catch2/catch_test_macros.hpp>
#include <chrono>

#include "pbu.h"
// #include <sys/ioctl.h>
#include <cstring>
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
   const char* b   = "1234567";
   const char* c[] = {"a", "b", "c"};

   const pbu::Str a(b);

   // std::cout << "\n";
   // a.printSharps();
   // std::cout << "\n";
   // a.printWithNull();
   // std::cout << "\n";

   REQUIRE(!strcmp(a.c_str(), "1234567"));

   const pbu::Str multi_s(c, 3, "!?");
   REQUIRE(!strcmp(multi_s.c_str(), "a!?b!?c"));
   // std::cout << multi_s << "\n";
   {
      pbu::Str* arr;
      size_t arr_count;
      // REQUIRE(pbu::split(multi_s, "!?", arr, &arr_count));
      // REQUIRE(arr_count == 3);
      // REQUIRE(!strcmp(arr[0].c_str(), "a"));
      // REQUIRE(!strcmp(arr[1].c_str(), "b"));
      // REQUIRE(!strcmp(arr[2].c_str(), "c"));
   }

   const pbu::Str* concat_strs = new pbu::Str[2]{a, multi_s};

   pbu::Str concat_s = pbu::concat(concat_strs, 2, "|");
   // std::cout << concat_s << "\n";
}
