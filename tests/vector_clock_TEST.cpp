
#include <vector_clock.hpp>

#include <assert.h>


namespace exploration {
namespace test {

//--------------------------------------------------------------------------------------------------

void vector_clock_copy_constructor_from_small_other_test()
{
   auto other = VectorClock{3};
   other[0] = 1;
   other[1] = 2;
   other[2] = 3;
   
   auto clock = VectorClock{other, 5};
   
   assert(clock.size() == 5);
   assert(clock[0] == 1 && clock[1] == 2 && clock[2] == 3 && clock[3] == 0 && clock[4] == 0);
}

//--------------------------------------------------------------------------------------------------

void vector_clock_copy_constructor_from_large_other_test()
{
   auto other = VectorClock{3};
   other[0] = 1;
   other[1] = 2;
   other[2] = 3;
   
   auto clock = VectorClock{other, 2};
   
   assert(clock.size() == 2);
   assert(clock[0] == 1 && clock[1] == 2);
}

//--------------------------------------------------------------------------------------------------

void vector_clock_copy_constructor_from_matching_other_test()
{
   auto other = VectorClock{3};
   other[0] = 1;
   other[1] = 2;
   other[2] = 3;
   
   auto clock = VectorClock{other, 3};
   
   assert(clock.size() == 3);
   assert(clock[0] == 1 && clock[1] == 2 && clock[2] == 3);
}

} // end namespace test
} // end namespace program_model


int main()
{
   exploration::test::vector_clock_copy_constructor_from_small_other_test();
   exploration::test::vector_clock_copy_constructor_from_large_other_test();
   exploration::test::vector_clock_copy_constructor_from_matching_other_test();
   return 0;
}
