
#ifndef SOURCE_SET_HPP_INCLUDED
#define SOURCE_SET_HPP_INCLUDED

#include "sufficient_set.hpp"
#include "transition_io.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file source_set.hpp
 @brief Definition of class Source.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;

namespace exploration
{
    /**
     The Source implementation of SufficientSet is from 
     @cite abdulla-popl-14. 
     */
    class Source
    {
    public:
        
        // TYPES
        
        using execution = Execution;
       using transition = typename Execution::transition_t;
        
        // CTOR
        
        Source() { }
        
        //
		
		/**
		 @brief Returns !contains_locks.
		 */
		bool check_valid(const bool contains_locks) const;
        
        static std::string name();
        
        static std::string path();
        
        static void update_state(const execution& E, const transition& t);
        
        /**
         @cite abdulla-popl-14
         @verbatim
         Source::BacktrackPoints(E, index) {
            return HappensBefore::covering(E, index);
         }
         @endverbatim
         */
        template<typename Dependence>
        static BacktrackPoints backtrack_points(
            const execution& E,
            const unsigned int index,
            const HappensBefore<Dependence>& HB)
        {
            DEBUGF(outputname(), "backtrack_points", to_short_string(E[index]), "\n");
            VectorClock::indices_t Covering = HB.covering(index, E[index].instr());
            BacktrackPoints Points{};
            for (const auto& covering : Covering) {
                const auto tid_index = boost::apply_visitor(program_model::get_tid(), E[index].instr());
                Points.push_back({ tid_index, covering });
                DEBUG(tabs() << "\tPoints.add(<tid=" <<tid_index << ", index=" << covering << ">)\n");
            }
            return Points;
        }
        
        /**
         @cite abdulla-popl-14
         @details The implementation prioritizes point.tid if it is an
         alternative.
         @see HappensBefore::incomparable_after
         @see HappensBefore::front
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
            VectorClock::values_t v = HB.incomparable_after(point.index, index);
            v.insert(index);
            Tids Front = HB.tids(HB.front(v));
            /// @invariant !Front.empty()
            assert(!Front.empty());
            Tids SourcesFor{};
            std::set_intersection(
                S[point.index-1].backtrack().cbegin(), S[point.index-1].backtrack().cend(),
                Front.cbegin(), Front.cend(),
                std::inserter(SourcesFor, SourcesFor.begin())
            );
            DEBUG(
                tabs() << to_string_pre(point.index) << ".backtrack cap Front = "
                  << S[point.index-1].backtrack() << " cap "
                  << Front  << " = " << SourcesFor
            );
            if (SourcesFor.empty()) {
                Thread::tid_t add = (Front.find(point.tid) != Front.end())
                ? point.tid : *(Front.begin());
                DEBUG(tabs() << to_string_pre(point.index) << ".backtrack.add(" << add << ")");
                S[point.index-1].add_to_backtrack(add);
            }
        }
        
        static void update_after_exploration(const transition& t, SufficientSet&);
        
        static void add_to_pool(Tids&);
        
        static bool condition(const execution&, SufficientSet&, const Thread::tid_t&);
        
        static void pop_back();
        
    private:
        
        // DEBUGGING
        
        static std::string tabs();
        
        static std::string outputname();
        
    }; // end class Source
} // end namespace exploration

#endif
