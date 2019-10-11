#pragma once
#include "Sudoku.h"

template<int N>
struct Row_Right_Column_Down_Traverser {
	int x;
	int y;

	inline void seek_first(const Sudoku<N> * sudoku) {
		x = -1;
		y = 0;

		move(sudoku);
	}

    inline bool move(const Sudoku<N> * sudoku) {
		do {
			x++;

			if (x == sudoku->size) { 
				x = 0; 
				y++; 

				if (y == sudoku->size) {
					return true;
				}
			}
		} while (sudoku->get(x, y) != 0);

		return false;
	}
};

template<int N>
struct Most_Constrained_Traverser {
	int x;
	int y;

	inline Most_Constrained_Traverser() {
		x = -1;
		y = -1;
	}

	inline void seek_first(const Sudoku<N> * sudoku) {
		move(sudoku);
	}

	inline bool move(const Sudoku<N> * sudoku) {
		// If this is the case, we can initialize smallest_domain with Sudoku<N>::size, saving 1 (potential) swap
		assert(sudoku->empty_cells_length != Sudoku<N>::size * Sudoku<N>::size);

		int smallest_domain = Sudoku<N>::size;
		int smallest_x = -1;
		int smallest_y = -1;

		// Check the domain sizes of all empty cells
		for (int i = 0; i < sudoku->empty_cells_length; i++) {
			int index       = sudoku->empty_cells[i];
			int domain_size = sudoku->domain_sizes[index];
			
			if (domain_size < smallest_domain) {
				smallest_domain = domain_size;
				smallest_x = Sudoku<N>::get_x_from_index(index);
				smallest_y = Sudoku<N>::get_y_from_index(index);

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