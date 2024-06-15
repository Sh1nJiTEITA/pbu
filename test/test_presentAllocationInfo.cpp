#include <allocator.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iomanip>
#include <iostream>

char* __itoa(int value, char* result, int base)
{
   // check that the base if valid
   if (base < 2 || base > 36)
   {
      *result = '\0';
      return result;
   }

   char *ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do
   {
      tmp_value = value;
      value /= base;
      *ptr++ =
          "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvw"
          "xyz"[35 + (tmp_value - value * base)];
   } while (value);

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';

   // Reverse the string
   while (ptr1 < ptr)
   {
      tmp_char = *ptr;
      *ptr--   = *ptr1;
      *ptr1++  = tmp_char;
   }
   return result;
}
void preserveFractionAsInteger(double& d)
{
   uint64_t* ptr = reinterpret_cast<uint64_t*>(&d);
   uint64_t sign = *ptr & 0x8000000000000000;
   int exponent = (*ptr >> 52) & 0x7FF;
   if (exponent == 0)
   {
      std::cerr << "Subnormal numbers are not supported." << std::endl;
      return;
   }
   uint64_t fraction = *ptr & 0x000FFFFFFFFFFFFF;
   if (exponent != 0x7FF)
   {
      fraction |= 0x0010000000000000;
   }
   fraction <<= (52 - exponent);
   if (sign)
   {
      d = -static_cast<double>(fraction);
   }
   else
   {
      d = static_cast<double>(fraction);
   }
}

int extractFraction(float f, int decimalPlaces)
{
   std::cout << "float: " << f << "\n";
   float scaledValue = f - (float)(int)f;
   for (int i = 0; i < decimalPlaces; ++i)
   {
      scaledValue *= 10.0f;
      std::cout << scaledValue << "\n";
   }

   int scaledIntValue = static_cast<int>(scaledValue);
   int fraction       = scaledIntValue;

   return fraction;
}

TEST_CASE("ALLOCATION_INFO")
{
   SECTION("STD::COUT")
   {
      double a = 1280.9283123;
      // std::cout << extractFraction(a, 4);
      // preserveFractionAsInteger(a);
      pbu::PresentAllocationInfo b(100);
      b << a;
      std::cout << std::endl << b;
   }
}
