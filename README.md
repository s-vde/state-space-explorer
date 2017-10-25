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
cmake -DLLVM_BUILD_DIR=<path_to_llvm_build_dir>
```

The build process will produce a number of executables, corresponding to different exploration modes:
* `depth_first_search`: implements a simple depth first search on the state-space;
* `bounded_search`: implements a depth first search on the state-space bounded by a given bound function on executions;
* `dpor`: implements a classical dynamic partial order reduction.

---

## Using State-Space Explorer

### Running State-Space Explorer on the Instrumented Program
 
The State-Space Explorer can be run as follows:

```
./depth_first_search 
    --i <input_program> 
    --max <max_nr_executions>
```

```
./bounded_search
    --i <input_program> 
    --max <max_nr_executions>
    --bound-function <bound_function>
    --bound <bound_value>
```

```
./dpor 
    --i <input_program> 
    --max <max_nr_executions>
    --sufficient-set <sufficient_set>
```

where
- `<sufficient_set> in { persistent }`
- `<bound_function> in { preemptions }`
- `<bound>` is an integer
- `<input_program>` is the name of the input program, without extension, and without suffix corresponding to the number of threads
- `<max_nr_executions>` is the maximal number of executions that the exploration is allowed to see.

Other optional arguments to all exploration modes are:

|              |                             | Default                              |
| ------------ | --------------------------- | ------------------------------------ |
| ```--c```    | ```<compiler_options>```    | ""                                   |
| ```--o```    | ```<output_directory>```    | ```./statespace_explorer_output```   |
| ```--opt```  | ```<optimization_level>```  | 0                                    |

---

## Example Programs

A small set of multithreaded program examples in C and C++ can be found in the repository [multithreaded-c](https://github.com/s-vde/multithreaded-c).
