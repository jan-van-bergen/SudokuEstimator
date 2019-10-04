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
bool backtrack(Sudoku<N> * sudoku, int x, int y) {
	
    // Try all possible values for the cell at (x, y)
    for (int value = 1; value <= sudoku->size; value++)
    {
        // Check if the value is valid for the cell at (x, y)
        if (!sudoku->is_valid_move(x, y, value)) continue;

        // Try the current value
		sudoku->set(x, y, value);
		
		int next_x = x;
		int next_y = y;
		if (SudokuTraverser::move(sudoku, &next_x, &next_y)) return true;
		
        // If this value solved the sudoku return true,
        if (backtrack<SudokuTraverser>(sudoku, next_x, next_y)) return true;
		
        sudoku->set(x, y, 0);
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

	fclose(file);

	sudoku->set_current_state_as_start_state();
}

int main() {
	constexpr int N = 4;
    Sudoku<N> sudoku;

	load_sudoku_from_file(&sudoku, "./Data/Sudoku_16_1.txt");

	{
		ScopedTimer timer("Solver");
		
		bool success = backtrack<RowRightColumnDownTraverser>(&sudoku, 0, 0);

		printf("%s \n", success ? "success" : "fail");
	}

	printf("The solution is %s \n", sudoku.is_valid_solution() ? "valid" : "invalid");

	for (int j = 0; j < sudoku.size; j++) {
		for (int i = 0; i < sudoku.size; i++) {
			printf("%u ", sudoku.get(i, j));
		}

		printf("\n");
	}

	getchar();
}
