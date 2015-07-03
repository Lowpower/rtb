#include "printlog.h"

Printlog::Printlog()
{
	DEBUG("Init Printlog");
}

bool Printlog::Init(){
	DEBUG("Init Printlog");
	host = ConfigData::Get("MYSQL", "host", "");
	port = ConfigData::Get("MYSQL", "port", 0);
	user = ConfigData::Get("MYSQL", "user", "");
	passwd = ConfigData::Get("MYSQL", "passwd", "");
	DEBUG("host is %s, port is %d, user is %s, passwd is %s ",host.c_str(),port,user.c_str(),passwd.c_str());
	if(port && !host.empty() && !user.empty() && !passwd.empty())
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
	request_body.clear();
    DEBUG("Connect mysql success!\n");
	if (tablename == "pms_order_log")
		sprintf(sql, "select * from %s  where create_time BETWEEN DATE('%s') and DATE('%s') limit %d", tablename.c_str(), starttime.c_str(), stoptime.c_str(), count_limit);
	else
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

	DEBUG("sql = %s", sql);
    DEBUG("rows = %d, fields = %d\n", numRows, numFields);
    while (NULL != (row = mysql_fetch_row(pRes)))
    {
		string tmp=row[0];
		for (int i=1; i < numFields; i++)
			tmp = tmp + "\t" + row[i];
		request_body.push_back(tmp);
    }
    mysql_free_result(pRes);
	mysql_close(&mysql);
	return true;
}

static int fcgx_geterror(int rc, FCGX_Request * request)
{
  const int len=128;
  char buf[len];
  if(rc>=0) return rc; 
  rc=FCGX_GetError(request->out);
  INFO("FCGX_GetError %d", rc);
  if(rc>0) INFO("FCGX_GetError %s", strerror_r(rc, buf, len));
  return rc;
}

void Printlog::GetResponseBody(void){
	responseBody.clear();
	string str1 = "&lt;";
	string str2 = "&gt;";
	for (vector<string>::iterator iter = request_body.begin(); iter != request_body.end(); iter++){
		int pos = -1;
		for (int i = 0; i < (*iter).length(); i++){
			if((*iter)[i] == '<')
				pos = i;
			else if((*iter)[i] == '>' && pos > 0){
				(*iter).replace(pos, 1, str1);
				(*iter).replace(i + 3, 1, str2);
				pos = -1;
			}
		}
		responseBody = responseBody + (*iter) + "\n";
	}
}

bool Printlog::Response(FCGX_Request *request){
	DEBUG("return response!");
	GetResponseBody();
	DEBUG("content is %s",responseBody.c_str());
	int len = responseBody.length();
	int rc=FCGX_FPrintF(request->out,"HTTP/1.0 200 OK\r\n"
				"Content-Length: %u\r\n"
				"Content-Type: text\r\n\r\n",
				len);
	if(rc < 0) {
		rc = fcgx_geterror(rc, request);
	}
	if(len > 0){
		FCGX_PutStr(responseBody.c_str(), len, request->out);
	}
	FCGX_Finish_r(request);
	return rc;
}
