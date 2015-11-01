all: histogram histogramOptimized

histogram: histogram.c
	gcc -std=c99 histogram.c -o histogram -lpthread

histogramOptimized: histogram.c
	gcc -std=c99 histogram.c -o histogramOptimized -lpthread -O3

clean:
	rm histogram
