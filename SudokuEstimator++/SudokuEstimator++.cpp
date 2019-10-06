#include <iostream>

#include "ScopedTimer.h"

#include "AC3.h"

#include "DomainSudoku.h"
#include "SudokuTraverser.h"

//#include "BigInteger.h"
#include "BigInt.h"

constexpr int s = 25;

constexpr int N = 3;
Domain_Sudoku<N> sudoku;

Row_Right_Column_Down_Traverser<N> traverser; 
//Domain_Sorted_Traverser<N> traverser; 

constexpr int coordinate_count = Sudoku<N>::size * (Sudoku<N>::size - 1);
int coordinates[coordinate_count];

template<typename Sudoku_Traverser, int N>
long long backtrack_with_forward_check(Domain_Sudoku<N> * sudoku, Sudoku_Traverser * traverser) {
	int current_x = traverser->x;
	int current_y = traverser->y;
	
	assert(sudoku->get(current_x, current_y) == 0);

	int domain[Domain_Sudoku<N>::size];
	int domain_size = sudoku->get_domain(current_x, current_y, domain);

	assert(domain_size == sudoku->domain_sizes[current_x + current_y * Domain_Sudoku<N>::size]);

	long long total = 0;

    // Try all possible values for the cell at (x, y)
    for (int i = 0 ; i < domain_size; i++) {
		int value = domain[i];
		
        // Try the current value
		if (sudoku->set_with_forward_check(current_x, current_y, value)) {
			if (traverser->move(sudoku)) {
				// If the Sudoku was completed by this move, add 1 to the solution count
				total += 1LL;
			} else {
				// Otherwise, count the solutions that include this move
				total += backtrack_with_forward_check(sudoku, traverser);
			} 
		}

		traverser->x = current_x;
		traverser->y = current_y;

        sudoku->set_with_forward_check(current_x, current_y, 0);
    }

    return total;
}

BigInt knuth() {
    BigInt D = 1;

	int domain[sudoku.size];

    for (int i = 0; i < s; i++) {
		int coordinate = coordinates[i];

        int x = coordinate & 0x0000ffff;
        int y = coordinate >> 16;

        int domain_size = sudoku.get_domain(x, y, domain);

        if (domain_size == 0) return 0;

        D *= domain_size;

		int random_value_from_domain = domain[rand() % domain_size];

        // Use forward checking for a possible early out
		// If any domain becomes empty the Sudoku can't be completed and 0 can be returned.
        if (!sudoku.set_with_forward_check(x, y, random_value_from_domain)) return 0;
    }

    return D;
}

BigInt estimate_solution_count() {
    sudoku.reset();

    // Fill the first row with 1 to 9
    for (int i = 0; i < sudoku.size; i++) {
		sudoku.set_with_forward_check(i, 0, i + 1); // @TODO: do this only once and include it int the start state, NOTE: domains should be reset to account for this as well
	}

    // Select s random cells from the other rows.
	std::random_shuffle(coordinates, coordinates + coordinate_count);

    // Estime using Knuth's algorithm
    BigInt estimate = knuth();

    if (estimate.is_zero()) return estimate;

    ac3(&sudoku);

	traverser.seek_first(&sudoku);

    // Multiply our estimate, the exact amount of backtracking solutions and 9!

	BigInt backtrack = backtrack_with_forward_check(&sudoku, &traverser);

    return estimate * backtrack * 362880;
}

int main() {
	srand(time(NULL));

	assert(Sudoku<N>::size < (1 << 16));

	int index = 0;
	for (int j = 1; j < Sudoku<N>::size; j++) {
		for (int i = 0; i < Sudoku<N>::size; i++) {
			coordinates[index++] = j << 16 | i;
		}
	}

	const char * true_value;
	switch (Sudoku<N>::size) {
        case 4:  true_value = "288";                    break;
        case 9:  true_value = "6670903752021072936960"; break;
        default: true_value = "Unknown";                break;
    }

	BigInt sum = 0;
	long long n = 0;

	long long duration_sum = 0;
	auto timer = std::chrono::high_resolution_clock::now();

	while (true) {
		BigInt estimate = estimate_solution_count();

		sum += estimate;
		n++;
        
		// Every 100 iterations print the current stats
		if (n % 100 == 0) {
			BigInt avg = sum / n;

			printf("%u: Est: %s\n", n, estimate.to_string().c_str());
			printf("%u: Avg: %s\n", n, avg.to_string().c_str());
			printf("%u: Tru: %s\n", n, true_value);

			long long duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timer).count();
			timer = std::chrono::high_resolution_clock::now();

			if (duration >= 1000000) {
				printf("Duration: %llu us (%llu s)\n", duration, duration / 1000000);
			} else if (duration >= 1000) {
				printf("Duration: %llu us (%llu ms)\n", duration, duration / 1000);
			} else {
				printf("Duration: %llu us\n", duration);
			}

			duration_sum += duration;
			printf("Avg duration: %llu\n", duration_sum / (n / 100));
		}
	}
}
