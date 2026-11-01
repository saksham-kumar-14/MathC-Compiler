#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"

ttentry* scaled(ttentry* index, char* type){
    typecast(index, "int");
    ttentry* off = make_temp("int");
    quadStore("imul", index->name, widthstr(type), off->name);
    return off;
}

ttentry* arrayref(char* a, ttentry* index){
    int i = stsearch(a);
    if(i == -1 || strcmp(getType(i), "arr")){
        return make_rep("0", "int");
    }
    char* type = getEType(i);
    ttentry* off = scaled(index, type);
    ttentry* res = make_temp(type);
    char op[100];
    sprintf(op, "%cgetarr", type[0]);
    quadStore(op, a, off->name, res->name);
    return res;
}

list* arrassign(char* a, ttentry* index, ttentry* rep){
    int i = stsearch(a);
    char* type;
    if(i == -1 || strcmp(getType(i), "arr")){
        type = rep->type;
    }else{
        type = getEType(i);
    }

    typecast(index, "int");
    typecast(rep, type);
    ttentry* off = scaled(index, type);

    char op[100];
    sprintf(op, "%cstrarr", type[0]);
    quadStore(op, rep->name, off->name, a);

    if(i == -1) i = stsetlval(a);
    stsetarr(i, type);
    return NULL;
}

list* arrToassignment(char* d, char* s, struct ttentry* first, struct ttentry* last){
    return arrToassignment2(d, s, first, last, NULL);
}

list* arrToassignment2(char* d, char* s, struct ttentry* first, struct ttentry* last, struct ttentry* offset){
    int si = stsearch(s);
    if(si == -1 || strcmp(getType(si), "arr")){
        fprintf(stderr, "*** Error: %s is not an array, cannot copy from it\n", s);
        return NULL;
    }

    int di = stsearch(d);
    char* stype = getEType(si);
    char* dtype;
    if(di == -1 || strcmp(getType(di), "arr")){
        dtype = stype;
    }else{
        dtype = getEType(di);
    }
    if(di == -1) di = stsetlval(d);
    stsetarr(di, dtype);

    char* dwidth = widthstr(dtype);
    char* swidth = widthstr(stype);

    ttentry* doff;
    if(offset == NULL){
        doff = make_temp("int");
        quadStore("iset", "0", "", doff->name);
    }else{
        doff = scaled(offset, dtype);
    }
    char* doffset = doff->name;

    ttentry* soff = scaled(first, stype);
    char* soffset = soff->name;

    ttentry* sl = scaled(last, stype);
    char* slast = sl->name;

    char op[100];
    char val[100];

    ////////////////////// main loop
    int nquad = getnquad();
    int jmp1val = nquad;
    quadStore("jmpgt", soffset, slast, "");

    ttentry* sel = make_temp(stype);
    char* selement = sel->name;
    sprintf(op, "%cgetarr", stype[0]);
    quadStore(op, s, soffset, selement);

    // stype != dtype
    if(strcmp(stype, dtype)){
        ttentry* tmp = make_temp(dtype);
        sprintf(op, "%s2%s", stype, dtype);
        quadStore(op, selement, "", tmp->name);
        selement = tmp->name;
    }

    // D[Doffset] = Selement
    sprintf(op, "%cstrarr", dtype[0]);
    quadStore(op, selement, doffset, d);

    // Doffset = Doffset + dwidth
    quadStore("iadd", doffset, dwidth, doffset);

    // Soffset = Soffset + swidth
    quadStore("iadd", soffset, swidth, soffset);

    // goto begin
    sprintf(val, "%d", jmp1val);
    quadStore("jmp", "", "", val);

    // backpatch
    sprintf(val, "%d", getnquad());
    setQuad(jmp1val, val);
    return NULL;
}
