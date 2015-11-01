#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include<sys/time.h>

#define RANGE 10000

int size, parts, num_threads, *a, *histogram;

void *naive_parallel_histogram(void *arg){
	
	int tid = *(int *)arg;
	
	int chunkSize = size / num_threads;
	int si = chunkSize * tid;
	int ei = si + chunkSize;
	
	int bucketSize = RANGE / parts;
    for (int i = 0; i < size; i++) {
        histogram[a[i] / bucketSize]++;
    }

}

void sequential_histogram() {

	int bucketSize = RANGE / parts;
	for (int i = 0; i < size; i++) {
		histogram[a[i] / bucketSize]++;
	}
}

void print_histo(int *histogram, int parts) {

	printf("\n");
	int sum = 0;
	for (int i = 0; i < parts; ++i) {
		printf("%8d", histogram[i]);
		sum += histogram[i];
	}
	printf("\n Sum = %d", sum);
	printf("\n================================\n");
}



int main(int argc, char *argv[]){

	if (argc < 4) {
		printf("Enter arrayLength, numberOfParts and numberOfThreads: ");
		scanf("%d %d %d", &size, &parts, &num_threads);
	} else {
		size = atoi(argv[1]);
		parts = atoi(argv[2]);
		num_threads = atoi(argv[3]);
	}
	
	int *a = (int *) malloc(sizeof(int) * size);
	int *histogram = (int *) calloc(parts, sizeof(int));

	pthread_t threads[num_threads];
	struct timeval start, end;

	srand(time(NULL));
	for (int i = 0; i < size; i++) {
		a[i] = rand() % RANGE;
	}

	gettimeofday(&start, NULL);

	sequential_histogram();

    gettimeofday(&end, NULL);

	double diff = (end.tv_sec - start.tv_sec) * 1000000.0 +
		(end.tv_usec - start.tv_usec);

	printf("Sequential Histogram time duration: %.4fms\n", diff / 1000);
	
	print_histo(histogram, parts);
/*
	memset(histogram, 0, sizeof(int) * parts);

	gettimeofday(&start, NULL);

	for(int i = 0; i < num_threads; i++){
		pthread_create(&threads[i], 0, naive_parallel_histogram, (void *)&i);
	}

	void *status;
	for(int i = 0; i < num_threads; ++i) {
		pthread_join(threads[i], &status);
		free(status);	
	}

    gettimeofday(&end, NULL);

    diff = (end.tv_sec - start.tv_sec) * 1000000.0
            + (end.tv_usec - start.tv_usec);
    printf("Naive Parallel Hisrogram time duration: %.4fms\n", diff / 1000);

	print_histo(histogram, parts);
	*/
	return 0;
}
