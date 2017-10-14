
#include "dpor_TEST.cpp"
#include "vector_clock_TEST.cpp"


int main()
{
   using namespace exploration::test;
   vector_clock_copy_constructor_from_small_other_test();
   vector_clock_copy_constructor_from_large_other_test();
   vector_clock_copy_constructor_from_matching_other_test();

   boost::filesystem::path test_programs_dir = BOOST_PP_STRINGIZE(TEST_PROGRAMS_DIR);
   dpor_number_of_executions_test("shared_memory_access_non_concurrent.cpp", "0", "-std=c++14", 1);
   /// @cite Flanagan:2005:DPR:1040305.1040315 page 8
   dpor_number_of_executions_test("../../libs/record-replay/tests/test_programs/real_world/filesystem.c", "0", "-DNR_THREADS=13", 1);
   /// @cite Abdulla:2014:ODP:2535838.2535845
   dpor_number_of_executions_test("benchmarks/readers_nonpreemptive.c", "0", "", 5);
   
   
   return 0;
}
