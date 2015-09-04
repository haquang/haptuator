CC = g++
CPPFLAGS = -Wall -g -std=gnu++11 -w
LDFLAGS = -lm -lcomedi -lboost_system -lboost_thread  -lHD -lHDU -lncurses  -lgsl -lgslcblas -lm

all: main.o drivedataparser.o daqdevice.o daqacc.o haptuator.o phantom.o
	${CC}   main.o drivedataparser.o daqdevice.o daqacc.o haptuator.o phantom.o ${LDFLAGS} -o haptuator

drivedataparser.o: drivedataparser.cpp drivedataparser.h
	${CC} -c ${CPPFLAGS} ${LDFLAGS} drivedataparser.cpp

daqdevice.o: daqdevice.cpp daqdevice.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} daqdevice.cpp

haptuator.o: haptuator.cpp haptuator.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} haptuator.cpp
		
daqacc.o: daqacc.cpp daqacc.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} daqacc.cpp
	
phantom.o: phantom.cpp phantom.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} phantom.cpp
	
main.o: main.cpp drivedataparser.h daqdevice.h haptuator.h daqacc.h 
	${CC} -c ${CPPFLAGS} main.cpp 

clean:
	rm -rf haptuator *.o *~

