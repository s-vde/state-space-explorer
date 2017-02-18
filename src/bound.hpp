#pragma once

// SCHEDULER
#include "scheduler_settings.hpp"

// PROGRAM_MODEL
#include "execution.hpp"
#include "instruction_io.hpp"
#include "transition_io.hpp"
#include "state.hpp"

// UTILS
#include "debug.hpp"
#include "container_output.hpp"

//-----------------------------------------------------------------------------------------------100
/// @file bound.hpp
/// @author Susanne van den Elsen
/// @date 2015-2016
//--------------------------------------------------------------------------------------------------

namespace exploration
{
//--------------------------------------------------------------------------------------------------

class BoundState
{
public:
        
   explicit BoundState(int bound_value=0);
		
   int bound_value() const;
   void set_bound_value(int bound_value);
        
   std::string to_string() const;
        
private:
        
   int mBoundValue;
        
}; // end class BoundState

//--------------------------------------------------------------------------------------------------
    
std::ostream& operator<<(std::ostream&, const BoundState&);

//--------------------------------------------------------------------------------------------------
	
template<typename bound_function_t>
class bound
{
public:
        
   using execution_t = program_model::Execution;
   using transition_t = typename program_model::Execution::transition_t;
   
   //-----------------------------------------------------------------------------------------------
        
   explicit bound(const execution_t& execution, const typename bound_function_t::value_t bound_value)
   : mState({ BoundState() })
   , mBoundValue(bound_value) 
   { 
   }
   
   //-----------------------------------------------------------------------------------------------
        
   static std::string name;
   std::string path() const;
        
   /// @brief Nonpreemptive.
   scheduler::SchedulerSettings scheduler_settings();
        
   /// @brief Does nothing.
   inline void write_scheduler_files() { }
		
	/// @brief Returns true.
	bool check_valid(const bool contains_locks) const;
        
   /// @brief Does nothing.
   inline void reset() { }
        
   /// @brief Does nothing.
   inline void update_statistics(const execution_t&) { }
        
   /// @brief Does nothing.
   inline void restore_state(const transition_t&) { }
   
   //-----------------------------------------------------------------------------------------------

   /// @detail Adds a new BoundState to mState with a bound_value that is the sum of the bound_value 
   /// corresponding to E[transition.index()-1] and the step value of E[transition.index()-1] and 
   /// transition defined by bound_function_t.
   /// @note Assumes that bound_function_t is local (i.e. the step_value can be determined using only 
   /// E[transition.index()-1] and transition.
   
   void update_state(const execution_t& execution, const transition_t& transition)
   {
      /// @pre mState.size() == transition.index()
      assert(mState.size() == transition.index());
      mState.emplace_back(bound_function_t::value(execution, mState, transition.index()-1,
                                               transition.instr().tid()));
      DEBUGF(outputname(), "update_state", transition.instr(), to_string_post_state(transition) 
             << ".bound_value = " << mState.back().bound_value() << "\n");
   }
   
   //-----------------------------------------------------------------------------------------------
        
   /// @brief Does nothing.
   inline void update_after_exploration(transition_t& transition) { }
   
   //-----------------------------------------------------------------------------------------------
        
   /// @note Returns a subset of execution.final().enabled.
   
   program_model::Tids pool(const execution_t& execution)
   {
      program_model::Tids pool;
      std::copy_if(execution.final().enabled().begin(), execution.final().enabled().end(),
                   std::inserter(pool, pool.end()),
                   [this, &execution] (const auto& tid) 
                   {
                     return bound_function_t::value(execution, mState, execution.size(), tid) <= mBoundValue;
                   });
      return pool;
   }
   
   //-----------------------------------------------------------------------------------------------
        
   /// @brief Returns the first tid in the pool.
   
   static program_model::Thread::tid_t select_from_pool(const execution_t& execution, 
                                                        const program_model::Tids& pool)
   {
      /// @pre !pool.empty()
      assert(!pool.empty());
      DEBUGF(outputname(), "select_from_pool", pool, *(pool.begin()) << "\n");
      return *(pool.begin());
   }
   
   //-----------------------------------------------------------------------------------------------
        
   void pop_back();
        
   /// @brief Does nothing.
   inline void close(const std::string& statistics) const { }
        
   void dump_state(std::ostream& os, const std::size_t index) const;
        
   static std::string full_name();
        
private:
   
   static std::string outputname();
        
   std::vector<BoundState> mState;
   int mBoundValue;
        
}; // end class template bound<bound_function_t>

//--------------------------------------------------------------------------------------------------

template <typename bound_function_t>
std::string bound<bound_function_t>::name = "bound";

//--------------------------------------------------------------------------------------------------  

template <typename bound_function_t>
std::string bound<bound_function_t>::path() const
{
   std::string path = name;
   path += "/";
   path += bound_function_t::name();
   path += "/";
   path += std::to_string(mBoundValue);
   return path;
}
 
//--------------------------------------------------------------------------------------------------

template <typename bound_function_t>
inline std::string bound<bound_function_t>::outputname()
{
    return name;
}

//--------------------------------------------------------------------------------------------------

template <typename bound_function_t>
inline scheduler::SchedulerSettings bound<bound_function_t>::scheduler_settings()
{
   return scheduler::SchedulerSettings("Nonpreemptive");
}

//--------------------------------------------------------------------------------------------------

template <typename bound_function_t>
inline bool bound<bound_function_t>::check_valid(const bool contains_locks) const
{
   return true;
}

//--------------------------------------------------------------------------------------------------

template <typename bound_function_t>
inline void bound<bound_function_t>::pop_back()
{
   mState.pop_back();
}

//--------------------------------------------------------------------------------------------------

template <typename bound_function_t>
void bound<bound_function_t>::dump_state(std::ostream& os, const std::size_t index) const
{
   os << mState[index];
}

//--------------------------------------------------------------------------------------------------

template <typename bound_function_t>
std::string bound<bound_function_t>::full_name()
{
   std::string full_name = name;
   full_name += "<";
   full_name += bound_function_t::name();
   full_name += ">";
   return full_name;
}

//--------------------------------------------------------------------------------------------------
} // end namespace exploration
