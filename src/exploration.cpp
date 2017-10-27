
#include "exploration.hpp"


namespace exploration {

//--------------------------------------------------------------------------------------------------

namespace detail {

program_model::Execution replay(const scheduler::program_t& program,
                                const scheduler::schedule_t& schedule,
                                const boost::filesystem::path& records_dir,
                                const boost::optional<scheduler::timeout_t>& timeout)
{
   DEBUGF("StateSpaceExplorer", "replay", program, "under schedule " << schedule << "\n");
   scheduler::run_under_schedule(program, schedule, timeout, records_dir);

   program_model::Execution execution;
   if (!utils::io::read_from_file((records_dir / "record.txt").string(), execution))
      throw std::runtime_error("Could not parse " + (records_dir / "record.txt").string());
   return execution;
}

//--------------------------------------------------------------------------------------------------

void move_records(unsigned int nr, const boost::filesystem::path& source_dir)
{
   // record.txt
   {
      const boost::filesystem::path target_file = "record_" + std::to_string(nr) + ".txt";
      boost::filesystem::rename(source_dir / "record.txt", source_dir / target_file);
   }
   // record_short.txt
   {
      const boost::filesystem::path target_file = "record_short_" + std::to_string(nr) + ".txt";
      boost::filesystem::rename(source_dir / "record_short.txt", source_dir / target_file);
   }
}

} // end namespace detail

//--------------------------------------------------------------------------------------------------

ExplorationStatistics::ExplorationStatistics()
: mNrExplorations(0)
, mTimeCpu(0.0)
, mTimeWall(0.0)
, mTimeCpuStart()
, mTimeWallStart()
{
}

//--------------------------------------------------------------------------------------------------

unsigned int ExplorationStatistics::nr_explorations() const
{
   return mNrExplorations;
}

//--------------------------------------------------------------------------------------------------

void ExplorationStatistics::increase_nr_explorations()
{
   ++mNrExplorations;
}

//--------------------------------------------------------------------------------------------------

double ExplorationStatistics::time_cpu() const
{
   return mTimeCpu;
}

//--------------------------------------------------------------------------------------------------

void ExplorationStatistics::start_clock()
{
   mTimeCpuStart = std::clock();
   mTimeWallStart = wall_clock_t::now();
}

//--------------------------------------------------------------------------------------------------

void ExplorationStatistics::stop_clock()
{
   mTimeCpu = ((std::clock() - mTimeCpuStart) / (double)CLOCKS_PER_SEC);
   mTimeWall = std::chrono::duration<double>(wall_clock_t::now() - mTimeWallStart).count();
}

//--------------------------------------------------------------------------------------------------

void ExplorationStatistics::dump(const boost::filesystem::path& filename) const
{
   std::ofstream ofs(filename.string(), std::ofstream::app);
   ofs << "nr_explorations\t" << mNrExplorations << std::endl
       << "cpu_time(s)\t" << mTimeCpu << std::endl
       << "wall_time(s)\t" << mTimeWall << std::endl;
}

//--------------------------------------------------------------------------------------------------


ExplorationBase::ExplorationBase(const scheduler::program_t& program,
                                 const unsigned int max_nr_explorations)
: mProgram(program)
, mSchedule()
, mMaxNrExplorations(max_nr_explorations)
, mExecution()
, mStatistics()
, mDone(false)
, mLogSchedules()
{
}

//--------------------------------------------------------------------------------------------------

ExplorationStatistics ExplorationBase::statistics() const
{
   return mStatistics;
}

//--------------------------------------------------------------------------------------------------

const std::string ExplorationBase::name = "Exploration";

//--------------------------------------------------------------------------------------------------

std::string ExplorationBase::outputname()
{
   return text_color(name, utils::io::Color::MAGENTA);
}

//--------------------------------------------------------------------------------------------------

} // end namespace exploration
