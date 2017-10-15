
#ifndef PERSISTENT_SET_HPP_INCLUDED
#define PERSISTENT_SET_HPP_INCLUDED

#include "sufficient_set.hpp"
#include "transition_io.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file persistent_set.hpp
 @brief Definition of class Persistent.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;

namespace exploration
{
	// #todo Optimization settings, including coenabledness.
    class Persistent
    {
    public:
        
        // TYPES
        
        using execution = Execution;
       using transition = typename Execution::transition_t;
        
        // CTOR
        
        Persistent() { }
		
		//
		
		/**
		 @brief Returns true.
		 */
		bool check_valid(const bool contains_locks) const
		{
			return true;
		}
        
        static std::string name();
                
        static void update_state(const execution& E, const transition& t);
        
        /**
         @cite flanagan-popl-05
		 @code{.cpp}
		 BacktrackPoints Persistent::BacktrackPoints(State s)
		 {
		 	Points = {};
		 	for all(p in Tids | s.has_next(p)) {
		 		Dep = { i in dom(E) | dependent(E_i, s.next(p)) && coenabled(E_i, s.next(p)) && i -/> p }
		 	};
		 	if (!Dep.empty()) { Points.insert((p, max(Dep))); }
		 }
		 return Points;
		 }
		 @endcode
         @see HappensBefore::max_dependent
         */
        template<typename Dependence>
        static BacktrackPoints backtrack_points(
            const execution& E,
            const unsigned int index,
            const HappensBefore<Dependence>& HB)
        {
            DEBUGF(outputname(), "backtrack_points", to_short_string(E[index]), "\n");
            BacktrackPoints Points{};
            std::for_each(
                E[index].pre().next_cbegin(), E[index].pre().next_cend(),
                [&index, &HB, &Points] (const auto& next) {
                    auto maxdep = HB.max_dependent(index, next.second.instr, true, true);
                    if (maxdep > 0) {
                        Points.push_back({ next.first, maxdep });
                        DEBUG(tabs() << "\tPoints.add(" << Points.back() << ")\n");
                    }
                }
            );
            return Points;
        }
        
        /**
         @cite flanagan-popl-05
         @details The implementation prioritizes point.tid if it is an
         alternative to avoid an unnecessary call to Persistent::alternatives.
         */
        template<typename Dependence>
        static void add_backtrack_point(
            const execution& E,
            const unsigned int index,
            std::vector<SufficientSet>& S,
            const HappensBefore<Dependence>& HB,
            const backtrack_point& point)
        {
            DEBUGF(outputname(), "add_backtrack_point", point, "\n");
			const State& s = E[point.index].pre();
			SufficientSet& s_ = S[point.index-1];
            // Prioritize point.tid
            if (s.is_enabled(point.tid) && s_.sleepset().is_awake(point.tid)) {
                DEBUG(tabs() << to_string_pre(point.index) << ".backtrack.add(" << point.tid << ")");
                s_.add_to_backtrack(point.tid);
            } else {
                Tids Alt = alternatives(E, index, s_, HB, point);
                if (!Alt.empty()) {
                    DEBUG(tabs() << to_string_pre(point.index) << ".backtrack.add(" << *(Alt.begin()) << ")");
                    s_.add_to_backtrack(*(Alt.begin()));
                } else {
                    DEBUG(tabs() << to_string_pre(point.index) << ".backtrack.add(enabled)");
                    s_.add_to_backtrack(s.enabled());
                }
            }
        }
        
        static void update_after_exploration(const transition&, SufficientSet&);
        
        static void add_to_pool(Tids&);
        
        static bool condition(const execution&, SufficientSet&, const Thread::tid_t&);
        
        static void pop_back();
        
		/**
         @cite flanagan-popl-05 and addendum.
         */
        template<typename Dependence>
        static Tids alternatives(
            const execution& E,
            const unsigned int index,
            const SufficientSet& s,
            const HappensBefore<Dependence>& HB,
            const backtrack_point& point,
			bool use_sleepsets=true)
        {
            Tids Alt = HB.thread_transitive_relation(index, point.index, point.tid);
            Alt.insert(point.tid);
            const auto& enabled = E[point.index].pre().enabled();
            Tids AltEnabled{};
            std::set_intersection(
                Alt.begin(), Alt.end(),
                enabled.begin(), enabled.end(),
                std::inserter(AltEnabled, AltEnabled.end())
            );
			DEBUGF(outputname(), "alternatives", point, " = (" << Alt << " U {" << point.tid << "}) cap" << enabled);
			if (use_sleepsets) {
				DEBUG(" \\ " << s.sleepset() << " = " << s.sleepset().awake(AltEnabled) << "\n");
				return s.sleepset().awake(AltEnabled);
			} else {
				DEBUG(" = " << AltEnabled << "\n");
				return AltEnabled;
			}
        }
		
	private:
        
        // DEBUGGING
        
        static std::string tabs();
        
        static std::string outputname();
        
    }; // end class Persistent
} // end namespace exploration

#endif
