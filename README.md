# Monte Carlo Sudoku Solution Estimator

Implementation of a Monte Carlo style algorithm that estimates the amount of valid solutions to a Sudoku of arbitrary size.

Original idea based on <a>https://theartofmachinery.com/2017/08/14/monte_carlo_counting_sudoku_grids.html</a>

![Figure:_Sudoku 3x3 - Running Average of Estimates](Figures/Sudoku_3x3.png)
Plot of the running average of 5 million estimates of the number of 3x3 Sudoku solutions. The dotted line represents the actual number of Sudoku solutions. The average of the estimates converges quickly and gets really close to the actual number.

### Dependencies
- MPIR 3.0.0 - https://github.com/wbhart/mpir - Highly optimized math library for large numbers.

### How to build
The x64 haswell_avx ``.lib`` for MPIR is included in the repository. If you wish to build for a different platform or for a different architecture, you will need to download the MPIR library from http://www.mpir.org/downloads.html, and build a ``.lib`` for the desired platform.

The rest of the program should work out of the box using Visual Studio 2019. The project uses C++ 17 features such as ``if constexpr``.
