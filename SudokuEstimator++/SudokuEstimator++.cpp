#include <iostream>

#include "ScopedTimer.h"

#include "AC3.h"

#include "DomainSudoku.h"
#include "SudokuTraverser.h"

//#include "BigInteger.h"
#include "BigInt.h"

template<typename Sudoku_Traverser, int N>
bool backtrack_with_forward_check(Domain_Sudoku<N> * sudoku, Sudoku_Traverser * traverser) {
	int current_x = traverser->x;
	int current_y = traverser->y;
	
	int domain[Domain_Sudoku<N>::size];
	int domain_size = sudoku->get_domain(current_x, current_y, domain);

	assert(domain_size == sudoku->domain_sizes[current_x + current_y * Domain_Sudoku<N>::size]);

    // Try all possible values for the cell at (x, y)
    for (int i = 0 ; i < domain_size; i++) {
		int value = domain[i];
		
        // Try the current value
		if (sudoku->set_with_forward_check(current_x, current_y, value)) {
			if (traverser->move(sudoku)) return true;

			// If this value solved the sudoku return true,
			if (backtrack_with_forward_check(sudoku, traverser)) return true;
		}

		traverser->x = current_x;
		traverser->y = current_y;

        sudoku->set_with_forward_check(current_x, current_y, 0);
    }

    // No solutions possible with current configuration, so backtrack
    return false;
}

template<int N>
void load_sudoku_from_file(Domain_Sudoku<N> * sudoku, const char * file_path) {
	FILE * file = NULL;
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
				
					abort(); // ERROR: End Of File reached before end of Sudoku!
				}

				if (c == ' ' || c == '\n') break;

				assert(buffer_index < 8);
				buffer[buffer_index++] = c;
			}

			// NULL terminate the string
			buffer[buffer_index] = NULL;

			// Convert newly read string to int
			int value = atoi(buffer);

			sudoku->set_with_forward_check(i, j, value);
		}
	}

	fclose(file);

	sudoku->set_current_state_as_start_state();
}

int main() {
	constexpr int N = 4;
	Domain_Sudoku<N> sudoku;

	load_sudoku_from_file(&sudoku, "./Data/Sudoku_16_1.txt");

	{
		Scoped_Timer timer("AC3");

		ac3(&sudoku);
	}

	{
		Scoped_Timer timer("Solver");
		
		//Row_Right_Column_Down_Traverser traverser; 
		Domain_Sorted_Traverser<N> traverser(&sudoku); 

		bool success = backtrack_with_forward_check(&sudoku, &traverser);

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
