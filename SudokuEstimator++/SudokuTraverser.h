#pragma once
#include "Sudoku.h"

struct Row_Right_Column_Down_Traverser {
	int x;
	int y;

	inline Row_Right_Column_Down_Traverser() {
		x = 0;
		y = 0;
	}

	template<int N>
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
	int * indices;
	int * counters;

public:
	int x;
	int y;

	inline Domain_Sorted_Traverser(const Domain_Sudoku<N> * sudoku) {
		indices = new int[Domain_Sudoku<N>::size * Domain_Sudoku<N>::size];
            
		int index = 0;
		for (int j = 0; j < Domain_Sudoku<N>::size; j++)
			for (int i = 0; i < Domain_Sudoku<N>::size; i++)
				indices[index++] = i + j * Domain_Sudoku<N>::size;

		// Sort cell indices by their domain sizes
		std::sort(indices, indices + sizeof(indices) / sizeof(int), [&sudoku](int a, int b) {
			return sudoku->domain_sizes[a] < sudoku->domain_sizes[b];
		});

		x = indices[0] % Domain_Sudoku<N>::size;
		y = indices[0] / Domain_Sudoku<N>::size;

		counters = new int[Domain_Sudoku<N>::size * Domain_Sudoku<N>::size];
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