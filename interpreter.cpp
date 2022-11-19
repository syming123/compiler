#include<iostream>
#include<fstream>
#include<map>
#include<string>
using namespace std;

/* 最大CODE表长度 */
#define MAX_CODE_SIZE 1000
/* 最大数据栈大小 */
#define MAX_STACK_SIZE 1000


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
int CX;

/* OPR运算类型 */
enum OPR_TYPE{
    OPR_EXIT,OPR_ADD,OPR_SUB,OPR_MULTI,OPR_DIV,OPR_READ,OPR_WRITE,OPR_WRITELN,
    OPR_EQ,OPR_NEQ,OPR_LESS,OPR_GREAT,OPR_LESSEQ,OPR_GREATEQ
};

/* 数据栈 */
int DATA_STACK[MAX_STACK_SIZE];
/* 四个寄存器 */
INSTRUCTION I;
int P,T,B;


/* 获取变量绝对地址 */
int ABS_ADR(int BASE,int SUBLEV,int DX){
    while(SUBLEV > 0){
        BASE = DATA_STACK[BASE];
        SUBLEV--;
    }
    return BASE + DX;
}


void INTERPRETER_RUN(){
    P = 0;
    T = -1;
    B = 0;

    while(P < CX){
        I = CODE[P++];
        if(I.f == LIT){
            DATA_STACK[++T] = I.a;
        }
        else if(I.f == LOD){
            int lev = I.l;
            int dx = I.a;
            DATA_STACK[++T] = DATA_STACK[ABS_ADR(B,lev,dx)];
        }
        else if(I.f == STO){
            int lev = I.l;
            int dx = I.a;
            DATA_STACK[ABS_ADR(B,lev,dx)] = DATA_STACK[T--];
        }
        else if(I.f == CAL){
            int lev = I.l;
            int adr = I.a;
            DATA_STACK[T+1] = ABS_ADR(B,lev,0); //SL静态链
            DATA_STACK[T+2] = B; //DL动态链
            DATA_STACK[T+3] = P; //RA返回地址
            B = T + 1;
            P = adr;
        }
        else if(I.f == INT){
            T += I.a;
        }
        else if(I.f == JMP){
            P = I.a;
        }
        else if(I.f == JPC){
            if(DATA_STACK[T--] == 0) P = I.a;
        }
        else if(I.f == OPR){
            OPR_TYPE op = (OPR_TYPE)I.a;
            if(op == OPR_EXIT){
                P = DATA_STACK[B+2];
                T = B - 1;
                B = DATA_STACK[B+1];
                if(P == 0) return;
            }
            else if(op == OPR_ADD){
                int t = DATA_STACK[T--];
                t = DATA_STACK[T] + t;
                DATA_STACK[T] = t;
            }
            else if(op == OPR_SUB){
                int t = DATA_STACK[T--];
                t = DATA_STACK[T] - t;
                DATA_STACK[T] = t;
            }
            else if(op == OPR_MULTI){
                int t = DATA_STACK[T--];
                t = DATA_STACK[T] * t;
                DATA_STACK[T] = t;
            }
            else if(op == OPR_DIV){
                int t = DATA_STACK[T--];
                t = DATA_STACK[T] / t;
                DATA_STACK[T] = t;
            }
            else if(op == OPR_READ){
                scanf("%d",&DATA_STACK[++T]);
            }
            else if(op == OPR_WRITE){
                printf("%d",DATA_STACK[T--]);
            }
            else if(op == OPR_WRITELN){
                printf("\n");
            }
            else if(op == OPR_EQ){
                int a = DATA_STACK[T--];
                int b = DATA_STACK[T];
                DATA_STACK[T] = (b == a ? 1 : 0);
            }
            else if(op == OPR_NEQ){
                int a = DATA_STACK[T--];
                int b = DATA_STACK[T];
                DATA_STACK[T] = (b != a ? 1 : 0);
            }
            else if(op == OPR_LESS){
                int a = DATA_STACK[T--];
                int b = DATA_STACK[T];
                DATA_STACK[T] = (b < a ? 1 : 0);
            }
            else if(op == OPR_GREAT){
                int a = DATA_STACK[T--];
                int b = DATA_STACK[T];
                DATA_STACK[T] = (b > a ? 1 : 0);
            }
            else if(op == OPR_LESSEQ){
                int a = DATA_STACK[T--];
                int b = DATA_STACK[T];
                DATA_STACK[T] = (b <= a ? 1 : 0);
            }
            else if(op == OPR_GREATEQ){
                int a = DATA_STACK[T--];
                int b = DATA_STACK[T];
                DATA_STACK[T] = (b >= a ? 1 : 0);
            }
        }
    }
}


int main(){
    string file_name = "demo.pl0";
    fstream infile;
    infile.open(file_name);
    while(infile){
        int F,L,A;
        infile>>F>>L>>A;
        if(!infile) break;
        CODE[CX].f = (INSTRUCT)F;
        CODE[CX].l = L;
        CODE[CX].a = A;
        CX++;
    }
    

    // //输出CODE表内容
    // for(int i = 0; i < CX; i++){
    //     printf("%d\t%d\t%d\n",CODE[i].f,CODE[i].l,CODE[i].a);
    // }

    
    //运行
    INTERPRETER_RUN();

}