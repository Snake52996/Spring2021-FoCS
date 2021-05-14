all: palindrome

palindrome: palindrome.c table
	gcc -Wall -O3 -s -std=c99 -o palindrome palindrome.c
table: generate
	./generate
generate: generator.c
	gcc -Wall -O3 -s -std=c99 -o generate generator.c
.PHONY: clean
clean:
	rm palindrome 2>/dev/null
	rm table 2>/dev/null
	rm generate 2>/dev/null
