#include "aeoscli.h"
#define NUMDEV 5

int main(int argc, char **argv){
char addr[24];
char num[16];
char msg[BUFFSIZE];
int sock, res, numdev=NUMDEV, i, ch[NUMDEV]={31}, target=0, isdevselected=0, valdevs=0;
bdaddr_t *devs;
char comm=0; //8-byte buffer for commands
devs=(bdaddr_t*)malloc(NUMDEV*sizeof(bdaddr_t));
int valDevs[NUMDEV]={-1};
printf("Getting remote addresses\n");
if((res=getRemoteAddresses(devs,&numdev))==ERROR){
printf("error%x\n",aeoserr);
return -1;
}
if(numdev==0){
printf("No devices found\n");
return -1;
}
for(i=0;i<numdev;i++){
ba2str(&devs[i],addr);
printf("%d:%s ",i,addr);
res=isRunningServer(aeosuuid,&devs[i],&ch[i]);
if(res==OK && ch[i]<=30){
printf("; channel=%d",ch[i]);
valDevs[i]=1;
valdevs++;
}
printf("\n");
}
if(valdevs<1){
printf("No device running server to connect to\n");
return -1;
}
if(valdevs==1){
for(i=0;i<numdev;i++){if(valDevs[i]==1){target=i;}}
sock=getConnectedSocket(devs[target],ch[target]);
isdevselected=1; //flag
//target 0 by default
}else{
while(isdevselected==0){
printf("Device?<");
for(i=0;i<numdev;i++){
 if(valDevs[i]==1){printf("%d/",i);}
 }
printf(" >: ");
scanf("%d%*c",&target);
if(valDevs[target]==1){isdevselected=1; sock=getConnectedSocket(devs[target],ch[target]);}
else{printf("Incorrect device chosen\n");}

}
}
printf("Selected dev%d\n",target);
//menu
printf("m(ultiple) <number>\ne(x)it\n");
while(comm!='x'){
//menu loop
printf(": ");
scanf("%c",&comm);
switch(comm){
case 'm':
 printf("Number? ");
 scanf("%s",num);
 printf("%s\n",num);
 num[strlen(num)]=0; //proper termination
 while(1){
 res=read(fileno(stdin),msg,BUFFSIZE);
 msg[res]=0; //proper termination
 if(msg[0]=='`') break;
 res=sendMessage(&sock,num,msg);
 if(res<OK){printf("Error sending message\n");}
 }
 scanf("%*c");
 break;
case 'x':
 printf("Closing..\n");
 break;
default:
 printf("Invalid command\n");
 break;
}
}
destroy(sock);
return 0;
}
