
all: serial posix omp

serial: serial.o
	g++ -o serial serial.o -fopenmp

serial.o: matrixSerial.cc
	g++ -c -o serial.o matrixSerial.cc


posix: posix.o
	g++ -o posix posix.o -fopenmp -pthread

posix.o: matrixPthread.cc
	g++ -c -o posix.o matrixPthread.cc


omp: omp.o
	g++ -o omp omp.o -fopenmp

omp.o: matrixOMP.cc
	g++ -c -o omp.o matrixOMP.cc
