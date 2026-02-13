#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bigint.h"

int main(void) {
	BigInt *a = new_bi(255), *b = new_bi(255), *res;

	res = mul_bi(a, b);

	_print_bi_spac(res);
	return 0;
}