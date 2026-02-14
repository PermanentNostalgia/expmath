#ifndef _BIGINT_H_
#define _BIGINT_H_

#include <stdint.h>

typedef char retcode;

// 2's complement
typedef struct bigint {
	uint64_t field; // The mem array's length
	uint8_t *mem; // Storing number
} BigInt, BIGINT;

// Constructor
BigInt *new_bi(int64_t);
BigInt *new_bi_with_str(const char *);

// Copycat
BigInt *copy_bi(const BigInt *);
BigInt *copy_calctemp(void);

// Mathmatic-Operation
BigInt *sum_bi(const BigInt *, const BigInt *);
BigInt *sum_bi_with_assign(BigInt *, const BigInt *);
BigInt *sub_bi(const BigInt *, const BigInt *);
BigInt *sub_bi_with_assign(BigInt *, const BigInt *);
BigInt *mul_bi(const BigInt *, const BigInt *);
BigInt *mul_bi_with_assign(BigInt *, const BigInt *);
BigInt *div_bi(const BigInt *, const BigInt *);
BigInt *div_bi_with_assign(BigInt *, const BigInt *);
BigInt *mod_bi(const BigInt *, const BigInt *);
BigInt *mod_bi_with_assign(BigInt *, const BigInt *);

retcode convert_sign_bi(BigInt *);

// Comparison-Operator
retcode is_bigger_bi(const BigInt *, const BigInt *);
retcode is_smaller_bi(const BigInt *, const BigInt *);
retcode is_same_bi(const BigInt *, const BigInt *);
retcode is_bigger_same_bi(const BigInt *, const BigInt *);
retcode is_smaller_same_bi(const BigInt *, const BigInt *);

// BitwiseCompare-Operator
BigInt *and_bi(const BigInt *, const BigInt *);
BigInt *and_bi_with_assign(BigInt *, const BigInt *);
BigInt *or_bi(const BigInt *, const BigInt *);
BigInt *or_bi_with_assign(BigInt *, const BigInt *);
BigInt *xor_bi(const BigInt *, const BigInt *);
BigInt *xor_bi_with_assign(BigInt *, const BigInt *);
BigInt *not_bi(const BigInt *);

// BitwiseShift-Operator
BigInt *logi_lsh_bi(const BigInt *, const BigInt *);
BigInt *logi_rsh_bi(const BigInt *, const BigInt *);
BigInt *arit_rsh_bi(const BigInt *, const BigInt *);
BigInt *circ_lsh_bi(const BigInt *, const BigInt *);
BigInt *circ_rsh_bi(const BigInt *, const BigInt *);

// Etc-method
retcode is_signed_bi(const BigInt *);
retcode set_bi_calctemp_with_number(int64_t);

// TypeChange-method
char *bi_to_str(const BigInt *);

// Destructor
void free_bi(BigInt *);
void flush_bi_calctemp(void);
void flush_bi_outputstrtemp(void);

// Static Func(Operation)
static BigInt *_operate(const BigInt *, const BigInt *, BigInt *(*)(const BigInt *, const BigInt *), BigInt *);

static BigInt *_sum_bi(const BigInt *, const BigInt *);
static BigInt *_sub_bi(const BigInt *, const BigInt *);
static BigInt *_mul_bi(const BigInt *, const BigInt *);

static retcode _compare_operate(const BigInt *, const BigInt *);

static BigInt *_bitwise_compare_operate(const BigInt *, const BigInt *, uint8_t(*)(uint8_t,uint8_t), BigInt *);
static uint8_t _bitwise_and(uint8_t, uint8_t);
static uint8_t _bitwise_or(uint8_t, uint8_t);
static uint8_t _bitwise_xor(uint8_t, uint8_t);

// Static Func(Util)
static retcode _reset_calctemp(void);
static retcode _set_oprand(BigInt **, BigInt **);
static retcode _padding_bytes(uint8_t **, uint64_t, uint64_t);
static retcode _optimize_bytes(BigInt *);

// ONLY FOR DEBUG
void _print_bi_spac(const BigInt *);

#endif