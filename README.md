# state-space-explorer
Framework for exploring the state-space of multithreaded programs.

### Subprojects
The *state-space-explorer* project uses the following sub projects which can be cloned/downloaded separately from this github account:
- *cpp-utils*: https://github.com/s-vde/cpp-utils
- *record-replay*: https://github.com/s-vde/record-replay (which requires LLVM and Clang 3.6.2, see [README](https://github.com/s-vde/record-replay/blob/master/README.md))
- *multithreaded-c*: https://github.com/s-vde/multithreaded-c (optional, containing example input programs)

### Using the explorer
The explorer can be built running

```
mkdir build
cd build
cmake ../ -DRECORD_REPLAY=<abs_path_to_record_replay> -DUTILS_SRC=<abs_path_to_cpp_utils>
make
```

and run with

`./explore <mode> <program_dir> <program> <nr_threads> <max_nr_executions>`

where
- `<mode> in { bs, dpor_persistent, dpor_source, bpor }`
- `<program_dir>` is the directory containing the input program source files
- `<program>` is the name of the input program, without extension, and without suffix corresponding to the number of threads
- `<nr_threads>` is the number of threads in the input program
- `<max_nr_executions>` is the maximal number of executions that the exploration is allowed to see.

The exploration dumps records of the performed executions
and a .dot representation of the explored part of the input program's execution tree.
