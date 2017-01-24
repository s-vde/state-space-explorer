
#include "dependence.hpp"

namespace exploration
{
    bool Dependence::dif_read_write()
    {
        return true;
    }
    
    bool Dependence::dependent(const Instruction& t1, const Instruction& t2)
    {
        return
            same_thread(t1, t2) ||
            /* AND */ (t1.obj() == t2.obj() &&
					   (one_write(t1, t2) || one_lock(t1, t2)));
    }
    
    
    bool Dependence::coenabled(const Instruction& t1, const Instruction& t2)
    {
        return !(same_thread(t1, t2) || lock_unlock(t1, t2));
    }
    
    bool Dependence::same_thread(const Instruction& t1, const Instruction& t2)
    {
        return t1.tid() == t2.tid();
    }
   
   namespace
   {
      bool is_memory_modification(const Instruction& instruction)
      {
         return
            instruction.op() == Object::Op::WRITE ||
            instruction.op() == Object::Op::RMW;
      }
   } // end namespace
    
    bool Dependence::one_write(const Instruction& t1, const Instruction& t2)
    {
       return is_memory_modification(t1) || is_memory_modification(t2);
    }
    
    bool Dependence::one_lock(const Instruction& t1, const Instruction& t2)
    {
        return
            t1.op() == Object::Op::LOCK ||
            t2.op() == Object::Op::LOCK;
    }
    
    bool Dependence::lock_unlock(const Instruction& t1, const Instruction& t2)
    {
        return
            t1.obj() == t2.obj() &&
            /* OR */ ((t1.op() == Object::Op::LOCK && t2.op() == Object::Op::UNLOCK) ||
                	  (t1.op() == Object::Op::UNLOCK && t2.op() == Object::Op::LOCK));
    }
} // end namespace exploration
