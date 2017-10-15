#pragma once

// SCHEDULER
#include "schedule.hpp"
#include "scheduler_settings.hpp"

// PROGRAM_MODEL
#include "execution.hpp"
#include "state.hpp"
#include "transition_io.hpp"

// UTILS
#include "color_output.hpp"
#include "container_output.hpp"
#include "debug.hpp"

//--------------------------------------------------------------------------------------------------
/// @file depth_first_search.hpp
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------

namespace exploration
{
//--------------------------------------------------------------------------------------------------

class dfs_state
{
public:
		
   dfs_state() = default;
        
   void add_to_done(const program_model::Thread::tid_t& tid);
   program_model::Tids undone(const program_model::Tids& T) const;
        
private:
        
   program_model::Tids mDone;
        
   friend std::ostream& operator<<(std::ostream&, const dfs_state&);
        
}; // end class dfs_state

//--------------------------------------------------------------------------------------------------
    
std::ostream& operator<<(std::ostream&, const dfs_state&);

//--------------------------------------------------------------------------------------------------
  
/// @brief Implements a depth-first traversal of the state-space, by treating the Execution object 
/// as a stack.
/// @details The argument to template parameter Reduction is used to (potentially) reduce the 
/// exploration tree by restricting the pool of thread ids to explore in each internal node.

template<typename reduction_t>
class depth_first_search
{
public:
        
   using execution_t = program_model::Execution;
   using transition_t = typename execution_t::transition_t;
        
	template<typename ... Args>
	explicit depth_first_search(const execution_t& E, Args ... args)
   : mState({ dfs_state() })
	, mReduction(E, std::forward<Args>(args) ...) 
   { 
   }
		
   static std::string name;
           
   /// @brief Wrapper of mReduction.scheduler_settings.
   scheduler::SchedulerSettings scheduler_settings();
        
   /// @brief Wrapper of mReduction.write_scheduler_files.
   void write_scheduler_files();
		
	/// @brief Wrapper of mReduction.check_valid.
	bool check_valid(const bool contains_locks) const;
   
   //-----------------------------------------------------------------------------------------------
   
   ///@brief Wrapper of mReduction.update_statistics.

   inline void update_statistics(const execution_t& execution)
   {
      mReduction.update_statistics(execution);
   }
   
   //-----------------------------------------------------------------------------------------------
        
   /// @brief Wrapper of mReduction.restore_state, checking preconditions.

   inline void restore_state(const transition_t& transition)
   {
      /// @pre mState.size() > t.index()
      assert(mState.size() > transition.index());
      mReduction.restore_state(transition);
   }
   
   //-----------------------------------------------------------------------------------------------
        
   /// @brief Adds a new dfs_state at the back of mState and calls mReduction.update_state.

   void update_state(const execution_t& execution, const transition_t& transition)
   {
      /// @pre mState.size() == t.index()
      assert(mState.size() == transition.index());
      mState.emplace_back();
      mReduction.update_state(execution, transition);
      /// @post mState.size() == t.index()+1
      assert(mState.size() == transition.index()+1);
   }
   
   //-----------------------------------------------------------------------------------------------
        
   /// @brief Adds t.instr to mState[t.index-1].pre.done and calls 
   /// mReduction.update_after_exploration.

   void update_after_exploration(transition_t& transition)
   {
      /// @pre mState.size() == t.index()+1 (i.e. did not yet pop_back())
      assert(mState.size() == transition.index()+1);
      DEBUGF(outputname(), "update_after_exploration", to_short_string(transition), "\n");
		const auto tid = boost::apply_visitor(program_model::get_tid(), transition.instr());
      DEBUG("\t" << to_string_pre_state(transition) << ".done.add(" << tid << ")\n");
      mState[transition.index()-1].add_to_done(tid); // t.pre
      mReduction.update_after_exploration(transition);
   }
   
   //-----------------------------------------------------------------------------------------------
        
   /// @brief Wrapper of mReduction.reset.
   void reset();
        
   /// @brief Backtracks the states along the current execution until it encounters i such that 
   /// exists tid in (mMode.pool(i) \ mState[i-1].done()). Then it returns the new schedule 
   /// pre(E, i).tid.
   
   //-----------------------------------------------------------------------------------------------

   scheduler::schedule_t new_schedule(execution_t& execution, scheduler::schedule_t& schedule)
   {
      while (!execution.empty()) 
      {
         update_after_exploration(execution.last());
         pop_back(execution, schedule);
			DEBUG(execution.final() << "\n");
         program_model::Tids pool_undone = mState.back().undone(mReduction.pool(execution));
         if (!pool_undone.empty()) 
         {
            const auto next = mReduction.select_from_pool(execution, pool_undone);
            if (next >= 0) 
            {
               DEBUG("\tnext= " << next << "\n");
               /// @invariant E.final().is_enabled(mReduction.select_from_pool(E, PoolUndone))
               /// (i.e. mReduction.pool(E) is a subset of E.final().enabled())
               assert(execution.final().is_enabled(next));
               schedule.push_back(next);
               break;
            } 
            else 
            {
               DEBUG("\tnext= none");
            }
         }
      }
      DEBUG("\tnew schedule= " << schedule << "\n");
      return schedule;
   }
   
   //-----------------------------------------------------------------------------------------------
        
   /// @brief Wrapper of mReduction.close.
   void close(const std::string& statistics) const;
        
   void dump_state(std::ostream& os, const std::size_t index) const;
        
   static std::string full_name();
        
private:
   
   //-----------------------------------------------------------------------------------------------
        
   void pop_back(execution_t& execution, scheduler::schedule_t& schedule)
   {
       execution.pop_last();
       mState.pop_back();
       mReduction.pop_back();
       schedule.pop_back();
   }
   
   //-----------------------------------------------------------------------------------------------
   
   static std::string outputname();
   
   std::vector<dfs_state> mState;
   reduction_t mReduction;
        
}; // end class template depth_first_search<Reduction>

//--------------------------------------------------------------------------------------------------

template <typename reduction_t>
std::string depth_first_search<reduction_t>::name = "depth_first_search";

//--------------------------------------------------------------------------------------------------

template <typename reduction_t>
std::string depth_first_search<reduction_t>::full_name()
{
    std::string full_name = name;
    full_name += "<";
    full_name += reduction_t::full_name();
    full_name += ">";
    return full_name;
}

//--------------------------------------------------------------------------------------------------

template <typename reduction_t>
std::string depth_first_search<reduction_t>::outputname()
{
    return text_color(name, utils::io::Color::BLUE);
}

//--------------------------------------------------------------------------------------------------

template <typename reduction_t>
inline scheduler::SchedulerSettings depth_first_search<reduction_t>::scheduler_settings()
{
   return mReduction.scheduler_settings();
}

//--------------------------------------------------------------------------------------------------

template <typename reduction_t>
inline void depth_first_search<reduction_t>::write_scheduler_files()
{
   mReduction.write_scheduler_files();
}

//--------------------------------------------------------------------------------------------------

template <typename reduction_t>
inline bool depth_first_search<reduction_t>::check_valid(const bool contains_locks) const
{
   return mReduction.check_valid(contains_locks);
}

//--------------------------------------------------------------------------------------------------

template <typename reduction_t>
inline void depth_first_search<reduction_t>::reset()
{
    mReduction.reset();
}

//--------------------------------------------------------------------------------------------------

template <typename reduction_t>
inline void depth_first_search<reduction_t>::close(const std::string& statistics) const
{
    mReduction.close(statistics);
}

//--------------------------------------------------------------------------------------------------

template <typename reduction_t>
inline void depth_first_search<reduction_t>::dump_state(std::ostream& os, 
   const std::size_t index) const
{
   os << mState[index] << " ";
   mReduction.dump_state(os, index);
}

//--------------------------------------------------------------------------------------------------
} // end namespace exploration
