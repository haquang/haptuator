/*
 * daqacc.h
 *
 *  Created on: Aug 25, 2015
 *      Author: haquang
 */

#ifndef DAQACC_H_
#define DAQACC_H_
#include <comedilib.h>
#include "daqdevice.h"
#include "defines.h"

class DaqAcc :public DaqDevice{

private:
	int _subdev;
	int _chanel;
	int _range_idx = 1;
	int _aref = AREF_GROUND;
	float _supply_voltage;
	float _acc;
	float _bias_volt=0;
	comedi_range* _range_info;
	bool _calibrating = false;
	int _calibration_size = ACC_CAL_SIZE;
	int _calibration_count = 0;
	float _mean_volt = 0;
	float _sensivity;
public:
	DaqAcc();
	DaqAcc(comedi_t* dev,int subDev,int channel,int range = 0);
	float getSupplyVoltage();
	int readSupplyVoltage(int supply_channel); // Read supply voltage from channel for calibration
	int calibration();
	void setCalibrationMode();
	bool getCalibrationMode();
	float getBiasVol();
    float getAcc();
	virtual ~DaqAcc();
};

#endif /* DAQACC_H_ */
