
#include "vector_clock.hpp"
#include <assert.h>
#include "container_io.hpp"

namespace exploration
{
    VectorClock::VectorClock(const unsigned int n)
    : mClock(n, 0)
    , mSize(n) { }
    
    /**
     @details Like std::vector[], this subscript operator does not throw if 
     mClock.size() > index and yields undefined behaviour otherwise.
     */
    const VectorClock::value_t& VectorClock::operator[](const index_t index) const
    {
		/// @pre 
        return mClock[index];
    }
    
    void VectorClock::set(const index_t index, const value_t value)
    {
        mClock[index] = value;
    }
    
    size_t VectorClock::size() const
    {
        return mSize;
    }
    
    VectorClock::value_t VectorClock::min() const
    {
        return *std::min_element(mClock.begin(), mClock.end());
    }
    
    VectorClock::value_t VectorClock::max() const
    {
        return *std::max_element(mClock.begin(), mClock.end());
    }
    
    void VectorClock::max(const VectorClock& other)
    {
        /// @pre mSize == other.size()
        assert(mSize == other.size());
        for (index_t i = 0; i < size(); ++i) {
            mClock[i] = std::max((*this)[i], other[i]);
        }
    }
    
    void VectorClock::filter_values_greater_than(const VectorClock& other)
    {
        /// @pre mSize == othersize()
        assert(mSize == other.size());
        for (index_t i = 0; i < size(); ++i) {
            if (mClock[i] <= other[i]) {
                mClock[i] = 0;
            }
        }
    }
    
    VectorClock::Indices_t VectorClock::indices_such_that(
        const std::function<bool(const value_t&)>& pred) const
    {
        Indices_t I{};
        for (index_t i = 0; i < size(); ++i) {
            if (pred(mClock[i])) {
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
            mClock.begin(), mClock.end(), std::inserter(V, V.end()),
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
    
    std::ostream& operator<<(std::ostream& os, const VectorClock& clock)
    {
        os << clock.mClock;
        return os;
    }
    
    std::istream& operator>>(std::istream& is, VectorClock& clock)
    {
        is >> clock.mClock;
        return is;
    }
} // end namespace exploration
