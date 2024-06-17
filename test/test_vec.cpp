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
      // pbu::PresentAllocationInfo info = vec.rprInfo();
      // std::cout << info;
   }
   SECTION("REALLOCATION VIA ADDING OR EMPLACING")
   {
      pbu::Vec<int> vec(1);
      REQUIRE(vec.allocationCount() == 1);
      REQUIRE(vec.reallocationCount() == 0);
      vec.add(10);
      vec.add(20);
      REQUIRE(vec.allocationCount() == 2);
      REQUIRE(vec.reallocationCount() == 1);
   }
   SECTION("SIMPLE ITERATORS")
   {
      pbu::Vec<int> vec(4);
      for (int i = 0; i < 4; ++i)
      {
         vec.emp(i);
      }
      int i = 0;
      for (int* it = vec.begin(); it != vec.end(); ++it)
      {
         REQUIRE(*it == i);
         i++;
      }
      for (int* it = vec.rbegin(); it != vec.rend(); --it)
      {
         REQUIRE(*it == i);
         i--;
      }
   }
   {
      pbu::Vec<int> v(5);
      SECTION("Reserve and capacity")
      {
         v.reserve(10);

         REQUIRE(v.capacity() == 10);
      }

      SECTION("Clearing the vector")
      {
         v.add(5);
         v.add(15);

         v.clear();

         REQUIRE(v.size() == 0);
         REQUIRE(v.capacity() == 5);
      }
      SECTION("Copy constructor")
      {
         v.add(7);
         v.add(14);

         pbu::Vec<int> v2(v);

         REQUIRE(v2.size() == 2);
         REQUIRE(v2[0] == 7);
         REQUIRE(v2[1] == 14);
      }
      //
      SECTION("Move constructor")
      {
         v.add(3);
         v.add(6);

         pbu::Vec<int> v2(PBU_MOV(v));

         REQUIRE(v.size() == 0);
         REQUIRE(v2.size() == 2);
         REQUIRE(v2[0] == 3);
         REQUIRE(v2[1] == 6);
      }
      //
      SECTION("Assignment operator")
      {
         v.add(2);
         v.add(4);

         pbu::Vec<int> v2(3);

         v2 = v;

         REQUIRE(v2.size() == 2);
         REQUIRE(v2[0] == 2);
         REQUIRE(v2[1] == 4);
      }

      SECTION("Move assignment operator")
      {
         v.add(1);
         v.add(3);

         pbu::Vec<int> v2(2);

         v2 = std::move(v);

         REQUIRE(v.size() == 0);
         REQUIRE(v2.size() == 2);
         REQUIRE(v2[0] == 1);
         REQUIRE(v2[1] == 3);
      }
   }
}
