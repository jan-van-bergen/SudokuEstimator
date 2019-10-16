#pragma once
#include <cassert>

template<int N, int M = N> // N is the height of a block, M is the width of a block. The width and height of the entire Sudoku are N*M
struct Sudoku {
	static constexpr int size = N * M;

	// Converts 2d grid cell coordinates (i, j) into a one dimensional index in a size * size grid
	inline static constexpr int get_index(int i, int j) {
		return i + j * size;
	}

	// Converts one dimensional index obtainable from get_index back into its x coordinate
	inline static constexpr int get_x_from_index(int index) {
		return index % size;
	}
	
	// Converts one dimensional index obtainable from get_index back into its y coordinate
	inline static constexpr int get_y_from_index(int index) {
		return index / size;
	}

	// Sudoku grid, contains all currently filled in numbers
	// If a number is not filled in, the value is 0
	int grid[size * size];

	// 'constraints' stores a 1d array for each cell (x, y) in the Sudoku
	// It contains at position [(x + y*size)*size + v - 1] the amount of variables in the row, column and block of position (x, y), that have value v.
	// Thus if the value stored is 0, it means that v is in the domain of (x, y), as no other variable constrains that value.
	// This setup allows for fast checking of domains
	int constraints [size * size * size];
	int domain_sizes[size * size];

	int empty_cells      [size * size]; // Keeps a list of indices that are currently empty
	int empty_cells_index[size * size]; // Used to transform a cell index (i, j) into its index in the 'empty_cells' list
	int empty_cells_length;				// Keeps track of the length of 'empty_cells'

	inline Sudoku() {
		reset();
		
		// Lookup table for a function that evaluates whether its input is zero.
		// This is done to prevent branching
		is_zero[0] = 1;
		for (int i = 1; i < size; i++) {
			is_zero[i] = 0;
		}
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
	inline bool is_valid_move(int x, int y, int value) const {
		return constraints[get_index(x, y) * size + value - 1] == 0;
	}

	// Checks if the current configuration is a valid Sudoku grid
	inline bool is_valid_solution() const {
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				int current_value = grid[get_index(x, y)];

				if (current_value < 1 || current_value > size) return false;

				// Check row, skipping the current cell
				for (int i = 0; i < size; i++) {
					if (i != x && grid[get_index(i, y)] == current_value) {
						return false;
					}
				}

				// Check column, skipping the current cell
				for (int j = 0; j < size; j++) {
					if (j != y && grid[get_index(x, j)] == current_value) {
						return false;
					}
				}

				// Check block, skipping the current cell
				int bx = M * (x / M);
				int by = N * (y / N);

				for (int j = by; j < by + N; j++) {
					for (int i = bx; i < bx + M; i++) {
						if ((i != x || j != y) && grid[get_index(i, j)] == current_value) {
							return false;
						}
					}
				}
			}
		}

		// None of the checks failed, the Sudoku is correct
		return true;
	}
	
	// Gets the domain of cell (x, y)
	// Stores the resulting domain in result_domain, which should be an array of length >= size
	// The size of the domain is returned
	inline int get_domain(int x, int y, int result_domain[size]) const {
		int domain_size = 0;

		for (int value = 1; value <= size; value++) {
			if (is_valid_move(x, y, value)) {
				result_domain[domain_size++] = value;
			}
		}

		// For empty cells the domain size should be the same as the cached one
		assert(get(x, y) != 0 || domain_size == domain_sizes[get_index(x, y)]);

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

	// Retrieves the value at cell (x, y)
	inline int get(int x, int y) const {
		return grid[get_index(x, y)];
	}
	
	// Sets the cell at (x, y) to the given value, using forward checking
	// Updates all related domains (cells in the same row, column and block) that the cell has the new value
	// If any of those domains become empty false is returned, true otherwise
	inline bool set_with_forward_check(int x, int y, int value) {
		int index = get_index(x, y);
		
		assert(grid[index] == 0);
		assert(value >= 1 && value <= size);

		// Update all related domains that this grid is now a number
		bool valid = update_domains<+1>(x, y, value - 1);

		grid[index] = value;

		// Remove the current cell from the empty cell list in O(1) time by swapping with the last element in that list
		// First look up the index of the current cell (x, y) in the empty cell list
		int empty_cell_index = empty_cells_index[index];
		// Look up the index of the last cell in the empty cell list
		int last_empty_cell  = empty_cells[empty_cells_length - 1];
		// Where previously the (x, y) cell was stored in the empty cell list we now store the last empty cell
		empty_cells[empty_cell_index] = last_empty_cell;
		// Update the fact that the previously last cell can now be found somewhere else
		empty_cells_index[last_empty_cell] = empty_cell_index;
		// Remove last element from empty cell list
		empty_cells_length--;

		return valid;
	}

	// Resets the cell at (x, y) to zero
	// Updates all related domains (cells in the same row, column and block) that the cell no longer has a value
	inline void reset_cell(int x, int y) {
		int index = get_index(x, y);

		assert(grid[index] != 0);
		assert(empty_cells_length < size * size);

		// Update all related domains that this grid is no longer a number
		update_domains<-1>(x, y, grid[index] - 1);

		grid[index] = 0;

		// Store the cell after the last element in the empty cell list
		empty_cells[empty_cells_length] = index;
		empty_cells_index[index]= empty_cells_length;
		empty_cells_length++;
	}
	
private:
	int is_zero[size]; // Lookup table to avoid branching. Evaluates to 1 if its input is 0, evaluates to 0 for any other input

	// Updates the domain of the variable at (i, j). 
	// Returns true if it's domains isn't empty, false otherwise
	template<int Change> inline bool update_domain(int i, int j, int value);

	template<>
	inline bool update_domain<+1>(int i, int j, int value) {
		int index = get_index(i, j);

		// If there were previously no constraints on this value, now there is one.
		// This means the domain size of the cell at (i, j) has shrunk by 1
		domain_sizes[index] -= is_zero[constraints[index * size + value]++]; 

		return domain_sizes[index] != 0;;
	}

	template<>
	inline bool update_domain<-1>(int i, int j, int value) {
		int index = get_index(i, j);

		// If there was previously a constraint on this value, now there are none.
		// This means the domain size of the cell at (i, j) has increased by 1
		domain_sizes[index] += is_zero[--constraints[index * size + value]];

		return true;
	}

	template<int Change>
	inline bool update_domains(int x, int y, int value) {
		// Calculate current block bounds
		int bx = M * (x / M);
		int by = N * (y / N);
		int bxe = bx + M;
		int bye = by + N;

		bool valid = true;

		// Update all domains in the current row, skipping the cell at (x, y)
		for (int i = 0;   i < x;    i++) valid &= update_domain<Change>(i, y, value);
		for (int i = x+1; i < size; i++) valid &= update_domain<Change>(i, y, value);

		// Update all domains in the current column, skipping the cell at (x, y)
		for (int j = 0;   j < y;    j++) valid &= update_domain<Change>(x, j, value);
		for (int j = y+1; j < size; j++) valid &= update_domain<Change>(x, j, value);

		// Update all domains in the current block, except for the cells in row y and column x
		for (int j = by; j < y; j++) {
			for (int i = bx; i < x; i++) {
				valid &= update_domain<Change>(i, j, value);
			}
			for (int i = x+1; i < bxe; i++) {
				valid &= update_domain<Change>(i, j, value);
			}
		}
		for (int j = y+1; j < bye; j++) {
			for (int i = bx; i < x; i++) {
				valid &= update_domain<Change>(i, j, value);
			}
			for (int i = x+1; i < bxe; i++) {
				valid &= update_domain<Change>(i, j, value);
			}
		}
		
		int index = get_index(x, y) * size;
		// All values other than 'value' can be removed from the domain at (x, y)
		for (int i = 0;         i < value; i++) constraints[index + i] += Change;
		for (int i = value + 1; i < size;  i++) constraints[index + i] += Change;

		// The domain update was valid if no domains were made empty
		return valid;
	}
};
