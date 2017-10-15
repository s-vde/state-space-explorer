
#include "bound.hpp"
#include "bound_functions/preemptions.hpp"
#include "depth_first_search.hpp"
#include "dpor.hpp"
#include "exploration.hpp"
#include "options.hpp"

namespace exploration {
template <typename bound_function_t>
using bounded_search = Exploration<depth_first_search<bound<bound_function_t>>>;
}

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

      const std::string bound_function = options.map()["bound-function"].as<std::string>();
      const unsigned int bound = options.map()["bound"].as<unsigned int>();
      
      boost::filesystem::path output_dir;
      try
      {
         output_dir = options.map()["o"].as<std::string>();
      } 
      catch(const std::exception&)
      {
         output_dir = "./statespace_explorer_output" / required.first.filename() / "bounded";
      }

      if (bound_function == "preemptions")
      {
         exploration::bounded_search<bound_functions::Preemptions> bs(required.first,
                                                                      required.second, bound);
         bs.run({}, output_dir.string() + "-preemptions-" + std::to_string(bound));
         return 0;
      }
      else
      {
         std::cout << "bound_function has to be in { preemptions }\n";
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
