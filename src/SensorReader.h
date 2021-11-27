/*
 * SensorReader.h
 *
 *  Created on: Nov 26, 2021
 *      Author: traveller
 */

#ifndef SENSORREADER_H_
#define SENSORREADER_H_

#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include "protected_memory.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/neutrino.h>

#define SHM_SIZE sizeof(struct protected_memory)

class SensorReader {
	//TODO: change time reference to use global time, not this local time
	int time = 0;	//the current time
	int period;	//period of the timer
	int priority = 127;	//priority of the thread

	std::string sensor_name;	//the name of the sensor: used for filename and shared memory name
	int line_width;	//the width of a single line in the file
	std::fstream file;	//the file object to read from
	int shm_fd;		//file descriptor for shared memory
	struct protected_memory* shared_value;	//shared memory ptr

	timer_t timer;		//timer ID
	int chan = 0;			//channel ID (for trigger via PULSE)

private:
	void read_value();
	bool create_thread();
	static void sensor_reader_trampoline(sigval);	//trampoline function used by thread
public:
	SensorReader(std::string filename, int line_width, int priority = 127);
	virtual ~SensorReader();
	int set_timer_period(int);
};
#endif /* SENSORREADER_H_ */
