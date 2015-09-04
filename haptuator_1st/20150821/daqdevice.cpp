#include "daqdevice.h"

DaqDevice::DaqDevice()
{

}
DaqDevice::DaqDevice(comedi_t* dev): _dev(dev){
	_an_input = new lsampl_t[COMEDI_IN_CHAN_NUM];
	_converter_an_input  = new comedi_polynomial_t[COMEDI_IN_CHAN_NUM];
	_converter_an_output = new comedi_polynomial_t[COMEDI_OUT_CHAN_NUM];
	_counter =  new lsampl_t[COMEDI_COUNT_CHAN_NUM];

}

int DaqDevice::readData(int subdev,int channel,int range_idx,int aref){
    if (comedi_data_read(_dev, subdev, channel, range_idx, aref, &_an_input[channel]) < 0){
        return COMEDI_ERROR;
    } else {
        return COMEDI_OK;
    }
}

int DaqDevice::DAQcalibration(int subdev,int channel,int range_idx)
{
    int retval;
    int flags;
    comedi_calibration_t* parsed_calibration;
    comedi_polynomial_t* converter;
    comedi_conversion_direction options;

    if (COMEDI_AN_IN_SUB == subdev)
    {
	converter = &_converter_an_input[channel];
	options = COMEDI_TO_PHYSICAL;
    }
	
    else if (COMEDI_AN_OUT_SUB == subdev)
    {
	converter = &_converter_an_output[channel];
	options = COMEDI_FROM_PHYSICAL;
    }	
    else 
	return COMEDI_ERROR;

    flags = comedi_get_subdevice_flags(_dev, subdev);
    if(flags < 0)
    {
        return COMEDI_ERROR;
    }
    if(flags & SDF_SOFT_CALIBRATED) /* board uses software calibration */
    {
        char *calibration_file_path = comedi_get_default_calibration_path(_dev);

        /* parse a calibration file which was produced by the
                comedi_soft_calibrate program */
        parsed_calibration = comedi_parse_calibration_file(calibration_file_path);
        if(parsed_calibration == NULL)
        {
            return COMEDI_ERROR;
        }

        /* get the comedi_polynomial_t for the subdevice/channel/range
                we are interested in */

        retval = comedi_get_softcal_converter(subdev, channel, range_idx,
                                              options, parsed_calibration, converter);

        comedi_cleanup_calibration(parsed_calibration);
       

        if(retval < 0)
        {
            return COMEDI_ERROR;
        }
    }else /* board uses hardware calibration */
    {
        retval = comedi_get_hardcal_converter(_dev, subdev, channel, range_idx,
                                              options, converter);
        if(retval < 0)
        {
            return COMEDI_ERROR;
        }
    }

    return COMEDI_OK;
}

lsampl_t DaqDevice::getRawAnalogInput(int channel)
{
    return(_an_input[channel]);
}

float DaqDevice::getVolAnalogInput(int channel)
{
    return  comedi_to_physical(_an_input[channel], &_converter_an_input[channel]);
}

int DaqDevice::writeData(int subdev,int channel, int range_idx,int aref,double value)
{
    lsampl_t raw;

    raw = comedi_from_physical(value,&_converter_an_output[channel]);

    if (comedi_data_write(_dev, subdev, channel, range_idx, aref,raw) < 0)
    {
        return COMEDI_ERROR;
    } else {
        return COMEDI_OK;
    }
}

int DaqDevice::setupCounter(int subdev,int channel,int initial_count)
{
	int retval;
	lsampl_t counter_mode;

	retval = comedi_reset(_dev, subdev);
	if(retval < 0) return COMEDI_ERROR;

	retval = comedi_set_gate_source(_dev, subdev, 0, 0, NI_GPCT_GATE_PIN_GATE_SELECT(0) /* NI_GPCT_GATE_PIN_i_GATE_SELECT *//*| CR_EDGE*/);
	if(retval < 0) return COMEDI_ERROR;
	retval = comedi_set_gate_source(_dev, subdev, 0, 1, NI_GPCT_DISABLED_GATE_SELECT | CR_EDGE);
	if(retval < 0)
	{
		fprintf(stderr, "Failed to set second gate source.  This is expected for older boards (e-series, etc.)\n"
			"that don't have a second gate.\n");
	}

	counter_mode = NI_GPCT_COUNTING_MODE_NORMAL_BITS;
	// output pulse on terminal count (doesn't really matter for this application)
	counter_mode |= NI_GPCT_OUTPUT_TC_PULSE_BITS;
	/* Don't alternate the reload source between the load a and load b registers.
		Doesn't really matter here, since we aren't going to be reloading the counter.
	*/
	counter_mode |= NI_GPCT_RELOAD_SOURCE_FIXED_BITS;
	// count up
	counter_mode |= NI_GPCT_COUNTING_DIRECTION_UP_BITS;
	// don't stop on terminal count
	counter_mode |= NI_GPCT_STOP_ON_GATE_BITS;
	// don't disarm on terminal count or gate signal
	counter_mode |= NI_GPCT_NO_HARDWARE_DISARM_BITS;
	retval = comedi_set_counter_mode(_dev, subdev, channel, counter_mode);
	if(retval < 0) return COMEDI_ERROR;

	/* set initial counter value by writing to channel 0.  The "load a" and "load b" registers can be
	set by writing to channels 1 and 2 respectively. */
	retval = comedi_data_write(_dev, subdev, channel, 0, 0, initial_count);

	retval = comedi_arm(_dev, subdev, NI_GPCT_ARM_IMMEDIATE);
	if(retval < 0) return COMEDI_ERROR;

	return COMEDI_OK;
}

int DaqDevice::readCounter(int subdev,int channel)
{
   if (comedi_data_read(_dev, subdev, channel, 0, 0, &_counter[channel]) < 0){
        return COMEDI_ERROR;
    } else {
        return COMEDI_OK;
    }
}

lsampl_t DaqDevice::getCounter(int channel)
{
    return(_counter[channel]);
}
