/*
 * Driver.cpp
 *
 *  Created on: Nov 29, 2021
 *      Author: travellers
 */

#include "Driver.h"

const char* Driver::sensors[5] = {"currentGear.txt",
		 "engineCoolantTemperature.txt",
		 "engineSpeed.txt",
		 "fuelConsumption.txt",
		 "vehicleSpeed.txt"};

Driver::Driver( int priority) {

	for(int i = 0; i < 5; i++) {
		shm_fd[i] = shm_open(sensors[i], O_RDWR, 0666);
			if (shm_fd[i] == -1) {
				perror("in shm_open()");
				exit(1);
			}
			shared_value[i] = (protected_memory*) mmap(0,sizeof(struct protected_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd[i], 0);
			if (shared_value[i] == MAP_FAILED) {
				perror("in mmap()");
				exit(1);
			}
	}

	this->priority=priority;
	this->period = period;

	pthread_attr_t* thread_attr = new pthread_attr_t;
	pthread_attr_init(thread_attr);

	//create the timer
	struct sigevent sigev;
	SIGEV_THREAD_INIT(&sigev, write_value_trampoline, this, thread_attr);
	int res = timer_create(CLOCK_REALTIME, &sigev, &timer);
	if(res == -1) {
			perror("Could not create timer");
			exit(1);
	}
}

Driver::~Driver() {
	//free the timer
	timer_delete(timer);

	for(int i = 0; i < 5; i++) {
		//ensure that the shared memory is free
		shared_value[i]->tex.unlock();
		//close the shared memory
		close(shm_fd[i]);
	}
}

void Driver::write_value_trampoline(sigval sigsrp) {
	//trampoline function called by the timer in new thread
	//allows the use of a class member function when it otherwise wouldn't be allowed
	((Driver*) sigsrp.sival_ptr)->write_value();
}

void Driver::write_value() {
	time += period;

	//acquire values
	float values[5];
	for(int i = 0; i < 5; i++) {
		shared_value[i]->tex.lock();
		values[i] = shared_value[i]->value;
		shared_value[i]->tex.unlock();
	}

	stdout_lock.lock();
	//put space between previous output and this one
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

	std::cout << "Time: " << time << std::endl;
	std::cout << "\t0: Current Gear: " 			<< values[0]
		 << "\n\t1: Engine Coolant Temperature: " 	<< values[1]
		 << "\n\t2: Engine Speed: " 				<< values[2]
		 << "\n\t3: Fuel Consumption: " 			<< values[3]
		 << "\n\t4: Vehicle Speed: "				<< values[4]
		 << "\nEnter sensor number, followed by the period to set" << std::endl;
	stdout_lock.unlock();
}

int Driver::set_timer_period(int period) {
	//sets the timer period
	//the first call to this function starts the timer
	this->period = period;

	itimerspec timer_value;
	timer_value.it_value.tv_sec = period;
	timer_value.it_value.tv_nsec = 0;
	timer_value.it_interval.tv_sec = period;
	timer_value.it_interval.tv_nsec = 0;
	return timer_settime(timer, 0, &timer_value, NULL);
}

void Driver::get_output_lock() {
	stdout_lock.lock();
}
void Driver::free_output_lock() {
	stdout_lock.unlock();
}
