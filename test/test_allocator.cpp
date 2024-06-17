#include <catch2/benchmark/catch_benchmark.hpp>
#include <experimental/filesystem>
#include <string>
#define CATCH_CONFIG_MAIN
#include <string.h>
// #include <windows.h>

#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include "allocator.hpp"

void gen_file()
{
   std::string filename = "large.txt";
   std::ofstream f(filename.c_str(), std::ios::binary);
   if (!f)
   {
      std::cerr << "FILE WASN'T CREATOR TO FILL IN" << std::endl;
   }
   const size_t file_size   = 1LL << 30;
   const size_t buffer_size = 4096;
   char buffer[buffer_size];

   std::mt19937 rng;
   std::uniform_int_distribution<int> dist(0, 255);

   for (size_t i = 0; i < buffer_size; ++i)
   {
      buffer[i] = static_cast<char>(dist(rng));
   }

   size_t bytes_written = 0;
   while (bytes_written < file_size)
   {
      f.write(buffer, buffer_size);
      bytes_written += buffer_size;
   }
   f.close();
}

TEST_CASE("ALLOCATOR")
{
   SECTION("SINGLE_ALLOCATOR")
   {
      pbu::Allocator<char> a;
      char* ptr_a = a.allocate(sizeof(char) * 20);
#ifdef __linux__
      strcpy(ptr_a, "12356");
#elif _WIN32 || _WIN64
      strcpy_s(ptr_a, sizeof(char) * 20, "12356");
#endif
      a.deallocate(ptr_a, sizeof(char) * 10);
      // std::clog << std::endl;
      REQUIRE(a.allocationCount() == 1);
      REQUIRE(a.deallocationCount() == 1);
   }
   SECTION("SINGLE_ALLOCATOR_2")
   {
      pbu::Allocator<int> a;
      int* ptr_a = a.allocate(sizeof(int) * 20);
      a.construct(ptr_a, 10);
      // std::cout << *ptr_a << "\n";
      a.deallocate(ptr_a, sizeof(int));
      // std::clog << std::endl;
      REQUIRE(a.allocationCount() == 1);
      REQUIRE(a.constructionCount() == 1);
      REQUIRE(a.deallocationCount() == 1);
   }
   SECTION("ALLOCATOR USING IN STD::VECTOR<INT> NO RESERVE")
   {
      pbu::Allocator<int> a;
      {
         // std::cout << "Creating allocator...\n";
         std::vector<int, pbu::Allocator<int>> vec({});

         // std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         // std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         // std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         a = vec.get_allocator();
      }
      REQUIRE(a.allocationCount() == 3);
      REQUIRE(a.constructionCount() == 6);
      REQUIRE(a.deallocationCount() == 2);
      REQUIRE(a.destructionCount() == 3);

      // std::clog << std::endl;
   }

   SECTION("ALLOCATOR USING IN STD::VECTOR<INT> WITH RESERVE")
   {
      pbu::Allocator<int> a;
      {
         // std::cout << "Creating allocator...\n";
         std::vector<int, pbu::Allocator<int>> vec({});
         vec.reserve(3);

         // std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         // std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         // std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         a = vec.get_allocator();
      }
      REQUIRE(a.allocationCount() == 1);
      REQUIRE(a.constructionCount() == 3);
      REQUIRE(a.deallocationCount() == 0);
      REQUIRE(a.destructionCount() == 0);
   }

   SECTION("REALLOCATION_TEST")
   {
      SECTION("int* test -> less elements")
      {
         pbu::Allocator<int> a;
         size_t element_count = 10;
         int* vec             = a.allocate(element_count);

         {  // Construction
            for (size_t i = 0; i < element_count; ++i)
            {
               a.construct(&vec[i], i);
            }

            for (size_t i = 0; i < element_count; ++i)
            {
               REQUIRE(vec[i] == i);
            }
         }

         size_t new_element_count = 5;
         {
            vec = a.reallocate(vec, element_count, new_element_count);
            for (size_t i = 0; i < new_element_count; ++i)
            {
               REQUIRE(vec[i] == i);
            }
         }

         for (size_t i = 0; i < element_count; ++i)
         {
            a.destroy(&vec[i]);
         }
         a.deallocate(vec, element_count);
      };

      SECTION("int* test -> more elements")
      {
         pbu::Allocator<int> a;
         size_t element_count = 10;
         int* vec             = a.allocate(element_count);

         {  // Construction
            for (size_t i = 0; i < element_count; ++i)
            {
               a.construct(&vec[i], i);
            }

            for (size_t i = 0; i < element_count; ++i)
            {
               REQUIRE(vec[i] == i);
            }
         }

         size_t new_element_count = 20;
         {
            vec = a.reallocate(vec, element_count, new_element_count);
            for (size_t i = element_count; i < new_element_count; ++i)
            {
               a.construct(&vec[i], i);
            }
            for (size_t i = 0; i < new_element_count; ++i)
            {
               REQUIRE(vec[i] == i);
            }
         }

         for (size_t i = 0; i < element_count; ++i)
         {
            a.destroy(&vec[i]);
         }
         a.deallocate(vec, element_count);
      };

      SECTION("int** test")
      {
         pbu::Allocator<int*> a;
         pbu::Allocator<int> _a;

         size_t element_count = 10;

         int** vec = a.allocate(element_count);

         for (size_t i = 0; i < element_count; ++i)
         {
            vec[i] = _a.allocate(1);
            _a.construct(vec[i], i);
         }

         for (size_t i = 0; i < element_count; ++i)
         {
            // std::cout << *vec[i] << "\n";
            REQUIRE(*vec[i] == i);
         }

         for (size_t i = 0; i < element_count; ++i)
         {
            _a.destroy(vec[i]);
            _a.deallocate(vec[i], 1);
         }
         a.deallocate(vec, element_count);
      }
   }
}
//
// size_t get_file_size(std::string path)
// {
//    std::FILE* file = std::fopen(path.c_str(), "rb");
//    if (!file)
//    {
//       std::cerr << "Failed to open file: " << path << std::endl;
//       return 0;
//    }
//
//    std::fseek(file, 0, SEEK_END);
//    std::size_t size = std::ftell(file);
//    std::fclose(file);
//
//    return size;
// }
//
// size_t get_available_memory()
// {
//    MEMORYSTATUSEX memInfo;
//    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
//    GlobalMemoryStatusEx(&memInfo);
//    return memInfo.ullAvailPhys;
// }
//
// void process_file(std::string path, float memory_load = 0.01f)
// {
//    size_t file_size = get_file_size(path);
//    size_t ava_size  = (size_t)get_available_memory() * memory_load;
//    size_t parts     = (size_t)(file_size / ava_size);
//
//    // std::cout << "Number of parts: " << parts << "\n";
//
//    std::ifstream f(path);
//    REQUIRE(f);
//    size_t last_seek = 0;
//    std::vector<char> part_info;
//    for (size_t i = 0; i < parts; ++i)
//    {
//       f.read(part_info.data(), ava_size);
//       // std::cout << "Was read " << ava_size << " of " << file_size << "\n";
//       f.seekg(-1, std::ios::cur);
//    }
// }
//
// TEST_CASE("ALLOCATOR~READ_LARGE_FILE")
// {
//    std::cout << get_file_size("large.bin");
//    BENCHMARK("Simple")
//    {
//       std::ifstream f("large.bin");
//       std::vector<std::string> vl;
//
//       std::string l;
//       while (std::getline(f, l))
//       {
//       }
//    };
//    BENCHMARK("Smart"){
//       process_file("large.bin");
//    };
// };
