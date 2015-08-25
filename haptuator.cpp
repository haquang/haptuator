//============================================================================
// Name        : haptuator.cpp
// Author      : Haquang
// Version     :
// Copyright   : 
// Description : Simple experiment for haptuator
//============================================================================

#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "daqdevice.h"
#include "daqacc.h"
#include "drivedataparser.h"


#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

#define VMAX  -51.0f
#define VMIN  -339.0f

using namespace std;

enum mode{RECORD,RUN};
mode MODE;
int i_record_count = 1;
/* Global variable*/
fstream f_data;
DaqDevice* m_daq;
DriveDataParser data_parser;
float speed = 0;
DaqAcc* m_daq_acc;

// Phantom device
const int DT_MAX=5000;
int DT=500;
int MAX_FORCE=5;
HDErrorInfo error;
HDCallbackCode hOmniCallback;
static HHD Device;
static HDSchedulerHandle hPhantomMain;
bool start = true;
hduVector3Dd init_pos;
hduVector3Dd posistion;
hduVector3Dd force;

vector<float> p_cur_pos {0,0,0};
vector<float> p_prv_pos {0,0,0};
vector<float> p_speed {0,0,0};
vector<float> p_prv_force {0,0,0};
float K = 0.25f;
float K_spring = 0.75f;
float epsilon = 0.00001f;
float pos_epsilon = 0.01f;
vector<float> Ai;
vector<float> Bi;
vector<float> fi;
float A0;


// Haptuator variable


vector<float> acc;

/*
 * Haptuator class
 */

class haptuator {
public:
	haptuator(DaqDevice* daq){
		t = 0.0f;
		m_daq = daq;
		timer  = new boost::asio::deadline_timer(t_read_service, boost::posix_time::microseconds(TIMER_HAPTUATOR));
	}
	void start(){
		t = 0.0f;
		m_thread = new boost::thread(boost::bind(static_cast<size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run), &t_read_service));
		timer->async_wait(boost::bind(&haptuator::run,this));
	}

	float waveform(float t) {

		int i;
		float result;
		result = A0/2;
		for (i = 0;i< Ai.size();i++){
			result += Ai[i] *cos(2*M_PI*fi[i]*t) + Bi[i]*sin(2*M_PI*fi[i]*t);
		}
		return result;
	}

	void run(){
		float acceleration;
		if (t <= 0.1f)
		{
			t += 0.00001f ;
			acceleration = waveform(t);
			//						cout << acceleration << endl;

			//	f_data << acceleration << endl;
			if (COMEDI_ERROR == m_daq->writeData(COMEDI_AN_OUT_SUB,COMEDI_MOTOR_OUT,COMEDI_RANGE,AREF_GROUND,acceleration))
				printf("Error writing to DAQ board");
		} else {
			//	count = 0;
			acceleration = 0.0f;
			boost::this_thread::sleep( boost::posix_time::milliseconds(100));
		}
		timer->async_wait(boost::bind(&haptuator::run,this));

		return;

	}
	void setInterpolationData(float _A0,vector<float> _Ai,vector<float> _Bi,vector<float> _fi){
		A0 = _A0;
		Ai = _Ai;
		Bi = _Bi;
		fi = _fi;
		data_state = true;
	}
	bool isDataLoaded(){return data_state;}


private:
	boost::thread* m_thread;
	boost::asio::io_service t_read_service;
	boost::asio::deadline_timer* timer;
	bool data_state = false;
	float t;
	float A0;
	vector<float> Ai;
	vector<float> Bi;
	vector<float> fi;
	DaqDevice* m_daq;

};
haptuator* hap;

/*
 * Saturation function:
 *
 * - Saturate the output force to device for safety reason
 */
void Saturation(double *force){
	if (force[0]<-MAX_FORCE) force[0]=-MAX_FORCE;
	if (force[1]<-MAX_FORCE) force[1]=-MAX_FORCE;
	if (force[2]<-MAX_FORCE) force[2]=-MAX_FORCE;
	if (force[0]>MAX_FORCE) force[0]=MAX_FORCE;
	if (force[1]>MAX_FORCE) force[1]=MAX_FORCE;
	if (force[2]>MAX_FORCE) force[2]=MAX_FORCE;
}
/*
 * Calibration
 */

int phantomCalibrate(){
	int calibrationStyle;
	int supportedCalibrationStyles;
	HDErrorInfo error;

	hdGetIntegerv(HD_CALIBRATION_STYLE, &supportedCalibrationStyles);
	if (supportedCalibrationStyles & HD_CALIBRATION_ENCODER_RESET)
	{
		calibrationStyle = HD_CALIBRATION_ENCODER_RESET;
		printf("HD_CALIBRATION_ENCODER_RESE..\n\n");
	}
	if (supportedCalibrationStyles & HD_CALIBRATION_INKWELL)
	{
		calibrationStyle = HD_CALIBRATION_INKWELL;
		printf("HD_CALIBRATION_INKWELL..\n\n");
	}
	if (supportedCalibrationStyles & HD_CALIBRATION_AUTO)
	{
		calibrationStyle = HD_CALIBRATION_AUTO;
		printf("HD_CALIBRATION_AUTO..\n\n");
	}

	do
	{
		hdUpdateCalibration(calibrationStyle);
		printf("Calibrating.. (put stylus in well)\n");
		if (HD_DEVICE_ERROR(error = hdGetError()))
		{
			hduPrintError(stderr, &error, "Reset encoders reset failed.");
			break;
		}
	}   while (hdCheckCalibration() != HD_CALIBRATION_OK);


	printf("\n\nCalibration complete.\n");
}
/*
 * Phantom Open
 *
 */
int  phantomOpen()
{
	HDErrorInfo error;
	Device = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		//hduPrintError(stderr, &error, "Failed to initialize haptic device");
		cout<< "Failed to initialize haptic device"<< endl; //: %s", &error);
		return -1;
	}
	printf("Found %s.\n\n", hdGetString(HD_DEVICE_MODEL_TYPE));
	hdEnable(HD_FORCE_OUTPUT);


}

/*
 * Phantom Stop
 */

void phantomClose()
{
	hdStopScheduler();
	hdUnschedule(hPhantomMain);
	hdDisableDevice(Device);
}
/*
 * Start phantom
 */

void phantomRun()
{
	hdStartScheduler();
	hdStartScheduler();
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		cout << "Failed to start the scheduler" << endl;//, &error);
		return;
	}
	phantomCalibrate();
}
HDCallbackCode HDCALLBACK phantom_callback(void *pUserData)
{
	float f_acc;
	p_prv_pos = p_cur_pos;
	for (int i = 0;i<3;i++){
		p_prv_force[i] = force[i];
	}

	hdBeginFrame(hdGetCurrentDevice());

	hdGetDoublev(HD_CURRENT_POSITION,posistion);

	if (start)
	{
		init_pos = posistion;
		start = false;
		hdEndFrame(Device);
		return HD_CALLBACK_CONTINUE;
	}

	for (int i = 0;i<3;i++)
	{
		p_cur_pos[i] = posistion[i] - init_pos[i];
		p_speed[i] = (p_cur_pos[i] - p_prv_pos[i])/0.001;
	}

	//cout << "Pos: " << p_cur_pos[0] << "   " << p_cur_pos[1] << "   " << p_cur_pos[2] << endl;

	if (p_cur_pos[1] > 0)
		force[1] =  0;
	else
		force[1] = -K_spring*p_cur_pos[1];

	force[2] = -K * p_cur_pos[2];

	force[0] = -K * p_cur_pos[0];;

	Saturation(force);
	//cout << "Force: " << force[0] << "   " << force[1] << "   " << force[2] << endl;

	if (RUN == MODE)
		hdSetDoublev(HD_CURRENT_FORCE, force); // Only output force in running mode

	hdEndFrame(hdGetCurrentDevice());

	if (RUN == MODE){ // RUNNING MODE
		if ((p_prv_force[1] <= epsilon) && (p_cur_pos[1] < p_prv_pos[1]) && (force[1] >= epsilon))
			{
				//	cout << "Control the haptuator " << endl;
				//	cout << "Speed: " << p_speed[1] << endl;

				speed = p_speed[1];
				if (speed >= VMAX)
					speed = VMAX;
				else if (speed <= VMIN)
					speed = VMIN;

				//		cout << "Speed: " << speed << endl;

				A0 = data_parser.speedInterpA0(speed);
				Ai = data_parser.speedInterpA(speed);
				Bi = data_parser.speedInterpB(speed);

				//		cout << "A0: " << endl;
				//		cout << A0 << endl;
				//
				//		cout << "Ai: " << endl;
				//		for (vector<float>::iterator it = Ai.begin() ; it != Ai.end(); ++it)
				//			cout << ' ' << *it;
				//		cout << '\n';
				//
				//		cout << "Bi: " << endl;
				//		for (vector<float>::iterator it = Bi.begin() ; it != Bi.end(); ++it)
				//			cout << ' ' << *it;
				//		cout << '\n';

				hap->setInterpolationData(A0,Ai,Bi,fi);
				//		cout << "Playing the haptuator" << endl;
				if (hap->isDataLoaded())
					hap->start();
			}
	} else { // RECORDING MODE
		f_acc = m_daq_acc->getAcc();
		f_data << p_speed[1] << "  " << f_acc << endl;
	}


	return HD_CALLBACK_CONTINUE;
}
/*
 * Key press function
 */
void keyPress(){
	cout<<"Press <t> to start playing the haptuator (TESTING)."<<endl;
	cout<<"Press <r> to start recording data."<<endl;
	cout<<"Press <l> to load the data."<<endl;
	cout<<"Press <s> to start phantom."<<endl;
	cout<<"Press <q> to exit the program."<<endl;
	string filename;
	std::stringstream ss;
	char c;
	char lastKey;
	while (true) {
		cin >> lastKey;
		switch ( lastKey ) {
		case 'l':

			cout << "Load default data ? Y/N " << endl;
			cin >> c;
			if (c == 'Y' || c == 'y'){
				filename = "driven_data.txt";
				cout << "Default driven data file " << filename << endl;
			}
			else
			{
				cout << "Filename: " << endl;
				cin >> filename;
			}

			cout << "Loading drive data" << endl;
			if(data_parser.loadData(filename))
			{
				cout << "Drive Data loaded! \n";
			}


			break;
		case 't':
			cout << "Put input speed: " << endl;
			cin >> speed;
			// TESTING
			//
			if (speed >= VMAX)
				speed = VMAX;
			else if (speed <= VMIN)
				speed = VMIN;

			A0 = data_parser.speedInterpA0(speed);
			Ai = data_parser.speedInterpA(speed);
			Bi = data_parser.speedInterpB(speed);

			cout << "A0: " << endl;
			cout << A0 << endl;

			cout << "Ai: " << endl;
			for (vector<float>::iterator it = Ai.begin() ; it != Ai.end(); ++it)
				cout << ' ' << *it;
			cout << '\n';

			cout << "Bi: " << endl;
			for (vector<float>::iterator it = Bi.begin() ; it != Bi.end(); ++it)
				cout << ' ' << *it;
			cout << '\n';

			hap->setInterpolationData(A0,Ai,Bi,fi);
			cout << "Playing the haptuator" << endl;
			if (hap->isDataLoaded())
				hap->start();
			//	acc = data_parser.getAccFromSpeed(speed);
			//	hap->setAccSignal(acc);
			//	cout << "Playing the haptuator" << endl;
			//	if (hap->isDataLoaded())
			//		hap->start();
			break;
		case 'r':
			cout << "Start recording data" << endl;
			MODE = RECORD; // Recording mode
			if (f_data.is_open())
				f_data.close();
			filename = "Test_Acc_N";
			ss.str("");
			ss << i_record_count++;
			filename.append(ss.str());
			filename.append(".txt");
			f_data.open(filename.c_str(),std::fstream::out);
			cout << "Writing data to " << filename << endl;
			phantomRun();
			break;
		case 's':
			cout << "Start running phantom device" << endl;
			MODE = RUN; // Running mode
			phantomRun();
			break;
		case 'q':
			cout << "Exiting..." << endl;
			boost::this_thread::interruption_point();
			exit(0);
			break;
		default:
			break;
		}
	}

}

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


int main(int argc, char *argv[]) {
	float bias_voltage;
	cout << "Starting....." << endl;

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
	m_daq = new DaqDevice(device);

	/* Calibration DAQ for haptuator */
	if (COMEDI_ERROR == m_daq->DAQcalibration(COMEDI_AN_OUT_SUB,COMEDI_MOTOR_OUT,COMEDI_RANGE)) // Analog output
	{
		printf("Error calibrating DAQ board - Analog output\n");
	}
	hap = new haptuator(m_daq);

	/*
	 * Calibrating Accelerator
	 */
	m_daq_acc = new DaqAcc(device,COMEDI_AN_IN_SUB,COMEDI_ADC_IN,COMEDI_RANGE);
	if ((COMEDI_ERROR == m_daq_acc->DAQcalibration(COMEDI_AN_IN_SUB,COMEDI_ADC_IN,COMEDI_RANGE)) || (COMEDI_ERROR == m_daq_acc->DAQcalibration(COMEDI_AN_IN_SUB,ACC_POWER_SUPPLY_PORT,COMEDI_RANGE)))
	{
		printf("Error calibrating DAQ board\n");
	}
	if (COMEDI_ERROR == m_daq_acc->readSupplyVoltage(ACC_POWER_SUPPLY_PORT)){
		printf("Error reading power supply voltage for Acc calibration\n");
	} else {
		printf("Input voltage: %f\n", m_daq_acc->getSupplyVoltage());
	}
	/*Timer for calibration*/
	boost::asio::io_service t_cali_service;
	boost::asio::deadline_timer timer_cali(t_cali_service, boost::posix_time::microseconds(1));
	timer_cali.async_wait(boost::bind(calibration,boost::asio::placeholders::error, &timer_cali));

	m_daq_acc->setCalibrationMode();
	t_cali_service.run();
	while (m_daq_acc->getCalibrationMode()){}

	bias_voltage = m_daq_acc->getBiasVol();
	printf("Bias voltage: %f\n", bias_voltage);
	/* Rendering frequency*/
	for (int i = 14;i<=29;i++){
		fi.push_back(i*5);
	}


	/*
	 * Phantom device
	 * */
	phantomOpen();
	hPhantomMain = hdScheduleAsynchronous(phantom_callback, 0, HD_MAX_SCHEDULER_PRIORITY);

	//	f_data.open("data.txt",std::fstream::out);

	/* Thread for key press*/
	boost::thread key_thread(&keyPress);
	key_thread.join();

	return 0;
}
