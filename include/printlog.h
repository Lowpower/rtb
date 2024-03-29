#ifndef __PRINTLOG_H__
#define __PRINTLOG_H__

#include <string>
#include <vector>
#include "mysql.h"
#include "log.h"
#include "config_data.h"

using namespace std;

class Printlog
{	
public:
	Printlog();
	bool Init();
	
	void SetStarttime(const string &time){starttime = time; }
	const string& GetStarttime()const{return starttime; }
	
	void SetStoptime(const string &time){stoptime = time;}
	const string& GetStoptime()const{return stoptime;}
	
	void SetTablename(const string &name){tablename = name;}
	const string& Gettablename(){return tablename;}
	
	void SetDbname(const string &name){dbname = name;}
	const string& Getdbname(){return dbname;}
	
	void SetCountlimit(const int &num){count_limit = num;}
	const int& GetCountlimit(){return count_limit;}

	int ProcessPrintlog(RequestInfo &req);
	bool GetRefer();
	bool QuerySql();
	bool Response(FCGX_Request * request);
private:
	void GetResponseBody();
	string starttime;
	string stoptime;
	string tablename;
	string dbname;
	int count_limit;
	vector<string> request_body;
	string responseBody;
	string host;
	int port;
	string user;
	string passwd;
	string refer;
};

#endif
