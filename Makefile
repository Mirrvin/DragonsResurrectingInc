all: smoki

smoki: main.o
	mpic++ main.o -o smoki

main.o: main.cpp main.h
	mpic++ main.cpp -c -Wall

clear: 
	rm *.o smoki
run:
	mpirun --hostfile ./hostfile -np 4 ./smoki
