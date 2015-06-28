#include "myTime.h"

void GetTimeInfo(TimeInfo &ti) {

#ifdef SYSTEM_WIN
	ti.sTime = GetTickCount();
#else
	gettimeofday(&ti.sTime, NULL);
#endif
}

long CalcTime(const TimeInfo &beg, const TimeInfo &end) {

	long msec;

#ifdef SYSTEM_WIN
	msec = end.sTime - beg.sTime;
#else
	long sec;
	long usec;

	sec = end.sTime.tv_sec - beg.sTime.tv_sec;
	usec = end.sTime.tv_usec - beg.sTime.tv_usec;

	if (0 > usec) {
		sec--;
		usec += 1000000;
	}

	msec = sec * 1000 + usec / 1000;
#endif

	return msec;
}