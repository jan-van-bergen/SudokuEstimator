#pragma once
#include <mpirxx.h>

// MPIR Library is used to handle big integer math
// This alias is used for convenience
typedef mpz_class BigInteger;

// Convenience function to check if a given Big_Integer is zero
#define mpz_is_zero(big_integer) (mpz_size(big_integer.__get_mp()) == 0)
