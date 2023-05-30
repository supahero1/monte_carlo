.PHONY: build
build: main.c
	gcc *.c -c
	nasm -f elf64 -g *.s
	gcc *.o -o main -g3 -lglfw -lGL -lm
	#valgrind --leak-check=full --show-leak-kinds=all ./main
	./main

