#pragma once

#include "visible_instruction.hpp"

//-------------------------------------------------------------------------------------------------
/// @file dependence.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//-------------------------------------------------------------------------------------------------


namespace exploration {

class Dependence
{
public:
   using instruction_t = program_model::visible_instruction_t;

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
   static bool dependent(const instruction_t&, const instruction_t&);

   /**
     Following @cite flanagan-popl-95, we consider to instructions
     on an object that is not a lock to be co-enabled.
     An aquire and release on the same lock object are not co-enabled.
     In the context of POR it is always safe to assume that instructions may be
     co-enabled even if they may not. However, this may decrease the obtained reduction.
     */
   static bool coenabled(const instruction_t&, const instruction_t&);
};

} // end namespace exploration
