all: prog1.o
	gcc prog1.o -o prog1 -g

prog1.o: prog1.c 
	gcc -c prog1.c 

clean:
	rm -f *.o
	
