#include "IOV_Client.h"
#include "util/base64/base64.h" //base64编码

DBUtil db; //

/*
 * 设置非阻塞方式socket
 */
int setnonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
    {
        return -1;
    }
    return 0;
}
/*
 * 用于查看发送给服务器的指令是否成功返回信息。
 * 详细：指令发送后就立即开始接收，无阻塞，接收一次若无数据就睡眠一秒，继续接收；接收一次成功了，立即返回。若连续10接收失败，返回接收失败
 *
 * 目前用于：检查登录、心跳、新设备上传等处的指令是否发送成功哦你。
 */
bool checkRecv(int sock_fd,char buf[MAXDATASIZE],string back_data){
	int recvbytes=0;
	int number_to_quit = 0;
	while(1){
		recvbytes=recv(sock_fd, buf, MAXDATASIZE, 0);
		//login nonblcok 下返回值不一定大于0，可能errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN
		//上述情况只是没收到数据，并没有断开，所以需要循环
		if(recvbytes>0){
			buf[recvbytes] = '\0';
			string back(buf);
			if(back==back_data){ //back success
				break;
			}else{
				cout<<"recv fail for "<<number_to_quit<<" time"<<endl;
			}
		}
		number_to_quit++;
		if(number_to_quit>10)//超过最大失败次数，就返回了
		{
			cout<<"与服务器连接失败。"<<endl;
			return false;
		}
		sleep(1);//睡眠1秒继续连接
	}
	return true;
}

/*
 * 获得字串,没有去掉字串前后的空格
 */
vector<string> getSubString(string str,string splitmark){
	vector<string> strvec;
	string::size_type pos1, pos2;
	pos2 = str.find(',');
	pos1 = 0;
	while (string::npos != pos2)
	{
		//这里是用两个标号推进的，如果不是这样，而是每次匹配剩下的字符串都要通过substr复制，那效率就会很低了
			strvec.push_back(str.substr(pos1, pos2 - pos1));
			pos1 = pos2 + 1;
			pos2 = str.find(',', pos1);
	}
	strvec.push_back(str.substr(pos1));
	return strvec;
}

string test1(char* szBin)        // 存储szbinary所表达的二进制数据。
{
	char *p;
	unsigned char n = 0;
	unsigned char rightValue = 1;
		p = szBin + strlen(szBin);
	while ( p > szBin)
	{
	   p--;
	   if ( *p == '1' )
		   n += rightValue;
	   rightValue *= 2;
	}
//	printf( "%02X", n );    //以十六进制形式输出该二进制数据。
	stringstream ss;
	ss<<hex<<setw(2) << setfill('0')<<static_cast<int>(n);   //代替printf( "%02X", n )，以两位输出
	string str = ss.str();
	return str;
}

/*
 * 二进制转换为16进制，并且除掉了前面的0
 */
string binaryToHex(char* szBinary){
	int m,i,l;
	char tmp[9];
	for(i=0;szBinary[i]!='\0';i++)// 检查szbinary的合法性：只能有0和1字符。
	{
	   if(szBinary[i]=='0')
		   continue;
	   else if(szBinary[i]=='1')
		   continue;
	   else
		   printf("input numbers wrong!\n"); cin.get();
	}
//		printf("bits:%d\n",i);
	m=8-(i%8);
	string result ="";
	if(m!=0||m==8)
	{
		if(m==8)
			m=0;
		memcpy( szBinary+m, szBinary, strlen(szBinary)+1 );
		for(i=0;i<m;i++)
			szBinary[i]='0';
		i=0;
		while(szBinary[i]!='\0')
		{
			 for(l=0;l<8;i++,l++)
				 tmp[l]=szBinary[i];
			 tmp[l]=0;
			 result.append(test1(tmp));//调用输出函数。
		}
	}
	else
	{
		i=0;
		while(szBinary[i]!='\0')
		{
		   for(l=0;l<8;i++,l++)
			   tmp[l]=szBinary[i];
		   tmp[l] = 0;//szBinary[l]=szBinary[i];
		   result.append(test1(tmp));//调用输出函数。
		}
	}
	string re =""; //去掉空格
	for(i=0;i<result.length();i++){
		if(result[i]!='0')
			break;
	}
	return result.substr(i);
}

/*
 * 清空所有向量，每次从数据库中获取到的记录数量就以向量的长度为准了
 */
void normalInit(){
	 motorid.clear(); //正常非报警数据
	 lat.clear();
	 lon.clear();
	 dis.clear();
	 a.clear();
	 gprs.clear();
	 speed.clear();
	 dir.clear();
	 gpsLocate.clear();
	 weekday.clear();
	 state_id.clear();
	 sys_state_id.clear();
	 uploadTime.clear();
	 createTime.clear();

}

void alarmInit(){
	motorid2.clear(); //存放报警数据
	 lat2.clear();
	 lon2.clear();
	 dis2.clear();
	 a2.clear();
	 gprs2.clear();
	 speed2.clear();
	 dir2.clear();
	 gpsLocate2.clear();
	 weekday2.clear();
	 state_id2.clear();
	 sys_state_id2.clear();
	 uploadTime2.clear();
	 createTime2.clear();
}
/*
 * 返回一条正常的数据,如果有必要，就获取新的1000条数据   ，否则，直接从内存中获取数据，报警字段均为0
 * LBSU,REAL,ctsm,0000000004,20200000005;20140627113703;A;113.252432;22.564152;30;270;131;0;7,END
 *
 */
string getNormalData(int &minNor_id,int &act_Nor_id,int &serivalnum){
	if(act_Nor_id==0){//
		normalInit();
		db.getData(motorid,lat,lon,dis,a,gprs,speed,dir,gpsLocate,weekday,state_id,sys_state_id,uploadTime,createTime,minNor_id,ArraySize,"normal");
	}

	if(motorid.size()==0)//如果没有取到数据，就直接返回
		return "nodata";

	 //从内存中读,直到ArraySize条都读完
	string result = "";
	stringstream ss; //流水号固定格式
	ss<<setfill('0')<<setw(10)<<serivalnum;
	string serival = ss.str();

	string locate="A";// 定位标志。。偶数的时候表示定位精确
	int locate_int = atoi(gpsLocate[act_Nor_id].c_str());
	if(locate_int%2==0)
		locate = "A";
	else
		locate ="V";
	
	//状态信息，报警信息默认为0
	char nor[8];
	memset(nor,'0',sizeof(nor));
	//1、 状态号码分离   2、 筛选出两边都有的，放在数组中，然后对应，生成2进制数据，再转化为16进制的结果
	vector<string> normalinfors = getSubString(sys_state_id[act_Nor_id],",");
	vector<string>::iterator iter1 = normalinfors.begin(), iter2 = normalinfors.end();
	while (iter1 != iter2)
	{
		if(*iter1=="5")     //ACC开
			nor[5] ='1';
		else if(locate == "A") //判断设备是否准确定位
			nor[6] = '1';
		else if(state_id[act_Nor_id].find("12")>0) //到报警字段中找是否设防
			nor[4] = '1';
		++iter1;
	}
	//车子没有熄火的时候一直保持acc开的，就是状态中有5这个标志位，若没有，则表示车辆熄火，这个记录一下。
	if(nor[5] !='1'){
		nor[0] = '1';
	}
	nor[7]='\0';
	string nor_state = binaryToHex(nor);//传入2进制表示的   char 数组
	if(nor_state=="0"||nor_state==""){
		nor_state = "0";
	}

	result="LBSU,REAL,ctsm,"+serival+","+motorid[act_Nor_id]+";"+uploadTime[act_Nor_id]+";"+locate
			+";"+lon[act_Nor_id]+";"+lat[act_Nor_id]+";"+speed[act_Nor_id]+";"+dir[act_Nor_id]
			+";"+dis[act_Nor_id]+";0;"+nor_state+",END";

	act_Nor_id++;
	serivalnum++;
	if(act_Nor_id==motorid.size()){//内存中的数据都读过了，就重置0，重新到数据库中加载。
		act_Nor_id=0;
	}

//	cout<<motorid[0]<<endl;
//	cout<<lat[0]<<endl;        		//纬度  30.123
//	cout<<lon[0]<<endl;				//经度 120.123
//	cout<<dis[0]<<endl;
//	cout<<a[0]<<endl;
//	cout<<gprs[0]<<endl;
//	cout<<speed[0]<<endl;           //已转换
//	cout<<dir[0]<<endl; 			//已转换
//	cout<<gpsLocate[0]<<endl;     //偶数的时候表示定位精确，反之表示有飘移
//	cout<<weekday[0]<<endl;			//2014-03-25  胡工说明，总为0
//	cout<<state_id[0]<<endl;
//	cout<<sys_state_id[0]<<endl;
//	cout<<uploadTime[0]<<endl;      //设备上传时间
//	cout<<createTime[0]<<endl;		//服务器接收时间
//	cout<<minNor_id<<endl;
//	cout<<ArraySize<<endl;

	return result;
}

/*
 * 获得一条报警信息，先到内存中找，如果没有就到数据库中取1000条，放在内存中慢慢发
 *  LBSU,ALARM,htgl,0000000004,13911447899;20120627113703;A;113.252432;22.564152;30;270;130;100;7,END
 */
string getAlaData(int &minAla_id,int &act_Ala_id,int &serivalnum){
	if(act_Ala_id==0){           // 最多获取1000条
		alarmInit();
		db.getData(motorid2,lat2,lon2,dis2,a2,gprs2,speed2,dir2,gpsLocate2,weekday2,state_id2,sys_state_id2,uploadTime2,createTime2,minAla_id,ArraySize,"alarm");
	}
	if(motorid2.size()==0)//如果没有取到数据，就直接返回
		return "nodata";

	//从内存中读,直到ArraySize条都读完
	string result = "";
	stringstream ss; //流水号固定格式
	ss<<setfill('0')<<setw(10)<<serivalnum;
	string serival = ss.str();

	string locate="A";// 定位标志。。偶数的时候表示定位精确
	int locate_int = atoi(gpsLocate2[act_Ala_id].c_str());
	if(locate_int%2==0)
		locate = "A";
	else
		locate ="V";
	
	char al[14];
	memset(al,'0',sizeof(al));
	vector<string> alarminfors = getSubString(state_id2[act_Ala_id],",");
	vector<string>::iterator iter1 = alarminfors.begin(), iter2 = alarminfors.end();
	while (iter1 != iter2)
	{
		if(*iter1=="6")//超速
			al[11] ='1';
		else if(*iter1=="4")//入界
			al[9] = '1';
		else if(*iter1=="5")//出界
			al[8] = '1';
		else if(*iter1=="3")//碰撞，震动
			al[4] = '1';
		else if(*iter1=="13")//剪线，断电
			al[2] = '1';
		++iter1;
	}
	al[13]='\0';
	char al_state[8];
	memset(al_state,'0',sizeof(al_state));
	vector<string> al_state_infors = getSubString(sys_state_id2[act_Ala_id],",");
	vector<string>::iterator iter3 = al_state_infors.begin(), iter4 = al_state_infors.end();
	while (iter3 != iter4)
	{
		if(*iter3=="5")  //ACC开
			al_state[5] ='1';
		else if(locate == "A") //判断设备是否准确定位
			al_state[6] = '1';
		else if(state_id2[act_Ala_id].find("12")>0) //到报警字段中找是否设防
			al_state[4] = '1';
		++iter3;
	}
	//车子没有熄火的时候一直保持acc开的，就是状态中有5这个标志位，若没有，则表示车辆熄火，这个记录一下。
	if(al_state[5] !='1'){
		al_state[0] = '1';
	}
	al_state[7]='\0';
	string al_infor = binaryToHex(al);//传入2进制表示的   char 数组
	string al_state_result = binaryToHex(al_state);//传入2进制表示的  char数组

	if(al_infor=="0"||al_infor==""){
		al_infor = "0";
	}
	if(al_state_result=="0"||al_state_result==""){
		al_state_result = "0";
	}
	result="LBSU,ALARM,ctsm,"+serival+","+motorid2[act_Ala_id]+";"+uploadTime2[act_Ala_id]+";"+locate
			+";"+lon2[act_Ala_id]+";"+lat2[act_Ala_id]+";"+speed2[act_Ala_id]+";"+dir2[act_Ala_id]
			+";"+dis2[act_Ala_id]+";"+al_infor+";"+al_state_result+",END";

	act_Ala_id++;
	serivalnum++;
	if(act_Ala_id==motorid2.size()){//内存中的数据都读过了，就重置0，重新到数据库中加载。
		act_Ala_id=0;
	}

	return result;
}


int sock_fd, recvbytes;
char buf[MAXDATASIZE];
struct hostent *host;
struct sockaddr_in serv_addr;

int minNor_id=0;
int minAla_id = 0;
int act_Nor_id =0;
int act_Ala_id = 0;
int serivalnum=0;
bool heart_beat = false;

void init(){
	if( (host=gethostbyname(SERVER_IP) ) == NULL) {
		herror("gethostbyname出错！");
	}
	//建立socket
	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket创建出错！");
	}
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(SERVPORT);
	serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(serv_addr.sin_zero),8);
	if(connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect出错！");
	}
	setnonblocking(sock_fd);//设置为非阻塞模式（放在connect后面）
	cout<<"connect success..."<<endl;

	//初始化流水号，位置数据的id
	db.initMin_num(minNor_id,minAla_id,serivalnum);

	//发出登录请求，并查看响应
	stringstream ss; //流水号固定格式
	ss<<setfill('0')<<setw(10)<<serivalnum;
	string serival = ss.str();
	string login_smg = LOGIN_SMG1+serival+LONIN_SMG2;
	char send_ls[MAXDATASIZE];
	strcpy(send_ls,login_smg.c_str());
	if(send(sock_fd,send_ls, MAXDATASIZE, 0) == -1) {
		close(sock_fd);
	}
	//如果登录失败不会跳出该函数，一直在函数中循环
	bool isLogin = checkRecv(sock_fd,buf,LOGIN_SMG_BACK1+serival+LOGIN_SMG_BACK2);
	if(!isLogin)
		cout<<"login fail...."<<endl;
	else{
		cout<<"login success...."<<endl;
		memset(buf, '\0', sizeof(buf));

		sleep(1);

		act_Nor_id =0;
		act_Ala_id = 0;
	}
	serivalnum++;

}

/*
 * 获得点名数据，点名数据上的序列号要和收到的序列号是一样的
 */
string getCallData(string str){
	vector<string> strs = getSubString(str,",");
	string ser = strs[3];
	string device = strs[4];

	vector<string> calldata = db.getCallData(device);
	if(calldata.size()==0)
		return "nodata";

	string result = "";
	stringstream ss;
	ss<<setfill('0')<<setw(10)<<ser;
	string serival = ss.str();

	string locate="A";// 定位标志。。偶数的时候表示定位精确
	int locate_int = atoi(calldata[13].c_str());
	if(locate_int%2==0)
		locate = "A";
	else
		locate ="V";

	char al[14];
	memset(al,'0',sizeof(al));
	vector<string> alarminfors = getSubString(strs[5],",");
	vector<string>::iterator iter1 = alarminfors.begin(), iter2 = alarminfors.end();
	while (iter1 != iter2)
	{
		if(*iter1=="6")//超速
			al[11] ='1';
		else if(*iter1=="4")//入界
			al[9] = '1';
		else if(*iter1=="5")//出界
			al[8] = '1';
		else if(*iter1=="10") //疲劳
			al[5] = '1';
		else if(*iter1=="3")//碰撞，震动
			al[4] = '1';
		else if(*iter1=="13")//剪线，断电
			al[2] = '1';
		++iter1;
	}
	al[13]='\0';
	char al_state[8];
	memset(al_state,'0',sizeof(al_state));
	vector<string> al_state_infors = getSubString(strs[6],",");
	vector<string>::iterator iter3 = al_state_infors.begin(), iter4 = al_state_infors.end();
	while (iter3 != iter4)
	{
		if(*iter3=="5")
			al_state[5] ='1';
		else if(locate == "A") //判断设备是否准确定位
			al_state[6] = '1';
		else if(state_id[act_Ala_id].find("12")>0) //到报警字段中找是否设防
			al_state[4] = '1';
		++iter3;
	}
	//车子没有熄火的时候一直保持acc开的，就是状态中有5这个标志位，若没有，则表示车辆熄火，这个记录一下。
	if(al_state[5] !='1'){
		al_state[0] = '1';
	}

	al_state[7]='\0';
	string al_infor = binaryToHex(al);//传入2进制表示的   char 数组
	string al_state_result = binaryToHex(al_state);//传入2进制表示的   char 数组

	if(al_infor=="0"||al_infor==""){
		al_infor = "0";
	}
	if(al_state_result=="0"||al_state_result==""){
		al_state_result = "0";
	}
	result="LBSU,CALL,ctsm,"+serival+","+strs[0]+";"+strs[3]+";"+locate
			+";"+strs[2]+";"+strs[1]+";"+strs[7]+";"+strs[8]
			+";"+strs[11]+";"+al_infor+";"+al_state_result+",END";
	return result;
}

/*
 *	用户名密码如果不对就返回nodata
 * 　　LBSD,PASS, htgl,0000000008,13811473790; MTIzNDU2,END
 */
string getPassData(string str){
	vector<string> strs = getSubString(str,",");
	string seri = strs[3];
	string usernameAndpass = strs[4];

	vector<string> usernamePwd = getSubString(usernameAndpass,";");//用户名和密码
	if(usernamePwd.size()==2){
		string username = usernamePwd[0];//用户名
		string pwd64 = usernamePwd[1]; //密码的64编码
		string pwd ; //输出

		bool isDecodeOK = Base64Decode(pwd64, &pwd);// 64编码转换
		if(isDecodeOK && USERNAME==username && PWD==pwd){
			return PASS_SMG_BACK1+seri+PASS_SMG_BACK2;
		}
	}
	return "nodata";

}

/*
 * 返回新的设备,返回每条信息中格式为：  (暂时放一放)
 * 车主手机-设备id-车牌号 （暂不含设备手机号码）
 */
void sendNewDeviceInfor(int &serivalnum){

	stringstream ss; //流水号固定格式（int 要转化为string）
	ss<<setfill('0')<<setw(10)<<serivalnum;
	string serival = ss.str();

	vector<string> devices =db.getNewDevice();
	for(int i=0;i<devices.size();i++){
		cout<<devices[i]<<endl;
		vector<string> infors = getSubString(devices[i],",");
		//形成新设备上传指令
		string regi = REGI1+serival+","+infors[0]+";"+infors[1]+";"+";"+infors[2]+";"+";";
		cout<<regi<<endl;
		//上传
		if(send(sock_fd, regi.c_str(), MAXDATASIZE, 0) == -1) {
			perror((infors[1]+"新设备数据上传出错！").c_str());
			continue;
		}
		//开始检查有没有上传成功
		bool isuploadSuc = checkRecv(sock_fd,buf,REGI_BACK1+serivalnum+REGI_BACK2);
		//若上传成功了，保存到数据库，没有成功的就不管了，下次继续上传
		if(isuploadSuc){
			db.saveNewDevice(infors[1]);
			serivalnum++;
		}
	}
}

int main() //命令行开启，输入参数
{
	//初始化，建立socket，登录
    init();

	//========================一旦进入while循环就不跳出了，除非手动关闭，  登录成功后发1000条跳一次心跳=======================
    while(1){
    	if(act_Nor_id==0&&act_Ala_id==0){  //一轮以后才进去一次(act_Nor_id循环1000次会重新置零)
    		stringstream ss; //流水号固定格式
			ss<<setfill('0')<<setw(10)<<serivalnum;
			string serival = ss.str();
    		char send_sh[MAXDATASIZE];
    		string heart_smg = HEARTBEAT_SMG1+serival+HEARTBEAT_SMG2;
    		strcpy(send_sh,heart_smg.c_str());

    		cout<<"send heartbeat...."<<endl;
			if(send(sock_fd, send_sh, MAXDATASIZE, 0) == -1) {//发送心跳
				perror("send heartbeat 出错！");
			}
			heart_beat = checkRecv(sock_fd,buf,HEARTBEAT_SMG_BACK1+serival+HEARTBEAT_SMG_BACK2);
			if(!heart_beat){
				cout<<"heartbeat back fail...."<<endl; //如果没有心跳返回，就不发了
				continue;
			}
			cout<<"heartbeat back success...."<<endl;
    		serivalnum++;
    		sleep(1);
    	}
    	memset(buf, '\0', sizeof(buf));


		// ****************检查点名和密码同步，结果无阻塞返回*****************************
		recvbytes=recv(sock_fd, buf, MAXDATASIZE, 0);
		if (recvbytes == -1 ) {
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN){//几种特殊情况，不断开socket

			}else{
				//socket 需要释放
				cout<<"关闭socket，重启初始化"<<endl;
				close(sock_fd);
				init();
				continue;
			}
		}else if(recvbytes == 0){
			//socket 需要释放
			cout<<"关闭socket，重启初始化"<<endl;
			close(sock_fd);
			init();
			continue;
		}else{                             //其他信息
			buf[recvbytes] = '\0';
			printf("Received: %s",buf);
			string str(buf);
			if(str!=""&&str.length()!=0)
				cout<<"收到服务端发送的数据   "<<str<<endl;
			//点名
			if(str.find(RECV_CALL_SMG)){//LBSD,CALL, htgl,0000000011,13911447899,END
				string calldata = getCallData(str);
				if(calldata!="nodata"){
					if(send(sock_fd, calldata.c_str(), MAXDATASIZE, 0) == -1) {//发送点名回应（服务器能不能成功接收暂时不管）
						perror("发送点名信息出错！");
					}
				}
			}
			//密码同步
			else if(str.find(PASS_SMG)){
				string passdata = getPassData(str);
				if("nodata"!=passdata){
					if(send(sock_fd, passdata.c_str(), MAXDATASIZE, 0) == -1) {//发送密码回应（服务器能不能成功接收暂时不管）
						perror("发送点名信息出错！");
					}
				}

			}
		}

		//--------------发送位置信息---------------------
		string data = getNormalData(minNor_id,act_Nor_id,serivalnum);
		if(data!="nodata"){
			cout<<"normal: "<<data<<endl;
			//向服务器发送常规数据
			if(send(sock_fd, data.c_str(), MAXDATASIZE, 0) == -1) {
				perror("发送正常位置信息 出错！");
			}
		}else{
			cout<<"目前数据库中没有新的非报警数据可以上传"<<endl;
		}

		//--------------发送报警信息-----------------------
		string alaData = getAlaData(minAla_id,act_Ala_id,serivalnum);
		cout<<alaData<<endl;
		if(alaData!="nodata"){
			cout<<"alarm: "<<alaData<<endl;
			//向服务器发送报警数据
			if(send(sock_fd, alaData.c_str(), MAXDATASIZE, 0) == -1) {
				perror("发送报警信息出错！");
			}
		}else{
			cout<<"目前数据库中没有新的报警数据可以上传"<<endl;
		}

		//查看有没有新的设备，如果有，全部获取到，并且循环在这里发完（成功的做个标记，下次不发了，失败的不做标记下一次继续发）-----------
//		sendNewDeviceInfor(serivalnum);


		//------------------写入数据库---------------------
		db.writeMin_num(minNor_id,minAla_id,serivalnum);

    }

}
