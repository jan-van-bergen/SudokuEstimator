#pragma once
#include <queue>
#include "Sudoku.h"

template<int N, int M>
bool ac3(Sudoku<N, M> * sudoku) {
	assert(sudoku->size < 256); // Cell coordinates need to be packed into a single byte

	std::queue<unsigned int> constraints;

	// Enqueue all constraints
	for (int yi = 0; yi < sudoku->size; yi++) {
		for (int xi = 0; xi < sudoku->size; xi++) {
			// Calculate current block bounds
			int bx = M * (xi / M);
			int by = N * (yi / N);

			int bxe = bx + M;
			int bye = by + N;

			unsigned int current_cell        = xi << 24 | yi << 16; 
			unsigned int current_cell_row    = current_cell | yi; 
			unsigned int current_cell_column = current_cell | xi << 8; 

			// Update all domains in the current row, skipping the cells that are also in the current block
			for (int xj = 0;   xj < bx;           xj++) constraints.push((unsigned int)(current_cell_row | xj << 8));
			for (int xj = bxe; xj < sudoku->size; xj++) constraints.push((unsigned int)(current_cell_row | xj << 8));

			// Update all domains in the current column, skipping the cells that are also in the current block
			for (int yj = 0;   yj < by;           yj++) constraints.push((unsigned int)(current_cell_column | yj));
			for (int yj = bye; yj < sudoku->size; yj++) constraints.push((unsigned int)(current_cell_column | yj));

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

	int domain_i[Sudoku<N, M>::size];
	int domain_j[Sudoku<N, M>::size];

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
					int index_i = Sudoku<N, M>::get_index(xi, yi);
					int& constraint = sudoku->constraints[index_i * Sudoku<N, M>::size + domain_i[di]];

					// If domain_i[di] was previously unconstrained, it is now
					if (constraint == 0) {
						// Remove the value from the domain
						int& domain_size = sudoku->domain_sizes[index_i];
						domain_size--;

						// If the domain is now empty, the Sudoku is not valid
						if (domain_size == 0) return false;
					}

					constraint++;

					// Calculate current block bounds
					int bx = M * (xi / M);
					int by = N * (yi / N);

					int bxe = bx + M;
					int bye = by + N;

					unsigned int current_cell        = xi << 8 | yi;
					unsigned int current_cell_row    = current_cell | yi << 16; 
					unsigned int current_cell_column = current_cell | xi << 24; 

					// Update all domains in the current row, skipping the cells that are also in the current block
					for (int xk = 0;   xk < bx;           xk++) if (xk != xj) constraints.push((unsigned int)(xk << 24 | current_cell_row));
					for (int xk = bxe; xk < sudoku->size; xk++) if (xk != xj) constraints.push((unsigned int)(xk << 24 | current_cell_row));

					// Update all domains in the current column, skipping the cells that are also in the current block
					for (int yk = 0;   yk < by;           yk++) if (yk != yj) constraints.push((unsigned int)(current_cell_column | yk << 16));
					for (int yk = bye; yk < sudoku->size; yk++) if (yk != yj) constraints.push((unsigned int)(current_cell_column | yk << 16));

					// Update all domains in the current block, except for (xi, yi) and (xj, yj)
					for (int yk = by; yk < bye; yk++) {
						for (int xk = bx; xk < bxe; xk++) {
							if ((xk != xi || yk != yi) && (xk != xj || yk != yj)) {
								constraints.push((unsigned int)(xk << 24 | yk << 16 | current_cell));
							}
						}
					}

					break;
				}
			}
		}
	} while (constraints.size() > 0);

	return true;
}