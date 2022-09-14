#include "boomer.h"

int Boomer::ProcessPrintlog(Printlog &logagent, RequestInfo &req, FCGX_Request &request){
	ParamDict tmpdict = req.GetQueryDict();
    if (tmpdict["tbname"].empty() || tmpdict["starttime"].empty() || tmpdict["stoptime"].empty() || tmpdict["limits"].empty() || tmpdict["dbname"].empty()){
        ERROR("parameter empty");
       	return -3;
    }  
    logagent.SetTablename(tmpdict["tbname"]);
    logagent.SetDbname(tmpdict["dbname"]);
    logagent.SetStarttime(tmpdict["starttime"]);
    logagent.SetStoptime(tmpdict["stoptime"]);
    if (atoi(tmpdict["limits"].c_str()) > 1000)
        logagent.SetCountlimit(1000);
    else
        logagent.SetCountlimit(atoi(tmpdict["limits"].c_str()));
    if(!logagent.QuerySql()){
        ERROR("query failed");
        return -1;
    }   
    if(!logagent.Response(&request)){
        ERROR("reponse failed!");
        return -2;
    }
	return 0;
}
