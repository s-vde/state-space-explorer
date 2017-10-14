
#include <vector_clock.hpp>

#include <gtest/gtest.h>


namespace exploration {
namespace test {

//--------------------------------------------------------------------------------------------------

TEST(VectorClockCopyFromSmallOtherTest, CopyFilledWithZeros)
{
   auto other = VectorClock{3};
   other[0] = 1;
   other[1] = 2;
   other[2] = 3;
   
   auto clock = VectorClock{other, 5};
   
   ASSERT_EQ(clock.size(),5);
   ASSERT_EQ(clock[0], 1);
   ASSERT_EQ(clock[1], 2);
   ASSERT_EQ(clock[2], 3);
   ASSERT_EQ(clock[3], 0);
   ASSERT_EQ(clock[4], 0);
}

//--------------------------------------------------------------------------------------------------

TEST(VectorClockCopyFromLargeOtherTest, CopyIsSubsequence)
{
   auto other = VectorClock{3};
   other[0] = 1;
   other[1] = 2;
   other[2] = 3;
   
   auto clock = VectorClock{other, 2};
   
   ASSERT_EQ(clock.size(), 2);
   ASSERT_EQ(clock[0], 1);
   ASSERT_EQ(clock[1], 2);
}

//--------------------------------------------------------------------------------------------------

TEST(VectorClockCopyFromMatchingOtherTest, CopyIsEqual)
{
   auto other = VectorClock{3};
   other[0] = 1;
   other[1] = 2;
   other[2] = 3;
   
   auto clock = VectorClock{other, 3};
   
   ASSERT_EQ(clock.size(), 3);
   ASSERT_EQ(clock[0], 1);
   ASSERT_EQ(clock[1], 2);
   ASSERT_EQ(clock[2], 3);
}

} // end namespace test
} // end namespace exploration
