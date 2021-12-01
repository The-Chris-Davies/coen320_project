#include <iostream>
#include "SensorReader.h"

//
#include "Driver.h"
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
	int sizes[5] = {3, 3, 4, 7, 3};
	//create readers
	for(int i = 0; i < 5; i++) {
		readers[i] = new SensorReader(names[i], sizes[i], 4);
	}

	//step 2: start timers
	for(auto& reader: readers) {
			reader->set_timer_period(5);
		}
	
	Driver overdrive;// initialize a driver
	overdrive.set_timer_period(1);// set the period to 1
	
	while(true){
		int number, newper;
		cin >> number >> newper;
		readers[number] -> set_timer_period(newper);
		overdrive.get_output_lock();
		cout << "set period for " << names[number] << " to " << newper << endl;
		overdrive.free_output_lock();
	}

	return EXIT_FAILURE;	//this should never happen
}
