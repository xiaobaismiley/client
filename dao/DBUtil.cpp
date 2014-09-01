#include "DBUtil.h"

DBUtil::DBUtil() {
    username = "system";  //本地oracle获取数据
    pass = "cnhubei123";
    srvName = "webgps3";
    try
    {
        env = Environment::createEnvironment();
        conn = env->createConnection(username, pass, srvName);
    } catch(SQLException e)
    {	env = NULL;
        conn = NULL;
        cout<<e.what()<<endl;
    }
}
/*
 * double、int等类型 转化为string
 */
string DBUtil::numberToString(double d){
	std::stringstream ss;
	ss<<setprecision(9)<<d;
	string str = ss.str();
	return str;
}

/*
 * 上次已经传过的记rownum，最开始两个都是0，以后每次更新，只有在程序退出的时候更新到数据库中
 */
void DBUtil::initMin_num(int &a,int  &b,int &serival){
	try{
		Statement *stmt = conn->createStatement("select * from tb_count");
		ResultSet *rs = stmt->executeQuery();
		if (rs->next() == true)
		{
			a = rs->getInt(1);
			b = rs->getInt(2);
			serival=rs->getInt(3);
		}
		stmt->closeResultSet(rs);
		conn->terminateStatement(stmt);
	} catch(SQLException e) {
		cout << "initMin_num@DBUtil error, find me～～" << endl;
		cout << e.what() << endl;
	}
}

/*
 * 写入计数
 */
void DBUtil::writeMin_num(int &a,int  &b,int &serival){
	cout<<a<<" "<<b<<" "<<serival<<endl;
	try{
		string sql = "update tb_count set minnor_id=:1,MINALA_ID=:2,SERIVALNUM=:3";
		Statement *stmt = conn->createStatement(sql);
		stmt->setAutoCommit(true);//
		stmt->setInt(1, a);
		stmt->setInt(2, b);
		stmt->setInt(3, serival);
		stmt->executeUpdate();
		conn->terminateStatement(stmt); //结束本条SQL会话

	} catch(SQLException e) {
		cout << "writeMin_num@DBUtil error, find me～～" << endl;
		cout << e.what() << endl;
	}
}

/*
 *获取1000条位置数据，存放在内存中
 */
void DBUtil::getData(vector<string>& motorid,vector<string>& lat,vector<string>& lon,vector<string>& dis
		,vector<string>& a,vector<string>& gprs, vector<string>& speed, vector<string>& dir
		,vector<string>& gpsLocate,vector<string>& weekday,vector<string>& state_id,vector<string>& sys_state_id,
		vector<string>&uploadTime,vector<string>& createTime,int &minNum,int allNum,string type){
	try{
		//get 1000 pieces infor
		string sql ="";
		if("normal"==type)
			sql = "select * from instant_infor where id >:1 and rownum <= :2 and ( state_id='12' or state_id is  null)";
		else if("alarm" == type)
			sql = "select * from instant_infor where id >:1 and rownum <= :2 and state_id!='12' and state_id is not null ";

		Statement *stmt = conn->createStatement(sql);
		stmt->setInt(1, minNum);
		stmt->setInt(2, allNum);
		ResultSet *rs = stmt->executeQuery();
		int num =0;
		while (rs->next())
		{
			minNum = rs->getInt(1);  //string 型
			string mid =rs->getString(2);
			string mid2 = DEVICEHEAD+mid.substr(5);  //根据协议
			motorid.push_back(mid2);

			string slat = numberToString(rs->getNumber(3));lat.push_back(slat);
			string slon = numberToString(rs->getNumber(4));lon.push_back(slon);
	
			Timestamp uptime = rs->getTimestamp(5);
			string uptime_f = uptime.toText("yyyyMMddHH24miss",0,"");
			uploadTime.push_back(uptime_f);

			Timestamp recvtime = rs->getTimestamp(6);
			string recvtime_f = recvtime.toText("yyyyMMddHH24miss",0,"");
			createTime.push_back(recvtime_f);			

			string alarmid =rs->getString(7);state_id.push_back(alarmid);
			string stateid =rs->getString(8);sys_state_id.push_back(stateid);
			//速度和方向两边协议中不同，需要转换
			int sp = rs->getInt(9);
			string sspeed = numberToString(sp*2*1.852);speed.push_back(sspeed);
			int sd = rs->getInt(10);
			string sdir = numberToString(sd*10);dir.push_back(sdir);
			string sa = numberToString(rs->getNumber(11));a.push_back(sa);
			string sgprs = numberToString(rs->getNumber(12));gprs.push_back(sgprs);
			string sdis = numberToString(rs->getNumber(13));dis.push_back(sdis);
			string sweekday = numberToString(rs->getNumber(14));weekday.push_back(sweekday);
			string sgpslocate = numberToString(rs->getNumber(15));gpsLocate.push_back(sgpslocate);

			num++;
		}
		stmt->closeResultSet(rs);
		conn->terminateStatement(stmt);
	} catch(SQLException e) {
		cout << "getData@DBUtil error, find me～～" << endl;
		cout << e.what() << endl;
	}
}

/*
 * 返回指定设备号码的一条最新数据
 */
vector<string> DBUtil::getCallData(string device){
	vector<string> result ;
	try{
		string sql ="select b.* from (select motor_id, max(uploadtime)as uploadtime from instant_infor group by motor_id) a ,instant_infor b where a.motor_id = ':1' and a.uploadtime = b.uploadtime ";

		Statement *stmt = conn->createStatement(sql);
		stmt->setString(1, device);
		ResultSet *rs = stmt->executeQuery();
		while (rs->next())
		{
			string mid =rs->getString(2);
			string mid2 = DEVICEHEAD+mid.substr(5);  //根据协议
			result.push_back(mid2);

			string slat = numberToString(rs->getNumber(3));result.push_back(slat);
			string slon = numberToString(rs->getNumber(4));result.push_back(slon);

			Timestamp uptime = rs->getTimestamp(5);
			string uptime_f = uptime.toText("yyyyMMddHH24miss",0,"");
			result.push_back(uptime_f);

			Timestamp recvtime = rs->getTimestamp(6);
			string recvtime_f = recvtime.toText("yyyyMMddHH24miss",0,"");
			result.push_back(recvtime_f);

			string alarmid =rs->getString(7);result.push_back(alarmid);
			string stateid =rs->getString(8);result.push_back(stateid);
			//速度和方向两边协议中不同，需要转换
			int sp = rs->getInt(9);
			string sspeed = numberToString(sp*2*1.852);result.push_back(sspeed);
			int sd = rs->getInt(10);
			string sdir = numberToString(sd*10);result.push_back(sdir);
			string sa = numberToString(rs->getNumber(11));result.push_back(sa);
			string sgprs = numberToString(rs->getNumber(12));result.push_back(sgprs);
			string sdis = numberToString(rs->getNumber(13));result.push_back(sdis);
			string sweekday = numberToString(rs->getNumber(14));result.push_back(sweekday);
			string sgpslocate = numberToString(rs->getNumber(15));result.push_back(sgpslocate);

		}
		stmt->closeResultSet(rs);
		conn->terminateStatement(stmt);
	} catch(SQLException e) {
		cout << "getCallData@DBUtil error, find me～～" << endl;
		cout << e.what() << endl;
	}
	return result;
}


/*
 * 获取所有新设备的信息  输出格式： 车主手机-设备id-车牌号
 */
vector<string> DBUtil::getNewDevice(){
	vector<string> result ;
	try{
		//查询到有新的设备，获取车主手机号，设备id，车牌号（车辆标识）
		string sql ="select c.PHONE_NUMBER,b.device_num ,b.licence from user_motorcycle a,motorcycle b,motor_users c where b.isnew ='y' and a.motorid = b.id  and a.userid = c.id";

		Statement *stmt = conn->createStatement(sql);
		ResultSet *rs = stmt->executeQuery();
		while (rs->next())
		{
			string phone =rs->getString(1);
			string deviceid = rs->getString(2);
			string carnumber = rs->getString(3);
			string r = phone.append(",").append(deviceid).append(",").append(carnumber);
			result.push_back(r);
		}
		stmt->closeResultSet(rs);
		conn->terminateStatement(stmt);
	} catch(SQLException e) {
		cout << "getNewDevice@DBUtil error, find me～～" << endl;
		cout << e.what() << endl;
	}
	return result;
}

/*
 *新设备上传后，做标记，说明该设备已经不是新设备了
 */
void DBUtil::saveNewDevice(string deviceid){
	try{
		//查询到有新的设备，获取车主手机号，设备id，车牌号（车辆标识）
		string sql ="update motorcycle set isnew = 'n' where id = ':1'";
		Statement *stmt = conn->createStatement(sql);
		stmt->setString(1, deviceid);
		stmt->execute();

		conn->terminateStatement(stmt);
	} catch(SQLException e) {
		cout << "getNewDevice@DBUtil error, find me～～" << endl;
		cout << e.what() << endl;
	}
}

DBUtil::~DBUtil(){

}


time_t DBUtil::convert_string_to_time_t(const string & time_string)
{
	struct tm tm1;
	time_t time1;
	int i = sscanf(time_string.c_str(), "%d/%d/%d %d:%d:%d" ,
				&(tm1.tm_year),
				&(tm1.tm_mon),
				&(tm1.tm_mday),
				&(tm1.tm_hour),
				&(tm1.tm_min),
				&(tm1.tm_sec),
				&(tm1.tm_wday),
				&(tm1.tm_yday));

	tm1.tm_year -= 1900;
	tm1.tm_mon --;
	tm1.tm_isdst=-1;
	time1 = mktime(&tm1);

    return time1;
}


