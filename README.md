# LLVM JIT: An Example

This code builds a minimal JIT engine to execute LLVM-IR.
It does not cover generation of LLVM-IR.

## Build
  - Adding repository for [LLVM-9](https://apt.llvm.org/)
    * For Ubuntu 18.04
      ```bash
      sudo apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-9 main
      ```
    * Others: Visit the LLVM apt website above and follow instructions.
  - `sudo apt-get install libllvm9 llvm-9-dev clang-9`
  - `make`

This should produce an executable named `lljit.exe`.

## Run
  - This will JIT compile the LLVM-IR in `examples/helloworld.ll` and
    execute the function `jitentry` in it. It should print
    "Hello world" on your terminal.
     ```bash
     ./lljit.exe examples/helloworld.ll
     ```
  - The helloworld example LLVM-IR was generated from
    its C file using the command
    ```bash
    clang-9 -emit-llvm -S helloworld.c
    ```

### Testsuite
Building the project should also build `testsuite.exe`, which can
simply be run as `./testsuite.exe`, and it will JIT compile and
run the `helloworld.ll` example described above.
