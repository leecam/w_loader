all: loader test_prog

loader: loader.c loader.S layout.ld
	gcc -o loader loader.c loader.S -nostdlib -g -T layout.ld -fPIC

test_prog:
	gcc -o test_prog test_prog.c -static
