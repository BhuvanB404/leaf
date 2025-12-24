Leaf — Minimal Compiler (C++)

A small example compiler implemented in C++ that parses a tiny language, generates x86-64 assembly, assembles it with NASM and links it to produce an executable.

Prerequisites
- A C++17-compatible compiler 
- NASM 


Quick start

Clone the repository :

```bash
git clone https://github.com/BhuvanB404/leaf.git
cd leaf
```

Build

Compile the compiler driver :

```bash
g++ -std=c++17 -O2 -I src -o leaf src/main.cpp
```

Notes:
- The program expects a source file in the project-specific `.hy` format (example below).
- The compiler will generate `out.asm`, then invoke `nasm -felf64 out.asm` and `ld -o out out.o` to produce an executable called `out`.

Makefile

A minimal `Makefile` can automate the build steps. If a `Makefile` is present in this directory, use:

```bash
make        
make run    
make clean  
```


Create a minimal input file `example.hy` with a single exit statement:

```c
exit(0);
```

Then run:

```bash
./leaf example.hy

./out
```

Cleaning

```bash
rm -f leaf out out.o out.asm
```

License

This directory is licensed under the MIT License — see the included `LICENSE` file for details.



