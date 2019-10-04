// SudokuEstimator++.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

// Moves x and y to the next coordinate
// Returns true if the end of the Sudoku has been reached
bool move(int * x, int * y, int sudoku_size) {
	(*x)++;

	if (*x == sudoku_size) {
		*x = 0;
		(*y)++;

		if (*y == sudoku_size) {
			return true;
		}
	}

	return false;
}

template<typename SudokuTraverser, int N>
bool backtrack(Sudoku<N> * sudoku, SudokuTraverser * traverser) {
	
    // Try all possible values for the cell at (x, y)
    for (int value = 1; value <= sudoku->size; value++)
    {
        // Check if the value is valid for the cell at (x, y)
        if (!sudoku->is_valid_move(traverser->x, traverser->y, value)) continue;

        // Try the current value
		sudoku->set(traverser->x, traverser->y, value);
		
		int old_x = traverser->x;
		int old_y = traverser->y;

		if (traverser->move(sudoku)) return true;

        // If this value solved the sudoku return true,
        if (backtrack(sudoku, traverser)) return true;
		
        // otherwise reset the value at (x, y) to 0 and continue, trying another value
		traverser->x = old_x;
		traverser->y = old_y;

        sudoku->set(traverser->x, traverser->y, 0);
    }

    // No solutions possible
    return false;
}

template<int N>
void load_sudoku_from_file(Sudoku<N> * sudoku, const char * file_path) {
	FILE * file;
	if (fopen_s(&file, file_path, "r") != 0) {
		abort();
	}

	assert(file);

	char buffer[8];
	int  buffer_index = 0;

	for (int j = 0; j < sudoku->size; j++) {
		for (int i = 0; i < sudoku->size; i++) {
			buffer_index = 0;

			// Read until next space, newline or EOF
			while (true) {
				char c = fgetc(file);

				if (c == EOF) {
					if (i == sudoku->size - 1 && j == sudoku->size - 1) {
						break;
					}
				
					abort(); // End Of File reached before end of Sudoku!
				}

				if (c == ' ' || c == '\n') break;

				buffer[buffer_index++] = c;
			}

			// NULL terminate the string
			buffer[buffer_index] = NULL;

			// Convert newly read string to int
			int value = atoi(buffer);

			sudoku->set(i, j, value);
		}
	}

	fclose( file );

	sudoku->set_current_state_as_start_state();
}

int main() {
    Sudoku<3> sudoku;

	load_sudoku_from_file(&sudoku, "./Data/Sudoku_9x9_1.txt");

	{
		ScopedTimer timer("Solver");
		
		RowRightColumnDownTraverser traverser;

		bool success = backtrack(&sudoku, &traverser);

		printf("%s \n", success ? "success" : "fail");
	}

	for (int j = 0; j < sudoku.size; j++) {
		for (int i = 0; i < sudoku.size; i++) {
			printf("%u ", sudoku.get(i, j));
		}

		printf("\n");
	}

	getchar();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
