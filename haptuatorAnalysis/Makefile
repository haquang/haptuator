CC = g++
CPPFLAGS = -Wall -g -std=gnu++11 -w -I/usr/realtime/include -O2 -fpermissive
LDFLAGS = -lm -lcomedi -lboost_system -lboost_thread -lncurses  -lgsl -lgslcblas -pthread

all: main.o daqdevice.o daqacc.o haptuator.o drivedataparser.o dynamicmodel.o inversedynamicmodel.o
	${CC}   main.o daqdevice.o daqacc.o haptuator.o drivedataparser.o dynamicmodel.o inversedynamicmodel.o ${LDFLAGS} -o haptuator

daqdevice.o: daqdevice.cpp daqdevice.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} daqdevice.cpp
	
haptuator.o: haptuator.cpp haptuator.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} haptuator.cpp
		
drivedataparser.o: drivedataparser.cpp drivedataparser.h
	${CC} -c ${CPPFLAGS} ${LDFLAGS} drivedataparser.cpp
	
daqacc.o: daqacc.cpp daqacc.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} daqacc.cpp

dynamicmodel.o: dynamicmodel.cpp dynamicmodel.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} dynamicmodel.cpp

inversedynamicmodel.o: inversedynamicmodel.cpp inversedynamicmodel.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} inversedynamicmodel.cpp
			
main.o: main.cpp drivedataparser.h daqacc.h daqdevice.h haptuator.h dynamicmodel.h inversedynamicmodel.h
	${CC} -c ${CPPFLAGS} main.cpp 

clean:
	rm -rf haptuator *.o *~

