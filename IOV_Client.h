/*
 * IOV_Client.h
 *
 *  Created on: 2014-3-25
 *      Author: Roger
 */

#ifndef IOV_CLIENT_H_
#define IOV_CLIENT_H_

#include <errno.h>    //EINTR  EWOULDBLOCK   EAGAIN
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>   //set nonblocking
#include <iostream>
#include <vector>
#include <iomanip>   //固定格式
#include <sstream>       //流输出
#include "dao/DBUtil.h"

using namespace std;

#define MAXDATASIZE 100    /*每次最大数据传输量 */
#define SERVPORT 6164
#define USERNAME "ctsm2014"
#define PWD "ctsm2014"
#define ArraySize 10

//LBSU 表示上行，LBSD表示下行
#define LOGIN_SMG1  "LBSU,LOGIN,ctsm,"            //login
#define LONIN_SMG2  ",ctsm2014;ctsm2014,END"
#define LOGIN_SMG_BACK1 "LBSD,LOGIN,ctsm,"       //login back
#define LOGIN_SMG_BACK2 ",ok;tcfCvbPJuaY=,END"

#define HEARTBEAT_SMG1 "LBSU,HEART,ctsm,"        //heartbeat
#define HEARTBEAT_SMG2 ",void,END"
#define HEARTBEAT_SMG_BACK1 "LBSD,HEART,ctsm,"    //heartbeat  back
#define HEARTBEAT_SMG_BACK2 ",ok,END"

#define RECV_CALL_SMG "LBSD,CALL"               //call

#define PASS_SMG "LBSD,PASS"             //PASS
#define PASS_SMG_BACK1   "LBSU,HEART,ctsm," //　　LBSU,PASS,htgl,0000000008,OK,END
#define PASS_SMG_BACK2 	",OK,END"

#define REGI1 "LBSU,REGI,ctsm,"    //add new device
#define REGI2 ",END"
#define REGI_BACK1 "LBSU,REGI,ctsm,"  //new device back
#define REGI_BACK2 ",ok;tcfCvbPJuaY=,END"

const char* SERVER_IP = "111.4.117.102";

vector<string> motorid; //存放常规数据，批量存放在内存中
vector<string> lat;
vector<string> lon;
vector<string> dis;
vector<string> a;
vector<string> gprs;
vector<string> speed;
vector<string> dir;
vector<string> gpsLocate;
vector<string> weekday;
vector<string> state_id;
vector<string> sys_state_id;
vector<string> uploadTime;
vector<string> createTime;

vector<string> motorid2; //存放报警数据
vector<string> lat2;
vector<string> lon2;
vector<string> dis2;
vector<string> a2;
vector<string> gprs2;
vector<string> speed2;
vector<string> dir2;
vector<string> gpsLocate2;
vector<string> weekday2;
vector<string> state_id2;
vector<string> sys_state_id2;
vector<string> uploadTime2;
vector<string> createTime2;




#endif /* IOV_CLIENT_H_ */
