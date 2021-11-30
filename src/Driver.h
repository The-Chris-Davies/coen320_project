/*
 * Driver.h
 *
 *  Created on: Nov 29, 2021
 *      Author: travellers
 */

#ifndef DRIVER_H_
#define DRIVER_H_

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

class Driver {
	//TODO: change time reference to use global time, not this local time
	int time = 0;	//the current time
	int period;	//period of the timer
	int priority = 127;	//priority of the thread

	static const char *sensors[5];

	int shm_fd[5];		//file descriptor for shared memory
	struct protected_memory* shared_value[5];	//shared memory ptr

	timer_t timer;		//timer ID
	int chan = 0;			//channel ID (for trigger via PULSE)

private:
	void write_value();  // diplay the 5 sensor values onto the console

	bool create_thread();
	static void write_value_trampoline(sigval);
public:
	Driver(int priority = 127);
	virtual ~Driver();
	int set_timer_period(int);
};
#endif /* DRIVER_H_ */
