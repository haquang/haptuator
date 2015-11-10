CC = g++
CPPFLAGS = -Wall -g -std=gnu++11 -w -I/usr/realtime/include -O2 -fpermissive
LDFLAGS = -lm -lcomedi -lboost_system -lboost_thread -lncurses  -lgsl -lgslcblas -pthread

all: main.o daqdevice.o daqacc.o haptuator.o referencemodel.o filtermodel.o
	${CC}   main.o daqdevice.o daqacc.o haptuator.o referencemodel.o filtermodel.o ${LDFLAGS} -o haptuator

daqdevice.o: daqdevice.cpp daqdevice.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} daqdevice.cpp
	
referencemodel.o: referencemodel.cpp referencemodel.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} referencemodel.cpp
	
filtermodel.o: filtermodel.cpp filtermodel.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} filtermodel.cpp
		
haptuator.o: haptuator.cpp haptuator.h referencemodel.h filtermodel.h
	${CC} -c ${CPPFLAGS} ${LDFLAGS} haptuator.cpp
	
daqacc.o: daqacc.cpp daqacc.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} daqacc.cpp
		
main.o: main.cpp daqacc.h daqdevice.h haptuator.h referencemodel.h filtermodel.h
	${CC} -c ${CPPFLAGS} main.cpp 

clean:
	rm -rf haptuator *.o *~

