# Monte Carlo Sudoku Solution Estimator

Implementation of a Monte Carlo style algorithm that estimates the amount of valid solutions to a Sudoku of arbitrary size.

Original idea based on <a>https://theartofmachinery.com/2017/08/14/monte_carlo_counting_sudoku_grids.html</a>

TODO:

* [ ] Use Reduced Latin Rectangles instead of Normalized to further reduce search space
* [ ] Implement better way to keep track of domains, so they can be accessed in `O(1)` time
