#include "myDef.h"

#ifndef SYSTEM_WIN
//------------------------------------------------------------
//              signal function
//------------------------------------------------------------
Sigfunc * hb_signal(int signo,Sigfunc *func) {
	struct sigaction act,oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else { 
#ifdef SA_RESTART 
		act.sa_flags |= SA_RESTART;
#endif      
	}
	if (sigaction(signo,&act,&oact) < 0)
		return(SIG_ERR);
	return (oact.sa_handler);	
}
#endif

bool OverNGB(size_f size, int nGB) {

	size >>= 30;
	if (size >= nGB)
		return true;
	return false;
}

bool IsBigEndian() {
	union w {
		int a;
		char b;
	} c;

	c.a = 1;
	if (1 == c.b)
		return false;
	return true;
}

const char* time2str(time_t t, char *buf) {

	struct tm tt;

	_CHECK_RS(buf, NULL, NULL);

#ifdef SYSTEM_WIN
	localtime_s(&tt, &t);
#else
	localtime_r(&t, &tt);
#endif

	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", 
		tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, 
		tt.tm_hour, tt.tm_min, tt.tm_sec);
	
	return buf;
}

const char* ThreadId2Str(TID tid, char *buf) {

	_CHECK_RS(buf, NULL, NULL);

	sprintf(buf, "%ul", tid);
	return buf;
}

const char* ProcessId2Str(PID pid, char *buf) {

	_CHECK_RS(buf, NULL, NULL);
#ifdef SYSTEM_WIN
	sprintf(buf, "%ul", pid);
#else
	sprintf(buf, "%d", pid);
#endif
	return buf;
}