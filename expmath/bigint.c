#include "bigint.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static BigInt *CALC_TEMP = 0;
static char *OUTPUT_STR_TEMP = 0;

BigInt *new_bi(int64_t n) {
	uint8_t sign = (n<0) ? 1 : 0;
	uint64_t abs_num = sign ? -n : n;
	uint8_t field = 1;
	uint8_t *mem;

	{
		uint64_t temp = abs_num;
		while((temp/=256) != 0) {
			field++;
		}
	}

	mem = (uint8_t *)malloc(field);
	if(mem==NULL) {
		return NULL;
	}

	{
		uint64_t temp = abs_num;
		uint8_t f_temp;
		uint8_t i, carry = 1;
		uint8_t *temp_mem;

		if(sign) {
			for(i=0; i<field; i++) {
				f_temp = ~(uint8_t)(temp%256) + carry;
				mem[i] = f_temp;
				if(f_temp<~(uint8_t)(temp%256) || f_temp<carry) carry = 1;
				else carry = 0;

				temp/=256;
			}
			
			if(mem[field-1]<128) {
				temp_mem = (uint8_t *)realloc(mem, ++field);
				if(temp_mem==NULL) {
					free(mem);
					return NULL;
				}
				temp_mem[field-1] = 0b11111111;
				mem = temp_mem;
			}
		} else {
			for(i=0; i<field; i++) {
				mem[i] = temp%256;
				temp/=256;
			}

			if(mem[field-1]>=128) {
				temp_mem = realloc(mem, ++field);
				if(temp_mem==NULL) {
					free(mem);
					return NULL;
				}

				temp_mem[field-1] = 0b00000000;
				mem = temp_mem;
			}
		}
	}

	BigInt *bi = (BigInt *)malloc(sizeof(BigInt));
	if(bi==NULL) {
		free(mem);
		return NULL;
	}

	bi->field = (uint64_t)field;
	bi->mem = mem;

	return bi;
}

BigInt *new_bi_with_str(const char *str) {
	uint8_t sign;

	if(str[0]=='-') {
		if(strlen(str)==1) return NULL;
		sign = 1;
	} else if(str[0]>='0' && str[0]<='9')
		sign = 0;
	else
		return NULL;

	BigInt *bi = new_bi(0);
	if(bi==NULL) {
		return NULL;
	}

	{
		uint64_t str_len = strlen(str), i = str_len, j;
		BigInt *ten_bi = new_bi(10);
		if(ten_bi==NULL) {
			return NULL;
		}

		retcode oper_res;
		BigInt *digit_bi;
		// fix this: var i always overflows
		do {
			i--;

			if(str[i]<'0' || str[i]>'9') {
				free_bi(bi);
				free_bi(ten_bi);
				return NULL;
			}

			digit_bi = new_bi(str[i]-'0');
			if(digit_bi==NULL) {
				free_bi(bi);
				free_bi(ten_bi);
				return NULL;
			}

			for(j=0; j<str_len-i-1; j++) {
				oper_res = mul_bi_with_assign(digit_bi, ten_bi);
				if(oper_res==-1) {
					free_bi(bi);
					free_bi(ten_bi);
					free_bi(digit_bi);
					return NULL;
				}
			}
			oper_res = sum_bi_with_assign(bi, digit_bi);
			
			free_bi(digit_bi);
			if(oper_res==-1) {
				free_bi(bi);
				free_bi(ten_bi);
				return NULL;
			}
		} while(i!=sign);
	}

	if(sign) convert_sign_bi(bi);

	return bi;
}

BigInt *copy_bi(const BigInt *bi) {
	if(bi==NULL) {
		return NULL;
	}

	BigInt *cbi = (BigInt *)malloc(sizeof(BigInt));

	if(cbi==NULL) {
		return NULL;
	}

	cbi->field = bi->field;
	cbi->mem = (uint8_t *)malloc(cbi->field);
	if(cbi->mem==NULL) {
		free(cbi);
		return NULL;
	}

	memcpy(cbi->mem, bi->mem, cbi->field);

	return cbi;
}

BigInt *copy_calctemp(void) {
	return copy_bi(CALC_TEMP);
}

BigInt *sum_bi(const BigInt *x, const BigInt *y) {
	return _operate(x, y, _sum_bi);
}

retcode sum_bi_with_assign(BigInt *x, const BigInt *y) {
	return _operate_with_assign(x, y, _sum_bi);
}

BigInt *sub_bi(const BigInt *x, const BigInt *y) {
	return _operate(x, y, _sub_bi);
}

retcode sub_bi_with_assign(BigInt *x, const BigInt *y) {
	return _operate_with_assign(x, y, _sub_bi);
}

BigInt *mul_bi(const BigInt *x, const BigInt *y) {
	return _operate(x, y, _mul_bi);
}

retcode mul_bi_with_assign(BigInt *x, const BigInt *y) {
	return _operate_with_assign(x, y, _mul_bi);
}

retcode is_bigger_bi(const BigInt *base, const BigInt *counter) {
	retcode res = _compare_operate(base, counter);
	
	if(res==1)
		return 1;
	else
		return 0;
}

retcode is_smaller_bi(const BigInt *base, const BigInt *counter) {
	retcode res = _compare_operate(base, counter);

	if(res==2)
		return 1;
	else
		return 0;
}

retcode is_same_bi(const BigInt *base, const BigInt *counter) {
	retcode res = _compare_operate(base, counter);

	if(res==0)
		return 1;
	else
		return 0;
}

retcode is_bigger_same_bi(const BigInt *base, const BigInt *counter) {
	retcode res = _compare_operate(base, counter);

	if(res==0 || res==1)
		return 1;
	else
		return 0;
}

retcode is_smaller_same_bi(const BigInt *base, const BigInt *counter) {
	retcode res = _compare_operate(base, counter);

	if(res==0 || res==2)
		return 1;
	else
		return 0;
}

BigInt *and_bi(const BigInt *x, const BigInt *y) {
	return _bitwise_compare_operate(x, y, _bitwise_and);
}

BigInt *or_bi(const BigInt *x, const BigInt *y) {
	return _bitwise_compare_operate(x, y, _bitwise_or);
}

BigInt *xor_bi(const BigInt *x, const BigInt *y) {
	return _bitwise_compare_operate(x, y, _bitwise_xor);
}

retcode convert_sign_bi(BigInt *x) {
	if(x==NULL) {
		if(CALC_TEMP==NULL) return -1;
		x = CALC_TEMP;
	}

	uint64_t i, field = x->field;
	uint8_t temp, carry = 1, *temp_mem;
	if(is_signed_bi(x)) {
		for(i=0; i<field; i++) {
			temp = ~(x->mem[i] - carry);
			if(x->mem[i]==0) carry = 1;
			else carry = 0;
			x->mem[i] = temp;
		}

		if(x->mem[field-1]==128) {
			temp_mem = (uint8_t *)realloc(x->mem, ++field);
			if(temp_mem==NULL) {
				return -1;
			}
			
			temp_mem[field-1] = 0;
			x->mem = temp_mem;
			x->field = field;
		}
	} else {
		for(i=0; i<field; i++) {
			temp = ~(x->mem[i]) + carry;
			if(temp<~(x->mem[i]) || temp<carry) carry = 1;
			else carry = 0;
			x->mem[i] = temp;
		}

		/* when the number is 2^(8n+7) (n is same or bigger than zero)
		 */ 
		if(x->mem[field-1]==0b11111111 && x->mem[field-2]==128) {
			temp_mem = (uint8_t *)realloc(x->mem, --field);
			if(temp_mem==NULL) {
				return -1;
			}
			x->mem = temp_mem;
			x->field = field;
		}
	}
}

retcode is_signed_bi(const BigInt *bi) {
	if(bi->mem[bi->field-1]>=128)
		return 1;
	else
		return 0;
}

char *bi_to_str(const BigInt *bi) {
	if(bi->field>(UINT64_MAX/8)) return NULL;

	if(bi==NULL) {
		if(CALC_TEMP==NULL) return NULL;
		bi = CALC_TEMP;
	}

	uint64_t str_len = bi->field*8;
	char *str = (char *)malloc(str_len+1);
	if(str==NULL) return NULL;

	uint64_t i = bi->field;
	uint8_t j, quotient;
	do {
		i--;

		quotient = bi->mem[i];
		for(j=0; j<8; j++) {
			str[8*(bi->field-i-1)+(8-j-1)] = quotient % 2 + '0';
			quotient>>=1;
		}
	} while(i!=0);
	str[str_len] = '\0';

	if(OUTPUT_STR_TEMP!=NULL)
		free(OUTPUT_STR_TEMP);

	OUTPUT_STR_TEMP = str;

	return OUTPUT_STR_TEMP;
}

retcode set_bi_calctemp_with_number(int64_t n) {
	flush_bi_calctemp();

	BigInt *bi = new_bi(n);
	if(bi==NULL) {
		return -1;
	}

	CALC_TEMP = bi;

	return 0;
}

void free_bi(BigInt *bi) {
	if(bi==NULL) {
		return;
	}

	free(bi->mem);
	free(bi);
}

void flush_bi_calctemp(void) {
	if(CALC_TEMP!=NULL) {
		free(CALC_TEMP->mem);
		free(CALC_TEMP);
		CALC_TEMP = NULL;
	}
}

void flush_bi_outputstrtemp(void) {
	if(OUTPUT_STR_TEMP!=NULL) {
		free(OUTPUT_STR_TEMP);
		OUTPUT_STR_TEMP = NULL;
	}
}

static BigInt *_operate(const BigInt *x, const BigInt *y, BigInt *(*operator)(const BigInt *, const BigInt *)) {
	if(_set_oprand(&x, &y)==-1) return NULL;

	BigInt *res = operator(x, y);
	if(res==NULL) {
		return NULL;
	}

	flush_bi_calctemp();
	CALC_TEMP = res;

	return CALC_TEMP;
}

static retcode _operate_with_assign(BigInt *x, const BigInt *y, BigInt *(*operator)(const BigInt *, const BigInt *)) {
	if(_set_oprand(&x, &y)==-1) return -1;

	BigInt *res = operator(x, y);
	if(res==NULL) {
		return -1;
	}

	free(x->mem);
	x->mem = res->mem;
	x->field = res->field;
	free(res);

	return 0;
}

static BigInt *_sum_bi(const BigInt *x, const BigInt *y) {
	uint64_t x_field = x->field, y_field = y->field;
	uint64_t n_field;
	uint8_t counter_pad_byte;
	const BigInt *base, *counter;

	if(x_field>y_field) {
		base = x;
		counter = y;
	} else {
		base = y;
		counter = x;
	}
	n_field = base->field;

	if(is_signed_bi(counter))
		counter_pad_byte = 0b11111111;
	else
		counter_pad_byte = 0b00000000;

	uint8_t *n_mem = (uint8_t *)malloc(n_field);

	if(n_mem==NULL) {
		return NULL;
	}

	{
		uint64_t i;
		uint8_t carry = 0;
		for(i=0; i<counter->field; i++) {
			n_mem[i] = base->mem[i] + counter->mem[i] + carry;
			if(n_mem[i]<base->mem[i] || n_mem[i]<counter->mem[i] || n_mem[i]<carry)
				carry = 1;
			else
				carry = 0;

		}

		for(i=counter->field; i<base->field; i++) {
			n_mem[i] = base->mem[i] + counter_pad_byte + carry;
			if(n_mem[i]<base->mem[i] || n_mem[i]<counter_pad_byte || n_mem[i]<carry)
				carry = 1;
			else
				carry = 0;
		}

		uint8_t *temp_mem;
		if(is_signed_bi(base)&&is_signed_bi(counter)) {
			if(carry) {
				temp_mem = realloc(n_mem, ++n_field);
				if(temp_mem==NULL) {
					free(n_mem);
					return NULL;
				}

				temp_mem[n_field-1] = 0b11111111;
				n_mem = temp_mem;
			}
		} else if(!is_signed_bi(base) && !is_signed_bi(counter)) {
			if(carry || n_mem[n_field-1]>=128) {
				temp_mem = realloc(n_mem, ++n_field);
				if(temp_mem==NULL) {
					free(n_mem);
					return NULL;
				}

				temp_mem[n_field-1] = carry;
				n_mem = temp_mem;
			}
		}
	}

	BigInt *res = (BigInt *)malloc(sizeof(BigInt));
	if(res==NULL) {
		free(n_mem);
		return NULL;
	}

	res->mem = n_mem;
	res->field = n_field;

	return res;
}

static BigInt *_sub_bi(const BigInt *x, const BigInt *y) {
	BigInt *negative_y = copy_bi(y);
	convert_sign_bi(negative_y);

	BigInt *res = _sum_bi(x, negative_y);

	return res;
}

static BigInt *_mul_bi(const BigInt *x, const BigInt *y) {
	if((x->field==1 && x->mem[0]==0) || (y->field==1 && y->mem[0]==0))
		return new_bi(0);

	uint8_t res_sign = 0;
	BigInt *cp_x = NULL, *cp_y = NULL;
	if(is_signed_bi(x)) {
		cp_x = copy_bi(x);
		if(cp_x==NULL) return NULL;

		convert_sign_bi(cp_x);
		x = cp_x;

		res_sign++;
	}

	if(is_signed_bi(y)) {
		cp_y = copy_bi(y);
		if(cp_y==NULL) {
			free_bi(cp_x);
			return NULL;
		}

		convert_sign_bi(cp_y);
		y = cp_y;

		res_sign = (res_sign + 1) % 2;
	}
	
	BigInt *res = new_bi(0);
	if(res==NULL) {
		return NULL;
	}

	BigInt *pv = (BigInt *)malloc(sizeof(BigInt));
	if(pv==NULL) {
		free_bi(res);
		return NULL;
	}
	pv->mem = NULL;
	pv->field = 0;

	retcode sum_res;
	uint8_t coef, carry_coef;
	uint64_t i, j, exp, pv_field;
	for(i=0; i<x->field; i++) {
		carry_coef = 0;
		for(j=0; j<y->field; j++) {
			exp = i + j;
			coef = (x->mem[i] * y->mem[j] + carry_coef) % 256;
			carry_coef = (x->mem[i] * y->mem[j] + carry_coef) / 256;

			pv_field = (coef<128) ? exp + 1 : exp + 2;
			pv->mem = (uint8_t *)calloc(pv_field, 1);
			if(pv->mem==NULL) {
				free_bi(res);
				free(pv);
				return NULL;
			}

			pv->mem[exp] = coef;
			pv->field = pv_field;

			sum_res = sum_bi_with_assign(res, pv);

			free(pv->mem);
			if(sum_res==-1) {
				free_bi(res);
				free(pv);
				return NULL;
			}
		}
		if(carry_coef!=0) {
			exp++;
			pv_field = (carry_coef<128) ? exp + 1 : exp + 2;
			pv->mem = (uint8_t *)calloc(pv_field, 1);
			if(pv->mem==NULL) {
				free_bi(res);
				free(pv);
				return NULL;
			}

			pv->mem[exp] = carry_coef;
			pv->field = pv_field;

			sum_res = sum_bi_with_assign(res, pv);

			free(pv->mem);
			if(sum_res==-1) {
				free_bi(res);
				free(pv);
				return NULL;
			}
		}
	}

	if(res_sign) convert_sign_bi(res);

	return res;
}

/*
	What return value means
	-1: error
	0: the both is same
	1: the first argument is bigger
	2: the second argument is bigger
*/
static retcode _compare_operate(const BigInt *x, const BigInt *y) {
	if(_set_oprand(&x, &y)==-1) return -1;
	
	uint64_t x_field = x->field;
	uint64_t y_field = y->field;
	uint8_t x_sign = is_signed_bi(x);
	uint8_t y_sign = is_signed_bi(y);

	if(x_sign==0 && y_sign==1) {
		return 1;
	} else if(x_sign==1 && y_sign==0) {
		return 2;
	} else if(x_sign==0) {
		if(x_field>y_field) {
			return 1;
		} else if(x_field<y_field) {
			return 2;
		}
	} else if(x_sign==1) {
		if(x_field>y_field) {
			return 2;
		} else if(x_field<y_field) {
			return 1;
		}
	}

	uint64_t i;
	for(i=x_field; i>0; i--) {
		if(x->mem[i-1]>y->mem[i-1]) {
			return 1;
		} else if(x->mem[i-1]<y->mem[i-1]) {
			return 2;
		}
	}

	return 0;
}

static BigInt *_bitwise_compare_operate(const BigInt *x, const BigInt *y, uint8_t (*operator)(uint8_t,uint8_t)) {
	if(_set_oprand(&x, &y)==-1) return NULL;

	uint64_t n_field;
	uint8_t *n_mem, counter_pad;
	const BigInt *base, *counter;

	if(x->field>y->field) {
		base = x;
		counter = y;
	} else {
		base = y;
		counter = x;
	}
	n_field = base->field;

	n_mem = (uint8_t *)malloc(n_field);
	if(n_mem==NULL) {
		return NULL;
	}

	if(is_signed_bi(counter))
		counter_pad = 0b11111111;
	else
		counter_pad = 0b00000000;

	{
		uint64_t i;
		for(i=0; i<counter->field; i++) {
			n_mem[i] = operator(base->mem[i], counter->mem[i]);
		}

		for(i=counter->field; i<base->field; i++) {
			n_mem[i] = operator(base->mem[i], counter_pad);
		}
	}

	if(CALC_TEMP==NULL) {
		CALC_TEMP = (BigInt *)malloc(sizeof(BigInt));
		if(CALC_TEMP==NULL) {
			return NULL;
		}
	} else {
		free(CALC_TEMP->mem);
	}

	CALC_TEMP->mem = n_mem;
	CALC_TEMP->field = n_field;

	if(_optimize_bytes(CALC_TEMP)==-1) {
		return NULL;
	}

	return CALC_TEMP;
}

static uint8_t _bitwise_and(uint8_t x, uint8_t y) {
	return x & y;
}

static uint8_t _bitwise_or(uint8_t x, uint8_t y) {
	return x | y;
}

static uint8_t _bitwise_xor(uint8_t x, uint8_t y) {
	return x ^ y;
}

static retcode _reset_calctemp(void) {
	if(CALC_TEMP==NULL) {
		CALC_TEMP = (BigInt *)malloc(sizeof(BigInt));
		if(CALC_TEMP==NULL) {
			return -1;
		}
		CALC_TEMP->mem = NULL;
	} else {
		free(CALC_TEMP->mem);
		CALC_TEMP->mem = NULL;
	}

	return 0;
}

static retcode _set_oprand(BigInt **x, BigInt **y) {
	if(CALC_TEMP==NULL) {
		if(*x==NULL || *y==NULL) return -1;
	} else {
		if(*x==NULL && *y==NULL) {
			*x = CALC_TEMP;
			*y = CALC_TEMP;
		} else if(*x==NULL) *x = CALC_TEMP;
		else if(*y==NULL) *y = CALC_TEMP;
	}

	return 0;
}

static retcode _padding_bytes(uint8_t **p_mem, uint64_t size, uint64_t pad) {
	if(p_mem==NULL || *p_mem==NULL) return -1;

	uint64_t total_size = pad + size;
	uint8_t *temp_mem;
	if(total_size<pad || total_size<size)
		return -1;

	temp_mem = (uint8_t *)realloc(*p_mem, total_size);
	if(temp_mem==NULL)
		return -1;
	*p_mem = temp_mem;

	/* If the most significant byte(or, MSB) is the same or bigger than 128,
	* (when the number is negative), padding byte is 255(0b11111111)
	* When the number is positive, padding byte is 0(0b00000000)
	*/
	
	{
		uint64_t i;
		uint8_t pad_byte = ((*p_mem)[size-1]>=128) ? 0b11111111 : 0b00000000;
		for(i=size; i<total_size; i++) {
			(*p_mem)[i] = pad_byte;
		}
	}

	return 0;
}

static retcode _optimize_bytes(BigInt *bi) {
	uint8_t pad_byte, sign;

	if(bi->field == 1 || (bi->mem[bi->field-1]!=0 && bi->mem[bi->field-1]!=255)) return 1;

	sign = is_signed_bi(bi);
	if(sign)
		pad_byte = 0b11111111;
	else
		pad_byte = 0b00000000;

	{
		uint64_t dest_i = 1;
		uint8_t *n_mem;

		uint64_t i;
		for(i=bi->field-2; i>=0; i--) {
			if(bi->mem[i]!=pad_byte) {
				if((sign && bi->mem[i]>=128) || (~sign && bi->mem[i]<128))
					dest_i = i+1;
				else
					dest_i = i+2;
				break;
			}
		}

		n_mem = (uint8_t *)realloc(bi->mem, dest_i);
		if(n_mem==NULL) {
			return -1;
		}

		bi->mem = n_mem;
		bi->field = dest_i;
	}

	return 0;
}

void _print_bi_spac(const BigInt *bi) {
	static uint64_t count = 0;
	printf("\nspac print num: %lld\n", count);
	printf("Field: %lld\n", bi->field);
	printf("=== Mem ===\n");
	
	uint64_t i;
	for(i=0; i<bi->field; i++) {
		printf("%lld: [%lld] ", i, (bi->mem)[i]);
	}
	printf("\n=== End print spac ===\n\n");
	count++;
}