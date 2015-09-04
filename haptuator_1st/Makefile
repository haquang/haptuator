CC = g++
CPPFLAGS = -Wall -g -std=gnu++11 -w
LDFLAGS = -lm -lcomedi -lboost_system -lboost_thread  -lHD -lHDU -lncurses  -lgsl -lgslcblas -lm

all: haptuator.o drivedataparser.o daqdevice.o daqacc.o
	${CC}   haptuator.o drivedataparser.o daqdevice.o daqacc.o ${LDFLAGS} -o haptuator

drivedataparser.o: drivedataparser.cpp drivedataparser.h
	${CC} -c ${CPPFLAGS} ${LDFLAGS} drivedataparser.cpp

daqdevice.o: daqdevice.cpp daqdevice.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} daqdevice.cpp
	
daqacc.o: daqacc.cpp daqacc.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} daqacc.cpp
haptuator.o: haptuator.cpp drivedataparser.h daqdevice.h
	${CC} -c ${CPPFLAGS} haptuator.cpp 

clean:
	rm -rf haptuator *.o *~

