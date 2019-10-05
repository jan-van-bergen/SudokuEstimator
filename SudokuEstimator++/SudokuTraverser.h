#pragma once
#include "Sudoku.h"

struct Row_Right_Column_Down_Traverser {
	template<int N>
    inline static bool move(const Sudoku<N> * sudoku, int * x, int * y) {
		do {
			(*x)++;

			if (*x == sudoku->size) { 
				*x = 0; 
				(*y)++; 

				if (*y == sudoku->size) {
					return true;
				}
			}
		} while (sudoku->get(*x, *y) != 0);

		return false;
	}
};