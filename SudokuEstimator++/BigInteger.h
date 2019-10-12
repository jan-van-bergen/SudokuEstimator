#pragma once
#include <mpirxx.h>

typedef mpz_class Big_Integer;

#define mpz_is_zero(big_integer) (mpz_size(big_integer.__get_mp()) == 0)
