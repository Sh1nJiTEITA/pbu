#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <iostream>
#include <strv.hpp>
TEST_CASE("TESTING STR")
{
   SECTION("CHAR ADD")
   {
      pbu::Str a;
      a.add('c');
      a.add('o');
      a.add('c');
      a.add('k');
      REQUIRE(strcmp(a.data(), "cock") == 0);
   }
   SECTION("CHAR* ADD")
   {
      pbu::Str a;
      a.add("hi");
      INFO("Expected 'hi', but got: " << a.rprInfo().get());
      REQUIRE(strcmp(a.data(), "hi") == 0);
      a.add(", iam SERJ");
      INFO("Expected 'hi, iam SERJ', but got: " << a.data());
      REQUIRE(strcmp(a.data(), "hi, iam SERJ") == 0);
   }
   SECTION("CHAR* ADD(+)")
   {
      pbu::Str a;
      pbu::Str b = a + "hi";
      INFO("Expected 'hi', but got: " << b.rprInfo().get());
      REQUIRE(strcmp(b.data(), "hi") == 0);
      pbu::Str c = b + ", iam SERJ";
      REQUIRE(strcmp(c.data(), "hi, iam SERJ") == 0);
      REQUIRE(strcmp(b.data(), "hi") == 0);
      REQUIRE(strcmp(a.data(), "\0") == 0);
      REQUIRE(strcmp(a.data(), "") == 0);
   }
   SECTION("CHAR* ADD(=+)")
   {
      pbu::Str a;
      a += "hi";
      INFO("Expected 'hi', but got: " << a.data());
      REQUIRE(strcmp(a.data(), "hi") == 0);
      a += ", iam SERJ";
      INFO("Expected 'hi, iam SERJ', but got: " << a.data());
      REQUIRE(strcmp(a.data(), "hi, iam SERJ") == 0);
   }
   SECTION("CONSTRUCTORS")
   {
      SECTION("STANDART")
      {
         pbu::Str a;
         REQUIRE(strcmp(a.data(), "") == 0);
         REQUIRE(strcmp(a.data(), "\0") == 0);
      }
      SECTION("COPY")
      {
         pbu::Str b;
         b.add("SMT");
         pbu::Str a(b);
         REQUIRE(strcmp(a.data(), "SMT") == 0);
         REQUIRE(strcmp(b.data(), "SMT") == 0);
      }
      SECTION("MOVE")
      {
         pbu::Str b;
         b.add("SMT");
         pbu::Str a(PBU_MOV(b));
         REQUIRE(strcmp(a.data(), "SMT") == 0);
         REQUIRE(strcmp(b.data(), "") == 0);
      }
      SECTION("CHAR*")
      {
         pbu::Str b("mama");
         REQUIRE(strcmp(b.data(), "mama") == 0);
         b.add("SMT");

         INFO("Expected 'mamaSMT', but got: " << b.rprInfo().get());
         REQUIRE(strcmp(b.data(), "mamaSMT") == 0);
         pbu::Str a(PBU_MOV(b));

         REQUIRE(strcmp(a.data(), "mamaSMT") == 0);
         REQUIRE(strcmp(b.data(), "") == 0);
      }
      SECTION("CHAR")
      {
         pbu::Str b('a');
         REQUIRE(strcmp(b.data(), "a") == 0);
      }
   }
   SECTION("pbu::Str::count")
   {
      pbu::Str a("a_b_c");
      REQUIRE(a.count("_") == 2);
      pbu::Str b("a__b__c");
      REQUIRE(b.count("__") == 2);
      pbu::Str c("a!?!b!?c!?!d#!!!e?!?!");
      REQUIRE(c.count("!?!") == 3);
      pbu::Str d("_a_b_c_");
      REQUIRE(d.count("_") == 4);
   }
   SECTION("pbu::Str::is_inside")
   {
      pbu::Str b("a__b__d");
      REQUIRE(b.is_inside('a'));
      REQUIRE(b.is_inside('b'));
      REQUIRE(b.is_inside("d"));
      REQUIRE(b.is_inside('_'));
      REQUIRE(b.is_inside("__"));
      REQUIRE(!b.is_inside("!13"));
      REQUIRE(!b.is_inside("b__a"));
   }
}
