#pragma once
#include <queue>
#include "DomainSudoku.h"

template<int N>
bool ac3(Domain_Sudoku<N> * sudoku) {
	assert(sudoku->size < 256); // Cell coordinates need to be able to be packed in a byte

    std::queue<unsigned int> constraints;

    // Enqueue all constraints
    for (int yi = 0; yi < sudoku->size; yi++) {
        for (int xi = 0; xi < sudoku->size; xi++) {
            // Calculate current block bounds
            int bx = N * (xi / N);
			int by = N * (yi / N);

            int bxe = bx + N;
			int bye = by + N;

			unsigned int current_cell = xi << 24 | yi << 16;

            // Update all domains in the current row, skipping the cells that are also in the current block
			for (int xj = 0;   xj < bx;           xj++) constraints.push((unsigned int)(current_cell | xj << 8 | yi));
			for (int xj = bxe; xj < sudoku->size; xj++) constraints.push((unsigned int)(current_cell | xj << 8 | yi));

            // Update all domains in the current column, skipping the cells that are also in the current block
			for (int yj = 0;   yj < by;           yj++) constraints.push((unsigned int)(current_cell | xi << 8 | yj));
			for (int yj = bye; yj < sudoku->size; yj++) constraints.push((unsigned int)(current_cell | xi << 8 | yj));

            // Update all domains in the current block, except for (xi, yi) and (xj, yj)
            for (int yj = by; yj < bye; yj++) {
                for (int xj = bx; xj < bxe; xj++) {
                    if (xj != xi || yj != yi) {
                        constraints.push((unsigned int)(current_cell | xj << 8 | yj));
					}
				}
			}
        }
	}

	int domain_i[Domain_Sudoku<N>::size];
	int domain_j[Domain_Sudoku<N>::size];

    do {
        unsigned int cij = constraints.front();
		constraints.pop();

        int xi = (int)(cij & 0xff000000) >> 24, yi = (int)(cij & 0x00ff0000) >> 16;
        int xj = (int)(cij & 0x0000ff00) >> 8,  yj = (int)(cij & 0x000000ff);

        bool modified = false;

		int domain_size_j = sudoku->get_domain(xj, yj, domain_j);

		if (domain_size_j == 1) {
			int domain_size_i = sudoku->get_domain(xi, yi, domain_i);

			for (int di = 0; di < domain_size_i; di++) {
				if (domain_i[di] == domain_j[0]) {
					int& constraint = sudoku->constraints[(xi + yi * Domain_Sudoku<N>::size) * Domain_Sudoku<N>::size + domain_i[di] - 1];

					// If domain_i[di] was previously unconstrained, it is now
					if (constraint == 0) {
						// Remove the value from the domain
						int& domain_size = sudoku->domain_sizes[xi + yi * Domain_Sudoku<N>::size];
						domain_size--;

						// If the domain is now empty, the Sudoku is not valid
						if (domain_size == 0) return false;
					}

					constraint++;

					modified = true;

					break;
				}
			}
		}

        if (modified) {
            // Calculate current block bounds
            int bx = N * (xi / N);
			int by = N * (yi / N);

            int bxe = bx + N;
			int bye = by + N;

			unsigned int current_cell = xi << 8 | yi;

            // Update all domains in the current row, skipping the cells that are also in the current block
			for (int xk = 0;   xk < bx;           xk++) constraints.push((unsigned int)(xk << 24 | yi << 16 | current_cell));
			for (int xk = bxe; xk < sudoku->size; xk++) constraints.push((unsigned int)(xk << 24 | yi << 16 | current_cell));

            // Update all domains in the current column, skipping the cells that are also in the current block
			for (int yk = 0;   yk < by;           yk++) constraints.push((unsigned int)(xi << 24 | yk << 16 | xi << 8 | yi));
			for (int yk = bye; yk < sudoku->size; yk++) constraints.push((unsigned int)(xi << 24 | yk << 16 | xi << 8 | yi));

            // Update all domains in the current block, except for (xi, yi) and (xj, yj)
            for (int yk = by; yk < bye; yk++) {
                for (int xk = bx; xk < bxe; xk++) {
                    if ((xk != xi || yk != yi) && (xk != xj || yk != yj)) {
                        constraints.push((unsigned int)(xk << 24 | yk << 16 | xi << 8 | yi));
					}
				}
			}
        }
    } while (constraints.size() > 0);

	return true;
}