
#include "exploration.hpp"


namespace exploration {

//--------------------------------------------------------------------------------------------------

void move_records(unsigned int nr, boost::filesystem::path target)
{
   boost::filesystem::path source("record.txt");
   target /= "record";
   target += std::to_string(nr);
   target += ".txt";
   boost::filesystem::rename(source, target);
}

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

void ExplorationBase::run_program()
{
   DEBUGF(outputname(), "run_program", "", "under schedule " << mSchedule << "\n");
   scheduler::run_under_schedule(mProgram, mSchedule);
   mExecution = program_model::Execution();
   if (!utils::io::read_from_file("record.txt", mExecution))
   {
      ERROR(full_name(), "run_program"); /* #todo Some error handling */
   }
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
