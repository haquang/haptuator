/*
 *  This program was created to analyze the performance of haptuator
 *
 */

#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <numeric>
#include <queue>
#include <rtai_posix.h>
#include <rtai_lxrt.h>
#include <stdio.h>

#include "daqdevice.h"
#include "daqacc.h"
#include "haptuator.h"
#include "drivedataparser.h"

#define MAG 1

using namespace std;
/*
 * RTAI
 */

pthread_t haptuator_thread;
//static RT_TASK *rt_task;
RT_TASK *maintask;
static RTIME period_ns = TIMER_HAPTUATOR;
RTIME haptuator_period; /* requested timer period, in counts */
bool stop;
int exectime;
enum MODE {SINEWAVE,RECTWAVE,TRIANGLEWAVE,CUSTOM};
MODE mode;
/* Global variable*/
fstream f_data;
DaqAcc* m_daq_acc;
Haptuator* haptuator;
boost::asio::io_service t_cali_service;

enum CONTROL_MODE {GAIN,PSD};

int Control_Mode = GAIN; // default: gain
double gain;
double mag = MAG;
/*
 * Interpolation
 */
vector<float> Ai;
vector<float> Bi;
vector<float> fi;
vector<float> Psd;
float A0;
float vmin;
float vmax;
DriveDataParser data_parser;

vector<float> ref_freq;
vector<float> ref_gain;
/*
 * Swept frequency
 */
double freq = 50; // Hz
double f_start = 50;
double f_end = 500;
double t = 0.0f;
float t1 = 0.0f;
float* acc_xyz;
double acc;
double acc_set;
float T = 0.5; //second;
double deltaT = (double) TIMER_HAPTUATOR / 1000000000;

/*
 * Memory
 */
struct data{
	double acc_measure;
	double acc;
	double psd;

	data(double x,double acc_in,double psdin){
		acc_measure = x;
		acc  = acc_in;
		psd = psdin;
	}
};

vector<data>* v_mem;
vector<int>* v_time;
float* bias_voltage;

Haptuator* hap;

float customSignal(float t) // For recording
{
	int i;
	float result;
	result = A0/2;
	for (i = 0;i< Ai.size();i++){
		result += Ai[i] *cos(2*M_PI*fi[i]*t) + Bi[i]*sin(2*M_PI*fi[i]*t);
	}

	return result;

}

/*
 * RTAI task for haptuator
 */
void *haptuator_control(void *arg)
{
	int retval;
	RT_TASK *rt_task;
	RTIME exec_start, exec_end;
	/*  make this thread LXRT soft realtime */
	rt_task = rt_task_init_schmod(nam2num("Haptuator"), 1, 0, 0, SCHED_FIFO, 0xF);
	printf("THREAD INIT:name = %lu, address = %p.\n", rt_task, "Haptuator");
	mlockall(MCL_CURRENT | MCL_FUTURE);

	// makes task hard real time (default: soft)
	// uncomment the next line when developing : develop in soft real time mode
	rt_make_hard_real_time();

	haptuator_period = nano2count(period_ns);
	/* make task periodic, starting one cycle from now */
	retval = rt_task_make_periodic(rt_task,
			rt_get_time() + haptuator_period, haptuator_period);
	if (0 != retval) {
		if (-EINVAL == retval) {
			/* task structure is already in use */
			rt_printk("periodic task: task structure is invalid\n");
		} else {
			/* unknown error */
			rt_printk("periodic task: error starting task\n");
		}
		return 0;
	}
	exectime = 0.0f;
	exec_start = rt_get_cpu_time_ns();


	while(!stop) {
		t += deltaT;
		if (t <= T){
			acc_xyz = m_daq_acc->getAcc();
			//			freq = f_start*exp((log(f_end)-log(f_start))*t/T);
			//			v_time->push_back(t);
			acc = (acc_xyz[0]+acc_xyz[1]+acc_xyz[2])/3;
			//			acc = acc_xyz[2];

			if (CUSTOM == mode){
				haptuator->renderVibration(t);
				acc_set = customSignal(t);		// Just for recording
			} else {
				haptuator->renderVibration(t,freq,mag,mode);
				acc_set = haptuator->getAccRender();
			}
			if (GAIN == Control_Mode)
				haptuator->setCtrl(gain * haptuator->getAccRender());
			else if (PSD == Control_Mode){
				haptuator->setAcc(acc); 
				haptuator->updatePSD();
				haptuator->setCtrl(gain * haptuator->getAccRender()); // Still using additional gain
			}
			haptuator->run();
			vector<double> tmpower;
			tmpower = haptuator->getPowrSpetrum();
			if (!tmpower.empty())
			{
				v_mem->push_back(data(acc,acc_set,tmpower[2]));
			}
			

		} else {
			stop = true;
		}
		rt_task_wait_period();
		//exec_end = rt_get_cpu_time_ns();
		//v_time->push_back(exec_end - exec_start);
		//if (exectime <= abs(exec_end - exec_start)) {
		//	exectime = abs(exec_end - exec_start);
		//}
		//exec_start = exec_end;

	}
	rt_make_soft_real_time();
	rt_task_delete(rt_task);

	//
	//		for (vector<int>::iterator it = v_time->begin();it<v_time->end();++it){
	//			printf("Exec time: %d\n",*it);
	//		}
	//		printf("THREAD: time = %d\n", exectime);
	printf("THREAD %lu ENDS\n", rt_task);

	return 0;
}



void reset(){
	stop = false;
	// reset time
	t = 0.0f;
	// reset acceleration data
	v_mem->clear();
	v_time->clear();
	haptuator->reset();
	// reintialize thread

	if (!(haptuator_thread = rt_thread_create(haptuator_control, NULL, 10000))){
		printf("ERROR IN CREATING THREAD\n");
		exit(1);
	}
}
/*
 * Calibration function for accelerator
 */
/*
 * Calibration function for accelerator
 */
void calibration(const boost::system::error_code& err,boost::asio::deadline_timer* t)
{
	if (err)
		return;
	if (!m_daq_acc->getCalibrationMode()){
		printf("Finished calibration\n");
	} else {
		m_daq_acc->calibration();
		t->async_wait(boost::bind(calibration,boost::asio::placeholders::error,t));
	}
}

/*
 * Flush data to file
 */
void flushToFile(string filename){

	vector<double> power;
	power = haptuator->getPowrSpetrum();

	if (f_data.is_open())
		f_data.close();

	f_data.open(filename.c_str(),std::fstream::out);
	f_data << "data = [";
	for (vector<data>::iterator it = v_mem->begin();it<v_mem->end();++it){
		f_data << (*it).acc_measure << " " << (*it).acc << " " << (*it).psd <<"\n";
	}
	f_data << "];";
	f_data.close();
}

/*
 * Read file for interpolate
 */

void readGain(string filename){
	ifstream ifs(filename.c_str());
	string line;
	stringstream split;
	int s_speed; // number of speed samples
	int s_freq;  // number of freq
	float f_val;

	if (ifs) {
		// read the fourth line for ref_freq
		getline(ifs,line);
		split.clear();
		split << line;
		while (split >> f_val){
			ref_freq.push_back(f_val);
		}

		// read the fourth line for ref_freq
		getline(ifs,line);
		split.clear();
		split << line;
		while (split >> f_val){
			ref_gain.push_back(f_val);
		}
		return;
	} else {
		std::cout << "Error reading file!" << endl;
	}
	return;
}


void keyPress(){
	cout<<"Press <s> to start playing haptuator with sinewave."<<endl;
	cout<<"Press <c> to start playing haptuator with rectangle wave."<<endl;
	cout<<"Press <e> to start playing haptuator with triangle wave."<<endl;
	cout<<"Press <i> to increase frequency by 5 Hz."<<endl;
	cout<<"Press <t> to start haptuator with custom signal."<<endl;
	cout<<"Press <r> to save acceleration data."<<endl;
	cout<<"Press <l> to load the signal data." << endl;
	cout<<"Press <g> to load the signal data." << endl;
	cout<<"Press <q> to exit the program."<<endl;
	char filename[255];
	string file;
	std::stringstream ss;
	float speed;
	char c;
	char lastKey;
	while (true) {
		cin >> lastKey;
		switch ( lastKey ) {
		case 'l':
			cout << "Load default data ? Y/N " << endl;
			cin >> c;
			if (c == 'Y' || c == 'y'){
				file = "driven_data.txt";
				cout << "Default driven data file " << file << endl;
			}
			else
			{
				cout << "Filename: " << endl;
				cin >> file;
			}

			cout << "Loading drive data" << endl;
			if(data_parser.loadData(file))
			{
				fi = data_parser.getFreq();
				A0 =  data_parser.getA0();
				Ai = data_parser.getA();
				Bi = data_parser.getB();
				Psd = data_parser.getPsd();
				haptuator->setInterpolationParameter(A0,Ai,Bi,fi);
				haptuator->setPSD(Psd);
				haptuator->disablePSD();
				cout << "Drive Data loaded! \n";
			}
			break;
		case 'g':
			cout << "Load default data for gain interpolation? Y/N";
			cin >> c;
			if (c == 'Y' || c == 'y'){
				file = "gain.txt";
				readGain(file);
				haptuator->setGainInterpolation(ref_freq,ref_gain);
			} else {
				haptuator->disablePSD();
			}
			break;
		case 's':
			cout << "Start haptuator:" << endl;
			mode = SINEWAVE;
			reset();
			rt_thread_join(haptuator_thread);
			break;

		case 'c':
			cout << "Start haptuator:" << endl;
			mode = RECTWAVE;
			reset();
			rt_thread_join(haptuator_thread);
			break;

		case 'e':
			cout << "Start haptuator:" << endl;
			mode = TRIANGLEWAVE;
			reset();
			rt_thread_join(haptuator_thread);
			break;
		case 'r':
			sprintf(filename, "acc%03.0f.m", freq);
			flushToFile(filename);
			cout << "Done recording to " << filename << endl;
			break;
		case 'i':
			freq += 5;
			cout<< "Freq: " << freq << endl;
			break;
		case 't':

			cout << "Start haptuator:" << endl;
			mode = CUSTOM;
			reset();
			rt_thread_join(haptuator_thread);
			break;

		case 'q':
			cout << "Exiting..." << endl;
			stop_rt_timer();
			rt_task_delete(maintask);

			printf("MASTER %p ENDS\n", maintask);
			boost::this_thread::interruption_point();
			exit(0);
			break;
		default:
			break;
		}
	}

}

/*
 * MAIN PROGRAM
 */

int main(int argc, char *argv[]) {
	bias_voltage = new float(ACC_SIZE);
	cout << "Starting....." << endl;
	bool argParsed = false;
	double Km,Bm,am1,am2;
	double gamma;
	/*
	 * Parsing arguments
	 */
	int arg = 1;

	if (!strcmp(argv[arg], "-Mode"))
	{
		arg++;
		if (!strcmp(argv[arg], "-Psd")){

			Control_Mode = PSD;
			gain = (double)atof(argv[++arg]);
			cout << "Gain: " << gain<< endl;
		}
		else if (!strcmp(argv[arg], "-Gain")){
			Control_Mode = GAIN;
			gain = (double)atof(argv[++arg]);
			cout << "Gain: " << gain<< endl;
			while(arg < argc - 1)
			{
				arg++;
				if(!strcmp(argv[arg], "-magnitude")) {
					mag = (double)atof(argv[++arg]);
					cout << "Magnitude: " << mag<< endl;
				}
				else if (!strcmp(argv[arg], "-freq"))
				{
					freq = (double)atof(argv[++arg]);
					cout << "Frequency: " << freq<< endl;
				}
				else
				{
					cerr << "Unrecognized option: " << argv[arg] << endl;
					exit(1);
				}
			}

		} else {
			cerr << "Unrecognized option: " << argv[arg] << endl;
			exit(1);
		}
	}

	/*
	 * RTAI
	 */

	maintask = rt_task_init(nam2num("MAIN"), 1, 0, 0);
	//	mlockall(MCL_CURRENT | MCL_FUTURE);



	/* DAQ device*/
	comedi_t *device;
	/* Connect to DAQ board*/
	device = comedi_open(COMEDI_DEFAULT_DEVIVE);
	if(device == NULL){
		printf("Error connecting DAQ board\n");
		return -1;
	} else {
		printf("Connected to DAQ board\n");
	}

	/* Calibration DAQ for haptuator */

	haptuator = new Haptuator(device,COMEDI_AN_OUT_SUB,COMEDI_AN_OUT_1,COMEDI_RANGE);
	if (COMEDI_ERROR == haptuator->DAQcalibration(COMEDI_AN_OUT_SUB,COMEDI_AN_OUT_1,COMEDI_RANGE)) // Analog output
	{
		printf("Error calibrating DAQ board - Analog output\n");
	}

	//	haptuator_thread = new HaptuatorThread(haptuator);

	/*
	 * Calibrating Accelerator
	 */
	int* chanel_list = new int(ACC_SIZE);
	chanel_list[0] = COMEDI_ADC_IN_0;
	chanel_list[1] = COMEDI_ADC_IN_1;
	chanel_list[2] = COMEDI_ADC_IN_2;

	m_daq_acc = new DaqAcc(device,COMEDI_AN_IN_SUB,chanel_list,ACC_POWER_SUPPLY_PORT,COMEDI_RANGE);

	if (COMEDI_ERROR == m_daq_acc->DaqCalibration())
	{
		printf("Error calibrating DAQ board\n");
	}
	if (COMEDI_ERROR == m_daq_acc->readSupplyVoltage()){
		printf("Error reading power supply voltage for Acc calibration\n");
	} else {
		printf("Input voltage: %f\n", m_daq_acc->getSupplyVoltage());
	}

	cout << "Calibrating accelerometer" << endl;
	m_daq_acc->setCalibrationMode();

	/*Timer for calibration*/

	boost::asio::deadline_timer timer_cali(t_cali_service, boost::posix_time::microseconds(1000));
	timer_cali.async_wait(boost::bind(calibration,boost::asio::placeholders::error, &timer_cali));

	m_daq_acc->setCalibrationMode();
	t_cali_service.run();
	while (m_daq_acc->getCalibrationMode()){}

	bias_voltage = m_daq_acc->getBiasVol();
	printf("Bias voltage X: %f\n", bias_voltage[0]);
	printf("Bias voltage Y: %f\n", bias_voltage[1]);
	printf("Bias voltage Z: %f\n", bias_voltage[2]);

	v_mem = new vector<data>;
	v_time = new vector<int>;
	cout << "Sampling time " << deltaT << " sec " << endl;
	// set realtime timer to run in pure periodic mode
	rt_set_periodic_mode();
	// start realtime timer and scheduler
	start_rt_timer(0);

	if (!(haptuator_thread = rt_thread_create(haptuator_control, NULL, 10000))){
		printf("ERROR IN CREATING THREAD\n");
		exit(1);
	}

	//	/* Thread for key press*/
	boost::thread key_thread(&keyPress);
	key_thread.join();



	return 0;
}

