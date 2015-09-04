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
#include "daqdevice.h"
#include "daqacc.h"
#include "haptuator.h"
#include "drivedataparser.h"

using namespace std;
enum MODE {SINEWAVE,CUSTOM};
MODE mode;
/* Global variable*/
fstream f_data;
DaqAcc* m_daq_acc_x;
DaqAcc* m_daq_acc_y;
DaqAcc* m_daq_acc_z;
Haptuator* haptuator;
DriveDataParser data_parser;


vector<float> Ai;
vector<float> Bi;
vector<float> fi;
float A0;
float vmin;
float vmax;

/*
 *  This class manage the thread for recording
 */
class RecordThread{
public:
	RecordThread(DaqAcc* daq_x,DaqAcc* daq_y,DaqAcc* daq_z){
		_daq_x = daq_x;
		_daq_y = daq_y;
		_daq_z = daq_z;
		_timer  = new boost::asio::deadline_timer(t_read_service, boost::posix_time::microseconds(TIMER_RECORD));
	}
	void start(){
		m_thread = new boost::thread(boost::bind(static_cast<size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run), &t_read_service));
		_timer->async_wait(boost::bind(&RecordThread::run,this));
	}

	void run(){
		//cout << "Record thread" << endl;
		f_data << m_daq_acc_x->getAcc() << " "<< m_daq_acc_y->getAcc() << " "<< m_daq_acc_z->getAcc() << " " << haptuator->getAccRender() << endl;
		boost::this_thread::sleep( boost::posix_time::microseconds(TIME_SLEEP));
		_timer->async_wait(boost::bind(&RecordThread::run,this));
		return;
	}

private:
	boost::thread* m_thread;
	boost::asio::io_service t_read_service;
	boost::asio::deadline_timer* _timer;
	DaqAcc* _daq_x;
	DaqAcc* _daq_y;
	DaqAcc* _daq_z;
};
RecordThread* record_thread;

/*
 * This class manage the haptuator control class
 */

#define FREQ 100  // Testing frequency
#define MAG 1.0f
class HaptuatorThread {
public:
	HaptuatorThread(Haptuator* daq){
		hap = daq;
		t = 0.0f;
		_timer  = new boost::asio::deadline_timer(t_haptuator_service, boost::posix_time::microseconds(TIMER_HAPTUATOR));
	}
	void start(){
		t = 0.0f;
		m_thread = new boost::thread(boost::bind(static_cast<size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run), &t_haptuator_service));
		_timer->async_wait(boost::bind(&HaptuatorThread::run,this));
	}

	void run(){
		//cout << "Haptuator thread" << endl;
		if (SINEWAVE == mode){
			t += (float) TIMER_HAPTUATOR / 1000000;

			if (COMEDI_ERROR == hap->renderVibration(t,FREQ,MAG))
				cout << "Error writing to DAQ board" << endl;


		} else if (CUSTOM == mode) {
			t += (float) TIMER_HAPTUATOR / 1000000;
			if (t <= 0.1f){
				if (COMEDI_ERROR == hap->renderVibration(t))
					cout << "Error writing to DAQ board" << endl;

			} else {
				boost::this_thread::sleep( boost::posix_time::milliseconds(100));
			}
		}
		_timer->async_wait(boost::bind(&HaptuatorThread::run,this));


		return;

	}

private:
	boost::thread* m_thread;
	boost::asio::io_service t_haptuator_service;
	boost::asio::deadline_timer* _timer;
	float t;;
	Haptuator* hap;

};
HaptuatorThread* haptuator_thread;

/*
 * Calibration function for accelerator
 */
void calibration(const boost::system::error_code& err,boost::asio::deadline_timer* t)
{
	if (err)
		return;
	if (!m_daq_acc_x->getCalibrationMode() && !m_daq_acc_y->getCalibrationMode() && !m_daq_acc_z->getCalibrationMode()){
		printf("Finished calibration\n");
	} else {
		m_daq_acc_x->calibration();
		m_daq_acc_y->calibration();
		m_daq_acc_z->calibration();

		t->async_wait(boost::bind(calibration,boost::asio::placeholders::error,t));
	}
}

void keyPress(){
	cout<<"Press <r> to start recording acceleration data"<<endl;
	cout<<"Press <s> to start playing haptuator with sinewave."<<endl;
	cout<<"Press <c> to start haptuator with custom signal."<<endl;
	cout<<"Press <l> to load the data." << endl;
	cout<<"Press <q> to exit the program."<<endl;
	string filename;
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
				fi = data_parser.getFreq();
				vmin = data_parser.getMinVel();
				vmax = data_parser.getMaxVel();
				cout << "Drive Data loaded! \n";
			}

			break;

		case 'r':
			cout << "Start recording acceleration" << endl;
			if (f_data.is_open())
				f_data.close();

			f_data.open("acc.m",std::fstream::out);
			f_data << "data = [";
			record_thread->start();
			break;
		case 's':
			cout << "Start playing haptuator" << endl;
			mode = SINEWAVE;
			haptuator_thread->start();
			break;
		case 'c':
			cout << "Start playing haptuator" << endl;
			mode = CUSTOM;
			cout << "Put input speed: " << endl;
			cin >> speed;

			if (speed >= vmax)
				speed = vmax;
			else if (speed <= vmin)
				speed = vmin;

			A0 = data_parser.speedInterpA0(speed);
			Ai = data_parser.speedInterpA(speed);
			Bi = data_parser.speedInterpB(speed);
			//fi = data_parser.getFreq();
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

			haptuator->setInterpolationParameter(A0,Ai,Bi,fi);
			haptuator_thread->start();
			break;
		case 'q':
			cout << "Exiting..." << endl;
			boost::this_thread::interruption_point();
			f_data << "];";
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

	/* Calibration DAQ for haptuator */

	haptuator = new Haptuator(device,COMEDI_AN_OUT_SUB,COMEDI_AN_OUT_0,COMEDI_RANGE);
	if (COMEDI_ERROR == haptuator->DAQcalibration(COMEDI_AN_OUT_SUB,COMEDI_AN_OUT_0,COMEDI_RANGE)) // Analog output
	{
		printf("Error calibrating DAQ board - Analog output\n");
	}
	haptuator_thread = new HaptuatorThread(haptuator);

	/*
	 * Calibrating Accelerator
	 */
	m_daq_acc_x = new DaqAcc(device,COMEDI_AN_IN_SUB,COMEDI_ADC_IN_0,COMEDI_RANGE);
	m_daq_acc_y = new DaqAcc(device,COMEDI_AN_IN_SUB,COMEDI_ADC_IN_1,COMEDI_RANGE);
	m_daq_acc_z = new DaqAcc(device,COMEDI_AN_IN_SUB,COMEDI_ADC_IN_2,COMEDI_RANGE);
	if ((COMEDI_ERROR == m_daq_acc_x->DAQcalibration(COMEDI_AN_IN_SUB,COMEDI_ADC_IN_0,COMEDI_RANGE)) || (COMEDI_ERROR == m_daq_acc_x->DAQcalibration(COMEDI_AN_IN_SUB,ACC_POWER_SUPPLY_PORT,COMEDI_RANGE))
			|| (COMEDI_ERROR == m_daq_acc_y->DAQcalibration(COMEDI_AN_IN_SUB,COMEDI_ADC_IN_1,COMEDI_RANGE)) || (COMEDI_ERROR == m_daq_acc_y->DAQcalibration(COMEDI_AN_IN_SUB,ACC_POWER_SUPPLY_PORT,COMEDI_RANGE))
			|| (COMEDI_ERROR == m_daq_acc_z->DAQcalibration(COMEDI_AN_IN_SUB,COMEDI_ADC_IN_2,COMEDI_RANGE)) || (COMEDI_ERROR == m_daq_acc_z->DAQcalibration(COMEDI_AN_IN_SUB,ACC_POWER_SUPPLY_PORT,COMEDI_RANGE)))
	{
		printf("Error calibrating DAQ board\n");
	}
	if (COMEDI_ERROR == m_daq_acc_x->readSupplyVoltage(ACC_POWER_SUPPLY_PORT) || COMEDI_ERROR == m_daq_acc_y->readSupplyVoltage(ACC_POWER_SUPPLY_PORT) || COMEDI_ERROR == m_daq_acc_z->readSupplyVoltage(ACC_POWER_SUPPLY_PORT)){
		printf("Error reading power supply voltage for Acc calibration\n");
	} else {
		printf("Input voltage: %f - %f - %f\n", m_daq_acc_x->getSupplyVoltage(),m_daq_acc_y->getSupplyVoltage(),m_daq_acc_z->getSupplyVoltage());
	}
	/*Timer for calibration*/
	boost::asio::io_service t_cali_service;
	boost::asio::deadline_timer timer_cali(t_cali_service, boost::posix_time::microseconds(1000));
	timer_cali.async_wait(boost::bind(calibration,boost::asio::placeholders::error, &timer_cali));

	m_daq_acc_x->setCalibrationMode();
	m_daq_acc_y->setCalibrationMode();
	m_daq_acc_z->setCalibrationMode();
	t_cali_service.run();
	while (m_daq_acc_x->getCalibrationMode() || m_daq_acc_y->getCalibrationMode() || m_daq_acc_z->getCalibrationMode()){}

	bias_voltage = m_daq_acc_x->getBiasVol();
	printf("Bias voltage X: %f\n", bias_voltage);
	bias_voltage = m_daq_acc_y->getBiasVol();
	printf("Bias voltage Y: %f\n", bias_voltage);
	bias_voltage = m_daq_acc_z->getBiasVol();
	printf("Bias voltage Z: %f\n", bias_voltage);

	record_thread = new RecordThread(m_daq_acc_x,m_daq_acc_y,m_daq_acc_z);

	/* Thread for key press*/
	boost::thread key_thread(&keyPress);
	key_thread.join();

	return 0;
}

