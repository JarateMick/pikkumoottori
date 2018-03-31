#define _CRT_SECURE_NO_WARNINGS 1 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>


#include "test1.h"
#define cdecl

#define MAXTOKENLEN 128
#define MAXTOKENS 64

enum type_tag {
	type, qualifier, identifier
};

struct Token {
	char type;
	char string[MAXTOKENLEN];
};


int top = -1;
struct Token stack[MAXTOKENS];
struct Token this;

#define pop      stack[--top]
#define push(s)  stack[++top] = s

enum type_tag classify_string()
{
	// return type qualifier or identifier
	const char* s = this.string;

	if (!strcmp(s, "const")) {
		strcpy(s, "read-only");
		return qualifier;
	}

	if (strcmp(s, "volatile") == 0)
		return qualifier;
	if (strcmp(s, "int") == 0)
		return type;
	else if (strcmp(s, "float") == 0)
		return type;
	else if (strcmp(s, "double") == 0)
		return type;
	else if (strcmp(s, "struct") == 0)
		return type;
	else if (strcmp(s, "unsigned") == 0)
		return type;
	else if (strcmp(s, "union") == 0)
		return type;
	else if (strcmp(s, "enum") == 0)
		return type;
	else if (strcmp(s, "short") == 0)
		return type;
	else if (strcmp(s, "void") == 0)
		return type;
	return identifier;
}

void gettoken(void)
{
	char* p = this.string;

	while ((*p = getchar()) == ' ');

	if (isalnum(*p))
	{
		while (isalnum(*++p = getchar()));
		ungetc(*p, stdin);
		*p = '\0';
		this.type = classify_string();
		return;
	}

	if (*p == '*') {
		strcpy(this.string, "pointer to");
		this.type = '*';
		return;
	}

	this.string[1] = '\0';
	this.type = *p;
	return;
}

char* teststr = "int alpha = 10;";
void read_to_first_identifier()
{
	gettoken(teststr);
	while (this.type != identifier) {
		push(this);
		gettoken();
	}

	printf("%s is ", this.string);
	gettoken();
}

deal_with_arrays() {
	while (this.type == '[') {
		printf("array ");
		gettoken();
		if (isdigit(this.string[0])) {
			printf("0..%d", atoi(this.string) - 1);
			gettoken();
		}
		gettoken();
		printf("of ");
	}
}

deal_with_function_args() {
	while (this.type != ')') {
		gettoken();
		printf("function returning ");
	}
}

deal_with_pointers() {
	while (stack[top].type == '*') {
		printf("%s ", pop.string);
	}
}

deal_with_declarator() {
	switch (this.type)
	{
	default:
	case '[': deal_with_arrays(); break;
	case '(': deal_with_function_args(); break;
		break;
	}

	deal_with_pointers();

	while (top >= 0)
	{
		if (stack[top].type == '(') {
			pop;
			gettoken();
			deal_with_declarator();
		}
		else
		{
			printf("%s ", pop.string);
		}
	}
}

typedef float vec[2];

typedef struct {
	float x, y;
} vec2;

// Vasen Oikee Dest

typedef struct {
	float values[16];
} Mat4;


// 0  1   2   3
// 4  5   6   7
// 8  9   10  11
// 12 13  14  15

typedef struct
{
	float x, y, z;
} Vec3;

void vec3_add_s(Vec3* v, float s)
{
	v->x += s;
	v->y += s;
	v->z += s;
}

void vec3_add_v(Vec3* dest, Vec3* right, Vec3* left)
{
	dest->x = right->x + left->x;
	dest->x = right->y + left->y;
	dest->x = right->z + left->z;
}


Mat4 mat4_zero()
{
	Mat4 result;
	memset(&result, 0, sizeof(Mat4));
	return result;
}

Mat4 mat4_identy()
{
	Mat4 result;
	memset(&result, 0, sizeof(Mat4));
	result.values[0] = 1;
	result.values[4] = 1;
	result.values[8] = 1;
	result.values[12] = 1;
	return result;
}

void mat4_translate(Mat4* t, Vec3* v)
{
	t->values[3] += v->x;
	t->values[7] += v->y;
	t->values[11] += v->z;
}

void mat4_translate_dest(Mat4* right, Vec3* left, Mat4* dest)
{
	dest->values[3] = right->values[3] + left->x;
	dest->values[7] = right->values[7] + left->y;
	dest->values[11] = right->values[11] + left->z;
}

void mat4_multiply_s(Mat4* right, float scalar)
{
	for (int i = 0; i < 16; ++i)
	{
		right->values[i] *= scalar;
	}
}

void mat4_multiply_m(Mat4* right, Mat4* left)
{
}


#include <stdint.h>
typedef uint32_t uint32;
typedef uint8_t uint8;

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

void printBinary(uint8 byte)
{
	printf(" " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
}

int main()
{


	{
		uint8 x = 0b0101100;
		uint8 a = x & (x - 1);

		printf(" x & (x - 1) ");
		printBinary(x);
		printBinary(a);

		// 0 if zero
		// also determine if is power of two by folliwing with zero test

		printf("is power of 2 %i ", x & (x - 1) == 0);

		x = 0b00010000;
		a = x & (x - 1);
		printf("is power of 2 %i ", (x & (x - 1)) == 0);
	}
	puts("");
	
	{
		// all 1 if none	
		// turn on the most rightmost 0-bit
		uint8 x = 0b01100111;
		uint8 a = x | (x + 1);

		printf(" x | (x + 1) ");
		printBinary(x);
		printBinary(a);
	}
	puts("");

	{
		// turn off trailing one's
		uint8 x = 0b01100111;
		uint8 a = x & (x + 1);

		printf(" x & (x + 1) ");
		printBinary(x);
		printBinary(a);

		// also can determine if 
		// integer is teh form 2n - 1
		// 0 or all 1's aply teh formula followed by 0-test
	}
	puts("");

	{
		// turn on trailing zero's
		uint8 x = 0b01100000;
		uint8 a = x | (x - 1);

		printf(" x & (x + 1) ");
		printBinary(x);
		printBinary(a);
	}

	{
		// create word with a single 1-bit at the position off the rightmost 0-bit in x
		uint8 x = 0b01100000;
		uint8 a = -x & (x + 1);

		printf(" x & (x + 1) ");
		printBinary(x);
		printBinary(a);


		// same for 
		uint8 x = 0b01100000;
		uint8 a = -x | (x - 1);
	}


	getchar();







	return 0;
	vec lol;
	lol[0] = 10;
	lol[1] = 20;



	return 0;
#ifdef cdecl
	read_to_first_identifier();
	deal_with_declarator();
	printf("\n");

	return 0;
#else


	printf("Hello c\n");

	time_t t = _I32_MAX;
	time_t now;
	time(&now);

	char* time = ctime(&t);
	printf("%s\n", time);

	const int a = 4;



	double seconds = difftime(t, now);
	double minutes = seconds / 60.0;
	double hours = minutes / 60.0;
	double days = hours / 24.0;
	double years = days / 365.0;

	printf("%f %f %f %f\n", years, days, hours, minutes, seconds);

	int p = 2;
	int apple = sizeof(int) * p;

	initAll();
#endif

	getchar();


	return 0;
}