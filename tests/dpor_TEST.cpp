
#include <depth_first_search.hpp>
#include <dpor.hpp>
#include <exploration.hpp>
#include <sufficient_sets/persistent_set.hpp>

#include <replay.hpp>

#include <gtest/gtest.h>

#include <boost/filesystem/path.hpp>
#include <boost/preprocessor/stringize.hpp>


namespace exploration {
namespace test {
namespace detail {

static const auto test_programs_dir =
   boost::filesystem::path(BOOST_PP_STRINGIZE(TEST_PROGRAMS_DIR));
static const auto tests_build_dir = boost::filesystem::path{BOOST_PP_STRINGIZE(TESTS_BUILD_DIR)};
static const auto test_data_dir = tests_build_dir / "test_data";

} // end namespace detail

//--------------------------------------------------------------------------------------------------

struct DporNrExecutionsTestData
{
   boost::filesystem::path test_program;
   std::string optimization_level;
   std::string compiler_options;
   unsigned int expected_nr_executions;

}; // end struct NrExplorationsTestData

struct DporNrExecutionsTest : public ::testing::TestWithParam<DporNrExecutionsTestData>
{
   boost::filesystem::path test_output_dir() const
   {
      return detail::test_data_dir / GetParam().test_program.filename() /
             boost::filesystem::path("0" + GetParam().optimization_level) / "dpor";
   }
}; // end struct InstrumentedProgramRunTest

TEST_P(DporNrExecutionsTest, NrExecutionsIsAsExpected)
{
   scheduler::instrument(detail::test_programs_dir / GetParam().test_program,
                         test_output_dir() / "instrumented", GetParam().optimization_level,
                         GetParam().compiler_options);

   const auto instrumented_program =
      test_output_dir() / "instrumented" / GetParam().test_program.stem();

   using dpor_t = Exploration<depth_first_search<dpor<Persistent>>>;
   dpor_t dpor{instrumented_program, GetParam().expected_nr_executions + 1};
   dpor.run({}, test_output_dir() / "output");

   ASSERT_EQ(dpor.statistics().nr_explorations(), GetParam().expected_nr_executions);
}

INSTANTIATE_TEST_CASE_P(
   DporNrExecutionsTests, DporNrExecutionsTest,
   ::testing::Values(DporNrExecutionsTestData{"shared_memory_access_non_concurrent.cpp", "0",
                                              "-std=c++14", 1},
                     // @cite Flanagan:2005:DPR:1040305.1040315 page 8
                     DporNrExecutionsTestData{
                        "../../libs/record-replay/tests/test_programs/real_world/filesystem.c", "0",
                        "-DNR_THREADS=13", 1},
                     // @cite Abdulla:2014:ODP:2535838.2535845
                     DporNrExecutionsTestData{"benchmarks/readers_nonpreemptive.c", "0", "", 5}));

//--------------------------------------------------------------------------------------------------

} // end namespace test
} // end namespace exploration
