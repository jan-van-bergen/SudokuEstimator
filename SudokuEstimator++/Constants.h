#pragma once
#include "BigInteger.h"
#include <cassert>

namespace Constants {	
	inline BigInteger factorial(BigInteger x) {
		assert(x >= 0);

		BigInteger result = 1;

		for (int i = 2; i <= x; i++) {
			result *= i;
		}

		return result;
	}

	inline BigInteger reduced_factor(int k, int n) {
		return (factorial(n) * factorial(n - 1)) / factorial(n - k);	
	}

	// True number of N*M x N*M Sudoku grids, if known. 
	// Source: https://en.wikipedia.org/wiki/Mathematics_of_Sudoku#Enumeration_results
	template<int N, int M> BigInteger get_true_value();
	template<> BigInteger get_true_value<2, 2>() { return BigInteger("288"); }
	template<> BigInteger get_true_value<2, 3>() { return BigInteger("28200960"); }
	template<> BigInteger get_true_value<2, 4>() { return BigInteger("29136487207403520"); }
	template<> BigInteger get_true_value<2, 5>() { return BigInteger("1903816047972624930994913280000"); }
	template<> BigInteger get_true_value<2, 6>() { return BigInteger("38296278920738107863746324732012492486187417600000"); }
	template<> BigInteger get_true_value<3, 3>() { return BigInteger("6670903752021072936960"); }
	template<> BigInteger get_true_value<3, 4>() { return BigInteger("81171437193104932746936103027318645818654720000"); }
	template<> BigInteger get_true_value<3, 5>() { return BigInteger("3508600000000000000000000000000000000000000000000000000000000000000000000000000000000"); }				// Estimate, real value unknown
	template<> BigInteger get_true_value<4, 4>() { return BigInteger("595840000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"); }	// Estimate, real value unknown

	// Number of M x N*M Latin Rectangles, this constant can be used to speed
	// up the process of estimating the amount of valid N*M x N*M Sudoku Grids
	// Source: http://combinatoricswiki.org/wiki/Enumeration_of_Latin_Squares_and_Rectangles
	template<int N, int M> BigInteger get_reduced_latin_rectangle_count();
	template<> BigInteger get_reduced_latin_rectangle_count<2, 2>() { return 3; }																													// Number of Reduced 2x4 Latin Rectangles
	template<> BigInteger get_reduced_latin_rectangle_count<2, 3>() { return 1064; }																												// Number of Reduced 3x6 Latin Rectangles
	template<> BigInteger get_reduced_latin_rectangle_count<2, 4>() { return 420909504; }																											// Number of Reduced 4x8 Latin Rectangles
	template<> BigInteger get_reduced_latin_rectangle_count<2, 5>() { return 746988383076286464; }																									// Number of Reduced 5x10 Latin Rectangles
	template<> BigInteger get_reduced_latin_rectangle_count<2, 6>() { return BigInteger(1 << 17) * BigInteger(9 * 5 * 131) * BigInteger(110630813) * BigInteger(65475601447957); }					// Number of Reduced 6x12 Latin Rectangles
	template<> BigInteger get_reduced_latin_rectangle_count<3, 3>() { return 103443808; }																											// Number of Reduced 3x9 Latin Rectangles
	template<> BigInteger get_reduced_latin_rectangle_count<3, 4>() { return BigInteger(1 << 9) * BigInteger(27 * 7) * BigInteger(1945245990285863); }												// Number of Reduced 4x12 Latin Rectangles
	template<> BigInteger get_reduced_latin_rectangle_count<3, 5>() { return BigInteger(1 << 22) * BigInteger(2187 * 19) * BigInteger(423843896863) * BigInteger(34662016427839511); }				// Number of Reduced 5x15 Latin Rectangles
	template<> BigInteger get_reduced_latin_rectangle_count<4, 4>() { return BigInteger(1 << 14) * BigInteger(243 * 2693) * BigInteger(42787) * BigInteger(1699482467) * BigInteger(8098773443); }	// Number of Reduced 4x16 Latin Rectangles

	template<int N, int M> BigInteger get_latin_rectangle_count() {
		return get_reduced_latin_rectangle_count<N, M>() * reduced_factor(M, N * M);
	}
};