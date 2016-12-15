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
   //-------------------------------------------------------------------------------------
   
   /// Let P be a program with threads from Tids and let E be an Execution of P. A
   /// VectorClock associated with the post State of a Transition t in E stores for each
   /// Thread::tid_t tid in Tids the index of the last Transition of that Thread in E
   /// happening-before t. The sequence of VectorClock objects associated with E
   /// represents a happens-before relation on E.
   
   class VectorClock : public datastructures::fixed_size_vector<int>
   {
   public:
      
      using index_t = int;
      using indices_t = std::set<index_t>;
      using values_t = std::set<value_t>;

      /// @brief Constructs a n-size 0-value VectorClock.

      explicit VectorClock(std::size_t n);

      /// @brief Transforms this VectorClock into max(*this, other) =
      /// <max(*this[0], other[0]), ..., max(*this[mSize-1], other[mSize-1])>

      void max(const VectorClock& other);
      
      /// @brief Transforms this VectorClock into gr(*this, other) =
      /// <gr(*this[0], other[0], ..., gr(*this[mSize-1], other[mSize-1])>,
      /// where gr(*this[i], other[i]) = *this[i] if *this[i] > other[i] and 0 otherwise.

      void filter_values_greater_than(const VectorClock& other);
      
   }; // end class VectorClock
   
   //-------------------------------------------------------------------------------------
   
   VectorClock::value_t min_element(const VectorClock& clock);
   
   VectorClock::value_t max_element(const VectorClock& clock);
   
   //-------------------------------------------------------------------------------------
   
   using value_predicate_t = std::function<bool(const VectorClock::value_t&)>;
   
   /// @brief Returns { i | pred(clock[i]) }.
   
   VectorClock::indices_t indices_such_that(const VectorClock& clock,
                                            const value_predicate_t& pred);
   
   //-------------------------------------------------------------------------------------
   
   /// @brief Returns { clock[i] | pred(clock[i]) }.
   
   VectorClock::values_t values_such_that(const VectorClock& clock,
                                          const value_predicate_t& pred);
   
   //-------------------------------------------------------------------------------------
   
} // end namespace exploration
