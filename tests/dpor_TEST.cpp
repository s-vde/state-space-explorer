
#include <depth_first_search.hpp>
#include <dpor.hpp>
#include <exploration.hpp>
#include <sufficient_sets/persistent_set.hpp>

#include <replay.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <assert.h>

#include <limits>


namespace exploration {
namespace test {

//--------------------------------------------------------------------------------------------------

inline void dpor_number_of_executions_test(const boost::filesystem::path& test_program,
                                           const unsigned int expected_nr_executions)
{
   const auto test_programs_dir = boost::filesystem::path(BOOST_PP_STRINGIZE(TEST_PROGRAMS_DIR));
   const auto tests_build_dir = boost::filesystem::path{BOOST_PP_STRINGIZE(TESTS_BUILD_DIR)};
   const auto output_dir = tests_build_dir / "test_data" / "test_programs_instrumented";
   scheduler::instrument((test_programs_dir / test_program).string(), output_dir.string(),
                         "-std=c++14");

   const auto instrumented_program = (output_dir / test_program).replace_extension("");

   using dpor_t = Exploration<depth_first_search<dpor<Persistent>>>;
   dpor_t dpor{instrumented_program.string(), std::numeric_limits<unsigned int>::max()};
   dpor.run();
   
   assert(dpor.statistics().nr_explorations() == expected_nr_executions);
}

//--------------------------------------------------------------------------------------------------

} // end namespace test
} // end namespace exploration
