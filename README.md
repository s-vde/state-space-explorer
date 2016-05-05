# state-space-explorer
Framework for exploring the state-space of multithreaded programs.

### Dependencies
The *state-space-explorer* project uses the following subprojects which can be cloned/downloaded separately from this github account:
- *cpp-utils*: https://github.com/s-vde/cpp-utils
- *record-replay*: https://github.com/s-vde/record-replay

The path of these subprojects relative to the *state-space-explorer* project root 
have to be set in `Makefile.config`. 
See the README of *record-replay* for instructions on how to build the LLVM project context required for building *record-replay*.

The repository *multithreaded-c* (https://github.com/s-vde/multithreaded-c)
contains a small collection of multithreaded programs to run the exploration on.

### Using the explorer

The explorer can be built running

`make explorer`

and run with

`./explore [mode] [dir] [program] [nr_threads] [max_nr_executions]`

where 
- `[mode] in { bs, dpor_persistent, dpor_source, bpor }`
- `[dir]` is the directory containing the input program source files
- `[program]` is the name of the input program, without extension, and without suffix corresponding to the number of threads
- `[nr_threads]` is the number of threads in the input program
- `[max_nr_executions]` is the maximal number of executions that the exploration is allowed to see.

The exploration dumps records of the performed executions 
and a .dot representation of the explored part of the input program's execution tree.
