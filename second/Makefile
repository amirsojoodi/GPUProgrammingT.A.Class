all: VectorAddParallel VectorAddParallelOptimized 

VectorAddParallel: VectorAddParallel.c 
	gcc -std=c99 VectorAddParallel.c -o VectorAddParallel -lpthread

VectorAddParallelOptimized: VectorAddParallel.c 
	gcc -std=c99 VectorAddParallel.c -o VectorAddParallelOptimized -O3 -lpthread 

clean:
	rm VectorAddParallel VectorAddParallelOptimized
