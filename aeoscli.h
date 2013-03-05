#ifndef AEOSCLI_H
#define AEOSCLI_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>

#define OK 0
#define ERROR -1
#define BUFFSIZE 1024
#define ERROR_FETCHDEVICE -0x0001
#define ERROR_OPENDEVICE  -0x0002
#define ERROR_INQUIRYERR  -0x0004
#define ERROR_SCONN       -0x0008
#define ERROR_SEARCHFAIL  -0x0016
#define ERROR_DEVNTFND	  -0x0032
#define ERROR_SOCK	  -0x0064
#define ERROR_CONN	  -0x0128
#define ERROR_WRITE	  -0x0256

extern uint8_t aeosuuid[];
extern int aeoserr;

char* getErrorDesc(int);
int getRemoteAddresses(bdaddr_t* add, int* sz); //return an array of devices in the vicinity
int isRunningServer(uint8_t*, bdaddr_t* device, int *channel); //return 0 if running -1 if not running
int getConnectedSocket(bdaddr_t device, int channel); //returns the number to the connected socket
int sendMessage(int* sock, const char *num, const char *msg); //returns the total no of bytes written to the socket
int destroy(int sock);
#endif
