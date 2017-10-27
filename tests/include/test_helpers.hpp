#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/preprocessor/stringize.hpp>


namespace exploration {
namespace test {
namespace detail {
   
//--------------------------------------------------------------------------------------------------   

static const auto test_programs_dir =
   boost::filesystem::path(BOOST_PP_STRINGIZE(TEST_PROGRAMS_DIR));
static const auto tests_build_dir = boost::filesystem::path{BOOST_PP_STRINGIZE(TESTS_BUILD_DIR)};
static const auto test_data_dir = tests_build_dir / "test_data";

} // end namespace detail

//--------------------------------------------------------------------------------------------------


struct NrExecutionsTestData
{
   boost::filesystem::path test_program;
   std::string optimization_level;
   std::string compiler_options;
   unsigned int expected_nr_executions;

}; // end struct NrExecutionTestData

//--------------------------------------------------------------------------------------------------

} // end namespace test
} // end namespace exploration
