/*
 * daqacc.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: haquang
 */

#include "daqacc.h"
#include <iostream>

DaqAcc::DaqAcc() {
	// TODO Auto-generated constructor stub

}

DaqAcc::DaqAcc(comedi_t* dev, int subdev, int channel, int range):DaqDevice(dev) {
	_subdev = subdev;
	_chanel = channel;
	_range_idx = range;
}

float DaqAcc::getSupplyVoltage() {
	// return supply voltage
	return _supply_voltage;
}

int DaqAcc::readSupplyVoltage(int supply_channel) {

	if (COMEDI_ERROR == readData(_subdev,supply_channel,_range_idx,_aref)){
		return COMEDI_ERROR;
	} else {
		_supply_voltage = getVolAnalogInput(supply_channel);
		_sensivity = _supply_voltage / 10;
		return COMEDI_OK;
	}
}

int DaqAcc::calibration() {
	if (_calibration_count < _calibration_size)
	{
		if (COMEDI_ERROR == readData(_subdev,_chanel,_range_idx,_aref)){
			return COMEDI_ERROR;
		} else {
			_mean_volt += getVolAnalogInput(_chanel);
			_calibration_count++;
		}
	}
	else
	{
		_mean_volt /= (float)_calibration_size;
		_bias_volt = _mean_volt;
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

float DaqAcc::getBiasVol() {
	return _bias_volt;
}

float DaqAcc::getAcc() {
	float vol;
	readData(_subdev,_chanel,_range_idx,_aref);
	vol = getVolAnalogInput(_chanel);
	_acc = (vol -_bias_volt)/_sensivity;
	return _acc;
}

DaqAcc::~DaqAcc() {
	// TODO Auto-generated destructor stub
}

