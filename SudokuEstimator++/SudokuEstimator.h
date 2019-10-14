#pragma once
#include <random>

#include "BigInteger.h"

#include "ScopedTimer.h"

#include "AC3.h"

#include "Sudoku.h"
#include "SudokuTraverser.h"

constexpr int s = 6;
constexpr int N = 3;

struct SudokuEstimator {
private:
	Sudoku<N> sudoku; // N^2 x N^2 Sudoku

	// Amount of N x N^2 Latin Rectangles, this constant can be used to speed
	// up the process of estimating the amount of valid N^2 x N^2 Sudoku Grids
	Big_Integer latin_rectangle_count; 

	//Row_Right_Column_Down_Traverser<N> traverser; 
	Most_Constrained_Traverser<N> traverser;

	static constexpr int coordinate_count = Sudoku<N>::size * (Sudoku<N>::size - N);
	int coordinates[coordinate_count];

	// Variables used for uniform random number generation
	std::random_device random_device;
	std::mt19937       rng;

	// Uses backtracking to count all possible valid Sudoku solutions, given the current configuration of the grid
	void backtrack_with_forward_check(Big_Integer& total);

	// Takes a random walk of length s through the tree of all possible Sudokus
	void knuth(Big_Integer& result);

	// Gives and estimate of the amount of valid Sudoku grids,
	// using a combination of Knuth's algorithm and backtracking
	void estimate_solution_count(Big_Integer& estimate, Big_Integer& backtrack);

public:
	void run();
};