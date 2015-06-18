#include <sstream>
using namespace std;

#include <arpa/inet.h>
#include <sys/time.h>

#include "log.h"

extern string g_host_name;

static void PrintLog(RequestInfo& req_info)
{
	const ParamDict& queries = req_info.GetQueryDict();
	syslog(LOG_NOTICE, 
		"%s"				//current time, e.g. 2013-01-01 12:00:00
		"%s"				//ip, e.g. 127.0.0.1
		"%s"				//process hostname
		"%.3f"			//process total time
		"%s"				//refer
		"%s"				//user agent
		, req_info.GetReqTimeStr().c_str()
		, req_info.GetIp().c_str()
		, g_host_name.c_str()
		, req_info.GetProcessTime() / 1000000.0
		, req_info.GetRefer().c_str()
		, req_info.GetUserAgent().c_str()
		);
}
