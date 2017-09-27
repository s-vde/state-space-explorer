
#include "dpor_TEST.cpp"
#include "vector_clock_TEST.cpp"


int main()
{
   using namespace exploration::test;
   vector_clock_copy_constructor_from_small_other_test();
   vector_clock_copy_constructor_from_large_other_test();
   vector_clock_copy_constructor_from_matching_other_test();

   boost::filesystem::path test_programs_dir = BOOST_PP_STRINGIZE(TEST_PROGRAMS_DIR);
   dpor_number_of_executions_test("shared_memory_access_non_concurrent.cpp", 1);

   return 0;
}
