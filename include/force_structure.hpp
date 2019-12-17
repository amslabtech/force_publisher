//
//	imu_structure.hpp
//
#ifndef _FORCE_STRUCTURE_HPP_
#define _FORCE_STRUCTURE_HPP_

#include <iostream>
#include <iomanip>
#include <time.h>
#include <sys/time.h>

struct ForceDataStructure {
	int32_t id;
	int32_t sec;
	int32_t usec;
	float  gain [8];	// forces
	float  force[8];	// forces

	void print() {
		std::cout
		<< std::setw(06) << std::dec << id
//		<< std::setw(12) << ts.tv_sec
//		<< std::setw(10) << ts.tv_usec
		<< std::setw(16) << force[0]
		<< std::setw(16) << force[1]
		<< std::setw(16) << force[2]
		<< std::setw(16) << force[3]
		<< std::setw(16) << force[4]
		<< std::setw(16) << force[5]
		<< std::endl;
	}

	void diff_time(struct timeval ts) {
		double t1 =  sec+ usec/1000000.0;
		double t2 = ts.tv_sec+ts.tv_usec/1000000.0;
		std::cout << t2 - t1;
	}
};

namespace force {
	const char* ccv2_force_addr = "192.168.0.172";
	const char* topic    = "force";
	const char* password = "mqtt";
};




#endif	// _FORCE_STRUCTURE_HPP_
