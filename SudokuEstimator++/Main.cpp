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
		printf("Something went wrong when attempting to determine the number of cores on this machine. Assuming single core.\n");

		thread_count = 1;
	}

	for (int i = 0; i < thread_count; i++) {
		std::thread(create_and_run_estimator).detach();
	}

	// Run function on the main thread that prints the results of all the other threads to the console
	report_results();
}
