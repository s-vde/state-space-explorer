
#include <test_helpers.hpp>

#include <depth_first_search.hpp>
#include <dpor.hpp>
#include <exploration.hpp>
#include <sufficient_sets/persistent_set.hpp>

#include <replay.hpp>

#include <gtest/gtest.h>


namespace exploration {
namespace test {

//--------------------------------------------------------------------------------------------------

struct DporNrExecutionsTest : public ::testing::TestWithParam<NrExecutionsTestData>
{
   boost::filesystem::path test_output_dir() const
   {
      return detail::test_data_dir / GetParam().test_program.filename() /
             boost::filesystem::path("0" + GetParam().optimization_level) / "dpor";
   }
}; // end struct InstrumentedProgramRunTest

TEST_P(DporNrExecutionsTest, NrExecutionsIsAsExpected)
{
   using dpor_t = Exploration<depth_first_search<dpor<Persistent>>>;
   dpor_t dpor{detail::test_programs_dir / GetParam().test_program,
               GetParam().expected_nr_executions + 1};
   dpor.run({}, GetParam().optimization_level, GetParam().compiler_options,
            test_output_dir() / "output");

   ASSERT_EQ(dpor.statistics().nr_explorations(), GetParam().expected_nr_executions);
}

INSTANTIATE_TEST_CASE_P(
   DporNrExecutionsTests, DporNrExecutionsTest,
   ::testing::Values(NrExecutionsTestData{"shared_memory_access_non_concurrent.cpp", "0",
                                              "-std=c++14", 1},
                     // @cite Flanagan:2005:DPR:1040305.1040315 page 8
                     NrExecutionsTestData{
                        "../../libs/record-replay/tests/test_programs/real_world/filesystem.c", "0",
                        "-DNR_THREADS=13", 1},
                     // @cite Abdulla:2014:ODP:2535838.2535845
                     NrExecutionsTestData{"benchmarks/readers_nonpreemptive.c", "0", "", 5}));

//--------------------------------------------------------------------------------------------------

} // end namespace test
} // end namespace exploration
