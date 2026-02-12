#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bigint.h"

int main(void) {
	BigInt *a = new_bi(18), *b = new_bi(17), *res;
	res = sum_bi(a, b);

	_print_bi_spac(res);

	mul_bi_with_assign(NULL, NULL);

	_print_bi_spac(res);
	return 0;
}