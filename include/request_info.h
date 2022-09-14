#ifndef __REQUEST_INFO_H__
#define __REQUEST_INFO_H__

#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <sys/time.h>
#include "fcgi_stdio.h"

using namespace std;
typedef unordered_set<uint32_t> Uint32Set;
typedef unordered_set<std::string> StringSet;

typedef unordered_map<std::string, std::string> ParamDict;

class RequestInfo
{
public:
	enum RequestType{
		RT_PRINTLOG = 0
	};
	static bool ParseKeyValue(const std::string &str, ParamDict &param_dict);
public:
	void SetRawRequest(FCGX_Request *req) { raw_req_ = req; }
	FCGX_Request* GetRawRequest() { return raw_req_; }

	void SetReqTime(const struct timeval &tv) { req_time_ = tv; }
	const struct timeval& GetReqTime() const{ return req_time_; }

	void SetReqTimeStr(const string &timestr) { req_time_str_ = timestr; }
	const string& GetReqTimeStr() const{ return req_time_str_; }

	void SetProcessTime(const int32_t &process_time) { process_time_ = process_time; }
	const int32_t& GetProcessTime() const{ return process_time_; }

	void SetReqType(const RequestType &rt){req_type = rt;}
	const RequestType& GetReqType(){return req_type;}

	void SetConsumeTime(const string &consume_name, const std::pair<int64_t, int64_t> &consume_time) { 
		consume_time_.insert(std::pair<string, std::pair<int64_t, int64_t> >(consume_name, consume_time)); 
	}
	void SetStartConsumeTime(const string &consume_name) {
#ifdef BOOMER_DEBUG
		struct timeval tv;
		gettimeofday(&tv, NULL);
		int64_t pt = tv.tv_sec * 1000000 + tv.tv_usec;
		consume_time_.insert(std::make_pair(consume_name, std::make_pair(pt,pt)));
#endif
	}
	void SetStopConsumeTime(const string &consume_name) {
#ifdef BOOMEr_DEBUG
		struct timeval tv;
		gettimeofday(&tv, NULL);
		int64_t pt = tv.tv_sec * 1000000 + tv.tv_usec;
		std::map<std::string, std::pair<int64_t,int64_t> >::iterator it = consume_time_.find(consume_name);
		if(it != consume_time_.end()) {
			it->second.second = pt; 
		} 
#endif
	}
	const std::map<std::string, std::pair<int64_t,int64_t> >& GetConsumeTime() const{ return consume_time_; }
	
	void SetIp(const string &ip){ ip_ = ip; }
	const string& GetIp() const{ return ip_; }

	void   SetUserAgent(const string &user_agent){ user_agent_ = user_agent; }
	const string& GetUserAgent()const{ return user_agent_; }
	
	void   SetCookie(const string &cookie){ cookie_str_ = cookie; }
	const string& GetCookie()const{ return cookie_str_; }

	void SetQuery(const string &query){ query_str_ = query; }
	const string& GetQuery()const{ return query_str_; }

	RequestInfo();

	bool ParseQuery();
	bool ParseCookie();

	const ParamDict& GetQueryDict() const{ return query_dict_; };
	const ParamDict& GetCookieDict() const{ return cookie_dict_; }

private:
	string ip_;
	string user_agent_;
	string cookie_;

	struct timeval req_time_;
	string req_time_str_;
	int32_t process_time_;
	std::map<std::string, std::pair<int64_t, int64_t> > consume_time_;
	FCGX_Request *raw_req_;
	string query_str_;
	string cookie_str_;
	ParamDict query_dict_;
	ParamDict cookie_dict_;
	RequestType req_type;
};

#endif
