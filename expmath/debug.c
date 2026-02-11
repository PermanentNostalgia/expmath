#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bigint.h"

int main(void) {
	BigInt *bi = new_bi(17);
	printf("bi in binary: %s", bi_to_str(bi));
	return 0;
}