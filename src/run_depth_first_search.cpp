
#include "bound.hpp"
#include "bound_functions/preemptions.hpp"
#include "depth_first_search.hpp"
#include "exploration.hpp"
#include "options.hpp"

#include <iostream>
#include <limits>


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
      
      boost::filesystem::path output_dir;
      try
      {
         output_dir = options.map()["o"].as<std::string>();
      } 
      catch(const std::exception&)
      {
         output_dir = "./statespace_explorer_output" / required.first.filename() / "dfs";
      }
      
      using namespace exploration;
      using dfs_t = Exploration<depth_first_search<bound<bound_functions::Preemptions>>>;

      dfs_t dfs(required.first, required.second, std::numeric_limits<int>::max());
      dfs.run({}, output_dir);

      return 0;
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
