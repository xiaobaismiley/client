CC=g++
OBJS=DBUtil.o IOV_Client.o  base64.o modp_b64.o
LIB=-L/u01/app/oracle/product/11.2.0/db_1/lib -L/u01/app/oracle/product/11.2.0/db_1/rdbms/lib/ 
INCLUDE=-I/u01/app/oracle/product/11.2.0/db_1/precomp/public -I/u01/app/oracle/product/11.2.0/db_1/rdbms/public 

IOV_Client: $(OBJS)
	$(CC) -o IOV_Client $(OBJS) $(LIB) -pthread -locci -lclntsh 
	
DBUtil.o:dao/DBUtil.cpp
	$(CC) -c  dao/DBUtil.cpp $(INCLUDE)
base64.o:util/base64/base64.cpp
	$(CC) -c  util/base64/base64.cpp $(INCLUDE)
modp_b64.o:util/base64/modp_base64/modp_b64.cc
	$(CC) -c  util/base64/modp_base64/modp_b64.cc  $(INCLUDE)
	
IOV_Client.o:IOV_Client.cpp
	$(CC) -c IOV_Client.cpp $(INCLUDE) -Wall -O -g 
clean:
	rm -rf *.o  & rm IOV_Client
