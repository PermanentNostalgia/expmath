#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bigint.h"

int main(void) {
	BigInt *a = new_bi(9223372036854775807), *b = new_bi(9223372036854775807), *res;

	res = mul_bi(a, b);

	_print_bi_spac(a);
	_print_bi_spac(b);
	_print_bi_spac(res);
	printf("%s", bi_to_str(res));
	return 0;
}