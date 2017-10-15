
#include <replay.hpp>

#include <boost/program_options.hpp>


namespace state_space_explorer {

class options
{
public:
   options()
   : m_options_desc("State-Space Exploration Options")
   {
      m_options_desc.add_options()("h", "help")(
         "bound", boost::program_options::value<unsigned int>()->default_value(0),
         "the bound to be used with a Bounded Search based exploration")(
         "bound-function",
         boost::program_options::value<std::string>()->default_value("preemptions"),
         "the bound function to be used with a Bounded Search based exploration "
         "(values:preemptions)")(
         "i", boost::program_options::value<std::string>(),
         "the system under test, instrumented with the Record-Replay compiler pass")(
         "max", boost::program_options::value<unsigned int>(),
         "the maximum number of executions explored")(
         "o", boost::program_options::value<std::string>(),
         "the directory where output files are dumped")(
         "sufficient-set",
         boost::program_options::value<std::string>()->default_value("persistent"),
         "the sufficient set implementation to be used with DPOR based exploration (values: "
         "persistent)");
   }

   void parse(int argc, char* argv[])
   {
      boost::program_options::store(
         boost::program_options::parse_command_line(argc, argv, m_options_desc), m_options_map);
      boost::program_options::notify(m_options_map);
   }

   const boost::program_options::variables_map& map() const { return m_options_map; }

   friend std::ostream& operator<<(std::ostream& os, const options& opt);

private:
   boost::program_options::options_description m_options_desc;
   boost::program_options::variables_map m_options_map;

}; // end class options

//----------------------------------------------------------------------------------------------------------------------


inline std::ostream& operator<<(std::ostream& os, const options& opt)
{
   os << opt.m_options_desc;
   return os;
}

//----------------------------------------------------------------------------------------------------------------------


std::pair<scheduler::program_t, unsigned int> get_required_options(const options& opt)
{
   try
   {
      const boost::filesystem::path sut{opt.map()["i"].as<std::string>()};
      const unsigned int max = opt.map()["max"].as<unsigned int>();
      return {sut.string(), max};
   }
   catch (const boost::bad_any_cast&)
   {
      throw std::invalid_argument(
         "Please run State-Space Exploration with required options --i and --max");
   }
}

//----------------------------------------------------------------------------------------------------------------------

} // end namespace state_space_explorer
