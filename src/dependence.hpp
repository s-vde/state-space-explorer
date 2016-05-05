
#ifndef DEPENDENCE_HPP_INCLUDED
#define DEPENDENCE_HPP_INCLUDED

#include "instruction.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file dependence.hpp
 @brief Definition of class Dependence.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace exploration
{
	using namespace program_model;
	
    class Dependence
    {
    public:
        
        static bool dif_read_write();
        
        /**
         @brief Returns 
		 	<code>same_thread(i1, i2) ||
			i1.obj = i2.obj && (one_write(i1, i2) || one_lock(i1, i2))</code>.
         }
         @note A HappensBefore relation based on this Dependence relation
         does not totally order all accesses to a given object obj,
         because of READ instructions.
         */
        static bool dependent(const Instruction&, const Instruction&);
        
        /**
         Following @cite flanagan-popl-95, we consider to instructions
         on an object that is not a lock to be co-enabled.
         An aquire and release on the same lock object are not co-enabled.
         In the context of POR it is always safe to assume that instructions may be
         co-enabled even if they may not. However, this may decrease the obtained reduction.
         */
        static bool coenabled(const Instruction&, const Instruction&);
    
    private:
        
        static bool same_thread(const Instruction& t1, const Instruction& t2);
        
        static bool one_write(const Instruction& t1, const Instruction& t2);
        
        static bool one_lock(const Instruction& t1, const Instruction& t2);
        
        /**
         @brief Returns true iff t1 and t2 are a (LOCK obj,UNLOCK obj)
         or (UNLOCK obj,LOCK obj) pair of the same lock object obj.
         */
        static bool lock_unlock(const Instruction& t1, const Instruction& t2);
    };
} // end namespace exploration

#endif
