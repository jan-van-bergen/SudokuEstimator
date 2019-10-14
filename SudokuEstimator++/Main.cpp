#include <thread>

#include "SudokuEstimator.h"

void create_and_run_estimator() {
	SudokuEstimator estimator;
	estimator.run();
}

int main() {
	// Acquire logical core count of this machine
	int thread_count = std::thread::hardware_concurrency();
	if (thread_count == 0) {
		printf("Something went wrong when attempting to determin the number of cores on this machine. Assuming single core.\n");

		thread_count = 1;
	}

	// Run the algorithm on n-1 other threads
	if (thread_count > 1) {
		std::thread * threads = new std::thread[thread_count - 1];

		for (int i = 0; i < thread_count - 1; i++) {
			threads[i] = std::thread(create_and_run_estimator);
		}
	}

	// Launch a thread that prints the results of all the other threads to the console
	std::thread report_results_thread(report_results);

	// Run on the main thread
	create_and_run_estimator();
}
