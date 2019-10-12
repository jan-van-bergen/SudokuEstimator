# Monte Carlo Sudoku Solution Estimator

Implementation of a Monte Carlo style algorithm that estimates the amount of valid solutions to a Sudoku of arbitrary size.

Original idea based on <a>https://theartofmachinery.com/2017/08/14/monte_carlo_counting_sudoku_grids.html</a>

TODO:

* [ ] ~~Look into faster Latin Rectangle generation.~~ Although asymptotically better algorithms exist, for small N the current approach is fast enough.
* [ ] Multithreading
* [x] ~~Custom BigInteger class~~ Integrated MPIR library.

# Dependencies
- MPIR 3.0.0 - https://github.com/wbhart/mpir - Highly optimized math library for large numbers.
