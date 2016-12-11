
#include "vector_clock.hpp"
#include <assert.h>
#include "container_io.hpp"

namespace exploration
{
   VectorClock::VectorClock(std::size_t n)
   : datastructures::fixed_size_vector<int>(n, 0)
   {
   }
    
    VectorClock::value_t VectorClock::min() const
    {
        return *std::min_element((*this).begin(), (*this).end());
    }
    
    VectorClock::value_t VectorClock::max() const
    {
        return *std::max_element((*this).begin(), (*this).end());
    }
    
    void VectorClock::max(const VectorClock& other)
    {
        /// @pre mSize == other.size()
        assert(size() == other.size());
        for (index_t i = 0; i < size(); ++i) {
            (*this)[i] = std::max((*this)[i], other[i]);
        }
    }
    
    void VectorClock::filter_values_greater_than(const VectorClock& other)
    {
        /// @pre mSize == othersize()
        assert(size() == other.size());
        for (index_t i = 0; i < size(); ++i) {
            if ((*this)[i] <= other[i]) {
                (*this)[i] = 0;
            }
        }
    }
    
    VectorClock::Indices_t VectorClock::indices_such_that(
        const std::function<bool(const value_t&)>& pred) const
    {
        Indices_t I{};
        for (index_t i = 0; i < size(); ++i) {
            if (pred((*this)[i])) {
                I.insert(i);
            }
        }
        return I;
    }
    
    VectorClock::Values_t VectorClock::values_such_that(
        const std::function<bool(const value_t&)>& pred) const
    {
        Values_t V{};
        std::copy_if(
            this->begin(), this->end(), std::inserter(V, V.end()),
            [&pred] (const value_t& val) { return pred(val); }
        );
        return V;
    }
    
    VectorClock::Indices_t VectorClock::indices_values_greater_than(const value_t value) const
    {
        return indices_such_that(
            [&value] (const value_t& val) { return val > value; }
        );
    }
    
    VectorClock::Values_t VectorClock::values_non_zero() const
    {
        return values_such_that(
            [] (const value_t& val) { return val != 0; }
        );
    }
} // end namespace exploration
