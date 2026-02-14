#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bigint.h"

int main(void) {
	uint8_t i;
	BigInt *a = new_bi(3432542);
	for(i=0; i<10; i++) {
		mul_bi_with_assign(a, a);
	}

	printf(bi_to_str(a));

	return 0;
}