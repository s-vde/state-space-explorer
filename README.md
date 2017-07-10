# State-Space Explorer
Framework for exploring the state-space of multithreaded programs.

---

## Cloning and Building State-Space Explorer 

#### Cloning the Repository

State-Space Explorer uses 
[cpp-utils](https://github.com/s-vde/cpp-utils) and
[record-replay](https://github.com/s-vde/record-replay) as submodules. Therefore, clone recursively:

```
git clone --recursive https://github.com/s-vde/state-space-explorer.git
```

Record-Replay requires LLVM and Clang 4.0 (see also [README](https://github.com/s-vde/record-replay/blob/master/README.md)).

#### Build State-Space Explorer

State-Space Explorer can be built using cmake. By default the build process will look for and use an installed LLVM. A custom built LLVM can be specified as follows:

```
cmake -DLLVM_DIR=<path_to_llvm_build_dir>/lib/cmake/llvm 
```

The build process will produce a number of executables, corresponding to different exploration modes:
* `depth_first_search`: implements a simple depth first search on the state-space;
* `bounded_search`: implements a depth first search on the state-space bounded by a given bound function on executions;
* `dpor`: implements a classical dynamic partial order reduction.

---

## Using State-Space Explorer

The State-Space Explorer can be run as follows:

```
./depth_first_search <program_dir> <program> <nr_threads> <max_nr_executions>
```

```
./bounded_search <bound_function> <bound_value> <program_dir> <program> <nr_threads> <max_nr_executions>
```

```
./dpor <mode> <program_dir> <program> <nr_threads> <max_nr_executions>
```

where
- `<mode> in { persistent }`
- `<bound_function> in { preemptions }`
- `<bound_value>` is an integer
- `<program_dir>` is the directory containing the input program source files
- `<program>` is the name of the input program, without extension, and without suffix corresponding to the number of threads
- `<nr_threads>` is the number of threads in the input program
- `<max_nr_executions>` is the maximal number of executions that the exploration is allowed to see.

The exploration dumps records of the performed executions
and a .dot representation of the explored part of the input program's execution tree.

---

## Example Programs

A small set of multithreaded program examples in C and C++ can be found in the repository [multithreaded-c](https://github.com/s-vde/multithreaded-c).
