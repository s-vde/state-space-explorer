
#ifndef LOCAL_BOUND_FUNCTION_HPP_INCLUDED
#define LOCAL_BOUND_FUNCTION_HPP_INCLUDED

#include <assert.h>
#include "execution.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file local_bound_function.hpp
 @brief Definition of class template LocalBoundFunction.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;

namespace bound_functions
{
	/**
     @brief Implements the basic functionality of a BoundFunction that is
     both monotonic and local.
     */
    template<typename Impl, typename ValueT>
    class LocalBoundFuction
    {
    public:
        
        using value_t = ValueT;

        static std::string name()
        {
            return Impl::name();
        }
        
        /**
         @brief Returns the bound value of pre+(E,index).tid based on Impl.
         @details Uses the property of a monotonic LocalBoundFunction that 
         the increase in bound value can be computed based only on the 
         last Transition in the Execution and the next Thread::tid_t.
         */
        template<typename Sequence>
        static value_t value(
            const Execution& E,
            const Sequence S,
            const unsigned int index,
            const Thread::tid_t& tid)
        {
            /// @pre index <= E.size() && index < S.size()
            assert(index <= E.size() && index < S.size());
            if (index < 1)  { return Impl::step_value(tid);                                     }
            else            { return S[index].boundvalue() + Impl::step_value(E[index], tid);   }
        }
    }; // end class template LocalBoundFunction<Impl,ValueT>
} // end namespace bound_functions

#endif
