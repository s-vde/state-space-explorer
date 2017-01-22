
#ifndef PREEMPTIONS_HPP_INCLUDED
#define PREEMPTIONS_HPP_INCLUDED

#include "local_bound_function.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file preemptions.hpp
 @brief Definition of class Preemptions.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;

namespace bound_functions
{
    class Preemptions : public LocalBoundFuction<Preemptions, int>
    {
    public:
		
		using index_t = Execution::index_t;
       using transition_t = typename Execution::transition_t;
		
        static std::string name();
        
        static value_t step_value(const Thread::tid_t& tid)
        {
            return 0;
        }
       
        static value_t step_value(const transition_t& last, const Thread::tid_t& tid)
        {
            return
                context_switch(last.instr().tid(), tid) &&
                last.post().is_enabled(last.instr().tid())
                ? 1 : 0;
        }
        
        /**
         @brief Returns a Thread::tid_t tid in T with minimal bound value
         after pre+(E,index), prioritizing elements from Prioritize.
         @details More formally, it returns a tid from the set
         ARGMIN({ tid' in T | value(pre+(E,index).tid') }). This implementation
         exploits the fact that Preemtions is monotonic and
         step_value(last,tid') is either 0 or 1 for all tid'.
         @note This function is implemented by the BoundFunction, because
         there may be instance-specific optimizations for selecting the
         element with minimal value.
         */
        static Tids::const_iterator min_value(
            const Execution& E,
            const index_t index,
            const Tids& T,
            const Tids& Prioritize={})
        {
            /// @pre !T.empty()
            assert(!T.empty());
            auto min = std::find_if(
                Prioritize.cbegin(), Prioritize.cend(),
                [&E, &index] (const auto& tid) {
                    return index < 1 ? step_value(tid) == 0 : step_value(E[index], tid) == 0;
                }
            );
            if (min != Prioritize.end()) { return min; }
            min = std::find_if(
                T.cbegin(), T.cend(),
                [&E, &index] (const auto& tid) {
                    return index < 1 ? step_value(tid) == 0 : step_value(E[index], tid) == 0;
                }
            );
            if (min == T.end()) { min = T.begin(); }
            return min;
        }

        /**
         @brief Returns <code>max({ j in dom(E) |
         j < index && (j == 0 || context_switch(E[j-1], E[j]))})</code>
         */
		// #todo 0 is not in dom(E) and 0 is never returned.
        static index_t last_context_switch_before(const Execution& E, const index_t index)
        {
            /// @pre index > 0
            assert(index > 0);
            unsigned int j;
            for (j = index-1; j > 1; --j) {
                if (Preemptions::context_switch(E[j-1].instr().tid(), E[j].instr().tid())) {
                    return j;
                }
            }
            assert(index == 1 || j == 1);
            return 1;
        }
        
    private:
        
		static bool context_switch(const Thread::tid_t& tid1, const Thread::tid_t& tid2);
		
    }; // end class Preemptions
} // end namespace bound_functions

#endif
