/*
 * protected_memory.h
 *
 *  Created on: Nov 26, 2021
 *      Author: traveller
 */
#include <mutex>

#ifndef PROTECTED_MEMORY_H_
#define PROTECTED_MEMORY_H_

struct protected_memory {
	std::mutex tex;
	float value;
};

#endif /* PROTECTED_MEMORY_H_ */
