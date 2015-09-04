CC = g++
CPPFLAGS = -Wall -g -std=gnu++11 -w
LDFLAGS = -lm -lcomedi -lboost_system -lboost_thread  -lHD -lHDU -lncurses  -lgsl -lgslcblas -lm

all: haptuator.o drivedataparser.o daqdevice.o cspline.o
	${CC}   haptuator.o drivedataparser.o daqdevice.o cspline.o ${LDFLAGS} -o haptuator

drivedatadarser.o: drivedatadarser.cpp drivedataparser.h cspline.h
	${CC} -c ${CPPFLAGS} ${LDFLAGS} drivedataparser.cpp

daqdevice.o: daqdevice.cpp daqdevice.h 
	${CC} -c ${CPPFLAGS} ${LDFLAGS} daqdevice.cpp
	
cspline.o: cspline.cpp cspline.h
	${CC} -c ${CPPFLAGS} ${LDFLAGS} cspline.cpp

haptuator.o: haptuator.cpp drivedataparser.h daqdevice.h
	${CC} -c ${CPPFLAGS} haptuator.cpp 

clean:
	rm -rf haptuator *.o *~

