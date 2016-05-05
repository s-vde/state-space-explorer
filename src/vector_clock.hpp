
#ifndef VECTOR_CLOCK_HPP_INCLUDED
#define VECTOR_CLOCK_HPP_INCLUDED

#include <set>
#include <vector>

/*---------------------------------------------------------------------------75*/
/**
 @file vector_clock.hpp
 @brief Definition of class VectorClock.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

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
    class VectorClock
    {
    public:
        
        // TYPES
        
        using value_t = int;
        using index_t = int;
        using clock_t = std::vector<value_t>;
        using Indices_t = std::set<index_t>;
        using Values_t = std::set<value_t>;
        
        // CTOR
        
        /**
         @brief Constructs a n-size 0-value VectorClock.
         */
        explicit VectorClock(const unsigned int n);
        
        // OPERATORS
        
        /**
         @brief Read-only subscript operator.
         */
        const value_t& operator[](const int index) const;
        
        // ITERATORS
        
        clock_t::const_iterator begin() const
        {
            return mClock.begin();
        }
        
        clock_t::const_iterator end() const
        {
            return mClock.end();
        }
        
        //
        
        /**
         @brief Sets mClock[index] to the given value.
         */
        void set(const index_t index, const value_t value);
        
        size_t size() const;
        
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
		
		// DATA MEMBERS
		
        /// @brief The actualc clock clock.
        clock_t mClock;
		
        /// @brief Caching the size of mClock.
        unsigned int mSize;
		
		// HELPER FUNCTIONS
		
        /// @brief Returns { i | pred(mClock[i]) }.
        Indices_t indices_such_that(const std::function<bool(const value_t&)>& pred) const;
		
        /// @brief Returns { mClock[i] | pred(i) }.
        Values_t values_such_that(const std::function<bool(const value_t&)>& pred) const;
        
    //friendly:
        
        friend std::ostream& operator<<(std::ostream&, const VectorClock&);
        friend std::istream& operator>>(std::istream&, VectorClock&);
        
    }; // end class VectorClock
    
    std::ostream& operator<<(std::ostream&, const VectorClock&);
    std::istream& operator>>(std::istream&, VectorClock&);
    
} // end namespace exploration

#endif
