all: vectorAddOptimized vectorAdd

vectorAdd: vectorAdd.c
	gcc -std=c99 vectorAdd.c -o vectorAdd


vectorAddOptimized: vectorAdd.c
	gcc -std=c99 vectorAdd.c -o vectorAddOptimized -O3

clean:
	rm vectorAdd vectorAddOptimized
