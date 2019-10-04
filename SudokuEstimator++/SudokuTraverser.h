#pragma once
#include <cassert>

struct RowRightColumnDownTraverser {
    int x;
	int y;

	inline RowRightColumnDownTraverser() {
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