#ifndef __LOG_H__
#define __LOG_H__

#include <unistd.h>
#include <syslog.h>
#include <stdio.h>
#include <string>
using namespace std;

#include "request_info.h"

#define ERROR(fmt, arg...)  syslog(LOG_ERR, "FATAL [%s:%d]"fmt, __FILE__, __LINE__, ##arg)
#define WARN(fmt, arg...)   syslog(LOG_WARNING, "WARN "fmt, ##arg)
#define NOTICE(fmt, arg...) syslog(LOG_NOTICE, "NOTICE "fmt, ##arg)
#define INFO(fmt, arg...)   syslog(LOG_INFO, "INFO "fmt, ##arg)

#ifndef BOOMER_DEBUG
#define DEBUG(...) 
#else
#define DEBUG(fmt, args...) syslog(LOG_DEBUG, " DEBUG [%s:%d]"fmt, __FILE__, __LINE__, ##args)
#endif

extern std::string g_exec_name;

struct LogItem {
    LogItem() {
        start_time.tv_sec = 0;
        start_time.tv_usec = 0;
        processing_time = 0;
        processing_tac_time = 0;
    }
    timeval start_time;
    std::string start_time_str;
    std::string processing_host_name;
    double processing_time;
    double processing_tac_time;
    std::string processing_debug;
	std::string user_ip;
};


class Log {
public:
    static void Init() {
    	static Log instance;
    	static char syslog_name[128] = {0};
    	snprintf(syslog_name, sizeof(syslog_name), "<%s>", g_exec_name.c_str());
    	openlog(syslog_name, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_LOCAL6);
    }
	static void Final() {
		closelog();
	}

	static void PrintLog(RequestInfo& req_info);

private:
    Log() {}
};

#endif
