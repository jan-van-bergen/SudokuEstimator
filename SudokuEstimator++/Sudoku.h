#pragma once
#include <bitset>
#include <cassert>

#define INDEX(i, j) (i + j * size)

template<int N> // N is the size of a block, meaning for a 9x9 sudoku N = 3
struct Sudoku {
	static const int size = N * N;

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
	inline bool is_valid_move(int x, int y, int value) {
		// Check if row rule would be violated
		for (int i = 0; i < size; i++) {
			if (grid[i, y] == value) {
				return false;
			}
		}

		// Check if column rule would be violated
		for (int j = 0; j < size; j++) {
			if (grid[x, j] == value) {
				return false;
			}
		}

		// Check if block rule would be violated
		int bx = N * (x / N);
		int by = N * (y / N);

		for (int j = by; j < by + N; j++) {
			for (int i = bx; i < bx + N; i++) {
				if (grid[i, j] == value) {
					return false;
				}
			}
		}

		// The value has passed all checks, so it's valid
		return true;
	}

	inline int get(int x, int y) {
		return grid[INDEX(x, y)];
	}

	inline void set(int x, int y, int value) {
		assert(!is_fixed[INDEX(x, y)]);

		grid[INDEX(x, y)] = value;
	}
};