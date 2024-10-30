
main: main.o bufferIO.o
	gcc main.o bufferIO.o -o main

main.o: main.c
	gcc -c main.c -o main.o

bufferIO.o: bufferIO.c 
	gcc -c bufferIO.c -o bufferIO.o

clean:
	rm *.o main 
	
