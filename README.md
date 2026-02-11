# expmath
expmath는 현재 개발 중에 있으며 추후 더 많은 기능이 추가될 예정입니다.

여기에 명시되지 않은 내용은 아직 개발 중이거나 라이브러리 외부에는 드러나지 않는 기능입니다.
## BigInt (bigint.h)
BigInt는 64비트 정수 표현 범위를 넘어선 더 큰 범위의 정수를 표현할 수 있도록 도와주는 구조체입니다.

BigInt 구조체 변수를 생성하는 가장 쉬운 방법은 new_bi 함수를 사용하는 것입니다.

new_bi 함수는 부호가 있는 64비트 정수를 인수로 받고 동적 할당된 BigInt 구조체의 주소를 반환합니다.

```C
BigInt *eleven = new_bi(11);
```

new_bi_with_str 함수에 정수를 나타내는 문자열을 인수로 넘겨주어 새로운 BigInt 구조체를 생성할 수도 있습니다.

```C
BigInt *something = new_bi_with_str("-324729432947829749217941797394");
```

더 이상 필요없는 BigInt 구조체는 메모리 누수 방지를 위해 반드시 free_bi 함수로 할당 해제를 해야합니다.

```C
free_bi(eleven);
free_bi(something);
```

## BigInt 구조체의 구조
```C
typedef struct bigint {
  uint8_t *mem;
  uint64_t field;
} BigInt, BIGINT;
```
### mem 멤버 변수
실질적으로 정수를 저장한 위치를 가리키는 포인터입니다.

8비트 단위로 끊어서 정수의 비트열을 저장하며 맨 마지막 바이트의 최상위 비트는 부호 비트입니다.

음의 정수는 일반적인 변수와 같이 절댓값이 같은 양의 정수의 비트열의 2의 보수로 저장됩니다.

### field 멤버 변수
mem 변수가 가리키는 메모리 공간의 크기를 나타냅니다.

부호가 없는 64비트 정수 변수로 표현되었기 때문에 mem 변수가 가리킬 수 있는 저장 공간의 이론상 최대 크기는 2^63-1 바이트로 따라서 BigInt 구조체가 표현할 수 있는 이론상 가장 큰 정수는 2^((2^63-1)*8)-1 입니다.

## OUTPUT_STR_TEMP
OUTPUT_STR_TEMP는 BigInt 구조체가 표현하는 정수를 문자열로 나타내는 함수를 사용한 결과값을 임시 저장하는 정적 전역 변수입니다.

OUTPUT_STR_TEMP는 기본적으로 널 포인터를 가리키며, 최초로 문자열 변환 함수가 호출되어 결과를 생성하였을 때 그 문자열의 주소를 저장합니다.

그 이후에 해당 함수가 호출되어 결과를 생성하면 OUTPUT_STR_TEMP가 가리키는 문자열을 할당 해제한 후 결과로 나온 문자열의 주소를 저장합니다.

## CALC_TEMP
CALC_TEMP는 일부 연산 함수의 결과를 저장하는 정적 전역 포인터입니다.

CALC_TEMP는 기본적으로 널 포인터를 가리키며, 최초로 연산 함수가 호출되어 결과를 생성하였을 때 그 BigInt 구조체의 주소를 저장하는 역할을 합니다.

그 이후에 연산 함수가 결과를 생성한다면 CALC_TEMP가 가리키는 구조체를 재활용하여 결과값을 저장합니다.

new_bi, new_bi_with_str을 제외한 모든 함수는 인수로 넘어온 NULL(0)을 CALC_TEMP가 가리키는 BigInt 구조체로 생각합니다.

> [!CAUTION]
> BigInt 구조체의 주소를 반환하는 연산 함수들은 CALC_TEMP가 가리키는 구조체에 결과값을 저장하고 그 구조체의 주소를 반환하는 구조이기 때문에
> 해당 함수들이 오류 없이 연산을 수행했고 flush_calctemp 함수가 실행된 적 없는 한 CALC_TEMP에 저장된 동일한 메모리 주소를 반환합니다.
> 해당 함수들을 실행하면 CALC_TEMP가 가리키는 BigInt 구조체의 내용이 변경되기 때문에 연산의 결과를 추후에 다시 사용해야된다면 copy_calctemp 같은 함수로 따로 저장해야합니다.

> [!IMPORTANT]
> CALC_TEMP에 결과값을 저장하는 함수를 실행한 적 없는 상태에서 연산 함수에 NULL값을 넘겨주었을 경우 연산이 불가능하기에 NULL값을 반환합니다.

## 연산 함수
사칙연산, 비트 연산, 비교 연산, 부호 연산 등을 수행합니다.

인수로 NULL(0)이 주어진 경우 CALC_TEMP가 가리키는 BigInt 구조체로 간주하고 계산합니다.

> [!IMPORTANT]
> 함수 내부적으로 연산에 필요한 동적 할당에 실패하였거나 CALC_TEMP가 널 포인터를 가리키는 상태에서 연산 함수의 인수로 NULL 값을 넘길 경우,
> 기타 오류가 발생하여 연산에 실패한 경우 반환형이 BigInt *인 함수는 NULL값을, retcode인 함수는 -1을 반환합니다.

### 기본 연산 함수
생성된 BigInt 구조체로 사칙연산을 수행할 수 있으며 현재 덧셈, 뺄셈, 곱셈 함수를 사용할 수 있습니다.

아래의 함수들은 CALC_TEMP가 가리키는 BigInt 구조체에 결과값을 임시 저장하고 그 주소를 반환합니다.
+ BigInt *sum_bi(const BigInt *, const BigInt *);
+ BigInt *sub_bi(const BigInt *, const BigInt *);
+ BigInt *mul_bi(const BigInt *, const BigInt *);

애래의 함수들은 CALC_TEMP에 결과값을 저장하지 않고 첫번째 인수가 가리키는 BigInt 구조체에 결과를 저장합니다.
+ BigInt *sum_bi_with_assign(BigInt *, const BigInt *);
+ BigInt *sub_bi_with_assign(BigInt *, const BigInt *);
+ BigInt *mul_bi_with_assign(BigInt *, const BigInt *);
```C
BigInt *a = new_bi(33), *b = new_bi(77);
sum_bi_with_assign(a, b);
mul_bi(NULL, a); // NULL은 CALC_TEMP를 의미함
```

### 비교 연산 함수
생성된 BigInt 구조체로 대소 비교를 할 수 있습니다.

비교 연산 함수들은 첫번째 인수가 가리키는 BigInt에 대하여 두번째 인수가 가리키는 BigInt가 큰 지, 작은 지, 같은 지 등을 비교하고 참이라면 1, 거짓이라면 0을 반환합니다. 

비교 연산 함수들은 CALC_TEMP의 내용을 변경하지 않습니다.
+ retcode is_bigger_bi(const BigInt *, const BigInt *);
+ retcode is_smaller_bi(const BigInt *, const BigInt *);
+ retcode is_same_bi(const BigInt *, const BigInt *);
+ retcode is_bigger_same_bi(const BigInt *, const BigInt *);
+ retcode is_smaller_same_bi(const BigInt *, const BigInt *);
```C
BigInt *two = new_bi(2), *hundred = new_bi(100);
if(is_bigger_bi(two, hundred)) printf("yes");
else printf("no"); // else의 구문이 실행됨

if(is_same_bi(two, hundred)) printf("yes"); // 실행되지 않음
```

### 비트 논리 연산 함수
생성된 BigInt 구조체로 비트 논리곱, 논리합, 베타적 논리합과 같은 비트 논리 연산을 수행할 수 있습니다.

두 BigInt의 mem이 가리키는 비트열 공간이 비트 논리 연산 대상입니다.

이때, 비트열의 크기가 상대적으로 작은 정수는 양수인 경우 0(0b00000000)으로, 음수인 경우 255(0b11111111)으로 패딩하여 상대 비트열과 크기를 맞춥니다.

아래의 함수들은 CALC_TEMP가 가리키는 BigInt 구조체에 결과값을 임시 저장하고 그 주소를 반환합니다.
+ BigInt *and_bi(const BigInt *, const BigInt *);
+ BigInt *and_bi(const BigInt *, const BigInt *);
+ BigInt *and_bi(const BigInt *, const BigInt *);

```C
BigInt *ten = new_bi(10), *eight = new_bi_with_str("8"), *res;
res = and_bi(ten, eight); // 8
res = or_bi(ten, eight); // 10
res = xor_bi(ten, eight); // 2
```

### 부호 연산 함수
생성된 BigInt 구조체의 부호를 반전 시킬 수 있습니다.

연산 성공시 0을, 실패시 -1을 반환합니다.
+ retcode convert_sign_bi(BigInt *);

### CALC_TEMP, OUTPUT_STR_TEMP 관련 함수
아래의 함수로 CALC_TEMP가 가리키는 구조체를 할당 해제하고, OUTPUT_STR_TEMP가 가리키는 문자열을 할당 해제할 수 있습니다.
+ void flush_bi_calctemp(void);
+ void flush_bi_outputstrtemp(void);

아래의 함수로 CALC_TEMP가 표현할 새로운 정수를 부호가 있는 64비트 정수 범위 내에서 지정할 수 있습니다.

실패할 경우 -1, 성공할 경우 0을 반환합니다.
+ retcode set_bi_calctemp_with_number(int64_t);

### 기타 함수
아래의 함수로 인수가 가리키는 BigInt의 부호를 판별할 수 있습니다.
양수인 경우 0, 음수인 경우 1을 반환하며 오류가 발생한 경우 -1을 반환합니다.
+ retcode is_signed_bi(const BigInt *);
