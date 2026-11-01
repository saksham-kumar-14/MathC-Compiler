#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"

stentry ST[MAXENT];
int nstentry = 0;

ttentry TT[MAXENT];
int nttentry = 1;

int stsearch ( char *v )
{
   int i;
   for (i=0; i<nstentry; ++i)
      if(!strcmp(v, ST[i].name)) return i;
   return -1;
}

int stsetlval ( char *v )
{
   int i;

   for (i=0; i<nstentry; ++i)
      if (!strcmp(v,ST[i].name)) return i;
   ST[nstentry].name = strdup(v);
   ST[nstentry].type = NULL;
   ST[nstentry].etype = NULL;
   ++nstentry;
   return i;
}

void stsetrval(int i, char* type){
    ST[i].type = type;
    ST[i].etype = NULL;
}

void stsetarr(int i, char* etype){
    ST[i].type = "arr";
    ST[i].etype = etype;
}

char* getType(int i){
    return ST[i].type;
}

char* getEType(int i){
    return ST[i].etype;
}

char* getName(int i){
    return ST[i].name;
}

int ttsetlval ()
{
    int i = nttentry;
    ++nttentry;
    char name[100];
    sprintf(name, "t%d", i);
    TT[i].name = strdup(name);
    TT[i].type = NULL;
    return i;
}

void ttsetrval(int i, char* type){
    TT[i].type = type;
}

char* ttgetType(int i){
    return TT[i].type;
}

char* ttgetname(int i){
    return TT[i].name;
}
