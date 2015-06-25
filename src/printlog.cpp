#include "printlog.h"

Printlog::Printlog()
{
	DEBUG("Init Printlog");
}

bool Printlog::Init(){
	DEBUG("Init Printlog");
	string host = ConfigData::Get("MYSQL", "host", "");
	int port = ConfigData::Get("MYSQL", "port", 0);
	string user = ConfigData::Get("MYSQL", "user", "");
	string passwd = ConfigData::Get("MYSQL", "passwd", "");
	DEBUG("host is %s, port is %d, user is %s, passwd is %s ",host.c_str(),port,user.c_str(),passwd.c_str());
	if(port && host.empty() && user.empty() && passwd.empty() && dbname.empty())
		return true;
	return false;
}

bool Printlog::GetRefer(){
	DEBUG("GetRequestbody start");
	refer.clear();
	if (dbname == "yellowcar" ){
		if(tablename == "wxpay_log")
			return true;
		else if (tablename == "alipay_log")
			return true;
		else{
			ERROR("no such table");
			return false;
		}
	}
	else if (dbname == "pms"){
		if(tablename == "pms_order_log")
			return true;
		else{
			ERROR("no such table");
			return false;
		}
	}
	else if (dbname == "vehicle_control_center"){
		if(tablename == "action_logs")
			return true;
		else{
			ERROR("no such table");
			return false;
		}
	}
	else{
		ERROR("no such DB");
		return false;
	}
	return true;
}

bool Printlog::QuerySql(){
	if (false == GetRefer())
		return false;

	DEBUG("QuerySql!");
	pthread_mutex_lock(&reply_locker_);
	MYSQL mysql;
    MYSQL_RES *pRes;
    MYSQL_ROW row;
    char sql[1024];

	mysql_init( &mysql );
    if (! mysql_real_connect(&mysql, host.c_str(), user.c_str(), 
		           passwd.c_str(), dbname.c_str(), port, NULL, 0))
    {
     	 const char* eStr = mysql_error(&mysql);
         ERROR("Fail to connect mysql, %s\n", eStr);
         return false;
    }
    DEBUG("Connect mysql success!\n");
	sprintf(sql, "select * from %s  where created_time BETWEEN DATE('%s') and DATE('%s') limit %d", tablename.c_str(), starttime.c_str(), stoptime.c_str(), count_limit);
    int res = mysql_query( &mysql, sql);
    if (res != 0)
    {
		 const char* eStr = mysql_error(&mysql);
		 ERROR("sql = %s, error = %s", sql, eStr);
         return false;
    }
	
    pRes = mysql_store_result(&mysql);

    int numRows = mysql_num_rows(pRes);
    int numFields = mysql_num_fields(pRes);

    DEBUG("rows = %d, fields = %d\n", numRows, numFields);
    while (NULL != (row = mysql_fetch_row(pRes)))
    {
		request_body.push_back(row[2]);
    }
    mysql_free_result(pRes);
	mysql_close(&mysql);
	pthread_mutex_unlock(&reply_locker_);
	return true;
}
