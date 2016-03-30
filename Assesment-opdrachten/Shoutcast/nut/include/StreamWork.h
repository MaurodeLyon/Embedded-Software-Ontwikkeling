#ifndef _StreamWork_H
#define _StreamWork_H
#include <dev/debug.h>
#ifdef ETHERNUT2
#include <dev/lanc111.h>
#else
#include <dev/nicrtl.h>
#endif
#include <dev/vs1001k.h>
//#include "lcdA.h"
#include <sys/version.h>
#include <sys/confnet.h>
#include <sys/heap.h>
#include <sys/bankmem.h>
#include <sys/thread.h>
#include <sys/timer.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <netinet/tcp.h>

#include <pro/dhcp.h>

int stopAlarm;

void stopStream(void);
int ConfigureLan(char *devname);
FILE* ConnectStation(TCPSOCKET *sock, u_long ip, u_short port, u_long *metaint);
int ProcessMetaData(FILE *stream);
void PlayMp3Stream(FILE *stream, u_long metaint);
void startStream(void);

#endif
