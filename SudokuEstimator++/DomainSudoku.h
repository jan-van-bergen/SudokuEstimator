#pragma once
#include "Sudoku.h"

template<int N>
struct Domain_Sudoku : public Sudoku<N> {
	static constexpr int size = Sudoku<N>::size;

    // 'constraints' stores a 1d array for each cell (x, y) in the Sudoku
    // It contains at position [(x + y*size)*size + v - 1] the amount of variables in the row, column and block of position (x, y), that have value v.
    // Thus if the value stored is 0, it means that v is in the domain of (x, y), as no other variable constrains that value.
    // This setup allows for fast checking of domains
	int constraints [size * size * size];
	int domain_sizes[size * size];

	inline Domain_Sudoku() {
		memset(constraints, 0, sizeof(constraints));
		
		for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
				domain_sizes[INDEX(i, j)] = size;
			}
		}
	}

	inline void reset() {
        for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
				int index = INDEX(i, j);

				Sudoku<N>::grid[index] = 0;

				for (int value = 0; value < size; value++) {
                    constraints[index * size + value] = 0;
				}

                domain_sizes[index] = size;
			}
		}
    }

	inline bool is_valid_move(int x, int y, int value) const {
		return constraints[INDEX(x, y) * size + value - 1] == 0;
	}

	// Gets the domain of cell (x, )
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
		assert(Sudoku<N>::get(x, y) != 0 || domain_size == domain_sizes[INDEX(x, y)]);

		return domain_size;
	}
	
	inline bool set_with_forward_check(int x, int y, int value) {
		bool valid = true;

		int index = INDEX(x, y);

		if (value == 0) { 
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

		int oldValue = constraints[index * size + value];
		int newValue = constraints[index * size + value] += change;

		if (oldValue == 1 && newValue == 0) {
			// The value was added to the domain,
			domain_sizes[index]++;
		} else if (oldValue == 0 && newValue == 1) {
			// The value was removed from the domain
			domain_sizes[index]--;
		}

		// Return true if the domain isn't empty yet
		return domain_sizes[index] != 0;
	}

	inline bool update_domains(int x, int y, int value, int change) {
		bool valid = true;

		int index = INDEX(x, y) * size;

		// All values other than 'value' can be removed from the domain at (x, y)
		for (int i = 0;         i < value; i++) constraints[index + i] += change;
		for (int i = value + 1; i < size;  i++) constraints[index + i] += change;

		// Calculate current block bounds
		int bx = N * (x / N);
		int by = N * (y / N);
		int bxe = bx + N;
		int bye = by + N;

		// Update all domains in the current row, skipping the cells that are also in the current block
		for (int i = 0;   i < bx;   i++) valid &= update_domain(i, y, value, change);
		for (int i = bxe; i < size; i++) valid &= update_domain(i, y, value, change);

		// Update all domains in the current column, skipping the cells that are also in the current block
		for (int j = 0;   j < by;   j++) valid &= update_domain(x, j, value, change);
		for (int j = bye; j < size; j++) valid &= update_domain(x, j, value, change);

		// Update all domains in the current block, except for the cell at (x, y)
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
