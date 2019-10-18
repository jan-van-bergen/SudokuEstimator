#pragma once
#include <mutex>
#include <random>

#include "BigInteger.h"

#include "Sudoku.h"
#include "SudokuTraverser.h"

constexpr int N = 3;
constexpr int M = 4;
constexpr int random_walk_length = 8;

static_assert(N <= M, "Values of N and M should be swapped such that N <= M");

constexpr unsigned int BATCH_SIZE = 100;

using Sudoku_NxM = Sudoku<N, M>; // Assertions cannot contain commas because they are macros, this alias is used to circumvent this.

struct SudokuEstimator {
private:
	Sudoku<N, M> sudoku; // N*M x N*M Sudoku

	//Row_Right_Column_Down_Traverser<N> traverser; 
	MostConstrainedTraverser<N, M> traverser;

	static constexpr int coordinate_count = Sudoku<N, M>::size * (Sudoku<N, M>::size - M);
	int coordinates[coordinate_count];

	static_assert(random_walk_length <= coordinate_count, "Length of the random walk cannot be longer than the available number of cells.");

	BigInteger estimate;
	BigInteger backtrack;

	// Variables used for uniform random number generation
	std::random_device random_device;
	std::mt19937       rng;

	// Uses backtracking to count all possible valid Sudoku solutions, given the current configuration of the grid
	void backtrack_with_forward_check();

	// Takes a random walk of length 'random_walk_length' through the tree of all possible Sudokus
	void knuth();

	// Gives and estimate of the amount of valid Sudoku grids,
	// using a combination of Knuth's algorithm and backtracking
	void estimate_solution_count();

public:
	void run();
};

// Function that runs in its own thread and reports the results of all the other threads.
void report_results();
