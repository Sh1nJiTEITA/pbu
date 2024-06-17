#include <catch2/catch_test_macros.hpp>

#include "flist.hpp"

TEST_CASE("TESTING LIST")
{
   pbu::FList<int> l;

   for (int i = 0; i < 10; ++i)
   {
      l.add(i);
   }
   int i = 0;
   for (auto it = l.begin(); it != l.end(); ++it)
   {
      REQUIRE(*it == i);
      i++;
   }
};
