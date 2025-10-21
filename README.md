# White Bird Engine

> A free game engine.
>
> Licensed under [Apache License Version 2.0](https://github.com/OppositeNor/white-bird-engine-core/blob/master/LICENSE).

- [简体中文](./README.zh-Hans.md)

This is the repository for the core layer of White Bird Engine. The full engine is still under development.

## How to build

Clone the repository:

```sh
git clone https://github.com/OppositeNor/white-bird-engine.git --recursive
cd white-bird-engine
```

Or:

```sh
git clone https://github.com/OppositeNor/white-bird-engine.git
cd white-bird-engine
git submodule update --init --recursive
```

Install build dependencies:

```sh
sudo pacman -S cmake clang ninja directx-shader-compiler
```

It is suggested to use Anaconda for setting up the build environment.

You can install Anaconda from [here](https://github.com/conda-forge/miniforge).

### Setting up build environment

First create a conda environment.

```sh
conda env create -n white-bird-engine python=3.13
conda activate white-bird-engine
```

Install the dependencies from `requirements.txt`

```sh
pip install -r requirements.txt
```

Run the python script to build the project.

### Build

```sh
python ./build.py
```

You can also indicate the build target.

```sh
python ./build.py -t debug          # Debug target
python ./build.py -t release        # Release target
python ./build.py -t deploy         # Deploy target (default)
python ./build.py -t debug-gcc      # Debug target with GCC compiler
python ./build.py -t release-gcc    # Release target with GCC compiler
python ./build.py -t deploy         # Deploy target with GCC compiler
```

## Benchmark results

```
Allocators (shuffle destruction order):
----------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations
----------------------------------------------------------------------------------------------------------
malloc_free_benchmark_with_shuffle                                     679 ns          676 ns       939593
heap_allocated_aligned_pool_benchmark_with_shuffle                    1460 ns         1456 ns       468061
heap_allocated_aligned_pool_impl_list_benchmark_with_shuffle           144 ns          144 ns      4848775
```

```
Allocators (ordered destruction):
----------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations
----------------------------------------------------------------------------------------------------------
malloc_free_benchmark_without_shuffle                                  679 ns          655 ns      1083339
heap_allocated_aligned_pool_benchmark_without_shuffle                  127 ns          125 ns      5411608
heap_allocated_aligned_pool_impl_list_benchmark_without_shuffle        141 ns          138 ns      5159772
```
