#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"

quadentry QT[MAXENT];
int nquad = 0;

void quadStore(char* op, char* arg1, char* arg2, char* result){
    QT[nquad].idx = 1000 + nquad;
    QT[nquad].op = strdup(op);
    QT[nquad].arg1 = strdup(arg1);
    QT[nquad].arg2 = strdup(arg2);
    QT[nquad].result = strdup(result);
    ++nquad;
}

void quadPrint(int i){
    printf("%d:   %-16s%-20s%-20s%s\n", 1000+i, QT[i].op, QT[i].arg1, QT[i].arg2, QT[i].result);
}

void quadPrintAll(){
    printf("        %-16s%-20s%-20s%s\n", "Operation", "Argument 1", "Argument 2", "Result");
    for(int i = 0; i < nquad; ++i){
        quadPrint(i);
    }
    printf("%d:\n", 1000+nquad);
}

void setQuad(int label, char* result){
    QT[label-1000].result = strdup(result);
}

int getnquad(){return 1000+nquad;}
void increasequad(){++nquad;}
