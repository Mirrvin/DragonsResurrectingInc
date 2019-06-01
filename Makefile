all: smoki

smoki: main.o
	mpic++ main.o -o -cl-opt-disable smoki

main.o: main.cpp main.h
	mpic++ main.cpp -c -Wall -cl-opt-disable

clear: 
	rm *.o smoki
run:
	mpirun -hostfile ./hostfile -np 4 ./smoki