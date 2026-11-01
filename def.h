#define MAXENT 100005

typedef struct _stentry {
   char* name;
   char* type;
   char* etype;
} stentry;

typedef struct ttentry{
    char* name;
    char* type;
} ttentry;


typedef struct quadentry{
    int idx;
    char* op;
    char* arg1;
    char* arg2;
    char* result;
} quadentry;

typedef struct list{
    int instr;
    struct list* next;
} list;

typedef struct bool_attr{
    list* truelist;
    list* falselist;
} bool_attr;

typedef struct for_attr{
    char* var;
    char* type;
    int up;
    int lastlabel;
    int steplabel;
    list* truelist;
    list* falselist;
} for_attr;

extern stentry ST[MAXENT];
extern int nstentry;
extern ttentry TT[MAXENT];
extern int nttentry;
extern quadentry QT[MAXENT];
extern int nquad;


int stsearch ( char *v );
int stsetlval ( char *v );
void stsetrval(int i, char* type);
void stsetarr(int i, char* etype);
char* getType(int i);
char* getEType(int i);
char* getName(int i);

int ttsetlval ();
void ttsetrval(int i, char* type);
char* ttgetType(int i);
char* ttgetname(int i);

ttentry* make_rep(char* name, char* type);
ttentry* make_const(char* text, char* type);
ttentry* make_temp(char* type);
ttentry* varrep(char* v);
int rank(char* type);
char* widthstr(char* type);

void typecast(ttentry* rep, char* type);
void convert(ttentry* rep1, ttentry* rep3);
ttentry* arith(ttentry* rep1, char* opname, ttentry* rep3);
ttentry* intrem(ttentry* rep1, ttentry* rep3);
ttentry* mkrational(ttentry* rep1, ttentry* rep3);
ttentry* mkcomplex(ttentry* rep1, ttentry* rep3);
list* assign(char* v, ttentry* rep);

ttentry* scaled(ttentry* index, char* type);
ttentry* arrayref(char* a, ttentry* index);
list* arrassign(char* a, ttentry* index, ttentry* rep);
list* arrToassignment(char* d, char* s, struct ttentry* first, struct ttentry* end);
list* arrToassignment2(char* d, char* s, struct ttentry* first, struct ttentry* end, struct ttentry* offset);

bool_attr* relexpr(ttentry* rep1, char* rel, ttentry* rep3);
bool_attr* boolor(bool_attr* b1, int m, bool_attr* b2);
bool_attr* booland(bool_attr* b1, int m, bool_attr* b2);
bool_attr* boolnot(bool_attr* b);
list* ifthen(bool_attr* b, int m, list* body);
list* ifelse(bool_attr* b, int m1, list* body1, list* n, int m2, list* body2);
list* nlist();
list* emptylist();
list* joinlist(list* l, int m, list* s);
list* endblock(list* l);
void endprog(list* l);

for_attr* forinit(char* v, ttentry* start);
for_attr* forcond(for_attr* f, int lastlabel, ttentry* last, int up);
for_attr* forstep(for_attr* f, int steplabel, ttentry* amt);
list* forend(for_attr* f, int bodylabel, list* body);

void quadStore(char* op, char* arg1, char* arg2, char* result);
void quadPrint(int idx);
void quadPrintAll();
void setQuad(int label, char* result);
int getnquad();
void increasequad();


list* makelist(int instr);
list* merge(list* l1, list* l2);
void backpatch(list* l, int instr);
