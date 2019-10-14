#include <time.h>
#include <iostream>

#include "SudokuEstimator.h"

int main() {
	// Set random seed
	srand(time(NULL));

	SudokuEstimator estimator;
	estimator.run();
}
