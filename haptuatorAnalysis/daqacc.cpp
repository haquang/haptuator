/*
 * daqacc.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: haquang
 */

#include "daqacc.h"
#include "daqdevice.h"
#include <iostream>


DaqAcc::DaqAcc() {
	// TODO Auto-generated constructor stub

}

DaqAcc::DaqAcc(comedi_t* dev, int subdev, int* channel,int supply_channel, int range):DaqDevice(dev) {
	_subdev = subdev;
	_chanel = channel;

	_range_idx = range;
	_range_info = NULL;

	_mean_volt = new float(ACC_SIZE);
	_bias_volt = new float(ACC_SIZE);
	_acc = new float(ACC_SIZE);
	_supply_channel = supply_channel;
}

float DaqAcc::getSupplyVoltage() {
	// return supply voltage
	return _supply_voltage;
}

int DaqAcc::readSupplyVoltage() {

	if (COMEDI_ERROR == readData(_subdev,_supply_channel,_range_idx,_aref)){
		return COMEDI_ERROR;
	} else {
		_supply_voltage = getVolAnalogInput(_supply_channel);
		_sensivity = _supply_voltage / 10;
		return COMEDI_OK;
	}
}

int DaqAcc::calibration() {
	int i;
	if (_calibration_count < _calibration_size)
	{
		for (i = 0;i<ACC_SIZE;i++){
			if (COMEDI_ERROR == readData(_subdev,_chanel[i],_range_idx,_aref)){
				return COMEDI_ERROR;
			} else {
				_mean_volt[i] += getVolAnalogInput(_chanel[i]);

			}
		}
		_calibration_count++;

	}
	else
	{
		for (i = 0;i< ACC_SIZE;i++){
			_mean_volt[i] /= (float)_calibration_size;
			_bias_volt[i] = _mean_volt[i];
		}

		_calibration_count = 0;
		_calibrating = false;
	}

	return COMEDI_OK;
}

void DaqAcc::setCalibrationMode() {
	_calibrating = true;
}

bool DaqAcc::getCalibrationMode() {
	return _calibrating;
}

float* DaqAcc::getBiasVol() {
	return _bias_volt;
}

float* DaqAcc::getAcc() {
	float vol;
	int i;
	for (i=0;i<ACC_SIZE;i++){
		readData(_subdev,_chanel[i],_range_idx,_aref);
		vol = getVolAnalogInput(_chanel[i]);
		_acc[i] = (vol -_bias_volt[i])/_sensivity * GRAVITY;
	}
	return _acc;
}

DaqAcc::~DaqAcc() {
	// TODO Auto-generated destructor stub
}

int DaqAcc::DaqCalibration() {
	if ((COMEDI_ERROR == DAQcalibration(COMEDI_AN_IN_SUB,_chanel[0],COMEDI_RANGE)) ||
				(COMEDI_ERROR == DAQcalibration(COMEDI_AN_IN_SUB,_chanel[1],COMEDI_RANGE)) ||
				(COMEDI_ERROR == DAQcalibration(COMEDI_AN_IN_SUB,_chanel[2],COMEDI_RANGE)) ||
				(COMEDI_ERROR == DAQcalibration(COMEDI_AN_IN_SUB,_supply_channel,COMEDI_RANGE)))
		return COMEDI_ERROR;
	else
		return COMEDI_OK;
}

void DaqAcc::reset() {
	for (int i = 0;i<3;i++){
		_mean_volt[i] = 0;
		_bias_volt[i] = 0;
	}
	_calibration_count = 0;


}
