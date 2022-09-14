#ifndef __BOOMER_H__
#define __BOOMER_H__

#include "global_type.h"
#include "printlog.h"

class Boomer
{
public:
	int ProcessPrintlog(Printlog &logagent, RequestInfo &req, FCGX_Request &request);
};

#endif
