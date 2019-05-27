all: smoki

smoki: main.o init.o
	mpicc main.o init.o -o smoki

init.o: init.c 
	mpicc init.c -c -Wall

main.o: main.c main.h
	mpicc main.c -c -Wall

clear: 
	rm *.o smoki
