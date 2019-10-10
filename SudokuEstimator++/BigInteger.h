#pragma once
#include <cassert>
#include <algorithm>
#include <vector>
#include <limits.h>

// Arbitraryly large unsigned integer
struct BigInteger {
private:
	static constexpr int default_size = 4;

	std::vector<unsigned int> bits;

	inline explicit BigInteger(int size) : bits(size) 
	{ }

public:
	inline BigInteger() : bits(default_size) 
	{ }

	inline void operator=(unsigned long long integer) {
		// Make sure at least 64 bits are available
		bits.reserve(2);

		bits[0] = (unsigned int)(integer);
		bits[1] = (unsigned int)(integer >> 32);
	}

	BigInteger operator+(const BigInteger& other) const;

	BigInteger operator*(const BigInteger& other) const;

	inline bool is_zero() {
		// The number is zero if all bits in the array are zero
		for (int i = 0; i < bits.size(); i++) {
			if (bits[i] != 0) {
				return false;
			}
		}

		return true;
	}
};
