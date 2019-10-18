#pragma once
#include "Sudoku.h"

template<int N, int M>
struct MostConstrainedTraverser {
	int x;
	int y;

	inline MostConstrainedTraverser() {
		x = -1;
		y = -1;
	}

	inline void seek_first(const Sudoku<N, M> * sudoku) {
		move(sudoku);
	}

	inline bool move(const Sudoku<N, M> * sudoku) {
		// If this is the case, it means there is at least 1 cell filled in, meaning not all domain sizes are N*M
		// We can thus initialize smallest_domain with Sudoku<N, M>::size, saving 1 (potential) swap
		assert(sudoku->empty_cells_length < Sudoku_NxM::size * Sudoku_NxM::size);

		int smallest_domain = Sudoku<N, M>::size;
		int smallest_x = -1;
		int smallest_y = -1;

		// Check the domain sizes of all empty cells
		for (int i = 0; i < sudoku->empty_cells_length; i++) {
			int index       = sudoku->empty_cells[i];
			int domain_size = sudoku->domain_sizes[index];
			
			if (domain_size < smallest_domain) {
				smallest_domain = domain_size;
				smallest_x = Sudoku<N, M>::get_x_from_index(index);
				smallest_y = Sudoku<N, M>::get_y_from_index(index);

				if (smallest_domain == 1) goto early_out;
			}
		}
		
		// If the Sudoku is completed, return true.
		if (smallest_x == -1) return true;
 early_out:
		x = smallest_x;
		y = smallest_y;
		
		return false;
	}
};