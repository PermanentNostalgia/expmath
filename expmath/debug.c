#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bigint.h"

int main(void) {
	BigInt *a = new_bi(2565252343), *b = new_bi(384789479), *res;

	res = mul_bi(a, b);

	_print_bi_spac(a);
	printf("%s", bi_to_str(a));
	_print_bi_spac(b);
	printf("%s", bi_to_str(b));
	_print_bi_spac(res);
	printf("%s", bi_to_str(res));
	return 0;
}