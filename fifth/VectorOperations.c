#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void initialize(int *a, int *b, int *c, int *d, int input_length) {
	for (int i = 0; i < input_length; i++) {
		a[i] = rand() % 100;
		b[i] = rand() % 100;
		c[i] = 0;
		d[i] = 0;
	}
}

void validate(int *a, int *b, int length) {
	for (int i = 0; i < length; ++i) {
		if (a[i] != b[i]) {
			printf("Different value detected at position: %d,"
					"expected %d but get %d\n", i, a[i], b[i]);
			break;
		}
	}
}

void vector_sincos(int *a, int *b, int *c, int size) {
	for (int i = 0; i < size; i++) {
		c[i] = (int) sin(a[i]) * (int) cos(b[i]);
	}
}

void vector_multiply(int *a, int *b, int *c, int size) {
	for (int i = 0; i < size; i++) {
		c[i] = a[i] * b[i];
	}
}

void vector_add(int *a, int *b, int *c, int size) {
	for (int i = 0; i < size; i++) {
		c[i] = a[i] + b[i];
	}
}

int main(int argc, char *argv[]) {

	int input_length;

	if (argc != 2) {
		printf("Usage is: VectorAddParallel input_length, type input_length: ");
		scanf("%d", &input_length);
		return EXIT_FAILURE;
	} else{
		input_length = atoi(argv[1]);
	}

	int *a, *b, *c, *d;
	a = (int *) malloc(sizeof(int) * input_length);
	b = (int *) malloc(sizeof(int) * input_length);
	c = (int *) malloc(sizeof(int) * input_length);
	d = (int *) malloc(sizeof(int) * input_length);

	initialize(a, b, c, d, input_length);

	// addition
	vector_add(a, b, c, input_length);
	vector_add(a, b, d, input_length);

	validate(c, d, input_length);

	// multiplication
	vector_multiply(a, b, c, input_length);
	vector_multiply(a, b, d, input_length);

	validate(c, d, input_length);

	// Sin & Cos
	vector_sincos(a, b, c, input_length);
	vector_sincos(a, b, d, input_length);

	validate(c, d, input_length);

	return 0;
}

