#pragma once

// DATASTRUCTURES
#include "fixed_size_vector.hpp"

// STL
#include <set>

//--------------------------------------------------------------------------------------90
/// @file vector_clock.hpp
/// @brief Definition of class VectorClock.
/// @author Susanne van den Elsen
/// @date 2015-2016
//----------------------------------------------------------------------------------------

namespace exploration
{
    /**
     A VectorClock associated with the post State of a Transition t in  an 
     Execution E of program P with Threads Tids stores for each Thread::tid_t 
     tid in  Tids the index of the last Transition of that Thread in E 
     happening-before t. The sequence of VectorClock objects associated with 
     E represents a happens-before relation on E. The VectorClock class 
     encapsulated a clock datastructure and provides operations on it.
     */
   class VectorClock : public datastructures::fixed_size_vector<int>
    {
    public:
        
        // TYPES
      
        using index_t = int;
        using clock_t = std::vector<value_t>;
        using Indices_t = std::set<index_t>;
        using Values_t = std::set<value_t>;

       /// @brief Constructs a n-size 0-value VectorClock.

       explicit VectorClock(std::size_t n);
        
        /**
         @brief Returns min({ mClock[i] })
         */
        value_t min() const;
        
        /**
         @brief Returns max({ mClock[i] })
         */
        value_t max() const;

        /**
         @brief Transforms this VectorClock into max(*this, other) =
         <max(*this[0], other[0]), ..., max(*this[mSize-1], other[mSize-1])>
         */
        void max(const VectorClock& other);
        
        /**
         @brief Transforms this->clock into gr(*this, other) =
         <gr(*this[0], other[0], ..., gr(*this[mSize-1], other[mSize-1])>,
         where gr(*this[i], other[i]) = *this[i] if *this[i] > other[i]
         and 0 otherwise.
         */
        void filter_values_greater_than(const VectorClock& other);

        /**
         @brief Returns { i | mClock[i] > value }.
         */
        Indices_t indices_values_greater_than(const value_t) const;
        
        /**
         @brief Returns { mClock[i] | i != 0 }.
         */
        Values_t values_non_zero() const;
        
    private:
		
		// HELPER FUNCTIONS
		
        /// @brief Returns { i | pred(mClock[i]) }.
        Indices_t indices_such_that(const std::function<bool(const value_t&)>& pred) const;
		
        /// @brief Returns { mClock[i] | pred(i) }.
        Values_t values_such_that(const std::function<bool(const value_t&)>& pred) const;
        
    }; // end class VectorClock
    
} // end namespace exploration
