#include "RNG.h"
#include <iostream>
#include <random>


int randomFromTo(int n1, int n2) {

	if (n1 >= n2)
		return 0;

	// Make new seed every call, so it is as random as it gets
	pcg32_random_t rng;
	randomizeSeed(&rng);
	// Calculate random number
	int bound = n2 - n1;
	int random_number = pcg32_boundedrand_r(&rng, bound);

	return n1 + random_number;
}

float randomFloat(){
	// Make new seed every call, so it is as random as it gets
	pcg32_random_t rng;
	randomizeSeed(&rng);
	return ldexp(pcg32_random_r(&rng), -32);
}

void randomizeSeed(pcg32_random_t* seed) {
	// Get random 64 bit values
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<uint16_t> distribution(0, 0xFFFFFFFFFFFFFFFF);
	//Randomize seed
	pcg32_srandom_r(seed, distribution(generator), distribution(generator));
}
