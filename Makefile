build: movies.c
	gcc -o movies  movies.c -Wall -g 
run:
	./movies
clean:
	rm -rf movies *.o
