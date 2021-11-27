#include <iostream>
#include "SensorReader.h"

using namespace std;

int main() {

	string name("test.txt");
	// step 1: initialize sensor readers
	SensorReader r1(name, 4, 2);
	//step 3: start timers

	r1.set_timer_period(1);

	sleep(1);
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
