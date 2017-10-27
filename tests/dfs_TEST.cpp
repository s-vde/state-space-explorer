
#include <test_helpers.hpp>

#include <bound.hpp>
#include <bound_functions/preemptions.hpp>
#include <depth_first_search.hpp>
#include <exploration.hpp>

#include <replay.hpp>

#include <gtest/gtest.h>


namespace exploration {
namespace test {

//--------------------------------------------------------------------------------------------------

struct DfsSmokeTest : public ::testing::TestWithParam<NrExecutionsTestData>
{
   boost::filesystem::path test_output_dir() const
   {
      return detail::test_data_dir / GetParam().test_program.filename() /
             boost::filesystem::path("0" + GetParam().optimization_level) / "dpor";
   }
}; // end struct DfsSmokeTest


TEST_P(DfsSmokeTest, DfsRunsForAtLeastGivenNrOfExecutions)
{
   scheduler::instrument(detail::test_programs_dir / GetParam().test_program,
                         test_output_dir() / "instrumented", GetParam().optimization_level,
                         GetParam().compiler_options);

   const auto instrumented_program =
      test_output_dir() / "instrumented" / GetParam().test_program.stem();

   using dfs_t = Exploration<depth_first_search<bound<bound_functions::Preemptions>>>;
   dfs_t dfs(instrumented_program, GetParam().expected_nr_executions,
             std::numeric_limits<int>::max());
   dfs.set_settings({false, false, scheduler::timeout_t{200}});

   ASSERT_NO_THROW(dfs.run({}, test_output_dir() / "output"));
}


INSTANTIATE_TEST_CASE_P(DfsSmokeTests, DfsSmokeTest,
                        ::testing::Values(NrExecutionsTestData{"benchmarks/readers_nonpreemptive.c",
                                                               "0", "", 2000}));

//--------------------------------------------------------------------------------------------------

} // end namespace test
} // end namespace exploration
