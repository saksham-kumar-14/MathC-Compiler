#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"

list* makelist(int instr){
    list* l = (struct list*)malloc(sizeof(struct list));
    l->instr = instr;
    l->next = NULL;
    return l;
}

list* merge(list* l1, list* l2){
    if(l1 == NULL) return l2;
    if(l2 == NULL) return l1;
    list* head = l1;
    while(head->next != NULL) head = head->next;
    head->next = l2;
    return l1;
}

void backpatch(list* l, int result){
    list* head = l;
    char sresult[16];
    sprintf(sresult,"%d",result);
    while(head != NULL){
        setQuad(head->instr, sresult);
        head = head->next;
    }
}
