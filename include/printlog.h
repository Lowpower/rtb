#ifndef __PRINTLOG_H__
#define __PRINTLOG_H__

#include <string>
#include "global_type.h"
#include "log.h"

using namespace std;

class Printlog
{	
public:
	Printlog();
	void SetStarttime(const string &time){starttime = time; }
	const string& GetStarttime()const{return starttime; }
	
	void SetStoptime(const string &time){stoptime = time;}
	const string& GetStoptime()const{return stoptime;}
	
	void SetTablename(const string &name){tablename = name;}
	const string& Gettablename(){return tablename;}
	
	void SetCountlimit(const int &num){count_limit = num;}
	const int& GetCountlimit(){return count_limit;}

	const string& GetRequestbody(const string &starttime, const string &stoptime, const string &tablename, const int &countlimit);
private:
	string starttime;
	string stoptime;
	string tablename;
	int count_limit;
	string request_body;
};

#endif
