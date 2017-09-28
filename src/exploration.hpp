#pragma once

#include "color_output.hpp"
#include "container_output.hpp"
#include "debug.hpp"
#include "error.hpp"
#include "execution.hpp"
#include "execution_io.hpp"
#include "replay.hpp"
#include "schedule.hpp"
#include "state.hpp"
#include "state_io.hpp"
#include "transition.hpp"
#include "utils_io.hpp"
#include <chrono>
#include <sys/stat.h>

#include <boost/filesystem.hpp>

//--------------------------------------------------------------------------------------------------
/// @file exploration.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace exploration {
namespace {

template <typename mode_t>
boost::filesystem::path output_dir(const scheduler::program_t& program, const mode_t& mode)
{
   const auto filename = boost::filesystem::path(program).filename();
   return boost::filesystem::current_path() / "output" / filename.string() / mode.path();
}


program_model::Execution filter_operations_on_shared_variables(const program_model::Execution& execution)
{
    program_model::Execution filtered_execution;
    std::unordered_map<program_model::Object::ptr_t, program_model::Tids> object_access_counter;
    std::for_each(execution.begin(), execution.end(), [&object_access_counter](const auto& transition)
    {
        const auto& instruction = transition.instr();
        const auto operand = boost::apply_visitor(program_model::get_operand(), instruction);
        if (const auto* memory_location = boost::get<program_model::Object>(&operand))
        {
            const auto tid = boost::apply_visitor(program_model::get_tid(), instruction);
            object_access_counter[memory_location->address()].insert(tid);
        }
    });
    
    std::for_each(execution.begin(), execution.end(), [&object_access_counter](const auto& transition)
    {
        const auto& instruction = transition.instr();
        const auto operand = boost::apply_visitor(program_model::get_operand(), instruction);
        if (const auto* memory_location = boost::get<program_model::Object>(&operand))
        {
            if (object_access_counter[memory_location->address()].size() > 1)
            {
                
            }
        }
    });
    
    return filtered_execution;
}

} // end namespace

//--------------------------------------------------------------------------------------------------


void move_records(unsigned int nr, boost::filesystem::path target);

//--------------------------------------------------------------------------------------------------


struct settings
{
   bool keep_records = false;
   bool keep_logs = false;

}; // end struct settings

//--------------------------------------------------------------------------------------------------


class ExplorationStatistics
{
public:

   ExplorationStatistics();

   unsigned int nr_explorations() const;
   void increase_nr_explorations();

   double time_cpu() const;
   void start_clock();
   void stop_clock();

   void dump(const boost::filesystem::path& filename) const;

private:

   using wall_clock_t = std::chrono::high_resolution_clock;

   unsigned int mNrExplorations;
   double mTimeCpu;
   double mTimeWall;
   std::clock_t mTimeCpuStart;
   std::chrono::time_point<wall_clock_t> mTimeWallStart;

}; // end class ExplorationStatistics

//--------------------------------------------------------------------------------------------------


class ExplorationBase
{
public:
   ExplorationBase(const scheduler::program_t& program, const unsigned int max_nr_explorations);

   ExplorationStatistics statistics() const;

protected:

   using execution = program_model::Execution;
   using transition = typename execution::transition_t;

   scheduler::program_t mProgram;
   scheduler::schedule_t mSchedule;
   unsigned int mMaxNrExplorations;
   execution mExecution;
   ExplorationStatistics mStatistics;
   bool mDone;
   std::ofstream mLogSchedules;
   settings m_settings;

   void run_program();

   static const std::string name;
   static std::string outputname();

}; // end class ExplorationBase

//--------------------------------------------------------------------------------------------------


/// @brief Provides the basic functionality for exploring the state-space of a multithreaded program.
/// @details An instantation of Exploration<Mode> uses the argument to template parameter Mode for the 
/// (on-the-fly) generation of a set of schedules/thread-interleavings (i.e. an exploration tree) 
/// under which to explore the program. Thereto, Mode may maintain additional state.

template <typename Mode>
class Exploration : public ExplorationBase
{
public:

   template <typename... Args>
   explicit Exploration(const scheduler::program_t& program, const unsigned int max_nr_explorations,
                        Args... args)
   : ExplorationBase(program, max_nr_explorations)
   , mMode(mExecution, std::forward<Args>(args)...)
   , m_output_dir(output_dir(program, mMode))
   {
   }

   /// @brief Sets up the exploration, runs the program under an initial schedule and traverses the 
   /// exploration tree generated by Mode (on-the-fly). For every leaf it visits, it runs the input 
   /// program mProgram under the schedule corresponding to that leaf.

   void run(const scheduler::schedule_t& s = {})
   {
      if (boost::filesystem::exists(m_output_dir))
      {
         boost::filesystem::remove_all(m_output_dir);
      }
      boost::filesystem::create_directories(m_output_dir);

      // Open scheduler log file here once, opening in append mode is costly
      mLogSchedules.open(m_output_dir.string() + "/schedules.txt");
      scheduler::write_settings(mMode.scheduler_settings());
      mSchedule = s;
      int from = 1;
      mStatistics.start_clock();
      while (!mDone && mStatistics.nr_explorations() < mMaxNrExplorations)
      {
         run_program();
         mMode.reset();
         if (mStatistics.nr_explorations() > 0 || mMode.check_valid(mExecution.contains_locks()))
         {
            update_state(from);
            if ((mSchedule = mMode.new_schedule(mExecution, mSchedule)).empty())
            {
               mDone = true;
               break;
            }
            from = mSchedule.size();
         }
         else
         {
            ERROR(full_name(), "Invalid input program");
            break;
         }
      }
      close();
   }

private:

   Mode mMode;
   boost::filesystem::path m_output_dir;

   /// @brief Runs the input program mProgram under the current schedule, updates mExecution and 
   /// mSchedule according to the performed Execution, calls mMode to update its state, and updates 
   /// log files.

   void run_program()
   {
      mMode.write_scheduler_files();
      ExplorationBase::run_program();
   }

   void update_statistics()
   {
      mStatistics.increase_nr_explorations();
      mMode.update_statistics(mExecution);
   }

   /// @brief Loops through the Transitions of mExecution and lets Mode restore and update its 
   /// internal state.

   void update_state(unsigned int from)
   {
      update_statistics();
      mSchedule = scheduler::schedule(mExecution);
      mLogSchedules << mSchedule << std::endl;
      if (m_settings.keep_records)
      {
         move_records(mStatistics.nr_explorations(), m_output_dir);
      }
      DEBUGFNL(outputname(), "UPDATE_STATE", "from=" << from, "");
      for (auto& t : mExecution)
      {
         if (t.index() < from)
         {
            mMode.restore_state(t);
         }
         else
         {
            mMode.update_state(mExecution, t);
         }
      }
      if (m_settings.keep_logs)
      {
         dump_branch(mStatistics.nr_explorations());
      }
   }

   void close()
   {
      mStatistics.stop_clock();
      const boost::filesystem::path statistics_file = m_output_dir / "statistics.txt";
      mStatistics.dump(statistics_file);
      mLogSchedules.close();
      mMode.close(statistics_file.string());
   }

   template <typename OutStream>
   void dump_state(OutStream& os, const transition& t) const
   {
      os << t.pre() << " ";
      mMode.dump_state(os, t.index() - 1);
      os << std::endl;
   }

   void dump_branch(const unsigned int nr) const
   {
      const boost::filesystem::path exploration_filename("exploration" + std::to_string(nr) + ".txt"); 
      const boost::filesystem::path exploration_file = m_output_dir / exploration_filename;
      std::ofstream ofs(exploration_file.string());
      for (const auto& t : mExecution)
      {
         dump_state(ofs, t);
         ofs << to_short_string(t) << std::endl;
      }
      ofs.close();
   }

   void dump_state(const unsigned int i) const { mMode.dump_state(i); }

   static std::string full_name()
   {
      std::string full_name = name;
      full_name += "<";
      full_name += Mode::full_name();
      full_name += ">";
      return full_name;
   }
   
}; // end class template Exploration<Mode>

//--------------------------------------------------------------------------------------------------

} // end namespace exploration
