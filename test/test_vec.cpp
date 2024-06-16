#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

// #include "allocator.hpp"
#include "vec.hpp"

TEST_CASE("VEC TEST")
{
   SECTION("ADDING")
   {
      pbu::Vec<int> vec(30);

      for (int i = 0; i < 20; i++)
      {
         vec.add(i);
      }
      for (int i = 0; i < 20; i++)
      {
         REQUIRE(vec[i] == i);
      }

      pbu::PresentAllocationInfo info = vec.rprInfo();
      std::cout << info;

      // REQUIRE(vec[0] == 1);
      // REQUIRE(vec[1] == 2);
   }
}
