//KARAVANGELI EFTYCHIA - 1115201800062

#ifndef LIBRARIES_H_
#define LIBRARIES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <features.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
#include <arpa/inet.h>
#include <pthread.h>


#define TOTAL_ARGS_SERVER 9
#define TOTAL_ARGS_CLIENT 7
#define MAX_PATH_LENGTH 4096

extern int RUNNING;

void Print_Error(char*);
void Print_Error_Value(char*, int);
void* Server_Job(void*);


#endif