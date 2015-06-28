#ifndef MY_SEM_H
#define MY_SEM_H

#ifndef SYSTEM_WIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>

#include "myDef.h"

#define MAX_BUF 1024
#define MAX_TRIES 100

union semun {
        int              val;    /* Value for SETVAL */
        struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
        unsigned short *array; /* Array for GETALL, SETALL */
        struct seminfo *__buf;
};

using namespace std;

class TSem {

	public:
		TSem();
		~TSem();

		bool sem_create( key_t key,int total,int total_array[] );
		bool sem_get( key_t key );
		bool sem_lock(int no,int count);
		bool sem_try_lock(int no,int count);
		bool sem_unlock(int no,int count);
		bool sem_delete();

		int get_sem_count(int no);
		int get_sem_total_count();
		bool print_info();
		int get_errno() { return m_errno; }
		const char* get_err_msg();
		int get_key() { return m_key; }
		int get_semid() { return m_semid; }

	private:
		bool set_op(int op,int no,int flag);
		time_t get_sem_otime();

	private:
		key_t	m_key;
		int m_semid;

		struct sembuf *m_ptr;
		
		int m_maxLimit;
		int m_errno;
		int m_LINE;
		char m_FILE[MAX_BUF];
		char m_errmsg[MAX_BUF];
};

#endif

#endif
