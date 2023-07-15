%{
#include<unistd.h>
#include<stdio.h>
#include "Ast.h"
#include "cds.h"
#include "c_container_auxiliary.h"
%}

%{
int yylex();
%}

//yylval
%union{
ast* a;
double d;
}

/*declare tokens*/
%token <a> INTEGER FLOAT 
%token <a> TYPE CONST STRUCT RETURN IF ELSE WHILE ID SPACE SEMI COMMA ASSIGNOP BREAK CONTINUE
%token <a> GREAT GREATEQUAL LESS LESSEQUAL NOTEQUAL EQUAL 
%token <a> PLUS MINUS STAR DIV MOD AND OR DOT NOT LP RP LB RB LC RC AERROR
%token <a> EOL
%type  <a> Program ExtDefList ExtDef ExtDecList Specifire StructSpecifire TACList TAC InitList
OptTag  Tag VarDec  FunDec VarList ParamDec Compst StmtList Stmt DefList Def DecList Dec Exp Args 


/*priority*/
%right ASSIGNOP
%left OR
%left AND 
%left NOTEQUAL EQUAL
%left GREAT GREATEQUAL LESS LESSEQUAL
%left PLUS MINUS
%left STAR DIV MOD
%right NOT
%left LP RP LB RB DOT
%%


Program:|ExtDefList {
    $$=newast("Program",1,$1);
    
    eval_print($$,0);    

    // printf("\nprint over\n");

    eval($$);
}        
    ;
ExtDefList:ExtDef ExtDefList {$$=newast("ExtDefList",2,$1,$2);}
	| {$$=newast("ExtDefList",0,-1);}
	;

//定义全局变量 int a,b[10],c;
/*定义全局结构体 struct myStruct
              {
                  int a = 10;
                  int b,c;
                  int d = b + c;
              }
             或者
             struct myStructWithout */
//定义函数 int main(int a,int b){}
ExtDef:Specifire ExtDecList SEMI    {$$=newast("ExtDef",2,$1,$2);}
	|Specifire SEMI	{$$=newast("ExtDef",1,$1);}
	|Specifire FunDec Compst	{$$=newast("ExtDef",3,$1,$2,$3);}
	;

//全局变量名
ExtDecList:Dec {$$=newast("ExtDecList",1,$1);}
	|Dec COMMA ExtDecList {$$=newast("ExtDecList",2,$1,$3);}
	;


//标识符
//int float
//struct
//返回值类型
/*Specifire*/
Specifire:TYPE {$$=newast("Specifire",1,$1);}
	|StructSpecifire {$$=newast("Specifire",1,$1);}
    |CONST TYPE {$$=newast("Specifire",2,$2,$1);}
	;

/*结构体格式: struct myStruct
    {
        int a = 10;
        int b,c;
        int d = b + c;
    }
    或者
    struct myStructWithout
*/
StructSpecifire:STRUCT OptTag LC DefList RC {$$=newast("StructSpecifire",5,$1,$2,$3,$4,$5);}
	|STRUCT Tag {$$=newast("StructSpecifire",2,$1,$2);}
	;
OptTag:ID {$$=newast("OptTag",1,$1);}
	|{$$=newast("OptTag",0,-1);}
	;
Tag:ID {$$=newast("Tag",1,$1);}
	;


/*Declarators*/
//变量的声明
//格式：a a[10] a[]
VarDec:ID {$$=newast("VarDec",1,$1);}
	| VarDec LB Exp RB {$$=newast("VarDec",3,$1,$2,$3);}
	| VarDec LB RB {$$=newast("VarDec",2,$1,$2);}
	;
//函数的声明
//格式： main(int a,int b)
FunDec:ID LP VarList RP {$$=newast("FunDec",4,$1,$2,$3,$4);}
	|ID LP RP {$$=newast("FunDec",3,$1,$2,$3);}
	;
//变量列表 声明单个变量的拼接 中间以逗号隔开
//格式：int a,int b
VarList:ParamDec COMMA VarList {$$=newast("VarList",2,$1,$3);}
	|ParamDec {$$=newast("VarList",1,$1);}
	;
//声明单个变量作为参数列表中的参数
//格式：int a
ParamDec:Specifire VarDec {$$=newast("ParamDec",2,$1,$2);}
    ;



/*Statement*/
//语句
//大括号基本块
/*
格式:
{
DefList:
    int a = 10;
    int b,c;
    int d = b + c;
StmtList:
}
}
*/
//基本块
//格式:{定义、语句}
Compst:LC TACList RC {$$=newast("Compst",3,$1,$2,$3);}
	;
//用于解决 定义 语句 定义的情况
TACList:TAC TACList {$$=newast("TACList",2,$1,$2);}
    | {$$=newast("TACList",0,-1);}
    ;

TAC:DefList StmtList {$$=newast("TAC",2,$1,$2);}
    ;

//语句链推导出语句加语句链
StmtList:Stmt StmtList{$$=newast("StmtList",2,$1,$2);}
	| {$$=newast("StmtList",0,-1);}
	;
//语句推导
//1.表达式;
//2.{基本块};
//3.返回语句: return 表达式;
//4.if(表达式)语句::=>{基本块}
//5.if(表达式)语句::=>{基本块} else语句=>{基本块}
//6.while(表达式)语句::=>{基本块}
Stmt:Exp SEMI {$$=newast("Stmt",1,$1);}
    |SEMI
    |Compst {$$=newast("Stmt",1,$1);}
	|RETURN Exp SEMI {$$=newast("Stmt",2,$1,$2);}
	|RETURN SEMI {$$=newast("Stmt",1,$1);}
    |BREAK SEMI{$$=newast("Stmt",1,$1);}
    |CONTINUE SEMI{$$=newast("Stmt",1,$1);}
    |IF LP Exp RP Stmt {$$=newast("Stmt",5,$1,$3,newast("assistIF",0,-1),$5,newast("assistELSE",0,-1));}
    |IF LP Exp RP Stmt ELSE Stmt {$$=newast("Stmt",7,$1,$3,newast("assistIF",0,-1),$5,newast("assistELSE",0,-1),$6,$7);}
    |WHILE LP Exp RP Stmt {$$=newast("Stmt",4,$1,$3,newast("assistWHILE",0,-1),$5);}
	;


/*Local Definitions*/
//局部变量的声明
//格式:int a = m+n,b,c;float x,y,z;
DefList:Def DefList{$$=newast("DefList",2,$1,$2);}
	| {$$=newast("DefList",0,-1);}
	;
//格式:int a = m+n,b,c;
Def:Specifire DecList SEMI {$$=newast("Def",2,$1,$2);}
	;
//格式: a = m+n , b, c
DecList:Dec COMMA DecList {$$=newast("DecList",2,$1,$3);}
    |Dec {$$=newast("DecList",1,$1);}
	;
// ASSIGNOP是等号
// VarDec 表示标识符或者数组 a、a[10]
// a 或者 a = 表达式
// a =10; a=b+c;
Dec:VarDec {$$=newast("Dec",1,$1);}
	|VarDec ASSIGNOP Exp {$$=newast("Dec",3,$1,$2,$3);}
	|VarDec ASSIGNOP LC InitList RC{$$=newast("Dec",3,$1,$2,$4);}
	;

InitList:InitList COMMA InitList {$$=newast("InitList",3,$1,$2,$3);}
    | LC InitList COMMA InitList RC  {$$=newast("InitList",2,$2,$4);}
    | Exp  {$$=newast("InitList",1,$1);}

/*Expressions*/
Exp:Exp ASSIGNOP Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp = Exp
        |Exp AND Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp && Exp
        |Exp OR Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp || Exp
        |Exp GREAT Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp > Exp
        |Exp GREATEQUAL Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp >= Exp
        |Exp LESS Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp < Exp
        |Exp LESSEQUAL Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp <= Exp
        |Exp NOTEQUAL Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp != Exp
        |Exp EQUAL Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp == Exp
        |Exp PLUS Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp + Exp
        |Exp MINUS Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp - Exp
        |Exp STAR Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp * Exp
        |Exp DIV Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp / Exp
        |Exp MOD Exp{$$=newast("Exp",3,$1,$2,$3);} // Exp % Exp
        |LP Exp RP{$$=newast("Exp",1,$2);}  // (Exp)
        |MINUS Exp {$$=newast("Exp",2,$1,$2);} // -Exp
        |PLUS Exp {$$=newast("Exp",2,$1,$2);} // +Exp
        |NOT Exp {$$=newast("Exp",2,$1,$2);} // !Exp
        |ID LP Args RP {$$=newast("Exp",3,newast("assistFuncCall",0,-1),$1,$3);}//带有参数的函数调用
        |ID LP RP {$$=newast("Exp",2,newast("assistFuncCall",0,-1),$1);}//没有参数的函数调用
        |Exp LB Exp RB {$$=newast("Exp",4,$1,$2,$3,$4);} // Exp[Exp] 数组求值
        |Exp DOT ID {$$=newast("Exp",3,$1,$2,$3);} //Exp.ID 结构体调用结构体内部成员
        |ID {$$=newast("Exp",1,$1);} // ID 变量
        |INTEGER {$$=newast("Exp",1,$1);} //整形数字面量
        |FLOAT{$$=newast("Exp",1,$1);} //浮点数字面量
        ;

//函数调用中的参数表达
Args:Exp COMMA Args {$$=newast("Args",3,$1,newast("assistArgs",0,-1),$3);}
        |Exp {$$=newast("Args",2,$1,newast("assistArgs",0,-1));}
        ;
%%


