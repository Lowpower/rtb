#include "request_info.h"
#include "log.h"

static void trim(string& s)
{
	s.erase(0, s.find_first_not_of(' '));
	s.erase(s.find_last_not_of(' ') + 1);
}

RequestInfo::RequestInfo()
{
    DEBUG("RequestInfo::RequestInfo()");
}

bool RequestInfo::ParseKeyValue(const std::string &str, ParamDict &param_dict)
{
}

bool RequestInfo::ParseQuery()
{
	if (query_str_.empty()) {
		return false;
	}

	string::size_type begin = 0;
	while(begin < query_str_.size()){
		string::size_type end = query_str_.find('&', begin);
		if(end == string::npos) end = query_str_.size();
		string kv = query_str_.substr(begin, end - begin);
		string::size_type eq = kv.find('=');
		if(eq != string::npos){
			string k = kv.substr(0, eq);
			string v = kv.substr(eq + 1);
			if(k != "" && v != ""){
				query_dict_.insert(ParamDict::value_type(k, v));
			}
		}
		begin = end + 1;
	}

	DEBUG("query param list:");
	for(ParamDict::iterator it = query_dict_.begin(); it != query_dict_.end(); ++it){
		DEBUG("%s:%s", it->first.c_str(), it->second.c_str());
	}
	return true;
}

bool RequestInfo::ParseCookie()
{
	if (cookie_str_.empty())
		return true;

	string::size_type begin = 0;
	while(begin < cookie_str_.size()){
		string::size_type end = cookie_str_.find(';', begin);
		if(end == string::npos) end = cookie_str_.size();
		string kv = cookie_str_.substr(begin, end - begin);
		string::size_type eq = kv.find('=');
		if(eq != string::npos){
			string k = kv.substr(0, eq);
			string v = kv.substr(eq + 1);
			trim(k);
			trim(v);
			cookie_dict_.insert(ParamDict::value_type(k, v));
		}
		begin = end + 1;
	}

	DEBUG("cookie param list");
	for(ParamDict::iterator it = cookie_dict_.begin(); it != cookie_dict_.end(); ++it){
		DEBUG("%s:%s", it->first.c_str(), it->second.c_str());
	}
	return true;
}
