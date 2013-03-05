#ifndef CONLIST_H
#define CONLIST_H

#include <stdio.h>
#define MAXCONTACTS 128

typedef struct con{
char name[32];
char num[16];
}Contact;

int readListFile(char* x,Contact* list, int* size);
//int getContacts(Contact* list int* size);
int closeListFile();

#endif
