#ifndef _MY_TIME_H
#define _MY_TIME_H

#include "myDef.h"

struct TimeInfo {
#ifdef SYSTEM_WIN
	DWORD sTime;
#else
	struct timeval sTime;
#endif
};

void GetTimeInfo(TimeInfo &ti);
long CalcTime(const TimeInfo &beg, const TimeInfo &end);

#endif