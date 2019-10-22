#pragma once
#include <mpirxx.h>
#include <cassert>

// MPIR Library is used to handle big integer math
// This alias is used for convenience
typedef mpz_class BigInteger;

namespace BigIntegerMath {
	// Convenience function to check if a given Big_Integer is zero
	inline bool is_zero(const BigInteger& integer) {
		return mpz_size(integer.__get_mp()) == 0;
	}

	inline BigInteger factorial(const BigInteger& x) {
		assert(x >= 0);

		BigInteger result = 1;

		for (int i = 2; i <= x; i++) {
			result *= i;
		}

		return result;
	}
};
