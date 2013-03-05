#include "conlist.h"
#define STATE_NAME 1
#define STATE_NUM  2


FILE* fhand;

int readListFile(char *file, Contact* l, int* sz){
Contact con;
int rd; int st; int c=0; int i=0;
fhand=fopen(file,"r");
if(!fhand) return -1;
//start reading
st=STATE_NAME;
while((rd=getc(fhand))!=EOF){
 if(rd=='#'){while(rd!='\n'){rd=getc(fhand);}}
 if(rd==','){st=STATE_NUM; con.name[c]=0; c=0; continue;}
 if(rd=='\n'){
 st=STATE_NAME;
 con.num[c]=0;
 c=0;
 if(i<*sz){
  l[i++]=con;
 }
 continue;
 }
 if(st==STATE_NAME){
  con.name[c++]=rd;
 }
 if(st==STATE_NUM){
  con.num[c++]=rd;
 }
}
//file has been read
if(i<*sz){*sz=i;}
//nothing more to do
return 0;
}

int closeListFile(){
return fclose(fhand);
}
