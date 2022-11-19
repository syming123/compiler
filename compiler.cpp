#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include<stack>
using namespace std;

/* 单词种类数 */
#define SYMBOL_SIZE 31
/* 最大标识符长度 */
#define MAX_IDENT_LENGTH 10
/* 最大TABLE大小 */
#define MAX_TABLE_SIZE 1000
/* 最大CODE大小 */
#define MAX_CODE_SIZE 1000
/* procedure最大嵌套层数 */
#define MAX_PROCEDURE_LAYER 3
/* 输入缓冲区大小 */
#define INPUT_BUF_SIZE 100
/* 错误类型数量 */
#define ERROR_NUMBER 10


/* 单词和单词表 */
enum SYMBOL{
    IDENT,NUMBER,
    CONSTSYM,VARSYM,PROCEDURESYM,BEGINSYM,ENDSYM,ODDSYM,
    IFSYM,THENSYM,CALLSYM,WHILESYM,DOSYM,READSYM,WRITESYM,
    ADD,SUB,MULTI,DIV,EQ,NEQ,LESS,GREAT,LESSEQ,GREATEQ,ASSIGN,
    COMMA,SEM,LEFT,RIGHT,DOT
};
map<string,SYMBOL> SYM_TABLE;

/* 符号表 */
struct TABLE_UNIT
{
    string name;
    SYMBOL kind;
    int level;
    int val;
    int adr;
    int size;
};
TABLE_UNIT TABLE[MAX_TABLE_SIZE];

/* 指令 */
enum INSTRUCT{
    LIT,LOD,STO,CAL,INT,JMP,JPC,OPR
};
struct INSTRUCTION
{
    INSTRUCT f;
    int l,a;
};
INSTRUCTION CODE[MAX_CODE_SIZE];

/* OPR运算类型 */
enum OPR_TYPE{
    OPR_EXIT,OPR_ADD,OPR_SUB,OPR_MULTI,OPR_DIV,OPR_READ,OPR_WRITE,OPR_WRITELN,
    OPR_EQ,OPR_NEQ,OPR_LESS,OPR_GREAT,OPR_LESSEQ,OPR_GREATEQ
};
map<SYMBOL,OPR_TYPE> SYM_TO_OPR;

/* 出错类型 */
string ERROR_TRPE[ERROR_NUMBER];

/* 全局变量 */
ifstream INFILE; //输入文件
int LL,CC; //行计数器和字符计数器
char LINE[INPUT_BUF_SIZE]; //读入缓冲区
char CH; //当前字符
bool END_FILE; //是否读取结束

SYMBOL SYM;
string ID;
int NUM;

int CX; //CODE表指针

int OPERATOR_WEIGNT[SYMBOL_SIZE]; //运算符权重

/* 函数 */
void INIT();
bool IS_LETTER();
bool IS_NUMBER();
void GETCH();
void GETSYM();
void ERROR(int);
void BOLCK(int,int);
void CONST_PROCESS(int*,int);
void VAR_PROCESS(int*,int,int);
void PROCEDURE_PROCESS(int*,int);
void STATEMENT(int*,int);
void GEN(INSTRUCT,int,int);
int FIND_IDENT(int*,string);
void EXPRESSION(int*,int);
void CONDITION(int*,int);

void INIT(){
    //初始化变量
    LL = 0;
    CC = 0;
    END_FILE = false;
    CX = 0;

    //初始化符号表
    SYM_TABLE["const"] = CONSTSYM;
    SYM_TABLE["var"] = VARSYM;
    SYM_TABLE["procedure"] = PROCEDURESYM;
    SYM_TABLE["begin"] = BEGINSYM;
    SYM_TABLE["end"] = ENDSYM;
    SYM_TABLE["odd"] = ODDSYM;
    SYM_TABLE["if"] = IFSYM;
    SYM_TABLE["then"] = THENSYM;
    SYM_TABLE["call"] = CALLSYM;
    SYM_TABLE["while"] = WHILESYM;
    SYM_TABLE["do"] = DOSYM;
    SYM_TABLE["read"] = READSYM;
    SYM_TABLE["write"] = WRITESYM;
    SYM_TABLE["+"] = ADD;
    SYM_TABLE["-"] = SUB;
    SYM_TABLE["*"] = MULTI;
    SYM_TABLE["/"] = DIV;
    SYM_TABLE["="] = EQ;
    SYM_TABLE["#"] = NEQ;
    SYM_TABLE["<"] = LESS;
    SYM_TABLE[">"] = GREAT;
    SYM_TABLE["<="] = LESSEQ;
    SYM_TABLE[">="] = GREATEQ;
    SYM_TABLE[":="] = ASSIGN;
    SYM_TABLE[","] = COMMA;
    SYM_TABLE[";"] = SEM;
    SYM_TABLE["("] = LEFT;
    SYM_TABLE[")"] = RIGHT;
    SYM_TABLE["."] = DOT;

    /* SYMBOL映射至OPR */
    SYM_TO_OPR[ADD] = OPR_ADD;
    SYM_TO_OPR[SUB] = OPR_SUB;
    SYM_TO_OPR[MULTI] = OPR_MULTI;
    SYM_TO_OPR[DIV] = OPR_DIV;
    SYM_TO_OPR[EQ] = OPR_EQ;
    SYM_TO_OPR[NEQ] = OPR_NEQ;
    SYM_TO_OPR[LESS] = OPR_LESS;
    SYM_TO_OPR[GREAT] = OPR_GREAT;
    SYM_TO_OPR[LESSEQ] = OPR_LESSEQ;
    SYM_TO_OPR[GREATEQ] = OPR_GREATEQ;

    /* 初始化错误类型 */
    ERROR_TRPE[0] = "Overflow Error";
    ERROR_TRPE[1] = "Symbol Error";
    ERROR_TRPE[2] = "Grammar Error";
    ERROR_TRPE[3] = "Ident Define Error";
    ERROR_TRPE[4] = "Read File Error";

    /* 赋予运算符权重 */
    OPERATOR_WEIGNT[ASSIGN] = 0;
    OPERATOR_WEIGNT[ADD] = 2;
    OPERATOR_WEIGNT[SUB] = 2;
    OPERATOR_WEIGNT[MULTI] = 3;
    OPERATOR_WEIGNT[DIV] = 3;
    OPERATOR_WEIGNT[LEFT] = 4;
    OPERATOR_WEIGNT[RIGHT] = 1;
}
bool IS_LETTER(char c){
    if(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
        return true;
    else return false;
}
bool IS_NUMBER(char c){
    if(c >= '0' && c <= '9') 
        return true;
    else return false;
}
void ERROR(int ERROR_ID){
    printf("Error in line %d, column %d: %s\n",LL,CC+1,ERROR_TRPE[ERROR_ID].c_str());
    printf("%d\n",SYM);
    exit(1);
}
void GETCH(){
    if(END_FILE){
        ERROR(4);
    }
    CC++;
    if(LL == 0 || CH == '\n'){
        INFILE.getline(LINE,INPUT_BUF_SIZE);
        LL++;
        CC = 0;
    }
    if(CC >= INPUT_BUF_SIZE || LINE[CC] == '\0'){
        if(INFILE.eof()){
            END_FILE = true;
        }
        CH = '\n';
        return;
    }
    CH = LINE[CC];
}
void GETSYM(){
    while(CH == ' ' || CH == '\n' || CH == '\t') GETCH();
    if(IS_LETTER(CH)){
        int k = 0;
        char word[MAX_IDENT_LENGTH+1];
        while(IS_LETTER(CH) || IS_NUMBER(CH)){
            if(k <= MAX_IDENT_LENGTH){
                word[k++] = CH;
                GETCH();
            }
            else{
                ERROR(0);
            }
        }
        word[k] = '\0';
        string kw = word;
        if(SYM_TABLE.count(kw) == 1){
            //保留字
            SYM = SYM_TABLE[kw];
            ID = "";
            NUM = 0;
        }
        else{
            //标识符
            SYM = IDENT;
            ID = kw;
            NUM = 0;
        }
    }
    else if(IS_NUMBER(CH)){
        //数字
        int num = 0;
        while(IS_NUMBER(CH)){
            num = num * 10 + CH - '0';
            GETCH();
        }
        SYM = NUMBER;
        ID = "";
        NUM = num;
        if(IS_LETTER(CH)){
            ERROR(1);
        }
    }
    else{
        //特殊字符
        char S[3];
        int k = 0;
        S[k++] = CH;
        GETCH();
        if(CH == '='){
            S[k++] = CH;
            GETCH();
        }
        S[k] = '\0';
        string s = S;
        if(SYM_TABLE.count(s) == 1){
            SYM = SYM_TABLE[s];
            ID = "";
            NUM = 0;
        }
        else{
            ERROR(1);
        }
    }
}
void BOLCK(int *TX,int LEV){
    if(LEV >= MAX_PROCEDURE_LAYER) ERROR(0);
    int DX = 3;
    if(SYM == CONSTSYM){
        GETSYM();
        CONST_PROCESS(TX,LEV);
        while(SYM == COMMA){
            GETSYM();
            CONST_PROCESS(TX,LEV);
        }
        if(SYM != SEM) ERROR(2);
        GETSYM();
    }
    if(SYM == VARSYM){
        GETSYM();
        VAR_PROCESS(TX,LEV,DX);
        DX++;
        while(SYM == COMMA){
            GETSYM();
            VAR_PROCESS(TX,LEV,DX);
            DX++;
        }
        if(SYM != SEM) ERROR(2);
        GETSYM();
    }
    while(SYM == PROCEDURESYM){
        GETSYM();
        PROCEDURE_PROCESS(TX,LEV);
    }
    GEN(INT,0,DX);
    STATEMENT(TX,LEV);
    GEN(OPR,0,0);
}
void CONST_PROCESS(int *TX, int LEV){
    if(SYM != IDENT) ERROR(2);
    TABLE[*TX].name = ID;
    TABLE[*TX].kind = CONSTSYM;
    TABLE[*TX].level = LEV;
    GETSYM();
    if(SYM != EQ) ERROR(2);
    GETSYM();
    if(SYM != NUMBER) ERROR(2);
    TABLE[*TX].val = NUM;
    GETSYM();
    (*TX)++;
}
void VAR_PROCESS(int *TX,int LEV,int DX){
    if(SYM != IDENT) ERROR(2);
    TABLE[*TX].name = ID;
    TABLE[*TX].kind = VARSYM;
    TABLE[*TX].level = LEV;
    TABLE[*TX].adr = DX;
    GETSYM();
    (*TX)++;
}
void PROCEDURE_PROCESS(int *TX,int LEV){
    if(SYM != IDENT) ERROR(2);
    TABLE[*TX].name = ID;
    TABLE[*TX].kind = PROCEDURESYM;
    TABLE[*TX].level = LEV;
    TABLE[*TX].adr = CX;
    GETSYM();
    if(SYM != SEM) ERROR(2);
    GETSYM();
    (*TX)++;
    BOLCK(TX,LEV+1);
}
void STATEMENT(int *TX,int LEV){
    if(SYM != BEGINSYM) ERROR(2);
    GETSYM();
    while(SYM != ENDSYM){
        if(SYM == IDENT){
            //表达式
            int idx = FIND_IDENT(TX,ID);
            if(idx == -1 || TABLE[idx].kind != VARSYM) ERROR(3);
            GETSYM();
            if(SYM != ASSIGN) ERROR(2);
            GETSYM();
            EXPRESSION(TX,LEV);
            GEN(STO,LEV-TABLE[idx].level,TABLE[idx].adr);
            if(SYM != SEM) ERROR(2);
            GETSYM();
        }
        else if(SYM == IFSYM){
            GETSYM();
            CONDITION(TX,LEV);
            if(SYM != THENSYM) ERROR(2);
            GETSYM();
            int jpc_cx = CX;
            GEN(JPC,0,0);
            STATEMENT(TX,LEV);
            CODE[jpc_cx].a = CX;
        }
        else if(SYM == WHILESYM){
            GETSYM();
            int jmp_cx = CX;
            CONDITION(TX,LEV);
            if(SYM != DOSYM) ERROR(2);
            GETSYM();
            int jpc_cx = CX;
            GEN(JPC,0,0);
            STATEMENT(TX,LEV);
            GEN(JMP,0,jmp_cx);
            CODE[jpc_cx].a = CX;
        }
        else if(SYM == READSYM){
            GETSYM();
            if(SYM != LEFT) ERROR(2);
            do{
                GETSYM();
                if(SYM != IDENT) ERROR(2);
                int idt = FIND_IDENT(TX,ID);
                if(idt == -1 || TABLE[idt].kind != VARSYM) ERROR(3);
                GEN(OPR,0,OPR_READ);
                GEN(STO,LEV-TABLE[idt].level,TABLE[idt].adr);
                GETSYM();
            }
            while(SYM == COMMA);
            if(SYM != RIGHT) ERROR(2);
            GETSYM();
            if(SYM != SEM) ERROR(2);
            GETSYM();
        }
        else if(SYM == WRITESYM){
            GETSYM();
            if(SYM != LEFT) ERROR(2);
            do{
                GETSYM();
                EXPRESSION(TX,LEV);
                GEN(OPR,0,OPR_WRITE);
                GEN(OPR,0,OPR_WRITELN);
            }
            while(SYM == COMMA);
            if(SYM != RIGHT) ERROR(2);
            GETSYM();
            if(SYM != SEM) ERROR(2);
            GETSYM();
        }
        else if(SYM == CALLSYM){
            GETSYM();
            if(SYM != IDENT) ERROR(2);
            int idt = FIND_IDENT(TX,ID);
            if(idt == -1 || TABLE[idt].kind != PROCEDURESYM) ERROR(3);
            GEN(CAL,LEV-TABLE[idt].level,TABLE[idt].adr);
            GETSYM();
            if(SYM != SEM) ERROR(2);
            GETSYM();
        }
        else if(SYM != ENDSYM){
            ERROR(2);
        }
    }
    GETSYM();
    if(SYM != SEM && SYM != DOT) ERROR(2);
    if(SYM == SEM) GETSYM();
    else printf("Progress Compiled\n");
}
void EXPRESSION(int* TX,int LEV){
    stack<SYMBOL> op_stack;
    op_stack.push(ASSIGN);
    if(SYM == SEM) ERROR(2);
    bool flag = true;
    while(flag){
        flag = false;

        // 防止write(expression)右括号被识别为表达式
        int bracket_num = 0;
        if(SYM == LEFT) bracket_num++;
        if(SYM == RIGHT){
            if(bracket_num == 0) break;
            bracket_num--;
        }

        if(SYM == IDENT){
            flag = true;
            int idx = FIND_IDENT(TX,ID);
            if(idx == -1) ERROR(3);
            if(TABLE[idx].kind == CONSTSYM){
                GEN(LIT,0,TABLE[idx].val);
            }
            else{
                GEN(LOD,LEV-TABLE[idx].level,TABLE[idx].adr);
            }
            GETSYM();
        }
        else if(SYM == NUMBER){
            flag = true;
            GEN(LIT,0,NUM);
            GETSYM();
        }
        else if(SYM == ADD || SYM == SUB || SYM == MULTI || SYM == DIV || SYM == LEFT || SYM == RIGHT){
            flag = true;
            SYMBOL tp = op_stack.top();
            while(OPERATOR_WEIGNT[tp] >= OPERATOR_WEIGNT[SYM] && SYM != LEFT){
                tp = op_stack.top();
                if(tp == LEFT && SYM != RIGHT) break;
                op_stack.pop();
                if(tp == LEFT && SYM == RIGHT) break;
                GEN(OPR,0,SYM_TO_OPR[tp]);
            }
            if(SYM != RIGHT) op_stack.push(SYM);
            GETSYM();
        }
    }
    while(op_stack.top() != ASSIGN){
        SYMBOL tp = op_stack.top();
        op_stack.pop();
        GEN(OPR,0,SYM_TO_OPR[tp]);
    }
}
void CONDITION(int *TX,int LEV){
    EXPRESSION(TX,LEV);
    if(SYM != EQ && SYM != NEQ && SYM != LESS && SYM != GREAT && SYM != LESSEQ && SYM != GREATEQ) ERROR(2);
    SYMBOL condition_op = SYM;
    GETSYM();
    EXPRESSION(TX,LEV);
    GEN(OPR,0,SYM_TO_OPR[condition_op]);
}
void GEN(INSTRUCT F,int L,int A){
    if(CX >= MAX_CODE_SIZE){
        ERROR(0);
    }
    else{
        CODE[CX].f = F;
        CODE[CX].l = L;
        CODE[CX].a = A;
        CX++;
    }
}
int FIND_IDENT(int *TX,string IDENT_NAME){
    for(int i = 0; i < *TX; i++){
        if(TABLE[i].name == IDENT_NAME){
            return i;
        }
    }
    return -1;
}


int main(){
    string FILE_NAME = "demo.txt";

    string FILE_NAME_NO_EX = "";
    for(int i = 0; i < FILE_NAME.length(); i++){
        if(FILE_NAME[i] == '.') break;
        FILE_NAME_NO_EX.push_back(FILE_NAME[i]);
    }

    //正常运行
    INIT();
    INFILE.open(FILE_NAME);
    GETCH();
    GETSYM();
    int TX = 0;
    BOLCK(&TX,0);

    //TABLE表输出
    for (int i = 0; i < TX; i++){
        printf("name:%s\tkind:%d\tval:%d\tlevel:%d\tadr:%d\n",
        TABLE[i].name.c_str(),TABLE[i].kind,TABLE[i].val,TABLE[i].level,TABLE[i].adr);
    }

    //CODE表输出
    for(int i = 0; i < CX; i++){
        printf("%d:\t%d\t%d\t%d\n",i,CODE[i].f,CODE[i].l,CODE[i].a);
    }

    //生成编译后文件
    ofstream OUTFILE;
    OUTFILE.open(FILE_NAME_NO_EX+".pl0",ios::out);
    for(int i = 0; i < CX; i++){
        OUTFILE<<CODE[i].f<<" "<<CODE[i].l<<" "<<CODE[i].a<<endl;
    }
    printf("Program Generation: %s.pl0\n",FILE_NAME_NO_EX.c_str());

    // //词法分析测试
    // INIT();
    // INFILE.open(FILE_NAME);
    // GETCH();
    // printf("%d\t%s\t%d\n",SYM,ID.c_str(),NUM);
    // while (true)
    // {
    //     GETSYM();
    //     printf("%d\t%s\t%d\n",SYM,ID.c_str(),NUM);
    // }
}