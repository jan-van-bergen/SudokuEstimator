#include "BigInteger.h"

BigInteger BigInteger::operator+(const BigInteger& other) const {
	int min_size;
	int max_size;
	const std::vector<unsigned int> * max_bits = NULL;

	int       bits_size =       bits.size();
	int other_bits_size = other.bits.size();

	if (bits_size > other_bits_size) {
		min_size = other_bits_size;
		max_size =       bits_size;

		max_bits = &bits;
	} else {
		min_size =       bits_size;
		max_size = other_bits_size;

		max_bits = &other.bits;
	}

	BigInteger result(max_size);

	unsigned int carry = 0;
	for (int i = 0; i < min_size; i++) {
		unsigned int other_plus_carry = other.bits[i] + carry;

		result.bits[i] = bits[i] + other_plus_carry;

		carry = bits[i] > std::numeric_limits<unsigned int>::max() - other_plus_carry;
	}

	for (int i = min_size; i < max_size; i++) {
		result.bits[i] = (*max_bits)[i]; // @TODO: shouldn't the carry be used here too?
	}

	return result;
}

BigInteger BigInteger::operator*(const BigInteger& other) const {
	int min_size;
	int max_size;
	const std::vector<unsigned int> * max_bits = NULL;

	int       bits_size =       bits.size();
	int other_bits_size = other.bits.size();

	if (bits_size > other_bits_size) {
		min_size = other_bits_size;
		max_size =       bits_size;

		max_bits = &bits;
	} else {
		min_size =       bits_size;
		max_size = other_bits_size;

		max_bits = &other.bits;
	}

	BigInteger result(max_size + 1);

	for (int i = 0; i < bits.size(); i ++) {
		unsigned int carry = 0;

		for (int j = 0; j < other.bits.size(); j++) {
			carry = bits[i] > std::numeric_limits<unsigned int>::max() / other.bits[j];

			
			abort(); // @UNFINISHED
		}
	}

	return result;
}