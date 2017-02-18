#pragma once

// EXPLORATION
#include "sufficient_sets/sufficient_set.hpp"

// SCHEDULER
#include "scheduler_settings.hpp"

// PROGRAM_MODEL
#include "execution.hpp"
#include "transition_io.hpp"

//-----------------------------------------------------------------------------------------------100
/// @file dpor.hpp
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------

namespace exploration
{
//--------------------------------------------------------------------------------------------------

class dpor_statistics
{
public:
        
   dpor_statistics();
		
   unsigned int nr_sleepset_blocked() const;
   void increase_nr_sleepset_blocked();
        
   // void dump(const std::string& filename) const;

private:
        
   unsigned int mNrSleepSetBlocked;
        
}; // end class dpor_statistics

//--------------------------------------------------------------------------------------------------
    
std::ostream& operator<<(std::ostream&, const dpor_statistics&);

//--------------------------------------------------------------------------------------------------
	
class dpor_base
{
public:
		
	using execution_t = program_model::Execution;
   using transition_t = typename execution_t::transition_t;
		
	explicit dpor_base(const execution_t&);
		
	/// @brief Sets the selection strategy to SleepSets.
	static scheduler::SchedulerSettings scheduler_settings();
		
	/// @brief Writes the sleepset of mState.back() to file schedules/sleepset.txt.
	void write_scheduler_files() const;
		
	/// @brief Wrapper of mHB.reset.
	void reset();
		
	/// @brief Wrapper of mHB.restore, checking preconditions.
	void restore_state(const transition_t& transition);
		
	/// @brief Calls mStatistics.increase_nr_sleepset_blocked iff E.status is BLOCKED.
	void update_statistics(const execution_t& execution);
		
	void close(const std::string& statistics_file) const;
		
protected:
	
	/// @details Adds transition.instr.tid() to transition.pre.backtrack(), propagates the
	/// SleepSet from transition.post to transition.pre, and sets transition.post.clock
	/// @note It is important for SleepSet propagation that this function is only called on *new* 
	/// Transitions (i.e. index >= from), because otherwise SleepSets are overridden during 
	/// propagation.
	void update_state(const execution_t& execution, const transition_t& transition);
	
	SufficientSet& pre_of_transition(const std::size_t index);
	
	static const std::string name;
	static std::string outputname();
		
	std::vector<SufficientSet> mState;
	HappensBefore<Dependence> mHB;
	dpor_statistics mStatistics;
		
}; // end class dpor_base

//--------------------------------------------------------------------------------------------------
	
/// @brief Implements partial-order reduction based on sufficient_set_t, to be used with DFS.
/// @details It adds a happens-before relation on an program_model::Execution object. 
/// The argument to template paramter sufficient_set_t determines - based on the happens-before 
/// relation - the set of program_model::Thread::tid_t's that is sufficient to explore from each 
/// State in the exploration tree to obtain the level of completeness associated with it.

template<typename sufficient_set_t> class dpor : public dpor_base
{
public:
        
	//-----------------------------------------------------------------------------------------------
	
   template<typename ... Args>
   explicit dpor(const execution_t& execution, Args ... args)
   : dpor_base(execution)
	, mSufficientSet(std::forward<Args>(args) ...) 
	{ 
	}
	
	//-----------------------------------------------------------------------------------------------
       
	/// @brief Wrapper for mSufficientSet::check_valid.

	bool check_valid(const bool contains_locks) const;
        
   std::string path() const;
	
	//-----------------------------------------------------------------------------------------------
		
   /// @details Calls dpor_base::update_state and uses the functionality of  sufficient_set_t to 
	/// add backtrack points.
   void update_state(const execution_t& execution, const transition_t& transition)
   {
		dpor_base::update_state(execution, transition);
		mSufficientSet.update_state(execution, transition);
      BacktrackPoints Points = mSufficientSet.backtrack_points(execution, transition.index(), mHB);
      DEBUG("\tBacktrackPoints = " << Points << "\n");
      for (const auto& point : Points) 
		{
         mSufficientSet.add_backtrack_point(execution, transition.index(), mState, mHB, point);
         DEBUG("\t" << to_string_pre(point.index) 
					<< ".backtrack = " << pre_of_transition(point.index).backtrack() << "\n");
      }
   }
	
	//-----------------------------------------------------------------------------------------------
        
   /// @details Adds transition.instr.tid to transition.pre.sleepset and calls 
	/// sufficient_set_t::update_after_exploration.

   void update_after_exploration(transition_t& transition)
   {
      /// @pre mState.size() == t.index()+1 (i.e. did not yet pop_back())
      assert(mState.size() == transition.index()+1);
      mState[transition.index()-1].sleepset().add(transition.instr().tid());
      DEBUGF(outputname(), "update_after_exploration", to_string_pre_state(transition),
             to_string_pre_state(transition) << ".sleep += { " << transition.instr().tid() << " } = "
             << mState.back().sleepset());
      mSufficientSet.update_after_exploration(transition, mState[transition.index()-1]);
   }
	
	//-----------------------------------------------------------------------------------------------
        
   /// @details Returns state.sleepset.awake(state.backtrack \ state.done) if sleepsets are enabled 
	/// and state.backtrack \ state.done otherwise.

   program_model::Tids pool(const execution_t&)
	{
      SufficientSet& s = mState.back();
		program_model::Tids sufficient = s.sleepset().awake(s.backtrack());
      mSufficientSet.add_to_pool(sufficient);
      return sufficient;
   }
	
	//-----------------------------------------------------------------------------------------------
        
   /// @brief Selects the first program_model::Thread::tid_t from the given pool that satisfies 
	/// the condition set by sufficient_set_t. If no such program_model::Thread::tid_t is found, it 
	/// returns -1.

   program_model::Thread::tid_t select_from_pool(const execution_t& execution, const Tids& pool)
   {
      /// @pre !pool.empty()
      assert(!pool.empty());
      DEBUGF(outputname(), "select_from_pool", pool, "\n");
      auto it = std::find_if(pool.begin(), pool.end(), [this, &execution] (const auto& tid) 
									  {
                    					return mSufficientSet.condition(execution, mState.back(), tid);
                				  });
      return (it != pool.end()) ? *it : -1;
   }
	
	//-----------------------------------------------------------------------------------------------
        
   void pop_back();
        
   void dump_state(std::ostream& os, const std::size_t index) const;
        
   static std::string full_name();
		
private:
		
   sufficient_set_t mSufficientSet;
		
}; // end class template dpor<sufficient_set_t>

//--------------------------------------------------------------------------------------------------

template <typename sufficient_set_t>
inline bool dpor<sufficient_set_t>::check_valid(const bool contains_locks) const
{
	 return mSufficientSet.check_valid(contains_locks);
}

//--------------------------------------------------------------------------------------------------

template <typename sufficient_set_t>
std::string dpor<sufficient_set_t>::path() const
{
	std::string path = name;
	path += "/";
	path += mSufficientSet.path();
	return path;
}

//--------------------------------------------------------------------------------------------------

template <typename sufficient_set_t>
void dpor<sufficient_set_t>::pop_back()
{
	mState.pop_back();
	mHB.pop_back();
	mSufficientSet.pop_back();
}	

//-------------------------------------------------------------------------------------------------- 

template <typename sufficient_set_t>
inline void dpor<sufficient_set_t>::dump_state(std::ostream& os, const std::size_t index) const
{
	os << mState[index];
}

//-------------------------------------------------------------------------------------------------- 

template <typename sufficient_set_t>
std::string dpor<sufficient_set_t>::full_name()
{
	 std::string full_name = name;
	 full_name += "<";
	 full_name += sufficient_set_t::name();
	 full_name += ">";
	 return full_name;
}

//-------------------------------------------------------------------------------------------------- 
} // end namespace exploration
