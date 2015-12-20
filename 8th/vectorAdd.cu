#include<stdio.h>
#include<cuda.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>

#define STREAM_NUMBER 10

//Macro for checking cuda errors following a cuda launch or api call
#define CUDA_CHECK_RETURN(value) {											\
	cudaError_t _m_cudaStat = value;										\
	if (_m_cudaStat != cudaSuccess) {										\
		fprintf(stderr, "Error %s at line %d in file %s\n",					\
				cudaGetErrorString(_m_cudaStat), __LINE__, __FILE__);		\
		exit(1);															\
	} }

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

void vector_add(int *a, int *b, int *c, int size) {
	for (int i = 0; i < size; i++) {
		c[i] = a[i] + b[i];
	}
}

__global__ void vector_add_kernel_coalesced_access(int *a_d, int *b_d, int *d_d, int work_per_thread, int input_length, int totalThreads) {

	int tid = blockIdx.x * blockDim.x + threadIdx.x;

	for(int i = 0; i < work_per_thread && tid < input_length; i++, tid += totalThreads){
		d_d[tid] = a_d[tid] + b_d[tid];
	}

}

int main(int argc, char *argv[]) {

	int input_length, block_size, work_per_thread;
	struct timeval start, end;
	cudaStream_t streams[STREAM_NUMBER];


	if (argc != 2) {
		printf("Usage is: VectorAddParallel input_length block_size work_per_thread\nNow, type input_length: ");
		scanf("%d", &input_length);
		printf("Type block_size: ");
		scanf("%d", &block_size);
		printf("Type work_per_thread: ");
		scanf("%d", &work_per_thread);
	} else{
		input_length = atoi(argv[1]);
		block_size = atoi(argv[2]);
		work_per_thread = atoi(argv[3]);
	}

	// Arrays declaration
	int *a_h, *b_h, *c_h, *d_h;
	int *a_d, *b_d, *d_d;

	// Allocation on Host
	//a_h = (int *) malloc(sizeof(int) * input_length);
	//b_h = (int *) malloc(sizeof(int) * input_length);
	c_h = (int *) malloc(sizeof(int) * input_length);
	//d_h = (int *) malloc(sizeof(int) * input_length);

	cudaMallocHost(&a_h, sizeof(int) * input_length);
	cudaMallocHost(&b_h, sizeof(int) * input_length);
	//cudaMallocHost(&c_h, sizeof(int) * input_length);
	cudaMallocHost(&d_h, sizeof(int) * input_length);
	
	// Allocation on Device
	CUDA_CHECK_RETURN(cudaMalloc((void **)&a_d, sizeof(int)*input_length));
	CUDA_CHECK_RETURN(cudaMalloc((void **)&b_d, sizeof(int)*input_length));
	CUDA_CHECK_RETURN(cudaMalloc((void **)&d_d, sizeof(int)*input_length));

	// Initialization on host side
	initialize(a_h, b_h, c_h, d_h, input_length);

	// Run host code
	gettimeofday(&start, NULL);
	vector_add(a_h, b_h, c_h, input_length);
	gettimeofday(&end, NULL);

	double diff = (end.tv_sec - start.tv_sec) * 1000000.0 +
		(end.tv_usec - start.tv_usec);

	printf("Host VectorAdd time calculation duration: %8.5fms\n", diff / 1000);

	// Run device code

	
	for (int i = 0; i < STREAM_NUMBER; ++i) {
		cudaStreamCreate(&streams[i]);
	}

	gettimeofday(&start, NULL);

	// Copy input data to device
	//CUDA_CHECK_RETURN(cudaMemcpy(a_d, a_h, sizeof(int)*input_length, cudaMemcpyHostToDevice));
	//CUDA_CHECK_RETURN(cudaMemcpy(b_d, b_h, sizeof(int)*input_length, cudaMemcpyHostToDevice));

	int streamSize = input_length / STREAM_NUMBER;
	int streamBytes = streamSize * 4;
	
	int grid_size = (streamSize - 1) / (block_size * work_per_thread) + 1;
	dim3 grid_dime(grid_size, 1, 1);
	dim3 block_dime(block_size, 1, 1);
	int totalThreads = grid_size * block_size;

	for (int i = 0; i < STREAM_NUMBER + 2; ++i) {
		int offset = i * streamSize;

		if(i < STREAM_NUMBER){
			cudaMemcpyAsync(&a_d[offset], &a_h[offset], streamBytes, cudaMemcpyHostToDevice, streams[i]);
			cudaMemcpyAsync(&b_d[offset], &b_h[offset], streamBytes, cudaMemcpyHostToDevice, streams[i]);
		}
		if(i > 0 && i < STREAM_NUMBER + 1){
			offset = (i-1) * streamSize;
			vector_add_kernel_coalesced_access<<< grid_dime, block_dime, 0, streams[i-1]>>>(&a_d[offset], &b_d[offset], &d_d[offset], work_per_thread, streamSize, totalThreads);
		}
		if(i > 1){
			offset = (i-2) * streamSize;
			cudaMemcpyAsync(&d_h[offset], &d_d[offset], streamBytes, cudaMemcpyDeviceToHost, streams[i-2]);
		}
	}
	
	//Copy back the result
	//CUDA_CHECK_RETURN(cudaMemcpy(d_h, d_d, sizeof(int)*input_length, cudaMemcpyDeviceToHost));

	CUDA_CHECK_RETURN(cudaDeviceSynchronize());	// Wait for the GPU launched work to complete
	CUDA_CHECK_RETURN(cudaGetLastError());
	gettimeofday(&end, NULL);

	diff = (end.tv_sec - start.tv_sec) * 1000000.0 +
		(end.tv_usec - start.tv_usec);

	printf("Device VectorAdd time calculation duration: %8.5fms\n", diff / 1000);

	// Validation
	validate(c_h, d_h, input_length);

	cudaFree(a_h);
	cudaFree(b_h);
	free(c_h);
	cudaFree(d_h);
	cudaFree(a_d);
	cudaFree(b_d);
	cudaFree(d_d);

	return 0;
}
