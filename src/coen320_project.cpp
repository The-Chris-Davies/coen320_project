#include <iostream>
#include "SensorReader.h"

using namespace std;

//sensor reader handlers
SensorReader* readers[5];

int main() {

	// step 1: initialize sensor readers

	//the names of each data file
	string names [5] = {string("currentGear.txt"),
						string("engineCoolantTemperature.txt"),
						string("engineSpeed.txt"),
						string("fuelConsumption.txt"),
						string("vehicleSpeed.txt")
					   };
	//the length of each entry in the file (including newlines)
	int sizes[5] = { 3, 3, 4, 7, 3};
	//create readers
	for(int i = 0; i < 5; i++) {
		readers[i] = new SensorReader(names[i], sizes[i], 4);
	}

	//step 2: start timers
	for(auto& reader: readers) {
			reader->set_timer_period(5);
		}

	//create a test reader to demonstrate shared memory access
	string name("test.txt");
	SensorReader test_reader(name, 4, 2);
	test_reader.set_timer_period(1);

	//test shared memory
	//open
	int shm_fd = shm_open(name.c_str(), O_RDWR, 0666);
	if (shm_fd == -1) {
		perror("in shm_open()");
		exit(1);
	}
	struct protected_memory* ptr = (protected_memory*) mmap(0,sizeof(struct protected_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		perror("in mmap()");
		exit(1);
	}
	while(true) {
		sleep(1);
		ptr->tex.lock();
		cout << "value: " << ptr->value << std::endl;
		ptr->tex.unlock();
	}

	//step 4: loop until heat death of the universe
	for(;;);

	return EXIT_FAILURE;	//this should never happen
}
