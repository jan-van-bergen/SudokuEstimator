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