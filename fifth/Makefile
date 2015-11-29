all: VectorOperations VectorOperationsProfile VectorOperationsLineProfile

VectorOperations: VectorOperations.c 
	gcc -std=c99 VectorOperations.c -o VectorOperations -lm

VectorOperationsProfile: VectorOperations.c 
	gcc -std=c99 VectorOperations.c -o VectorOperationsProfile -g -pg -lm

VectorOperationsLineProfile: VectorOperations.c
	gcc -std=c99 -fprofile-arcs -ftest-coverage VectorOperations.c -o VectorOperationsLineProfile -lm

clean:
	rm VectorOperations VectorOperationsProfile VectorOperationsLineProfile
