#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"

extern FILE *yyin;
extern int yyparse();

ttentry* make_rep(char* name, char* type){
    ttentry* n = (struct ttentry*)malloc(sizeof(struct ttentry));
    n->type = type;
    n->name = name;
    return n;
}

ttentry* make_const(char* text, char* type){
    char buf[100];
    if(!strcmp(type, "flt")){
        double v = atof(text);
        sprintf(buf, "%f", v);
        if(v != 0 && atof(buf) == 0) sprintf(buf, "%e", v);
    }else{
        if(text[0] == '+') ++text;
        strcpy(buf, text);
    }
    return make_rep(strdup(buf), type);
}

ttentry* make_temp(char* type){
    int ti = ttsetlval();
    ttsetrval(ti, type);
    return make_rep(ttgetname(ti), type);
}

ttentry* varrep(char* v){
    int i = stsearch(v);
    if(i == -1 || !strcmp(getType(i), "arr")){
        return make_rep("0", "int");
    }
    return make_rep(getName(i), getType(i));
}

int rank(char* type){
    if(!strcmp(type, "int")) return 0;
    if(!strcmp(type, "rtl")) return 1;
    if(!strcmp(type, "flt")) return 2;
    return 3;
}

char* widthstr(char* type){
    if(!strcmp(type, "int")) return "4";
    if(!strcmp(type, "cpx")) return "16";
    return "8";
}

void typecast(ttentry* rep, char* type){
    if(!strcmp(rep->type, type)) return;
    char op[200];
    sprintf(op, "%s2%s", rep->type, type);
    ttentry* t = make_temp(type);
    quadStore(op, rep->name, "", t->name);
    rep->name = t->name;
    rep->type = type;
}

void convert(ttentry* rep1, ttentry* rep3){
    if(rank(rep1->type) < rank(rep3->type)){
        typecast(rep1, rep3->type);
    }else if(rank(rep3->type) < rank(rep1->type)){
        typecast(rep3, rep1->type);
    }
}

ttentry* arith(ttentry* rep1, char* opname, ttentry* rep3){
    convert(rep1, rep3);
    ttentry* t = make_temp(rep1->type);
    char op[200];
    sprintf(op, "%c%s", rep1->type[0], opname);
    quadStore(op, rep1->name, rep3->name, t->name);
    return t;
}

ttentry* intrem(ttentry* rep1, ttentry* rep3){
    typecast(rep1, "int");
    typecast(rep3, "int");
    ttentry* t = make_temp("int");
    quadStore("irem", rep1->name, rep3->name, t->name);
    return t;
}

ttentry* mkrational(ttentry* rep1, ttentry* rep3){
    typecast(rep1, "int");
    typecast(rep3, "int");
    ttentry* t = make_temp("rtl");
    quadStore("mkrtl", rep1->name, rep3->name, t->name);
    return t;
}

ttentry* mkcomplex(ttentry* rep1, ttentry* rep3){
    typecast(rep1, "flt");
    typecast(rep3, "flt");
    ttentry* t = make_temp("cpx");
    quadStore("mkcpx", rep1->name, rep3->name, t->name);
    return t;
}

list* assign(char* v, ttentry* rep){
    int i = stsetlval(v);
    stsetrval(i, rep->type);
    char op[200];
    sprintf(op, "%cstr", rep->type[0]);
    quadStore(op, rep->name, "", getName(i));
    return NULL;
}

bool_attr* relexpr(ttentry* rep1, char* rel, ttentry* rep3){
    convert(rep1, rep3);
    bool_attr* b = (struct bool_attr*)malloc(sizeof(struct bool_attr));
    char op[100];
    sprintf(op, "jmp%c%s", rep1->type[0], rel);
    b->truelist = makelist(getnquad());
    quadStore(op, rep1->name, rep3->name, "");
    b->falselist = makelist(getnquad());
    quadStore("jmp", "", "", "");
    return b;
}

bool_attr* boolor(bool_attr* b1, int m, bool_attr* b2){
    backpatch(b1->falselist, m);
    bool_attr* b = (struct bool_attr*)malloc(sizeof(struct bool_attr));
    b->truelist = merge(b1->truelist, b2->truelist);
    b->falselist = b2->falselist;
    return b;
}

bool_attr* booland(bool_attr* b1, int m, bool_attr* b2){
    backpatch(b1->truelist, m);
    bool_attr* b = (struct bool_attr*)malloc(sizeof(struct bool_attr));
    b->truelist = b2->truelist;
    b->falselist = merge(b1->falselist, b2->falselist);
    return b;
}

bool_attr* boolnot(bool_attr* b1){
    bool_attr* b = (struct bool_attr*)malloc(sizeof(struct bool_attr));
    b->truelist = b1->falselist;
    b->falselist = b1->truelist;
    return b;
}

list* ifthen(bool_attr* b, int m, list* body){
    backpatch(b->truelist, m);
    return merge(b->falselist, body);
}

list* ifelse(bool_attr* b, int m1, list* body1, list* n, int m2, list* body2){
    backpatch(b->truelist, m1);
    backpatch(b->falselist, m2);
    return merge(merge(body1, n), body2);
}

list* nlist(){
    list* l = makelist(getnquad());
    quadStore("jmp", "", "", "");
    return l;
}

list* emptylist(){
    return NULL;
}

list* joinlist(list* l, int m, list* s){
    backpatch(l, m);
    return s;
}

list* endblock(list* l){
    backpatch(l, getnquad());
    return NULL;
}

void endprog(list* l){
    backpatch(l, getnquad());
}

for_attr* forinit(char* v, ttentry* start){
    for_attr* f = (struct for_attr*)malloc(sizeof(struct for_attr));
    f->var = strdup(v);
    f->type = start->type;
    assign(v, start);
    return f;
}

for_attr* forcond(for_attr* f, int lastlabel, ttentry* last, int up){
    f->up = up;
    f->lastlabel = lastlabel;
    typecast(last, f->type);
    char op[100];
    sprintf(op, "jmp%c%s", f->type[0], up ? "le" : "ge");
    f->truelist = makelist(getnquad());
    quadStore(op, f->var, last->name, "");
    f->falselist = makelist(getnquad());
    quadStore("jmp", "", "", "");
    return f;
}

for_attr* forstep(for_attr* f, int steplabel, ttentry* amt){
    f->steplabel = steplabel;
    typecast(amt, f->type);
    ttentry* t = make_temp(f->type);
    char op[100];
    char val[100];
    sprintf(op, "%c%s", f->type[0], f->up ? "add" : "sub");
    quadStore(op, f->var, amt->name, t->name);
    sprintf(op, "%cstr", f->type[0]);
    quadStore(op, t->name, "", f->var);
    sprintf(val, "%d", f->lastlabel);
    quadStore("jmp", "", "", val);
    return f;
}

list* forend(for_attr* f, int bodylabel, list* body){
    char val[100];
    backpatch(f->truelist, bodylabel);
    backpatch(body, getnquad());
    sprintf(val, "%d", f->steplabel);
    quadStore("jmp", "", "", val);
    return f->falselist;
}

int main ( int argc, char *argv[] )
{
    if (argc > 1) {
        yyin = fopen(argv[1],"r");
        if (yyin == NULL) {
            fprintf(stderr, "*** Error: cannot open %s\n", argv[1]);
            exit(1);
        }
    }
    if (yyparse()) exit(1);
    quadPrintAll();
    exit(0);
}
