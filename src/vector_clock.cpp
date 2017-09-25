
#include "vector_clock.hpp"

#include <algo.hpp>

#include <assert.h>


namespace exploration {

//--------------------------------------------------------------------------------------------------

VectorClock::VectorClock(std::size_t n)
: datastructures::fixed_size_vector<int>(n, 0)
{
}

//--------------------------------------------------------------------------------------------------

void VectorClock::max(const VectorClock& other)
{
   /// @pre mSize == other.size()
   assert(size() == other.size());
   for (index_t i = 0; i < size(); ++i)
   {
      (*this)[i] = std::max((*this)[i], other[i]);
   }
}

//--------------------------------------------------------------------------------------------------

void VectorClock::filter_values_greater_than(const VectorClock& other)
{
   /// @pre mSize == other.size()
   assert(size() == other.size());
   for (index_t i = 0; i < size(); ++i)
   {
      if ((*this)[i] <= other[i])
      {
         (*this)[i] = 0;
      }
   }
}

//--------------------------------------------------------------------------------------------------

VectorClock::value_t min_element(const VectorClock& clock)
{
   return *std::min_element(clock.begin(), clock.end());
}

//--------------------------------------------------------------------------------------------------

VectorClock::value_t max_element(const VectorClock& clock)
{
   return *std::max_element(clock.begin(), clock.end());
}

//--------------------------------------------------------------------------------------------------

VectorClock::indices_t indices_such_that(const VectorClock& clock, const value_predicate_t& pred)
{
   VectorClock::indices_t indices;
   utils::algo::copy_index_if(clock.begin(), clock.end(), std::inserter(indices, indices.end()),
                              pred, 0);
   return indices;
}

//--------------------------------------------------------------------------------------------------

VectorClock::values_t values_such_that(const VectorClock& clock, const value_predicate_t& pred)
{
   VectorClock::values_t values;
   std::copy_if(clock.begin(), clock.end(), std::inserter(values, values.end()), pred);
   return values;
}

//--------------------------------------------------------------------------------------------------

} // end namespace exploration
