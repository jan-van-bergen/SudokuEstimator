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

	inline void seek_first(const Domain_Sudoku<N> * sudoku) {
		move(sudoku);
	}

	inline bool move(const Domain_Sudoku<N> * sudoku) {
		int smallest_domain = Sudoku<N>::size;
		int smallest_x = -1;
		int smallest_y = -1;
        
		// Loop through the sudoku based on the precalculated domain sizes
		// This will slightly reduce search time in the beginning, as a lot the first domain sizes are all 1 
		for (int y = 1; y < Sudoku<N>::size; y++) {
			if (y % N == 0) continue; // These rows are always filled with numbers obtained from a N x N^2 Latin Rectangle, no need to check their domains

			for (int x = 0; x < Sudoku<N>::size; x++) {
				// Check if the spot at (x, y) is free
				if (sudoku->get(x, y) == 0) {
					int domain_size = sudoku->domain_sizes[x + y * sudoku->size];

					assert(domain_size > 0);

					// Check if it's domain is smaller than the currently found smallest domain
					if (domain_size < smallest_domain) {
						smallest_domain = domain_size;
						smallest_x = x;
						smallest_y = y;

						// Once we've found a domain of size 1, we don't have to keep looking for a smaller domain
						//if (smallest_domain == 1) break;
					}
				}
			}
		}

		// If the Sudoku is completed, return true.
		if (smallest_x == -1) return true;

		x = smallest_x;
		y = smallest_y;

		return false;
	}
};