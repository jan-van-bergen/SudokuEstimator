#pragma once
#include <queue>
#include "DomainSudoku.h"

template<int N>
bool ac3(Domain_Sudoku<N> * sudoku) {
	assert(sudoku->size < 256);

    std::queue<unsigned int> constraints;

    // Enqueue all constraints
    for (int yi = 0; yi < sudoku->size; yi++) {
        for (int xi = 0; xi < sudoku->size; xi++) {
            // Calculate current block bounds
            int bx = N * (xi / N);
			int by = N * (yi / N);

            int bxe = bx + N;
			int bye = by + N;

            // Update all domains in the current row
            for (int xj = 0; xj < sudoku->size; xj++) {
                if (xj < bx || xj >= bxe) {
                    constraints.push((unsigned int)(xi << 24 | yi << 16 | xj << 8 | yi));
				}
			}

            // Update all domains in the current column
            for (int yj = 0; yj < sudoku->size; yj++) {
                if (yj < by || yj >= bye) {
                    constraints.push((unsigned int)(xi << 24 | yi << 16 | xi << 8 | yj));
				}
			}

            // Update all domains in the current block
            for (int yj = by; yj < bye; yj++) {
                for (int xj = bx; xj < bxe; xj++) {
                    if (xj != xi || yj != yi) {
                        constraints.push((unsigned int)(xi << 24 | yi << 16 | xj << 8 | yj));
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
					int& domain = sudoku->domains[(xi + yi * Domain_Sudoku<N>::size) * Domain_Sudoku<N>::size + domain_i[di] - 1];

					// If domain_i[di] was previously unconstrained, it is now
					if (domain == 0) {
						int& domain_size = sudoku->domain_sizes[xi + yi * Domain_Sudoku<N>::size];
						domain_size--;

						// If the domain is now empty, the Sudoku is not valid
						if (domain_size == 0) return false;
					}

					domain++;

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

            // Update all domains in the current row
            for (int xk = 0; xk < sudoku->size; xk++) {
                if ((xk < bx || xk >= bxe) && xk != xj) {
                    constraints.push((unsigned int)(xk << 24 | yi << 16 | xi << 8 | yi));
				}
			}

            // Update all domains in the current column
            for (int yk = 0; yk < sudoku->size; yk++) {
                if ((yk < by || yk >= bye) && yk != yj) {
                    constraints.push((unsigned int)(xi << 24 | yk << 16 | xi << 8 | yi));
				}
			}

            // Update all domains in the current block
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