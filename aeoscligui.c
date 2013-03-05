#include <gtk/gtk.h>
#include <pthread.h>
#include "aeoscli.h"
#include "conlist.h"
#define MAXCONTACT 256
#define NUMDEV 5

GtkWidget* wind;
GtkWidget* phnum;
GtkWidget* phNumCombo;
GtkWidget* text;
GtkTextBuffer* textBuffer;
GtkWidget* sendButton;
Contact *conlist;
int sock,sz;

typedef struct ms{
char *num;
char *msg;
} Message;

static void destroyWindow(GtkWidget* w, gpointer data){
//to add many things, close socket, streams etc
gtk_main_quit();
destroy(sock);
}

void* sendThread(void* args);
void* initializeClient(void* args);

static void sendText(GtkWidget* w, gpointer data){
GtkTextIter start, end;  Message *m;
char *a, *b; int res; int act;
pthread_t tid;
m=(Message*)malloc(sizeof(Message));
//all sending routines go here
act=gtk_combo_box_get_active(GTK_COMBO_BOX(phNumCombo));
//m->num=(char*)gtk_entry_get_text(GTK_ENTRY(phnum));
m->num=&conlist[act].num;
gtk_text_buffer_get_bounds(textBuffer,&start,&end);
m->msg=(char*)gtk_text_buffer_get_text(textBuffer,&start,&end,0);
res=pthread_create(&tid,NULL,sendThread,(void*)m);
if(res<0){
printf("Error creating thread\n");
}else{
//gtk_text_buffer_set_text(textBuffer," \b",2);
gtk_text_buffer_delete(textBuffer,&start,&end);
}
}

int main(int argc, char **argv){
GtkWidget *box;
GtkWidget *scroll;
sz=MAXCONTACT; int i;
conlist=(Contact*)malloc(MAXCONTACT*sizeof(Contact));
//Load contact file
if(argc==2)
 readListFile(argv[1],conlist,&sz);
else
 readListFile("cons.cn",conlist,&sz);
closeListFile();
//initializing components
gtk_init(&argc,&argv);
initializeClient(NULL);
wind=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_title(GTK_WINDOW(wind),"Aeos Client");
phnum=gtk_entry_new();

phNumCombo=gtk_combo_box_text_new();
for(i=0;i<sz;i++){
gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(phNumCombo),i,strcat(&conlist[i].name,&conlist[i].num));
//gtk_combo_box_insert_text(GTK_COMBO_BOX(phNumCombo),1,"World");
}
gtk_combo_box_set_active(GTK_COMBO_BOX(phNumCombo),0);

text=gtk_text_view_new();
textBuffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text),GTK_WRAP_WORD);
scroll=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll),text);
//gtk_text_view_set_
//gtk_text_set_editable(GTK_TEXT(text),1);
sendButton=gtk_button_new_with_mnemonic("_Send");
//adding event listeners
g_signal_connect(wind,"destroy",G_CALLBACK(destroyWindow),NULL);
g_signal_connect(sendButton,"clicked",G_CALLBACK(sendText),NULL);
//adding to container and making visible
box=gtk_vbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(box),phNumCombo,TRUE,TRUE,0);
gtk_widget_show(phnum);
gtk_box_pack_start(GTK_BOX(box),scroll,TRUE,TRUE,0);
gtk_widget_show(scroll);
gtk_box_pack_start(GTK_BOX(box),sendButton,TRUE,TRUE,0);
gtk_widget_show(sendButton);

gtk_widget_show(box);
gtk_container_add(GTK_CONTAINER(wind),box);
gtk_widget_show_all(wind);
//main
gtk_main();
return 0;
}

void* sendThread(void* args){
int res;
Message* m=(Message*)args;
g_print("%s ",m->num);
g_print("%s",m->msg);
m->msg[strlen(m->msg)]=0;
///send message to server
res=sendMessage(&sock,m->num,m->msg);
if(res<0){g_print("Error sending");} //ideally an error dialog
////
free(m);
return NULL;
}

void* initializeClient(void* args){
GtkWidget* dia;
char addr[24];
int res, numdev=NUMDEV, i, ch[NUMDEV], target=0, isdevselected=0, valdevs=0, t;
bdaddr_t *devs;
devs=(bdaddr_t*)malloc(NUMDEV*sizeof(bdaddr_t));
bzero(ch,sizeof(int)*NUMDEV);
dia=gtk_dialog_new();
int valDevs[NUMDEV]={-1};

//g_signal_connect_swapped (dia,"response",G_CALLBACK(gtk_widget_destroy),dia);

g_print("Getting remote addresses\n");
if((res=getRemoteAddresses(devs,&numdev))==ERROR){
printf("error%x\n",aeoserr);
return NULL;
}
if(numdev==0){
g_print("No devices found\n");
return NULL;
}
for(i=0;i<numdev;i++){
ba2str(&devs[i],addr);
g_print("%d - %s ",i,addr);
res=isRunningServer(aeosuuid,&devs[i],&ch[i]);
if(res==OK && ch[i]<=30 && ch[i]>0){
g_print("; channel=%d",ch[i]);
valDevs[i]=1;
valdevs++;
}
g_print("\n");
}
if(valdevs<1){
g_print("No device running server to connect to\n");//announce through dialog
//don't launch gui
return NULL;
}
if(valdevs==1){
//one valid device among 1 device(s)
for(i=0;i<numdev;i++){if(valDevs[i]==1){target=i;}}
sock=getConnectedSocket(devs[target],ch[target]);
isdevselected=1; //flag
//target 0 by default
}else{
//if valid device more than 1
for(i=0;i<numdev;i++){
 if(valDevs[i]==1){//printf("%d/",i);
 ba2str(&devs[i],addr);
 gtk_dialog_add_button(GTK_DIALOG(dia),addr,i);
 }
 }
target=gtk_dialog_run(GTK_DIALOG(dia));
gtk_dialog_response(GTK_DIALOG(dia),GTK_RESPONSE_CLOSE);
g_print("Device %d",t);
//scanf("%d%*c",&target);//get button response
if(valDevs[target]==1){isdevselected=1; sock=getConnectedSocket(devs[target],ch[target]);}
else{printf("Incorrect device chosen\n");}

}
//copied
return NULL;
}
