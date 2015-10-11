#include<stdio.h>
#include<pthread.h>
#include<time.h>
#include<sys/time.h>

void sequential_naive_vectorAdd(int *a, int *b, int *c, int size){
    for(int i = 0; i < size; i++){
        c[i] = a[i] + b[i];
    }
}

typedef struct argument{
	int *a;
	int *b;
	int *c;
	int tid;
	int size;
	int num_thread;
} argument;

// vectorAddParallel Input_size thread
int main(int argc, char *argv[]){

	if(argc != 3){
		// print proper error
		return 1;
	}

	int size = atoi(argv[1]);
	int num_thread = atoi(argv[2]);
	
	pthread_t threads[num_thread];
	argument arguments[num_thread];

	int *a, *b, *c;
	a = (int *)malloc(sizeof(int) * size);
	b = (int *)malloc(sizeof(int) * size);
	c = (int *)malloc(sizeof(int) * size);

	for(int i = 0; i <size; i++){
		a[i] = rand() % 100;
		b[i] = rand() % 100;
		c[i] = 0;
	}

	gettimeofday(&start, NULL);

    sequential_naive_vectorAdd(a, b, c, size);

    gettimeofday(&end, NULL);

    double diff = (end.tv_sec - start.tv_sec) * 1000.0 +
        (end.tv_usec - start.tv_usec);

    printf("Naive VectorAdd time calculation duration: %8.5fms\n", diff);







	return 0;
}
