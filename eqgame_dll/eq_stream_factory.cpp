#include "global_define.h"
#include "eqemu_logsys.h"
#include "eq_stream_factory.h"

#ifdef _WINDOWS
	#include <winsock.h>
	#include <process.h>
	#include <io.h>
	#include <stdio.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/select.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <pthread.h>
#endif

#include <iostream>
#include <fcntl.h>

#include "op_codes.h"

#pragma warning( disable : 4244 4996 4267 )