#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>

float calTime()
{
	struct timeval start;
	float diff;

	gettimeofday(&start, NULL); 
	
	diff = (start.tv_sec) * 1000.0 + (start.tv_usec);
	//printf("%f\n",diff);
	return diff;
}

void sequential_vectorAdd(int *a, int *b, int *c, int size){
	for(int i = 0; i < size; i++){
		c[i] = a[i] + b[i];
	}
}

void sequential_vectorAdd_first_optimization(int *a, int *b, int *c, int size){
	for(int i = 0; i < size / 2; i++){
		c[i] = a[i] + b[i];
		c[size - i - 1] = a[size - i - 1] + b[size - i - 1];
	}
}


void sequential_vectorAdd_second_optimization(int *a, int *b, int *c, int size){
	for(int i = 0; i < size; i += 2){
        c[i] = a[i] + b[i];
        c[i+1] = a[i+1] + b[i+1];
    }   
}

void print_array(char *array_name, int *array, int size){
	printf("\n%s: \n[", array_name);
	for(int i = 0; i < size; i++){
		printf("%d, ", array[i]);
	}
	printf("]\n");
}

int main(int argc, char *argv[]){

	int *a, *b, *c;
	struct timeval start, end;

	int size = atoi(argv[1]);

	a = (int *)malloc(sizeof(int) * size);
	b = (int *)malloc(sizeof(int) * size);
	c = (int *)malloc(sizeof(int) * size);

	for(int i=0; i < size; i++){
		a[i] = rand() % 100;
		b[i] = rand() % 100;
		c[i] = 0;
	}

	gettimeofday(&start, NULL); 

	sequential_vectorAdd(a, b, c, size);

	gettimeofday(&end, NULL); 
	
	float diff = (end.tv_sec - start.tv_sec) * 1000.0 + 
		(end.tv_usec - start.tv_usec);
	
	printf("Naive VectorAdd time calculation duration: %8.5fms\n", diff);
	
	gettimeofday(&start, NULL); 

	sequential_vectorAdd_first_optimization(a, b, c, size);

	gettimeofday(&end, NULL); 
	
	diff = (end.tv_sec - start.tv_sec) * 1000.0 + 
		(end.tv_usec - start.tv_usec);
	
	printf("First VectorAdd time calculation duration: %8.5fms\n", diff);
	
	gettimeofday(&start, NULL); 

	sequential_vectorAdd_second_optimization(a, b, c, size);

	gettimeofday(&end, NULL); 
	
	diff = (end.tv_sec - start.tv_sec) * 1000.0 + 
		(end.tv_usec - start.tv_usec);
	
	printf("Second VectorAdd time calculation duration: %8.5fms\n", diff);
	// TODO: validation
	// TODO: validation
	// TODO: validation
	//print_array("A", a, size);
	//print_array("B", b, size);
	//print_array("C", c, size);


	return 0;
}
