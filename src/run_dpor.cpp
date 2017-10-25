
#include "dependence.hpp"
#include "depth_first_search.hpp"
#include "dpor.hpp"
#include "exploration.hpp"
#include "happens_before.hpp"
#include "options.hpp"
#include "sufficient_sets/bound_persistent_set.hpp"
#include "sufficient_sets/persistent_set.hpp"
#include "sufficient_sets/source_set.hpp"

#include <iostream>


using namespace exploration;
template <typename sufficient_set_t>
using dpor_t = Exploration<depth_first_search<dpor<sufficient_set_t>>>;


int main(int argc, char* argv[])
{
   state_space_explorer::options options;
   try
   {
      options.parse(argc, argv);
      if (options.map().count("h"))
      {
         std::cout << options << "\n";
         return 0;
      }

      const auto required = state_space_explorer::get_required_options(options);

      const std::string optimization_level = options.map()["opt"].as<std::string>();
      const std::string compiler_options = options.map()["c"].as<std::string>();

      const std::string& sufficient_set = options.map()["sufficient-set"].as<std::string>();

      boost::filesystem::path output_dir;
      try
      {
         output_dir = options.map()["o"].as<std::string>();
      }
      catch (const std::exception&)
      {
         output_dir = "./statespace_explorer_output" / required.first.filename() / "dpor";
      }

      if (sufficient_set == "persistent")
      {
         dpor_t<Persistent>(required.first, required.second)
            .run({}, optimization_level, compiler_options, output_dir);
         return 0;
      }
      else
      {
         std::cout << "mode has to be in { persistent }\n";
         return 1;
      }
   }
   catch (const std::invalid_argument& ex)
   {
      std::cout << ex.what() << "\n\n" << options << "\n";
   }
   catch (const boost::program_options::invalid_option_value&)
   {
      std::cout << "Please run State-Space Explorer with valid options\n" << options << "\n";
      return 1;
   }
}
