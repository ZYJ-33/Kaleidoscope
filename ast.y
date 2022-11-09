%language "c++"
%require "3.2"
%header

%define api.value.type variant
%define api.token.constructor
%define api.value.automove

%code requires
{
    #include <memory>
    #include <vector>
    #include <string>
    #include "./node.h"
    class Driver;
    class Scanner;
}

%code top
{
    #include "./scanner.hh"
    #include "./driver.hh"
    #include "ast.tab.hh"
    static yy::parser::symbol_type yylex(Driver& drv, Scanner& scan)
    {
        return scan.get_next_token();
    }
}

%lex-param {Driver& drv}
%lex-param {Scanner& scan}
%parse-param {Driver& drv}
%parse-param {Scanner& scan}

%token<double> NUM
%token<std::unique_ptr<std::string>> ID
%token LEFT_PARA
%token RIGHT_PARA
%token DEF
%token EXTERN
%token COMMA
%token IF
%token FI
%token THEN
%token ELSE
%token<char> TIME DIV ADD SUB AND OR EQUAL UNEQUAL

%nterm<std::unique_ptr<std::vector<std::unique_ptr<FuncAST>>>> funcs
%nterm<std::unique_ptr<FuncAST>> func
%nterm<std::unique_ptr<PrototypeAST>> proto
%nterm<std::unique_ptr<ExprAST>> expr
%nterm<std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>>> exprs_with_comma
%nterm<std::unique_ptr<std::vector<std::unique_ptr<std::string>>>> ids_with_comma
%nterm<std::unique_ptr<DeclAST>> decl
%nterm<std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>>> decls

%right ASSGIN
%left EQUAL UNEQUAL
%left OR
%left AND
%left ADD SUB
%left TIME DIV
%right COMMA

%%
start : decls funcs {drv.decls = $1; drv.funcs = $2;}
      ;

decl  : EXTERN proto {$$ = std::unique_ptr<DeclAST>(new DeclAST($2));}
      ;

decls : decl decls {$2->push_back($1); $$ = $2;}
      | {$$ = std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>>(new std::vector<std::unique_ptr<DeclAST>>());}
      ;

funcs : func funcs {$2->push_back($1); $$ = $2;}
      | {$$ = std::unique_ptr<std::vector<std::unique_ptr<FuncAST>>>(new std::vector<std::unique_ptr<FuncAST>>());}
      ;

func :DEF proto expr {$$ = std::unique_ptr<FuncAST>(new FuncAST($2, $3));}
     ;


proto :ID LEFT_PARA ids_with_comma RIGHT_PARA {$$ = std::unique_ptr<PrototypeAST>(new PrototypeAST($1, $3));}
      ;

ids_with_comma : ID ids_with_comma  {$2->push_back($1); $$=$2;}
               | COMMA ID ids_with_comma {$3->push_back($2); $$=$3;}
               | {$$ = std::unique_ptr<std::vector<std::unique_ptr<std::string>>>(new std::vector<std::unique_ptr<std::string>>());}
               ;

expr : ID {$$ = std::unique_ptr<VariableExprAST>(new VariableExprAST($1));}
     | NUM {$$ = std::unique_ptr<NumberExprAST>(new NumberExprAST($1));}
     | expr TIME expr{$$ = std::unique_ptr<BinOpExprAST>(new BinOpExprAST($2, $1, $3));}
     | expr DIV expr {$$ = std::unique_ptr<BinOpExprAST>(new BinOpExprAST($2, $1, $3));}
     | expr ADD expr {$$ = std::unique_ptr<BinOpExprAST>(new BinOpExprAST($2, $1, $3));}
     | expr SUB expr {$$ = std::unique_ptr<BinOpExprAST>(new BinOpExprAST($2, $1, $3));}
     | expr AND expr {$$ = std::unique_ptr<BinOpExprAST>(new BinOpExprAST($2, $1, $3));}
     | expr OR expr {$$ = std::unique_ptr<BinOpExprAST>(new BinOpExprAST($2, $1, $3));}
     | expr EQUAL expr {$$ = std::unique_ptr<BinOpExprAST>(new BinOpExprAST($2, $1, $3));}
     | expr UNEQUAL expr {$$ = std::unique_ptr<BinOpExprAST>(new BinOpExprAST($2, $1, $3));}
     | IF expr THEN expr FI {$$ = std::unique_ptr<IfExprAST>(new IfExprAST($2, $4));}
     | IF expr THEN expr ELSE expr FI {$$ = std::unique_ptr<IfElseExprAST>(new IfElseExprAST($2, $4, $6));}
     | ID LEFT_PARA exprs_with_comma RIGHT_PARA {$$ = std::unique_ptr<CallExprAST>(new CallExprAST($1, $3));}
     ;

exprs_with_comma : expr exprs_with_comma    {$2->push_back($1); $$ = $2;}
                | COMMA expr exprs_with_comma {$3->push_back($2); $$ = $3;}
                | {$$ = std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>>(new std::vector<std::unique_ptr<ExprAST>>());}
                ;
%%

void yy::parser::error(const std::string& mes)
{
    std::cout<<mes<<std::endl;
}

