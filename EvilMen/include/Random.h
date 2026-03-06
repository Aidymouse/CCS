#include <time.h>
#include <stdlib.h>


void seed_rand();
int rand_int(int min, int max);

#ifdef RANDOM_IMPLEMENTATION

void seed_rand() {
	srand(time(NULL));
}

/* Inclusive */
int rand_int(int min, int max) {
	int randint = (rand() % ((max+1) - min)) + min;
	return randint;
}

#endif
