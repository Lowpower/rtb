#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h> 
#include <string>
#include <time.h>
#include <errno.h>

#include "fcgi_stdio.h"

#include "log.h"
#include "config_data.h"
#include "ini_file.h"
//#include "clientcgi.h"

using namespace std;

std::string g_exec_name;
std::string g_host_name;

struct FcgiThreadParam{
	bool Init(int tid){
		tid_ = tid;
		return true;
	}
	int tid(){return tid_;}
private:
	int tid_;
};

void* fcgi_process(void* thd_param)
{
	FcgiThreadParam* param = (FcgiThreadParam*)thd_param;
	int tid = param->tid();
	srand(tid * tid * tid);
	
	int32_t ret = 0;
	struct timeval tv, tv1, tv2;
	char timeformatbuf[64];

	FCGX_Request request;
	FCGX_InitRequest(&request, 0, 0);
	
	INFO("child process %d init ok", param->tid());
	int32_t pt = 0;;
	struct tm cur_time;
	for(;;){
		static pthread_mutex_t req_locker = PTHREAD_MUTEX_INITIALIZER;
	    pthread_mutex_lock(&req_locker);
	    ret = FCGX_Accept_r(&request);
	    pthread_mutex_unlock(&req_locker);
        if (ret < 0){
			INFO("accept request error");
            continue;
        }
		
		localtime_r(&tv.tv_sec, & cur_time);
		strftime(timeformatbuf, sizeof(timeformatbuf), "%Y-%m-%d %H:%M:%S", &cur_time);

		RequestInfo req_info = RequestInfo();
		req_info.SetRawRequest(&request);
		
		gettimeofday(&tv, NULL);
		req_info.SetReqTime(tv);
		req_info.SetReqTimeStr(timeformatbuf);

		char *request_method = FCGX_GetParam("REQUEST_METHOD", request.envp);
		char *remote_addr_tmp = FCGX_GetParam("REMOTE_ADDR", request.envp);
		char *query_string_tmp = FCGX_GetParam("QUERY_STRING", request.envp);
		char *domain_cookie_tmp = FCGX_GetParam("DOMAIN_COOKIE", request.envp);
		char *real_ip = FCGX_GetParam("REAL_IP", request.envp);
		char *user_agent = FCGX_GetParam("USER_AGENT", request.envp);
		char *request_uri = FCGX_GetParam("REQUEST_URI", request.envp);
		char *serv_ip = FCGX_GetParam("SERVER_IP",request.envp);

		DEBUG("query_string:		[%s]", query_string_tmp);
		DEBUG("domain_cookie:		[%s]", domain_cookie_tmp);
		DEBUG("remote_addr:			[%s]", remote_addr_tmp);
		DEBUG("real_ip:				[%s]", real_ip);
		DEBUG("user_agent:			[%s]", user_agent);
		DEBUG("request_uri:			[%s]", request_uri);
		DEBUG("serv_addr:			[%s]", serv_ip);
		
		if (request_method == NULL || request_uri == NULL){
			ERROR("bad request, no method or uri!");
			FCGX_Finish_r(&request);
    		continue;
		}

		if ( strncasecmp(request_method, "POST", 4) == 0 ){
			if (0 == strncasecmp(request_uri, URI_POST_NORMAL, strlen(URI_POST_NORMAL))){
				//TODO
				INFO("post normal");
			}else{
				ERROR("Unknown POST uri!");
				FCGX_Finish_r(&request);
    			continue;
			}
        }else if (strncasecmp(request_method, "GET", 3) == 0){
			if (0 == strncasecmp(request_uri, URI_GET_NORMAL, strlen(URI_GET_NORMAL))){
				INFO("get normal");
			}else{
				DEBUG("Unknown GET uri!");
				FCGX_Finish_r(&request);
    			continue;
			}

			if (NULL == remote_addr_tmp && NULL == real_ip){
				ERROR("bad request, no client ip!");
				FCGX_Finish_r(&request);
				continue;
			}
			
			if (NULL == query_string_tmp || NULL == domain_cookie_tmp || NULL == user_agent){
				ERROR("bad request, no query info!");
				FCGX_Finish_r(&request);
				continue;
			}
			
			string ip;
			if (NULL != remote_addr_tmp){
				ip = remote_addr_tmp;
			}

			if (NULL != real_ip){
				string tmp_ip;
				tmp_ip = real_ip;
				string::size_type pos = tmp_ip.find_first_of(',', 0);
				if (string::npos != pos){
					ip = tmp_ip.substr(0, pos);
				}
			}

			if (0 == strcasecmp("unknown", ip.c_str())){
				if ((NULL != remote_addr_tmp) && (NULL != real_ip)){
					ERROR("DPI error: IP == \"unknown\"! REMOTE_ADDR:%s, X-Forwarded-For:%s, QUERY_STRING:%s", remote_addr_tmp, real_ip, query_string_tmp);
				}else if (NULL != remote_addr_tmp){
					ERROR("DPI error: IP == \"unknown\"! REMOTE_ADDR:%s, QUERY_STRING:%s", remote_addr_tmp, query_string_tmp);
				}else if (NULL != real_ip){
					ERROR("DPI error: IP == \"unknown\"! X-Forwarded-For:%s, QUERY_STRING:%s", real_ip, query_string_tmp);
				}
				if (NULL != remote_addr_tmp){
                    ip = remote_addr_tmp;
				}
			}
            
            req_info.SetIp(ip);
			if (NULL != query_string_tmp){
				req_info.SetQuery(string(query_string_tmp));
			}

			if (NULL != domain_cookie_tmp){
				req_info.SetCookie(string(domain_cookie_tmp));
			}

			if (NULL != user_agent){
				req_info.SetUserAgent(string(user_agent));
			}

			req_info.ParseCookie();
			
			if (false == req_info.ParseQuery()){
				ERROR("parse query error: QUERRY:%s", req_info.GetQuery().c_str());
				FCGX_Finish_r(&request);
				continue;
			}
		}else{
			DEBUG("Unknown request method!");
			FCGX_Finish_r(&request);
    		continue;
		}
		req_info.SetStopConsumeTime("InitParam");

		gettimeofday(&tv2, NULL);
		pt = (tv2.tv_sec - tv.tv_sec) * 1000000 + tv2.tv_usec - tv.tv_usec;
		req_info.SetProcessTime(pt);

#ifdef BOOMER_DEBUG
		// process and print consume time
		char buf_time[256] = {0};
		std::string log_str = "";
		snprintf(buf_time, sizeof(buf_time), "total process time: %d", req_info.GetProcessTime());
		log_str += buf_time;
		std::map<std::string, std::pair<int64_t, int64_t> > ct = req_info.GetConsumeTime();
		for(std::map<std::string, std::pair<int64_t, int64_t> >::iterator it = ct.begin(); it != ct.end(); it++){
			memset(buf_time, 0, sizeof(buf_time));
			snprintf(buf_time, sizeof(buf_time), ", %s : %d", it->first.c_str(), it->second.second - it->second.first);
			log_str += buf_time;
		}
		DEBUG("%s", log_str.c_str());
		DEBUG("process time: %d", req_info.GetProcessTime());
#endif
	}
	return NULL;
}

int main()
{
	// init rsyslog
	char buf[256] = {'\0'};
	readlink("/proc/self/exe", buf, 256);
	char *p = strrchr(buf, '/');
	*p = '\0';
	INFO("%s",buf);
	if(chdir(buf) < 0){
		fprintf(stderr, "change work dir to %d error", buf);
		return -1;
	}
	g_exec_name = p+1;
	
	Log::Init();
	INFO("fastcgi boomer starting---");
	INFO("init log ok");
	if(gethostname(buf, 256) != 0){
		ERROR("gethostname error, error:%d", errno);
		return -1;
	}
	g_host_name = buf;

	//Init configure data
	if (ConfigData::Init("./conf/boomer.ini") == false) {
        ERROR("init ConfigData failed: %s", ConfigData::ErrorStr().c_str());
        return -1;
    }
	INFO("init config data ok");

	int thread_num = ConfigData::Get("SYSTEM", "thread_num", 1);
    DEBUG("thread_num: %d", thread_num);
	
	FCGX_Init();
	pthread_t tids[thread_num];

	FcgiThreadParam thd_param[thread_num];
    for(int i = 0; i < thread_num; i++){
        if(false == thd_param[i].Init(i)){
            ERROR("thread parameter initialize error!\n");
            return -1;
        }
    }

    for(int i = 0; i < thread_num; i++) {
    	if(0 != pthread_create(&tids[i], NULL, (void* (*)(void*))fcgi_process, &thd_param[i])){
			ERROR("create fcgi thread failed");
			return -1;
		}
	}
	INFO("start fastcgi process ok");
	INFO("fastcgi boomer started");
	for(int i = 0; i < thread_num; i++){
		pthread_join(tids[i], NULL);
	}
	return 0;
}
