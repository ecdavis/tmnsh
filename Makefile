all:
	gcc -Wall -ansi -o tmnsh src/*.c

clean:
	rm -f src/*.o
