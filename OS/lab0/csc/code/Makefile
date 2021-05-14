all: fibo main
	echo -n "code/fibo.o code/main.o" > ../objects
fibo: fibo.c
	gcc -Wall -O3 -c -std=c99 $(INCLUDE) -o fibo.o fibo.c
main: main.c
	gcc -Wall -O3 -c -std=c99 $(INCLUDE) -o main.o main.c
.PHONY: clean
clean:
	rm *.o
