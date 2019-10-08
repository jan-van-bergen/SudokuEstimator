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
struct Domain_Sorted_Traverser {
private:
	static constexpr int index_count = Domain_Sudoku<N>::size * Domain_Sudoku<N>::size;

	int * indices;
	int * counters;

public:
	int x;
	int y;

	inline Domain_Sorted_Traverser() {
		indices = new int[index_count];

		int index = 0;
		for (int j = 0; j < Domain_Sudoku<N>::size; j++) {
			for (int i = 0; i < Domain_Sudoku<N>::size; i++) {
				indices[index] = index;
				index++;
			}
		}

		counters = new int[Domain_Sudoku<N>::size * Domain_Sudoku<N>::size];
	}

	inline void seek_first(const Domain_Sudoku<N> * sudoku) {
		// Sort cell indices by their domain sizes
		std::sort(indices, indices + index_count, [&sudoku](int a, int b) {
			return sudoku->domain_sizes[a] < sudoku->domain_sizes[b];
		});

		x = indices[0] % Domain_Sudoku<N>::size;
		y = indices[0] / Domain_Sudoku<N>::size;

		counters[x + y * Domain_Sudoku<N>::size] = 1;
	}

	inline bool move(const Sudoku<N> * sudoku) {
		do {
			int counter = counters[x + y * Domain_Sudoku<N>::size];

			if (counter == Domain_Sudoku<N>::size * Domain_Sudoku<N>::size) return true;

			x = indices[counter] % Domain_Sudoku<N>::size;
			y = indices[counter] / Domain_Sudoku<N>::size;

			counters[x + y * Domain_Sudoku<N>::size] = counter + 1;
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