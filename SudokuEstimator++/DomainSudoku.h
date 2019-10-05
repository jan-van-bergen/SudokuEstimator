#pragma once
#include "Sudoku.h"

template<int N>
struct Domain_Sudoku : Sudoku<N> {
	static constexpr int size = Sudoku<N>::size;

	int domains     [size * size * size];
	int domain_sizes[size * size];

	inline Domain_Sudoku() {
		memset(domains, 0, sizeof(domains));
		
		for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
				domain_sizes[INDEX(i, j)] = size;
			}
		}
	}

	inline bool is_valid_move(int x, int y, int value) {
		return domains[INDEX(x, y) * size + value - 1] == 0;
	}

	// Gets the domain of cell (x, )
	// Stores the resulting domain in result_domain, which should be an array of length >= size
	// The size of the domain is returned
	inline int get_domain(int x, int y, int result_domain[size]) {
		int index = INDEX(x, y);

		int domain_size = 0;

		for (int value = 1; value <= size; value++) {
			if (is_valid_move(x, y, value)) {
				result_domain[domain_size++] = value;
			}
		}

		return domain_size;
	}
	
	inline bool set_with_forward_check(int x, int y, int value) {
		bool valid = true;

		int index = INDEX(x, y);

		if (value == 0 && Sudoku<N>::grid[index] != 0) { 
			// If resetting a value, update all related domains that this grid is no longer a number
			valid = update_domains(x, y, Sudoku<N>::grid[index] - 1, -1);	
		} else if (value != 0) { 
			// If setting a value, update all related domains that this grid is now a number
			valid = update_domains(x, y, value - 1, 1);
		}

		Sudoku<N>::grid[index] = value;

		return valid;
	}
	
private:
	// Updates the domain of the variable at (i, j). Returns true if it's domains isn't empty, false otherwise
	inline bool update_domain(int i, int j, int value, int change) {
		int index = INDEX(i, j);

		int oldValue = domains[index * size + value];
		int newValue = domains[index * size + value] += change;

		if (oldValue == 1 && newValue == 0) {
			// Check if a value was added to the domain,
			domain_sizes[index]++;
		} else if (oldValue == 0 && newValue == 1) {
			// or removed from the domain
			domain_sizes[index]--;
		}

		// Return true if the domain isn't empty yet
		return domain_sizes[index] != 0;
	}

	inline bool update_domains(int x, int y, int value, int change) {
		bool valid = true;

		int index = INDEX(x, y) * size;

		// All values other than 'value' can be removed from the domain at (x, y)
		for (int i = 0; i < size; i++) {
			if (i != value) {
				domains[index + i] += change;
			}
		}

		// Calculate current block bounds
		int bx = N * (x / N);
		int by = N * (y / N);

		int bxe = bx + N;
		int bye = by + N;

		// Update all domains in the current row
		for (int i = 0; i < size; i++) {
			if ((i < bx || i >= bxe)) {
				valid &= update_domain(i, y, value, change);
			}
		}

		// Update all domains in the current column
		for (int j = 0; j < size; j++) {
			if ((j < by || j >= bye)) {
				valid &= update_domain(x, j, value, change);
			}
		}

		// Update all domains in the current block
		for (int j = by; j < bye; j++) {
			for (int i = bx; i < bxe; i++) {
				if (i != x || j != y) {
					valid &= update_domain(i, j, value, change);
				}
			}
		}

		// The domain update was valid if no domains were made empty
		return valid;
	}
};