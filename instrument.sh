#!/bin/bash

####################
# PARAMETERS

state_space_explorer_build=`echo $1`
llvm_base=`echo $2`
input_program=`echo $3`
output_dir=`echo $4`
compiler_options=`echo $5`


####################
# VARIABLES

record_replay_build=${state_space_explorer_build}/libs/record-replay/src

pass_build=${record_replay_build}/llvm-pass/LLVMRecordReplayPass.dylib
pass_name="-instrument-record-replay-lw"
scheduler_build=${record_replay_build}/scheduler

llvm_bin=${llvm_base}/build/bin

input_filename=$(basename "${input_program}")
input_extension="${input_filename##*.}"

compiler_options+=" -O0 -g -pthread -emit-llvm "

if [ ${input_extension} == "c" ]; then
   compiler=${llvm_bin}/clang
else
   compiler=${llvm_bin}/clang++
fi


####################
# INSTRUMENTATION

echo Instrumenting ${input_program}

test -d ${output_dir} || mkdir -p ${output_dir}
${compiler} ${compiler_options} -c ${input_program} -o ${output_dir}/${input_filename}.bc

echo [record-replay] run instrumentation pass
${llvm_bin}/opt -load ${pass_build} ${pass_name} \
    < ${output_dir}/${input_filename}.bc \
    > ${output_dir}/${input_filename}.instrumented.bc

echo [record-replay] create human-readable dump of instrumented file
${llvm_bin}/llvm-dis ${output_dir}/${input_filename}.instrumented.bc \
    -o ${output_dir}/${input_filename}.instrumented.txt

echo [record-replay] link with scheduler library
${compiler} ${output_dir}/${input_filename}.instrumented.bc \
   ${scheduler_build}/libRecordReplayScheduler.dylib -rpath ${scheduler_build} \
   -o ${output_dir}/"${input_filename%.*}"
