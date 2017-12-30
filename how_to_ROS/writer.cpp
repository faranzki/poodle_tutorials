#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Temperature.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class Subscriber{
	public:
		//subscriber functions
		void tempSubscriber(const sensor_msgs::Temperature::ConstPtr& tmp_subscriber); //subscribes to the temperature topic
		std::vector<float> & getTemperature();
		std::vector<std::string> & getTimeStampTemp();
	protected:
		std::vector<float> temp; //vector of the temperature measurements
		std::vector<std::string> time_temp; //vector of the temperature measurements' time stamps
};

void Subscriber::tempSubscriber(const sensor_msgs::Temperature::ConstPtr& tmp_subscriber){
	float temperature = tmp_subscriber->temperature;
	std_msgs::Header hdr = tmp_subscriber -> header;
	int secs = hdr.stamp.sec;
	int nanosecs = hdr.stamp.nsec;
	this -> temp.push_back(temperature);
	std::stringstream ss;
	ss << secs << "." << nanosecs;
	std::string time = ss.str();
	this -> time_temp.push_back(time);
}

std::vector<float> & Subscriber::getTemperature(){
	return this -> temp;
}

std::vector<std::string> & Subscriber::getTimeStampTemp(){
	return this -> time_temp;
}

class TempWriter{
	public:
		void writer(std::string filename, std::vector<float> data, int length, std::vector<double> time);
};

void TempWriter::writer(std::string filename, std::vector<float> data, int length, std::vector<double> time){
	std::ofstream file(filename.c_str());
	file << "Time" << ";" << "Temperature" << std::endl;
	for (int i = 0; i < length; i++){
		file << time[i] << ";" << data[i]<< std::endl;
	}
}

int main(int argc, char **argv){
	ros::init(argc, argv, "listener");
	//declare all the different classes
	ros::NodeHandle n;
	Subscriber sbscrbr;
	TempWriter tmpwrtr;
	ros::Subscriber sub1 = n.subscribe("imu/temp", 1000, &Subscriber::tempSubscriber, &sbscrbr);
	//run the subscriber nodes
	ros::spin();
	//get all the data stored as members of the listener class
	std::vector<float> tmp = sbscrbr.getTemperature();
	int length_temp = tmp.size();
	std::vector<std::string> time_temp = sbscrbr.getTimeTemp();
	//write the data to file with the writer functions of the writer classes
	std::string filename_tmp = "tempdata";
	tmpwrtr.writer(filename_tmp, tmp, length_temp, time_temp);
	return 0;
}