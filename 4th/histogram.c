#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include<sys/time.h>

#define RANGE 10000

int size, parts, num_threads, *a, *histogram, *histograms;

pthread_mutex_t *locks;
pthread_mutex_t lock;

void *naive_parallel_histogram(void *arg){
	
	int tid = (int)arg;
	
	int chunkSize = size / num_threads;
	int si = chunkSize * tid;
	int ei = si + chunkSize;
	
	int bucketSize = RANGE / parts;
    for (int i = si; i < ei; i++) {
        histogram[a[i] / bucketSize]++;
    }

}

void *first_parallel_histogram(void *arg){

    int tid = (int)arg;

    int chunkSize = size / num_threads;
    int si = chunkSize * tid;
    int ei = si + chunkSize;

    int bucketSize = RANGE / parts;
    for (int i = si; i < ei; i++) {
        pthread_mutex_lock(&lock);
        histogram[a[i] / bucketSize]++;
        pthread_mutex_unlock(&lock);
    }

}

void *second_parallel_histogram(void *arg){

    int tid = (int)arg;
    
    int chunkSize = size / num_threads;
    int si = chunkSize * tid;
    int ei = si + chunkSize;
    
    int bucketSize = RANGE / parts;
	int index;
    for (int i = si; i < ei; i++) {
		index = a[i] / bucketSize;
        pthread_mutex_lock(&locks[index]);
        histogram[index]++;
        pthread_mutex_unlock(&locks[index]);
    }

}

void *third_parallel_histogram(void *arg){

    int tid = (int)arg;

    int chunkSize = size / num_threads;
    int si = chunkSize * tid;
    int ei = si + chunkSize;

    int bucketSize = RANGE / parts;
    int index;
	int *hist = &(histograms[tid * parts]);
    for (int i = si; i < ei; i++) {
        index = a[i] / bucketSize;
        hist[index]++;
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
		//printf("%9d", histogram[i]);
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

	locks = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * parts);

	pthread_mutex_init(&lock, NULL);

	for(int i = 0; i < parts; i++){
		pthread_mutex_init(&locks[i], NULL);		
	}

	a = (int *) malloc(sizeof(int) * size);
	histogram = (int *) calloc(parts, sizeof(int));
	histograms = (int *) calloc(parts * num_threads, sizeof(int));

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
// Naive Parallel
	memset(histogram, 0, sizeof(int) * parts);

	gettimeofday(&start, NULL);

	for(int i = 0; i < num_threads; i++){
		pthread_create(&threads[i], 0, naive_parallel_histogram, (void *)i);
	}

	void *status;
	for(int i = 0; i < num_threads; ++i) {
		pthread_join(threads[i], &status);
	}

    gettimeofday(&end, NULL);

    diff = (end.tv_sec - start.tv_sec) * 1000000.0
            + (end.tv_usec - start.tv_usec);
    printf("Naive Parallel Hisrogram time duration: %.4fms\n", diff / 1000);

	print_histo(histogram, parts);
// First Parallel
    memset(histogram, 0, sizeof(int) * parts);

    gettimeofday(&start, NULL);

    for(int i = 0; i < num_threads; i++){
        pthread_create(&threads[i], 0, first_parallel_histogram, (void *)i);
    }

    for(int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], &status);
    }

    gettimeofday(&end, NULL);

    diff = (end.tv_sec - start.tv_sec) * 1000000.0
            + (end.tv_usec - start.tv_usec);
    printf("First Parallel Hisrogram time duration: %.4fms\n", diff / 1000);

    print_histo(histogram, parts);
// Second Parallel
    memset(histogram, 0, sizeof(int) * parts);

    gettimeofday(&start, NULL);

    for(int i = 0; i < num_threads; i++){
        pthread_create(&threads[i], 0, second_parallel_histogram, (void *)i);
    }

    for(int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], &status);
    }

    gettimeofday(&end, NULL);

    diff = (end.tv_sec - start.tv_sec) * 1000000.0
            + (end.tv_usec - start.tv_usec);
    printf("Second Parallel Hisrogram time duration: %.4fms\n", diff / 1000);

    print_histo(histogram, parts);

//Third?! Version! Really!?
	memset(histogram, 0, sizeof(int) * parts);
	for(int i = 0; i < parts; i++){
		pthread_mutex_destroy(&locks[i]);
	}

    gettimeofday(&start, NULL);

    for(int i = 0; i < num_threads; i++){
        pthread_create(&threads[i], 0, third_parallel_histogram, (void *)i);
    }

    for(int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], &status);
		for(int j = 0; j < parts; j++){
			histogram[j] += histograms[i * parts + j];
		}
    }

    gettimeofday(&end, NULL);

    diff = (end.tv_sec - start.tv_sec) * 1000000.0
            + (end.tv_usec - start.tv_usec);
    printf("Third Parallel Hisrogram time duration: %.4fms\n", diff / 1000);

    print_histo(histogram, parts);

//End
	
    pthread_mutex_destroy(&lock);
    for(int i = 0; i < parts; i++){
        pthread_mutex_destroy(&locks[i]);
    }

	free(histogram);
	free(histograms);
	free(a);

	return 0;
}
