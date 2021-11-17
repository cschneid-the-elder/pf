#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#define EIGHT_K 8192
#define HALF_A_GIG 536870912
#define PRIMES_SIZE HALF_A_GIG //EIGHT_K //256
#define MAX_PRIME ((unsigned long)PRIMES_SIZE * 8)

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

void initPrimes( void );
void setOsets( size_t, int *, int * );
void printFactors( unsigned long );

unsigned char primes[PRIMES_SIZE];
unsigned char masksOn[8] = {
0b10000000,
0b01000000,
0b00100000,
0b00010000,
0b00001000,
0b00000100,
0b00000010,
0b00000001
};
unsigned char masksOff[8] = {
0b01111111,
0b10111111,
0b11011111,
0b11101111,
0b11110111,
0b11111011,
0b11111101,
0b11111110
};

int main( int argc, char **argv ) {

	int byteOset=0,bitOset=0;
	unsigned long nb = strtoul(argv[1], NULL, 10);

	setOsets(nb, &byteOset, &bitOset);
	if (byteOset > PRIMES_SIZE) {
		printf("no, not %ld\n", nb);
		printf("MAX_PRIME = %ld\n", MAX_PRIME);
		exit(12);
	}

	initPrimes();

	/*
	for (int p=1; p<=MAX_PRIME; p++) {
		setOsets(p,&byteOset,&bitOset);
			printf("%d %d %d", p, byteOset, bitOset);
			printf(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(primes[byteOset]));
			printf(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(masksOn[bitOset]));
		if (primes[byteOset] & masksOn[bitOset]) {
			printf(" prime");
		}
		printf("\n");
	}
	*/

	printFactors(nb);

	return 0;
}

void initPrimes() {
	//printf("initPrimes entry\n");
	int byteOset=0,bitOset=1;
	size_t i=2,j=0;
	FILE *f = fopen("pf.dat", "r+");

	if (f!=NULL) {
		if (fread(primes, sizeof(primes[0]), PRIMES_SIZE, f) != PRIMES_SIZE) {
			perror("pf initPrimes fread");
		}
		fclose(f);
		return;
	}

	memset(primes, 0xFF, PRIMES_SIZE);

	//printf(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(primes[0]));
	//printf("\n");
	//printf(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(primes[256]));
	//printf("\n");

	primes[0] &= masksOff[0]; //turn off #1 because 1 is not prime

	for (; i <= sqrt(MAX_PRIME); i++) {
		printf("i = %08ld\n", i);
		setOsets(i,&byteOset,&bitOset);
		if (byteOset > PRIMES_SIZE) {
			printf("i=%ld byteOset=%d badness\n", i, byteOset);
			exit(16);
		}
		if (primes[byteOset] & masksOn[bitOset]) {
			//printf("beginning j loop i = %d byteOset = %d bitOset = %d\n", i, byteOset, bitOset);
			//int oByteOset = byteOset;
			for (j=i*i; j <= MAX_PRIME; j += i) {
				//if (j%10000 == 0) printf(" %010ld", j);
				setOsets(j,&byteOset,&bitOset);
				if (byteOset > PRIMES_SIZE - 1) {
					printf("j=%ld byteOset=%d badness\n", j, byteOset);
					exit(16);
				}
				primes[byteOset] &= masksOff[bitOset];
			}
			//printf("primes[%d]", oByteOset);
			//printf(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(primes[oByteOset]));
			//printf("\n");
		}
	}

	f = fopen("pf.dat", "w");
	if (f==NULL) {
		perror("pf initPrimes fopen");
	} else {
		if (fwrite(primes, sizeof(primes[0]), PRIMES_SIZE, f) != PRIMES_SIZE) {
			perror("pf initPrimes fwrite");
		}
	}
	fclose(f);
	//printf("initPrimes exit\n");

}

void setOsets( size_t val, int *pByteOset, int *pBitOset ) {

	//printf("val = %d ", val);
	if (val <= 8) {
		*pByteOset=0;
		*pBitOset=val-1;
	} else {
		size_t val1 = val;
		size_t val2 = val;
		*pByteOset=--val1/8;
		*pBitOset=--val2%8;
	}

	//printf("byteOset = %d bitOset = %d\n", *pByteOset, *pBitOset);
}

void printFactors( unsigned long nb ) {
	//printf("printFactors\n");
	int p=0,byteOset=0,bitOset=0,exp=1;
	unsigned long work=nb;

	for (p=2; work>1; ) {
		setOsets(p, &byteOset, &bitOset);
		//printf("p = %d byteOset = %d bitOset = %d\n", p, byteOset, bitOset);
		if (primes[byteOset] & masksOn[bitOset]) {
			if (work%p == 0) {
				if (exp==1) {
					printf(" %d", p);
				}
				exp++;
				work=work/p;
			} else {
				if (exp>2) printf("**%d", --exp);
				p++;
				exp=1;
			}
		} else {
			if (exp>1) printf("^%d", --exp); //superfluous
			p++;
			exp=1;
		}
	}
	if (work == nb) {
		printf(" %ld", work);
	} else {
		if (exp>2) printf("**%d", --exp);
	}
	printf("\n");
}

