# LLVM JIT: An Example

This code builds a minimal JIT engine to execute LLVM-IR.
It does not cover generation of LLVM-IR.

## Build
  - Adding repository for [LLVM-9](https://apt.llvm.org/)
    * For Ubuntu 18.04
      `sudo apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-9 main`
    * Others: Visit the LLVM apt website above and follow instructions.
  - `sudo apt-get install libllvm9 llvm-9-dev clang-9`
  - `make`
  
This should produce an executable named `lljit.exe`.

## Run
  - `./lljit.exe examples/helloworld.ll`. This will
  JIT compile the LLVM-IR in `examples/helloworld.ll` and
  execute the function in it named `jitentry`. It will print
  "Hello world" on your terminal.
  - The helloworld example LLVM-IR was generated from
  its C file using the command `clang-9 -emit-llvm -S helloworld.c`
