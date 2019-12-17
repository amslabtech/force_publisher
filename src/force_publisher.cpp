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
using namespace std;

std::mutex mtx;
ForceDataStructure forcedata;
volatile uint8_t renew_flag = 0;


//Declare any event handlers here. These will be called every time the associated event occurs.
static void CCONV
handler0(PhidgetVoltageRatioInputHandle ch, void * ctx, double voltageRatio)
{
	int channel;

	mtx.lock();
	//Getting the channel number to distinguish between Phidgets
	Phidget_getChannel((PhidgetHandle)ch, &channel);
//	printf("VoltageRatio [%d]: %lf\n", channel, voltageRatio);

	forcedata.force[channel] = voltageRatio;
	renew_flag |= 0x01<<channel;
	mtx.unlock();
}

//Declare any event handlers here. These will be called every time the associated event occurs.
static void CCONV
handler1(PhidgetVoltageRatioInputHandle ch, void * ctx, double voltageRatio)
{
	int channel;

	mtx.lock();
	//Getting the channel number to distinguish between Phidgets
	Phidget_getChannel((PhidgetHandle)ch, &channel);
//	printf("VoltageRatio [%d]: %lf\n", channel, voltageRatio);

	forcedata.force[channel+4] = voltageRatio;
	renew_flag |= 0x10<<channel;
	mtx.unlock();
}



int main()
{
	Mosquitto force_publisher;

	const char* ip_addr  = "localhost";
	const char* username = "force";
	struct timeval ts;

	//Declare your Phidget channels and other variables
	PhidgetVoltageRatioInputHandle fore_right;
	PhidgetVoltageRatioInputHandle fore_left;
	PhidgetVoltageRatioInputHandle center_left;
	PhidgetVoltageRatioInputHandle center_right;
	PhidgetVoltageRatioInputHandle rear_right;
	PhidgetVoltageRatioInputHandle rear_left;

	//Create your Phidget channels
	PhidgetVoltageRatioInput_create(&fore_left);
	PhidgetVoltageRatioInput_create(&fore_right);
	PhidgetVoltageRatioInput_create(&center_left);
	PhidgetVoltageRatioInput_create(&center_right);
	PhidgetVoltageRatioInput_create(&rear_left);
	PhidgetVoltageRatioInput_create(&rear_right);

	//Set addressing parameters to specify which channel to open (if any)
	Phidget_setDeviceSerialNumber((PhidgetHandle)fore_right,  589194);
	Phidget_setChannel((PhidgetHandle)fore_right,  0);
	Phidget_setDeviceSerialNumber((PhidgetHandle)fore_left,   589194);
	Phidget_setChannel((PhidgetHandle)fore_left,   1);
	Phidget_setDeviceSerialNumber((PhidgetHandle)center_left, 589194);
	Phidget_setChannel((PhidgetHandle)center_left, 2);
	Phidget_setDeviceSerialNumber((PhidgetHandle)center_right,589194);
	Phidget_setChannel((PhidgetHandle)center_right,3);

	Phidget_setDeviceSerialNumber((PhidgetHandle)rear_left,   583340);
	Phidget_setChannel((PhidgetHandle)rear_left,   0);
	Phidget_setDeviceSerialNumber((PhidgetHandle)rear_right,  583340);
	Phidget_setChannel((PhidgetHandle)rear_right,  3);
	
	//Assign any event handlers you need before calling open so that no events are missed.
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(   fore_left, handler0, NULL);
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(  fore_right, handler0, NULL);
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler( center_left, handler0, NULL);
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(center_right, handler0, NULL);
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(   rear_left, handler1, NULL);
	PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(  rear_right, handler1, NULL);

	//Open your Phidgets and wait for attachment
	Phidget_openWaitForAttachment((PhidgetHandle)fore_left,   7);
	Phidget_openWaitForAttachment((PhidgetHandle)fore_right,  7);
	Phidget_openWaitForAttachment((PhidgetHandle)center_left, 7);
	Phidget_openWaitForAttachment((PhidgetHandle)center_right,7);
	Phidget_openWaitForAttachment((PhidgetHandle)rear_left,   7);
	Phidget_openWaitForAttachment((PhidgetHandle)rear_right,  7);


	force_publisher.set_username_password(username,password);
	force_publisher.connect(ip_addr);
	force_publisher.subscribe(topic);

	cout << "\ndatasize = " << sizeof(ForceDataStructure) << " bytes\n";

	for(int i=0;;) {

		usleep(1*1000);

		if(renew_flag>=0x1F) {
			gettimeofday(&ts, NULL);
			forcedata.id  = i++;
			forcedata.sec = ts.tv_sec;
			forcedata.usec= ts.tv_usec;

cout << "flag:" << hex << +renew_flag ;
forcedata.print();
			mtx.lock();
			force_publisher.publish(topic,(void*)&forcedata,sizeof(forcedata));
			renew_flag = 0;
			mtx.unlock();
		}

	}
	sleep(1);

	force_publisher.cleanup_library();
}


