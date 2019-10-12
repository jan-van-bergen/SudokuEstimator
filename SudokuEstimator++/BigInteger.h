#pragma once
#include <mpirxx.h>

// MPIR is used to handle big integer math
typedef mpz_class Big_Integer;

// Convenience function to check if a given Big_Integer is zero
#define mpz_is_zero(big_integer) (mpz_size(big_integer.__get_mp()) == 0)
