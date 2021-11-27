/*
 * SensorReader.cpp
 *
 *  Created on: Nov 26, 2021
 *      Author: traveller
 */

#include "SensorReader.h"

SensorReader::SensorReader(std::string filename, int line_width, int priority) {
	this->sensor_name = filename;
	this->line_width = line_width;
	this->period = period;
	this->priority=priority;

	// initialize the shared memory
	shm_fd = shm_open(this->sensor_name.c_str(), O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1){
		perror("Shared memory could not be created, or already exists");
		exit(1);
	}
	//truncate shared memory size
	ftruncate(shm_fd, SHM_SIZE);

	//map the shared memory to the shared_value ptr
	shared_value = (protected_memory*) mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shared_value == MAP_FAILED) {
		perror("Map failed\n");
		exit(1);
	}

	// open the file
	file.open(this->sensor_name);
	if (!file.is_open()) {
		perror("Could not open file");
		exit(1);
	}

	pthread_attr_t* thread_attr = new pthread_attr_t;
	pthread_attr_init(thread_attr);

	//create the timer
	struct sigevent sigev;
	SIGEV_THREAD_INIT(&sigev, sensor_reader_trampoline, this, thread_attr);
	int res = timer_create(CLOCK_REALTIME, &sigev, &timer);
	if(res == -1) {
			perror("Could not create timer");
			exit(1);
	}
}

SensorReader::~SensorReader() {
	//free the timer
	timer_delete(timer);

	//ensure that the shared memory is free
	shared_value->tex.unlock();

	// close the file
	file.close();

	//close the shared memory
	close(shm_fd);
}
void SensorReader::sensor_reader_trampoline(sigval sigsrp) {
	//trampoline function called by the timer in new thread
	//allows the use of a class member function when it otherwise wouldn't be allowed
	((SensorReader*) sigsrp.sival_ptr)->read_value();
}

void SensorReader::read_value() {
	//reads the current sensor value and stores it in shared memory
	//this function is called in the thread

	time += period;

	//read current value
	float val;
	file.seekg(time * line_width);	//put file cursor at relevant value
	file >> val;

	std::cout << "Read value: " << val << std::endl;

	//put the value in shared memory
	shared_value->tex.lock();
	shared_value->value = val;
	shared_value->tex.unlock();
}

int SensorReader::set_timer_period(int period) {
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
