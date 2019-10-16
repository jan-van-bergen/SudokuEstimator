#include "SudokuEstimator.h"
#include <chrono>

#include "AC3.h"

struct Results {
	std::mutex mutex;

	Big_Integer  sum = 0;
	unsigned int n   = 0;

	unsigned long long time = 0;
} results;

void SudokuEstimator::backtrack_with_forward_check() {
	int current_x = traverser.x;
	int current_y = traverser.y;
	
	assert(sudoku.get(current_x, current_y) == 0);

	int domain[Sudoku<N, M>::size];
	int domain_size = sudoku.get_domain(current_x, current_y, domain);

	assert(domain_size == sudoku.domain_sizes[Sudoku_NxM::get_index(current_x, current_y)]);

	// Try all possible values for the cell at (x, y)
	for (int i = 0 ; i < domain_size; i++) {
		int value = domain[i];
		
		// Try the current value
		if (sudoku.set_with_forward_check(current_x, current_y, value)) {
			if (traverser.move(&sudoku)) {
				// If the Sudoku was completed by this move, add 1 to the solution count
				backtrack += 1;

				//assert(sudoku.is_valid_solution());
			} else {
				// Otherwise, count the solutions that include this move
				backtrack_with_forward_check();
			} 
		}

		traverser.x = current_x;
		traverser.y = current_y;

		sudoku.reset_cell(current_x, current_y);
	}
}

void SudokuEstimator::knuth() {
	estimate = 1;

	int domain[Sudoku<N, M>::size];

	for (int i = 0; i < random_walk_length; i++) {
		int coordinate = coordinates[i];
		int x = coordinate & 0x0000ffff;
		int y = coordinate >> 16;

		assert(y % N != 0); // First row of every block is filled with numbers from a Latin Rectangle
		assert(sudoku.get(x, y) == 0); // Cell should be empty
		
		int domain_size = sudoku.get_domain(x, y, domain);

		if (domain_size == 0) {
			estimate = 0;
			
			return;
		}

		estimate *= domain_size;

		std::uniform_int_distribution<int> distribution(0, domain_size - 1);
		int random_value_from_domain = domain[distribution(rng)];

		// Use forward checking for a possible early out
		// If any domain becomes empty the Sudoku can't be completed and 0 can be returned.
		if (!sudoku.set_with_forward_check(x, y, random_value_from_domain)) {
			estimate = 0;
			
			return;
		}
	}
}

void SudokuEstimator::estimate_solution_count() {
	// Reset all cells to 0 and clear domains
	sudoku.reset();

	// Fill every Nth row with a row from a random M x N*M Latin Rectangle
	// The first row is always 1 .. N*M
	int rows[M][Sudoku<N, M>::size];

	// Initialize each row of the Latin Rectangle with the numbers 1 .. N*M
	for (int row = 0; row < M; row++) {
		for (int i = 0; i < Sudoku<N, M>::size; i++) {
			rows[row][i] = i + 1;
		}
	}

	// Repeat until a valid Latin Rectangle is obtained
	retry: {
		// Randomly shuffle every row but the first one
		for (int row = 1; row < M; row++) {
			std::shuffle(rows[row], rows[row] + Sudoku<N, M>::size, rng);
		}

		// Check if the current permutation of rows is a Latin Rectangle
		for (int row = 1; row < M; row++) {
			for (int i = 0; i < Sudoku<N, M>::size; i++) {
				for (int j = 0; j < row; j++) {
					if (rows[row][i] == rows[j][i]) {
						// Not a valid Latin Rectangle, retry
						goto retry;
					}
				}
			}
		}
	}

	// Fill every Nth row of the Sudoku with a row from the Latin Rectangle
	for (int row = 0; row < M; row++) {
		for (int i = 0; i < Sudoku<N, M>::size; i++) {
			bool domains_valid = sudoku.set_with_forward_check(i, row * N, rows[row][i]);

			assert(domains_valid);
		}
	}

	// Select s random cells from the other rows.
	std::shuffle(coordinates, coordinates + coordinate_count, rng);

	// Estime using Knuth's algorithm
	knuth();

	if (mpz_is_zero(estimate)) return;

	// Reduce domain sizes using AC3
	// If a domain was made empty, return false
	if (!ac3(&sudoku)) {
		estimate = 0;

		return;
	}

	traverser.seek_first(&sudoku);

	// Count all Sudoku solutions that contain the current configuration as a subset
	backtrack = 0;
	backtrack_with_forward_check();
	
	if (mpz_is_zero(backtrack)) {
		estimate = 0;

		return;
	}

	// Multiply our estimate, the exact amount of backtracking solutions and a constant
	estimate *= backtrack;
}

void SudokuEstimator::run() {
	rng = std::mt19937(random_device());

	int index = 0;
	for (int j = 1; j < Sudoku<N, M>::size; j++) {
		if (j % N == 0) continue;

		for (int i = 0; i < Sudoku<N, M>::size; i++) {
			coordinates[index++] = j << 16 | i;
		}
	}

	Big_Integer batch_sum;
	
	while (true) {	
		auto start_time = std::chrono::high_resolution_clock::now();
		
		batch_sum = 0;

		// Sum 'batch_size' estimations
		for (unsigned int i = 0; i < BATCH_SIZE; i++) {
			estimate_solution_count();

			batch_sum += estimate;
		}

		auto      stop_time = std::chrono::high_resolution_clock::now();
		long long duration  = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count();

		// Store the result in a thread safe way
		results.mutex.lock();
		{
			results.sum  += batch_sum;
			results.n    += BATCH_SIZE;
			results.time += duration;
		}
		results.mutex.unlock();
	}
}

Big_Integer factorial(Big_Integer x) {
	assert(x >= 0);

	if (x <= 1) return 1;

	return x * factorial(x - 1);
}

Big_Integer reduced_factor(int k, int n) {
	return (factorial(n) * factorial(n - 1)) / factorial(n - k);	
}

void report_results() {
	// True number of N*M x N*M Sudoku grids, if known. 
	// Source: https://en.wikipedia.org/wiki/Mathematics_of_Sudoku#Enumeration_results
	const char * true_value = nullptr;

	// Number of M x N*M Latin Rectangles, this constant can be used to speed
	// up the process of estimating the amount of valid N*M x N*M Sudoku Grids
	// Source: http://combinatoricswiki.org/wiki/Enumeration_of_Latin_Squares_and_Rectangles
	Big_Integer latin_rectangle_count; 

	if constexpr (N == 2 && M == 2) { // 2x2 blocks
		true_value = "288";

		latin_rectangle_count = 3; // Number of Reduced 2x4 Latin Rectangles
	} else if constexpr (N == 2 && M == 3) { // 2x3 blocks
		true_value = "28200960";

		latin_rectangle_count =	1064; // Number of Reduced 3x6 Latin Rectangles
	} else if constexpr (N == 2 && M == 4) { // 2x4 blocks
		true_value = "29136487207403520";
		
		latin_rectangle_count = 420909504; // Number of Reduced 4x8 Latin Rectangles
	} else if constexpr (N == 2 && M == 5) { // 2x5 blocks
		true_value = "1903816047972624930994913280000";
		
		latin_rectangle_count = 746988383076286464; // Number of Reduced 5x10 Latin Rectangles
	} else if constexpr (N == 2 && M == 6) { // 2x6 blocks
		true_value = "38296278920738107863746324732012492486187417600000";

		latin_rectangle_count  = 1 << 17;
		latin_rectangle_count *= 9 * 5 * 131;
		latin_rectangle_count *= 110630813;
		latin_rectangle_count *= 65475601447957; // Number of Reduced 6x12 Latin Rectangles
	} else if constexpr (N == 3 && M == 3) { // 3x3 blocks
		true_value = "6670903752021072936960"; 
			
		latin_rectangle_count = 103443808; // Number of Reduced 3x9 Latin Rectangles
	} else if constexpr (N == 3 && M == 4) { // 3x4 blocks
		true_value = "81171437193104932746936103027318645818654720000";

		latin_rectangle_count  = 1 << 9;
		latin_rectangle_count *= 27 * 7;
		latin_rectangle_count *= 1945245990285863; // Number of Reduced 4x12 Latin Rectangles
	} else if constexpr (N == 3 && M == 5) { // 3x5 blocks

	} else if constexpr (N == 4 && M == 4) { // 4x4 blocks
		true_value = "595840000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"; // Estimate, real value is currently unknown

		// Number of reduced Latin Rectangles
		latin_rectangle_count  = 1 << 14;
		latin_rectangle_count *= 243 * 2693;
		latin_rectangle_count *= 42787;
		latin_rectangle_count *= 1699482467;
		latin_rectangle_count *= 8098773443; // Number of Reduced 4x16 Latin Rectangles
	} else {
		true_value = "Unknown";
		
		latin_rectangle_count = -1;
	}

	latin_rectangle_count *= reduced_factor(M, Sudoku<N, M>::size);

	Big_Integer        results_sum;
	unsigned int       results_n;
	unsigned long long results_time;
	
	Big_Integer avg;

	while (true) {
		using namespace std::chrono_literals;

		std::this_thread::sleep_for(1s);

		results.mutex.lock();
		{
			results_sum  = results.sum;
			results_n    = results.n;
			results_time = results.time;
		}
		results.mutex.unlock();

		if (results_n > 0) { // @PERFORMANCE
			avg = (results_sum * latin_rectangle_count) / results_n;

			printf(  "%u: Avg: ",                                      results_n); mpz_out_str(stdout, 10, avg.__get_mp());
			printf("\n%u: Tru: %s\n\nAvg Iteration Time: %llu us\n\n", results_n,  true_value, results_time / results_n);
		}
	}
}