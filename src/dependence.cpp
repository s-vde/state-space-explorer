
#include "dependence.hpp"

#include <visible_instruction_io.hpp>
#include <debug.hpp>


//-------------------------------------------------------------------------------------------------

namespace exploration {
namespace {

using instruction_t = Dependence::instruction_t;

bool same_thread(const instruction_t& instruction_1, const instruction_t& instruction_2)
{
   const auto tid_1 = boost::apply_visitor(program_model::get_tid(), instruction_1);
   const auto tid_2 = boost::apply_visitor(program_model::get_tid(), instruction_2);
   return tid_1 == tid_2;
}

//-------------------------------------------------------------------------------------------------

bool same_operand(const instruction_t& instruction_1, const instruction_t& instruction_2)
{
   const auto operand_1 = boost::apply_visitor(program_model::get_operand(), instruction_1);
   const auto operand_2 = boost::apply_visitor(program_model::get_operand(), instruction_2);
   return operand_1 == operand_2;
}

//-------------------------------------------------------------------------------------------------

bool is_memory_modification(const instruction_t& instruction)
{
   if (const auto* mem_instr = boost::get<const program_model::memory_instruction>(&instruction))
   {
      return mem_instr->operation() == program_model::memory_operation::Store ||
             mem_instr->operation() == program_model::memory_operation::ReadModifyWrite;
   }
   return false;
}

//-------------------------------------------------------------------------------------------------

bool one_memory_modification(const instruction_t& instruction_1, const instruction_t& instruction_2)
{
   return is_memory_modification(instruction_1) || is_memory_modification(instruction_2);
}

//-------------------------------------------------------------------------------------------------

bool is_lock(const instruction_t& instruction)
{
   if (const auto* lock_instr = boost::get<program_model::lock_instruction>(&instruction))
   {
      return lock_instr->operation() == program_model::lock_operation::Lock/* ||
         lock_instr->operation() == program_model::lock_operation::Trylock;*/;
   }
   return false;
}

//-------------------------------------------------------------------------------------------------

bool is_trylock(const instruction_t& instruction)
{
   if (const auto* lock_instr = boost::get<program_model::lock_instruction>(&instruction))
   {
      return lock_instr->operation() == program_model::lock_operation::Trylock;
   }
   return false;
}

//-------------------------------------------------------------------------------------------------

bool is_unlock(const instruction_t& instruction)
{
   if (const auto* lock_instr = boost::get<program_model::lock_instruction>(&instruction))
   {
      return lock_instr->operation() == program_model::lock_operation::Unlock;
   }
   return false;
}

//-------------------------------------------------------------------------------------------------

bool one_lock(const instruction_t& instruction_1, const instruction_t& instruction_2)
{
   return (is_lock(instruction_1) || is_trylock(instruction_1)) || 
      (is_lock(instruction_2) || is_trylock(instruction_2));
}

//-------------------------------------------------------------------------------------------------


bool lock_unlock_same_object(const instruction_t& instruction_1, const instruction_t& instruction_2)
{
   bool b = same_operand(instruction_1, instruction_2) &&
          ((is_lock(instruction_1) && is_unlock(instruction_2)) ||
           (is_unlock(instruction_1) && is_lock(instruction_2)));
           DEBUGF("\tDependence", "lock_unlock_same_obj", 
           boost::apply_visitor(program_model::instruction_to_short_string(), instruction_1) + ", " +
           boost::apply_visitor(program_model::instruction_to_short_string(), instruction_2), (b ? "true\n\n" : "false\n\n"));
           return b;
}

} // end namespace

//-------------------------------------------------------------------------------------------------


bool Dependence::dif_read_write()
{
   return true;
}

//-------------------------------------------------------------------------------------------------

bool Dependence::dependent(const instruction_t& instruction_1, const instruction_t& instruction_2)
{    
   bool b = same_thread(instruction_1, instruction_2) ||
          (same_operand(instruction_1, instruction_2) &&
           (one_memory_modification(instruction_1, instruction_2) ||
            one_lock(instruction_1, instruction_2)));
            
            DEBUGF("\tDependence", "dependent", 
            boost::apply_visitor(program_model::instruction_to_short_string(), instruction_1) + ", " +
            boost::apply_visitor(program_model::instruction_to_short_string(), instruction_2), (b ? "true\n" : "false\n"));
            return b;
}

//-------------------------------------------------------------------------------------------------

bool Dependence::coenabled(const instruction_t& instruction_1, const instruction_t& instruction_2)
{
   bool b = !same_thread(instruction_1, instruction_2) && !lock_unlock_same_object(instruction_1, instruction_2);
DEBUGF("\tDependence", "coenabled", 
boost::apply_visitor(program_model::instruction_to_short_string(), instruction_1) + ", " +
boost::apply_visitor(program_model::instruction_to_short_string(), instruction_2), (b ? "true\n\n" : "false\n\n"));
return b;
}

//-------------------------------------------------------------------------------------------------

} // end namespace exploration
