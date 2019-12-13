//
//	ForcePublisher
//
//	... Y.Kuroda
//
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <mutex>
#include <phidget22.h>

#include "mosquitto.hpp"		// c++ wrapper for mosquitto
#include "force_structure.hpp"	// force data types
#include "RTIMULib.h"			// force library
using namespace force;

std::mutex mtx;
volatile int renew_flag = 0;
ForceDataStructure forcedata;


//Declare any event handlers here. These will be called every time the associated event occurs.
static void CCONV onVoltageRatioChange(
	PhidgetVoltageRatioInputHandle ch, void * ctx, double voltageRatio) {
	int channel;

	//Getting the channel number to distinguish between Phidgets
	Phidget_getChannel((PhidgetHandle)ch, &channel);
//	printf("VoltageRatio [%d]: %lf\n", channel, voltageRatio);

	mtx.lock();
	forcedata.force[channel] = voltageRatio;
	renew_flag = 1<<channel;
	mtx.unlock();
}



int main()
{
	Mosquitto force_publisher;

	const char* ip_addr  = "localhost";
	const char* username = "force";
	struct timeval ts;


	//Declare your Phidget channels and other variables
	PhidgetVoltageRatioInputHandle fore_left;
	PhidgetVoltageRatioInputHandle fore_right;
	PhidgetVoltageRatioInputHandle center_left;
	PhidgetVoltageRatioInputHandle center_right;
	PhidgetVoltageRatioInputHandle rear_left;
	PhidgetVoltageRatioInputHandle rear_right;


	//Create your Phidget channels
	PhidgetVoltageRatioInput_create(&fore_left);
	PhidgetVoltageRatioInput_create(&fore_right);
	PhidgetVoltageRatioInput_create(&center_left);
	PhidgetVoltageRatioInput_create(&center_right);
	PhidgetVoltageRatioInput_create(&rear_left);
	PhidgetVoltageRatioInput_create(&rear_right);

	//Set addressing parameters to specify which channel to open (if any)
	Phidget_setDeviceSerialNumber((PhidgetHandle)fore_left,   123456);
	Phidget_setDeviceSerialNumber((PhidgetHandle)fore_right,  123456);
	Phidget_setDeviceSerialNumber((PhidgetHandle)center_left, 123456);
	Phidget_setDeviceSerialNumber((PhidgetHandle)center_right,123456);
	Phidget_setDeviceSerialNumber((PhidgetHandle)rear_left,   123456);
	Phidget_setDeviceSerialNumber((PhidgetHandle)rear_right,  123456);
	
	//Assign any event handlers you need before calling open so that no events are missed.
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(   fore_left, onVoltageRatioChange, NULL);
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(  fore_right, onVoltageRatioChange, NULL);
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler( center_left, onVoltageRatioChange, NULL);
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(center_right, onVoltageRatioChange, NULL);
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(   rear_left, onVoltageRatioChange, NULL);
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(  rear_right, onVoltageRatioChange, NULL);

	//Open your Phidgets and wait for attachment
	Phidget_openWaitForAttachment((PhidgetHandle)fore_left,   5000);
	Phidget_openWaitForAttachment((PhidgetHandle)fore_right,  5000);
	Phidget_openWaitForAttachment((PhidgetHandle)center_left, 5000);
	Phidget_openWaitForAttachment((PhidgetHandle)center_right,5000);
	Phidget_openWaitForAttachment((PhidgetHandle)rear_left,   5000);
	Phidget_openWaitForAttachment((PhidgetHandle)rear_right,  5000);





	force_publisher.set_username_password(username,password);
	force_publisher.connect(ip_addr);
	force_publisher.subscribe(topic);

	std::cout << "\ndatasize = " << sizeof(ForceDataStructure) << " bytes\n";

	int i=0;
	while(1) {

		usleep(1000);

		mtx.lock();
		if(renew_flag==0x3F) {
			gettimeofday(&ts, NULL);
			forcedata.id  = i;
			forcedata.sec = ts.tv_sec;
			forcedata.usec= ts.tv_usec;

			force_publisher.publish(topic,(void*)&forcedata,sizeof(forcedata));
			//forcedata.print();
			renew_flag = 0;
			i++;
		}
		mtx.unlock();

	}
	sleep(1);

	force_publisher.cleanup_library();
}


