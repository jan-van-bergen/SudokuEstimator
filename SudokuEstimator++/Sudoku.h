#pragma once
#include <bitset>
#include <cassert>

#define INDEX(i, j) (i + j * size)

template<int N> // N is the size of a block, meaning for a 9x9 sudoku N = 3
struct Sudoku {
	static constexpr int size = N * N;

	int grid[size * size];
	std::bitset<size * size> is_fixed;

	inline Sudoku() {
		for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
				grid    [INDEX(i, j)] = 0;
				is_fixed[INDEX(i, j)] = false;
			}
		}
	}

	// Records all non-zero cells as fixed
	inline void set_current_state_as_start_state() {
        for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
                is_fixed[INDEX(i, j)] = grid[INDEX(i, j)] != 0;
			}
		}
    }

	// Resets all non-fixed cells to zero
    inline void reset_to_start_state() {
        for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
                if (!is_fixed[INDEX(i, j)]) {
                    grid[INDEX(i, j)] = 0;
				}
			}
		}
    }

	// Returns true if the cell (x, y) can assume the given value, given the current state of the grid
	inline bool is_valid_move(int x, int y, int value) const {
		// Check if row rule would be violated
		for (int i = 0; i < size; i++) {
			if (grid[INDEX(i, y)] == value) {
				return false;
			}
		}

		// Check if column rule would be violated
		for (int j = 0; j < size; j++) {
			if (grid[INDEX(x, j)] == value) {
				return false;
			}
		}

		// Check if block rule would be violated
		int bx = N * (x / N);
		int by = N * (y / N);

		for (int j = by; j < by + N; j++) {
			for (int i = bx; i < bx + N; i++) {
				if (grid[INDEX(i, j)] == value) {
					return false;
				}
			}
		}

		// The value has passed all checks, so it's valid
		return true;
	}

	inline bool is_valid_solution() const {
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				int current_value = grid[INDEX(x, y)];

				if (current_value < 1 || current_value > size) return false;

				// Check row, skipping the current cell
				for (int i = 0; i < size; i++) {
					if (i != x && grid[INDEX(i, y)] == current_value) {
						return false;
					}
				}

				// Check column, skipping the current cell
				for (int j = 0; j < size; j++) {
					if (j != y && grid[INDEX(x, j)] == current_value) {
						return false;
					}
				}

				// Check block, skipping the current cell
				int bx = N * (x / N);
				int by = N * (y / N);

				for (int j = by; j < by + N; j++) {
					for (int i = bx; i < bx + N; i++) {
						if ((i != x || j != y) && grid[INDEX(i, j)] == current_value) {
							return false;
						}
					}
				}
			}
		}

		return true;
	}

	inline int get(int x, int y) const {
		return grid[INDEX(x, y)];
	}

	inline void set(int x, int y, int value) {
		assert(!is_fixed[INDEX(x, y)]);

		grid[INDEX(x, y)] = value;
	}

	inline bool is_free(int x, int y) const {
		return !is_fixed[INDEX(x, y)];
	}
};