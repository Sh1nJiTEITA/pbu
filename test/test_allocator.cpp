#include <catch2/benchmark/catch_benchmark.hpp>
#include <experimental/filesystem>
#include <string>
#define CATCH_CONFIG_MAIN
#include <string.h>
#include <windows.h>

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
      strcpy_s(ptr_a, sizeof(char) * 20, "12356");
      a.deallocate(ptr_a, sizeof(char) * 10);
      std::clog << std::endl;
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
      std::clog << std::endl;
      REQUIRE(a.allocationCount() == 1);
      REQUIRE(a.constructionCount() == 1);
      REQUIRE(a.deallocationCount() == 1);
   }
   SECTION("ALLOCATOR USING IN STD::VECTOR<INT> NO RESERVE")
   {
      pbu::Allocator<int> a;

      {
         std::cout << "Creating allocator...\n";
         std::vector<int, pbu::Allocator<int>> vec({});

         std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         a = vec.get_allocator();
      }
      REQUIRE(a.allocationCount() == 3);
      REQUIRE(a.constructionCount() == 3);
      REQUIRE(a.deallocationCount() == 3);
      REQUIRE(a.destructionCount() == 3);

      std::clog << std::endl;
   }

   SECTION("ALLOCATOR USING IN STD::VECTOR<INT> WITH RESERVE")
   {
      pbu::Allocator<int> a;
      {
         std::cout << "Creating allocator...\n";
         std::vector<int, pbu::Allocator<int>> vec({});
         vec.reserve(3);

         std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         std::cout << "Adding 10 to std::vector...\n";
         vec.push_back(10);

         a = vec.get_allocator();
      }
      REQUIRE(a.allocationCount() == 1);
      REQUIRE(a.constructionCount() == 1);
      REQUIRE(a.deallocationCount() == 1);
      REQUIRE(a.destructionCount() == 0);

      std::clog << std::endl;
   }

   SECTION("REALLOCATION_TEST")
   {
      // pbu::Allocator<int> a;
      // {
      //    std::cout << "Creating allocator...\n";
      //    std::vector<int, pbu::Allocator<int>> vec({10,20,30});
      //    a = vec.get_allocator();
      //    a.reallocate(vec.data(), 3 * sizeof(int), 2);
      // }
      // REQUIRE(a.allocationCount() == 1);
      // REQUIRE(a.constructionCount() == 1);
      // REQUIRE(a.deallocationCount() == 1);
      // REQUIRE(a.destructionCount() == 0);
      //
      //
      //
      // ~~~~~~~~~~~~~
      // pbu::Allocator<int> allocator;
      //
      // // Выделяем память для 5 целых чисел
      // int* data = allocator.allocate(5);
      // for (int i = 0; i < 5; ++i)
      // {
      //    allocator.construct(&data[i], i);
      // }
      //
      // // Перемещаем данные в новый массив большего размера
      // int* new_data = allocator.reallocate(data, 5, 10);
      // for (int i = 5; i < 10; ++i)
      // {
      //    allocator.construct(&new_data[i], i + 5);
      // }
      //
      // // Вывод данных
      // for (int i = 0; i < 10; ++i)
      // {
      //    std::cout << new_data[i] << " ";
      // }
      // std::cout << std::endl;
      //
      // // Освобождаем память
      // for (int i = 0; i < 10; ++i)
      // {
      //    allocator.destroy(&new_data[i]);
      // }
      // allocator.deallocate(new_data, 10);
      // std::clog << std::endl;
      //

      std::vector<int, pbu::Allocator<int>> vec;

      // Заполнение вектора значениями
      for (int i = 0; i < 5; ++i)
      {
         vec.push_back(i);
      }

      // Вывод текущих значений
      std::cout << "Initial vector values:\n";
      for (const auto& val : vec)
      {
         std::cout << val << " ";
      }
      std::cout << "\n";

      // Увеличение размера вектора
      vec.reserve(10);
      for (int i = 5; i < vec.size(); ++i)
      {
         vec.push_back(i);
      }

      // Вывод новых значений
      std::cout << "Updated vector values:\n";
      for (const auto& val : vec)
      {
         std::cout << val << " ";
      }
      std::cout << "\n";
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
