#pragma once
#include <cassert>

#include "Generated.h"

template<int N, int M = N> // N is the height of a block, M is the width of a block. The width and height of the entire Sudoku are N*M
struct Sudoku {
	static constexpr int size = N * M;

	// Converts 2d grid cell coordinates (i, j) into a one dimensional index in a size * size grid
	inline static constexpr int get_index(int i, int j) {
		return i + j * size;
	}

	// Sudoku grid, contains all currently filled in numbers
	// If a number is not filled in, the value is 0
	unsigned char grid[size * size];

	// 'constraints' stores a 1d array for each cell (x, y) in the Sudoku
	// It contains at position [(x + y*size)*size + v - 1] the amount of variables in the row, column and block of position (x, y), that have value v.
	// Thus if the value stored is 0, it means that v is in the domain of (x, y), as no other variable constrains that value.
	// This setup allows for fast checking of domains
	unsigned char constraints [size * size * size];
	unsigned char domain_sizes[size * size];

	unsigned char empty_cells      [size * size]; // Keeps a list of indices that are currently empty
	unsigned char empty_cells_index[size * size]; // Used to transform a cell index (i, j) into its index in the 'empty_cells' list
	int empty_cells_length;				// Keeps track of the length of 'empty_cells'

	inline Sudoku() {
		reset();
	}

	// Resets all cells to zero
	// Domains are reset to be the numbers 1 .. N*M
	inline void reset() {
		for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
				int index = get_index(i, j);

				grid[index] = 0;

				for (int value = 0; value < size; value++) {
					constraints[index * size + value] = 0;
				}

				domain_sizes[index] = size;

				empty_cells      [index] = index;
				empty_cells_index[index] = index;
			}
		}

		empty_cells_length = size * size;
	}
	
	// Checks if the cell at (x, y) is allowed to assume the given value
	inline bool is_valid_move(int cell_index, int value) const {
		return constraints[cell_index * size + value] == 0;
	}

	// Gets the domain of cell (x, y)
	// Stores the resulting domain in result_domain, which should be an array of length >= size
	// The size of the domain is returned
	inline int get_domain(int cell_index, int result_domain[size]) const {
		int domain_size = 0;

		for (int value = 0; value < size; value++) {
			if (is_valid_move(cell_index, value)) {
				result_domain[domain_size++] = value;
			}
		}

		// For empty cells the domain size should be the same as the cached one
		assert(grid[cell_index] != 0 || domain_size == domain_sizes[cell_index]);

		return domain_size;
	}
	
	// Prints the Sudoku to the console
	inline void print() const {
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				int value = grid[get_index(x, y)];
				
				// When dealing with double-digit numbers, we might need to
				// add one extra space to ensure proper alignment of numbers
				if constexpr (size > 9) { 
					if (value > 9) {
						printf("%u ", value);
					} else {
						printf("%u  ", value);
					}
				} else {
					printf("%u ", value);
				}
			}

			printf("\n");
		}
	}

	// Sets the cell at (x, y) to the given value, using forward checking
	// Updates all related domains (cells in the same row, column and block) that the cell has the new value
	// If any of those domains become empty false is returned, true otherwise
	inline bool set_with_forward_check(int cell_index, int value) {
		assert(grid[cell_index] == 0);
		assert(value >= 0 && value < size);

		// Update all related domains that this grid is now a number

		bool valid = Generated::table_set[cell_index](domain_sizes, constraints, value);
		grid[cell_index] = value + 1;

		// Remove the current cell from the empty cell list in O(1) time by swapping with the last element in that list
		// - First look up the index of the current cell (x, y) in the empty cell list
		// - Look up the index of the last cell in the empty cell list
		// - Where previously the (x, y) cell was stored in the empty cell list we now store the last empty cell
		// - Update the fact that the previously last cell can now be found somewhere else
		// - Remove last element from empty cell list
		int empty_cell_index = empty_cells_index[cell_index];
		int last_empty_cell  = empty_cells[empty_cells_length - 1];
		empty_cells      [empty_cell_index] = last_empty_cell;
		empty_cells_index[last_empty_cell]  = empty_cell_index;
		empty_cells_length--;

		return valid;
	}

	// Resets the cell at (x, y) to zero
	// Updates all related domains (cells in the same row, column and block) that the cell no longer has a value
	inline void reset_cell(int cell_index) {
		assert(grid[cell_index] != 0);
		assert(empty_cells_length < size * size);

		// Update all related domains that this grid is no longer a number
		Generated::table_reset[cell_index](domain_sizes, constraints, grid[cell_index] - 1);
		grid[cell_index] = 0;

		// Store the cell after the last element in the empty cell list
		empty_cells[empty_cells_length] = cell_index;
		empty_cells_index[cell_index]= empty_cells_length;
		empty_cells_length++;
	}
};
