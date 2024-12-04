# fast-conv

This project started as a hobby implementation of the 1D cellular automata "rule 30", proposed by Steven Wolfram.
The cellular automata is represented as a list of cells with a boolean state. The state of a cell at the next iteration is computed based on:
  * itself
  * the left and right neighbours
  * a seed

The algorithm describing the evolution look like:

```(python)
for i in range(1,n-1):
    new[i] = f(old[i-1], old[i], old[i+1], seed)

```

I was fascinated by this automata because the algorithm has the same data access pattern as the convolution of a 1D array with a 1x3 kernel.


## Convolutions
Are common numerical procedures in scientific computing and image processing. The critical aspect of fast implementations is the data reuse as adjacent elements share 2/3 of the memory reads.

```
          i    i+2
        / | \ / | \
  ...  x  x  x  x  ...
           \ | /
            i+1
```

Convolutions have been implemented with great success in GPUs (vectorized, multicore, multithreaded processors). The data reuse problem is solved by moving the overlapping reads to smaller and faster memories (local memories, using CUDA notation). It turns out that also *CPUs are vecotrized, multicore, multithreaded processors!* We could transfer the skills of GPGPU computing in the realm of CPU vectorization (SIMD).


## SIMD in the CPU
Modern CPUs implement SIMD (single instruction, multiple data) operations like GPUs, but with even greater control over the architecture. GPU threads can only communicate via the memory (local or global, right? please correct me) while SIMD lanes have data shuffling operations like shift and rotations, let's look at an example:

```
(0 1 2 3) conv (a b c) with periodic boundary is

      a * (3 0 1 2)
    + b * (0 1 2 3)
    + c * (1 2 3 0)

```

The first and the second addends can be obtained by rotating the SIMD register (0,1,2,3): this alone is a promise of a speedup. More general boundary conditions can be easily implemented.


## Experiments
SIMD programs compromise on readability and portability (i.e. AVX instructions are only available for x86 architectures). For this reason I decided to use a trick: if the cell state is stored in 8 bits then by using bigger size integers like int32 or int64 we can emulate SIMD add, shift and rotate. Future implementations will focus on the x86 target architecture or could also explore existing SIMD libraries for C++.
Here are the implementations available at `automata.cpp`:

  * *corner cases*: performs convolution on the inner cells, manually sets the extremal cells
  * *reuse*: does only one memory read per iteration, shuffles the overlapped data into a local array
  * *full reuse*: like reuse but unrolls the loop to remove local array shuffling
  * *packed*: packs cells into a bigger integer, performs SIMD-like operations

![plot](./img/automata/speedup.png)
