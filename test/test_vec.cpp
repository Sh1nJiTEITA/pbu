#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "vec.hpp"

TEST_CASE("VEC TEST")
{
   SECTION("ADDING")
   {
      pbu::Vec<int> vec(10);
      vec.reserve(20);
      vec.add(1);
      vec.add(2);
      
      REQUIRE(1 == 1);
      REQUIRE(vec[0] == 1);
      REQUIRE(vec[1] == 2);
   }
}
