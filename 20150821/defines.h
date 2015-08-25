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
#define COMEDI_MOTOR_OUT 0
#define COMEDI_ENC 1
#define COMEDI_ADC_IN 0


#define TIMER_SCHEDULE 1000


// SPEED 

#define MAX_SPEED 100  // RPS 
#define MAX_SPEED_VOL 4 // 4 V --> Input range (-4,4)

#define TIMER_HAPTUATOR 10
#define TIMER_UPDATE_FREQ 5000000
