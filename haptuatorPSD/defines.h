#define COMEDI_DEFAULT_DEVIVE "/dev/comedi0"   // Device ID for sensoray
#define COMEDI_ERROR -1
#define COMEDI_OK 0
#define COMEDI_RANGE 0
#define COMEDI_IN_CHAN_NUM 16
#define COMEDI_OUT_CHAN_NUM 4
#define COMEDI_COUNT_CHAN_NUM 3
// Sensoray Sub-channel
#define COMEDI_AN_IN_SUB 0
#define COMEDI_AN_OUT_SUB 1
#define COMEDI_DI_SUB1 2
#define COMEDI_DI_SUB2 3
#define COMEDI_DI_SUB3 4
#define COMEDI_COUNT_SUB 5

// Channel
#define COMEDI_AN_OUT_0 0
#define COMEDI_AN_OUT_1 1
#define COMEDI_ENC 1
#define COMEDI_ADC_IN_0 0
#define COMEDI_ADC_IN_1 2
#define COMEDI_ADC_IN_2 3

#define ACC_POWER_SUPPLY_PORT 1
#define ACC_CAL_SIZE 100

#define TIMER_SCHEDULE 1000
#define TIME_SLEEP 5
#define TIMER_RECORD 1000
#define ACC_SIZE 3

// FOR FOURIER TRANSFORM
#define BLOCK_SIZE 400   // This value must be the same with block size of FFT in matlab analysis

// SPEED 

#define MAX_SPEED 100  // RPS 
#define MAX_SPEED_VOL 4 // 4 V --> Input range (-4,4)

#define TIMER_HAPTUATOR 100000 // ns
#define TIMER_CALI 1000000 // ns
#define TIMER_UPDATE_FREQ 5000000
#define GRAVITY 9.81

#define SAMPLING 10000

// REFERENCE MODEL

#define KM 200
#define BM 0
#define AM_1 100
#define AM_2 100
#define GAMMA 0.1

