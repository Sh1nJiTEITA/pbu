#include <catch2/catch_test_macros.hpp>

#include <allocator.hpp>
#include <iostream>

TEST_CASE("ALLOCATION_INFO") { 
   SECTION("STD::COUT") { 
      pbu::PresentAllocationInfo info(20); 
      // info[0] = 'c';
      // info[1] = 'h';
      // info[2] = 'd';
      // info[3] = '\0';

      info << "HI" << "Iam sex machine" << info.setIntBase(10) << " buy...";
      // info.find_null_term(50);
      std::cout << "message: " << info << "\n";
   }

}
