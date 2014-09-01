/*
 * GPSData.h
 *
 *  Created on: Mar 5, 2014
 *      Author: roger
 */

#ifndef GPSDATA_H_
#define GPSDATA_H_

#include<iostream>
using namespace std;

/*
 * 包含存放一条数据中的所有信息：gps，终端id，时间，方向，速度等
 */
class GPSData{
public:

	const string& getAlarminfor() const {
		return alarminfor;
	}

	void setAlarminfor(const string& alarminfor) {
		this->alarminfor = alarminfor;
	}

	const string& getDeviceid() const {
		return deviceid;
	}

	void setDeviceid(const string& deviceid) {
		this->deviceid = deviceid;
	}

	double getDir() const {
		return dir;
	}

	void setDir(double dir) {
		this->dir = dir;
	}

	double getLat() const {
		return lat;
	}

	void setLat(double lat) {
		this->lat = lat;
	}

	char getLocateProperty() const {
		return locate_property;
	}

	void setLocateProperty(char locateProperty) {
		locate_property = locateProperty;
	}

	double getLon() const {
		return lon;
	}

	void setLon(double lon) {
		this->lon = lon;
	}

	double getMileage() const {
		return mileage;
	}

	void setMileage(double mileage) {
		this->mileage = mileage;
	}

	double getSpeed() const {
		return speed;
	}

	void setSpeed(double speed) {
		this->speed = speed;
	}

	const string& getStatusinfor() const {
		return statusinfor;
	}

	void setStatusinfor(const string& statusinfor) {
		this->statusinfor = statusinfor;
	}

	const string& getTime() const {
		return time;
	}

	void setTime(const string& time) {
		this->time = time;
	}

private:
	string deviceid;
	string time;
	char locate_property;
	double lat;
	double lon;
	double speed; //  km/h
	double dir;   //度
	double mileage; //km
	string alarminfor;
	string statusinfor;

public:
	GPSData(){
		this->locate_property='V';
		this->lat = lat;
		this->lon = lon;
		this->speed = speed;
		this->dir = dir;
		this->mileage = mileage;
	}
	GPSData(string deviceid,string time,char locate_pro,double lat,double lon,double speed,double dir,
			double mileage,string alarminfor,string statusinfor){
		this->deviceid = deviceid;
		this->time = time;
		this->locate_property = locate_pro;
		this->lat = lat;
		this->lon = lon;
		this->speed = speed;
		this->dir = dir;
		this->mileage = mileage;
		this->alarminfor = alarminfor;
		this->statusinfor = statusinfor;

	}


};



#endif /* GPSDATA_H_ */
