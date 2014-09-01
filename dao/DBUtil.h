/*
 * DBUtil.h
 *
 *  Created on: Mar 5, 2014
 *      Author: roger
 */

#ifndef DBUTIL_H
#define DBUTIL_H

#include <iostream>
#define WIN32COMMON //避免函数重定义错误
#include <occi.h>
#include <cstdlib>
#include <stdio.h>
#include<sstream>
#include<iomanip>
#include <time.h>
#include <string.h>
#include <map>
#define DEVICEHEAD "02"

using namespace std;
using namespace oracle::occi;

class DBUtil
{
    public:
        string username;
        string pass;
        string srvName;
        Environment *env;
        Connection *conn;
    public:
        DBUtil();

        void initMin_num(int &a,int  &b,int &serival);
        void writeMin_num(int &a,int  &b,int &serival);
        void getData(vector<string>& motorid,vector<string>& lat,vector<string>& lon,vector<string>& dis
				,vector<string>& a,vector<string>& gprs, vector<string>& speed, vector<string>& dir
				,vector<string>& gpsLocate,vector<string>& weekday,vector<string>& state_id,vector<string>& sys_state_id,
				vector<string>&uploadTime,vector<string>& createTime,int &minNum,int allNum,string sql);
        vector<string>getCallData(string device);
        //获取新设备的信息
        vector<string> getNewDevice();
        //新设备变为老设备
        void saveNewDevice(string deviceid);
        string numberToString(double d);
        time_t convert_string_to_time_t(const string & time_string);
        ~DBUtil();
};


#endif /* BDUTIL_H_ */
