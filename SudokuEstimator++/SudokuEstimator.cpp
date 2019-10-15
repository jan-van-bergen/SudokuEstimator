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
				
				assert(sudoku.is_valid_solution());
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

	// Fill every Nth row with a row from a random N x N*M Latin Rectangle
	// The first row is always 1 .. N*M
	int rows[M][Sudoku<N, M>::size];

	// Initialize each row of the Latin Rectangle with the numbers 1 .. N*M
	for (int row = 0; row < N; row++) {
		for (int i = 0; i < Sudoku<N, M>::size; i++) {
			rows[row][i] = i + 1;
		}
	}

	// Repeat until a valid Latin Rectangle is obtained
	retry: {
		// Randomly shuffle every row but the first one
		for (int row = 1; row < N; row++) {
			std::shuffle(rows[row], rows[row] + Sudoku<N, M>::size, rng);
		}

		// Check if the current permutation of rows is a Latin Rectangle
		for (int row = 1; row < N; row++) {
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
	for (int row = 0; row < N; row++) {
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
	assert(random_walk_length < coordinate_count);
	assert(Sudoku_NxM::size < (1 << 16)); // Coordinate indices (x, y) are packed together into a 32bit integer

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
		
		// Sum 'batch_size' estimations
		for (unsigned int i = 0; i < BATCH_SIZE; i++) {
			estimate_solution_count();

			batch_sum += estimate;
		}

		auto stop_time = std::chrono::high_resolution_clock::now();
		long long duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count();

		// Store the result in a thread safe way
		results.mutex.lock();
		{
			results.sum  += batch_sum;
			results.n    += BATCH_SIZE;
			results.time += duration;
		}
		results.mutex.unlock();

		// Reset sum for next iteration
		batch_sum = 0;
	}
}

void report_results() {
	// Amount of N x N*M Latin Rectangles, this constant can be used to speed
	// up the process of estimating the amount of valid N*M x N*M Sudoku Grids
	Big_Integer latin_rectangle_count; 

	const char * true_value;
	switch (Sudoku<N, M>::size) {
		case 4: {
			true_value = "288";

			latin_rectangle_count = 24 * 9; // 4! times number of 2x4 latin rectangles where the first row is 1 .. 4
		} break;
		case 9: {
			true_value = "6670903752021072936960"; 
			
			latin_rectangle_count = 362880 * 5792853248; // 9! times number of 3x9 latin rectangles where the first row is 1 .. 9
		} break;
		case 16: {
			true_value = "595840000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"; // Estimate, real value is currently unknown

			latin_rectangle_count  = 20922789888000;	// 16!
			latin_rectangle_count *= 1307674368000;		// Convert from number of Reduced Latin Rectangles to actual number
			latin_rectangle_count /= 479001600;

			// Number of reduced Latin Rectangles
			latin_rectangle_count *= 1 << 14;
			latin_rectangle_count *= 243 * 2693;
			latin_rectangle_count *= 42787;
			latin_rectangle_count *= 1699482467;
			latin_rectangle_count *= 8098773443;
		} break;
		case 25: {
			true_value = "TODO"; // @TODO

			latin_rectangle_count = -1; // @TODO: http://combinatoricswiki.org/wiki/Enumeration_of_Latin_Squares_and_Rectangles
		} break;
		default: {
			true_value = "Unknown";
			
			latin_rectangle_count = -1;
		} break;
	}

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
			avg = (results_sum / results_n) * latin_rectangle_count;

			printf(  "%u: Avg: ",     results_n); mpz_out_str(stdout, 10, avg.__get_mp());
			printf("\n%u: Tru: %s\n\nAvg Iteration Time: %llu us\n\n", results_n,  true_value, results_time / results_n);
		}
	}
}