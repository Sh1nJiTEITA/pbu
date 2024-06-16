#include <allocator.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <iomanip>
#include <iostream>

TEST_CASE("Test double to string conversion with specific base and precision",
          "[conversion]")
{
   SECTION("Decimal conversion with precision 5")
   {
      pbu::PresentAllocationInfo b(100);
      b << b.setBase(10) << b.setPrecision(5) << 1280.9283123;
      REQUIRE(strcmp(b.get(), "1280.92831") == 0);
   }

   SECTION("Decimal conversion with precision 2")
   {
      pbu::PresentAllocationInfo b(100);
      b << b.setBase(10) << b.setPrecision(2) << 123.456789;
      REQUIRE(strcmp(b.get(), "123.45") == 0);
   }

   SECTION("Decimal conversion with precision 0")
   {
      pbu::PresentAllocationInfo b(100);
      b << b.setBase(10) << b.setPrecision(0) << 123.456789;
      REQUIRE(strcmp(b.get(), "123.") == 0);
   }

   SECTION("Negative decimal conversion with precision 3")
   {
      pbu::PresentAllocationInfo b(100);
      b << b.setBase(10) << b.setPrecision(3) << -123.456789;
      REQUIRE(strcmp(b.get(), "-123.456") == 0);
   }

   SECTION("Hexadecimal conversion with precision 0")
   {
      pbu::PresentAllocationInfo b(100);
      b << b.setBase(16) << b.setPrecision(0) << 255.0;
      REQUIRE(strcmp(b.get(), "FF.") == 0);
   }

   SECTION("Hexadecimal conversion with precision 3")
   {
      pbu::PresentAllocationInfo b(100);
      b << b.setBase(16) << b.setPrecision(3) << 255.123;
      REQUIRE(strcmp(b.get(), "FF.1F7") == 0);
   }

   SECTION("Binary conversion with precision 5")
   {
      pbu::PresentAllocationInfo b(100);
      b << b.setIsTotalPrecision(true) << b.setBase(2) << b.setPrecision(5)
        << 0.625;
      REQUIRE(strcmp(b.get(), "0.10100") == 0);
   }

   SECTION("Octal conversion with precision 0")
   {
      pbu::PresentAllocationInfo b(100);
      b << b.setBase(8) << b.setPrecision(0) << 255.0;
      REQUIRE(strcmp(b.get(), "377.") == 0);
   }

   SECTION("Octal conversion with precision 3")
   {
      pbu::PresentAllocationInfo b(100);
      b << b.setBase(8) << b.setPrecision(3) << 255.123;
      REQUIRE(strcmp(b.get(), "377.076") == 0);
   }
}
