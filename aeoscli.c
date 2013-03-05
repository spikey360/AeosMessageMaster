#include "aeoscli.h"

int aeoserr=0x0000;
uint8_t aeosuuid[]={0xf5,0x98,0xa3,0xb4,0x4b,0x3a,0x89,0x5f,0xf5,0x98,0xa3,0xb4,0x4b,0x3a,0x89,0x5f};
int sock;

void printAddress(bdaddr_t* ad){
char x[32];
ba2str(ad,x);
printf("%s",x);
}

int getRemoteAddresses(bdaddr_t* add, int* sz){
 //bdaddr_t* devs=(bdaddr_t*)NULL;
 inquiry_info *inq;
 int devid, sock, len, flags;
 int maxdev, devfound=0;
 int i,x;
 char addr[32];
 //printf("getting route\n");
 if((devid=hci_get_route(NULL))<OK){
  fprintf(stderr,"Error fetching device\n");
  aeoserr=ERROR_FETCHDEVICE;
  return ERROR; //error condition: null returned
 }
 //printf("getting device\n");
 if((sock=hci_open_dev(devid))<OK){
  fprintf(stderr,"Error opening device\n");
  aeoserr=ERROR_OPENDEVICE;
  return ERROR;
 }
 len=8;
 maxdev=255;
 flags=IREQ_CACHE_FLUSH;
 inq=(inquiry_info*)malloc(maxdev*sizeof(inquiry_info));
 //printf("inquiring\n");
 if((devfound=hci_inquiry(devid,len,maxdev,NULL,&inq,flags))<OK){
 fprintf(stderr,"Error inquiring\n");
 aeoserr=ERROR_INQUIRYERR;
 return ERROR;
 }
 //printf("found %d\n",devfound);
 //devs=(bdaddr_t*)malloc(devfound*sizeof(bdaddr_t));
 x=devfound;
 if(*sz<devfound){
  x=*sz;
 }
 *sz=x;
 for(i=0;i<x;i++){
  add[i]=((inq+i)->bdaddr);
  //ba2str(&add[i],addr);
  //printAddress(&add[i]);
  //printf("\n");
  //printf("%s\n",addr);
 }
 aeoserr=OK;
 free(inq);
 close(sock);
 return OK;
} //return an array of devices in the vicinity

int isRunningServer(uint8_t* servid, bdaddr_t* device, int *channel){
uuid_t sdpuuid;
sdp_list_t *responseList=NULL, *attridList, *searchList;
//bdaddr_t target;
sdp_session_t *session;
int err;
session=sdp_connect(BDADDR_ANY, device, SDP_RETRY_IF_BUSY);
if(!session){
fprintf(stderr,"Device connection could not be established\n");
aeoserr=ERROR_DEVNTFND;
return ERROR;
}
sdp_uuid128_create(&sdpuuid, &aeosuuid);
searchList=sdp_list_append(NULL, &sdpuuid);
uint32_t range=0x0000FFFF;
attridList=sdp_list_append(NULL,&range);
//uint16_t maxServices=0x000F;
err=sdp_service_search_attr_req(session,searchList,SDP_ATTR_REQ_RANGE,attridList,&responseList);
//err=sdp_service_search_req(session,searchList,maxServices,&responseList);
//printf("%s\n",(err==0)?"search completed":"search failed");
if(err!=0){
aeoserr=ERROR_SEARCHFAIL;
return ERROR;
}
for(;responseList;responseList=responseList->next){
 sdp_record_t *rec=(sdp_record_t*)responseList->data;
 sdp_list_t* protoList;
// pcount++;
 if(sdp_get_access_protos(rec,&protoList)==0){
sdp_list_t *protos=protoList;
for(;protos;protos=protos->next){
sdp_list_t *pds=(sdp_list_t*)protos->data;
for(;pds;pds=pds->next){
sdp_data_t *d=(sdp_data_t*)pds->data;
int proto=0;
for(;d;d=d->next){
switch(d->dtd){
case SDP_UUID16:
case SDP_UUID32:
case SDP_UUID128:
	proto=sdp_uuid_to_proto(&d->val.uuid);
	break;
case SDP_UINT8:
	if(proto==RFCOMM_UUID){
		//printf("rfcomm channel: %d\n",d->val.int8);
		*channel=d->val.int8;
	}
	break;
}
}
}
sdp_list_free((sdp_list_t*)protos->data,0);
}
sdp_list_free(protoList,0);
}
//printf("service record 0x%x\n",rec->handle);
}
//printf("%d protocols found\n",pcount);
return 0;
}

int getConnectedSocket(bdaddr_t device, int channel){
 int sock;
 struct sockaddr_rc servaddr={0};
 servaddr.rc_family=AF_BLUETOOTH;
 servaddr.rc_channel=(uint8_t)channel;
 servaddr.rc_bdaddr=device;
 if((sock=socket(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM))<OK){
 fprintf(stderr,"Error creating socket\n");
 aeoserr=ERROR_SOCK;
 return ERROR;
 }
 if(connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr))<OK){
 fprintf(stderr,"Error connecting\n");
 aeoserr=ERROR_CONN;
 }
 aeoserr=OK;
 return sock;
 //don't close socket
}

int destroy(int x){
if(x){
close(x);
}
}

int sendMessage(int* sock, const char *num, const char *msg){
int w;
int r=strlen(msg);
w=write(*sock,num,strlen(num));
w+=write(*sock," ",1);
w+=write(*sock,msg,r);
w+=write(*sock,"\0",1);
if(w<OK){
aeoserr=ERROR_WRITE;
return ERROR;
}
aeoserr=OK;
return w;
}
