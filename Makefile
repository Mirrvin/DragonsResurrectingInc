CXX=mpic++

all: smoki

smoki: main.o Monitor.o Specialist.o
	$(CXX) main.o Monitor.o Specialist.o -o smoki

main.o: main.cpp main.h
	$(CXX) main.cpp -c -Wall

Monitor.o: Monitor.cpp Monitor.h
	$(CXX) Monitor.cpp -c -Wall

Specialist.o: specialists/Specialist.cpp specialists/Specialist.h
	$(CXX) specialists/Specialist.cpp -c -Wall

clear: 
	rm *.o smoki
run:
	mpirun --hostfile ./hostfile -np 8 ./smoki
