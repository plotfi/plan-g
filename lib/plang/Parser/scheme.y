%skeleton   "lalr1.cc"
%require    "2.5"
%debug
%defines

%define api.namespace           {Scheme}
%define api.parser.class {SchemeParser}

%code requires {
    #include "SchemeAST.h"

    // Forward declare the Scanner and Driver classes, as the Parser needs them
    // But they themselves cannot be declared without the Parser.
    namespace Scheme {
        class SchemeDriver;
        class SchemeScanner;
    }
}

%lex-param      { SchemeScanner &scanner }
%parse-param    { SchemeScanner &scanner }
%lex-param      { SchemeDriver &driver }
%parse-param    { SchemeDriver &driver }

%code {
    #include <iostream>
    #include <cstdlib>
    #include <fstream>

    #include "SchemeDriver.h"

    static int yylex(Scheme::SchemeParser::semantic_type *yylval,
                     Scheme::SchemeScanner &scanner,
                     Scheme::SchemeDriver &driver);
}

%union {
    int         *ival;
    bool        *bval;
    std::string *sval;
    

    Scheme::AST::ExprNode       *enode;
    Scheme::AST::DefineNode     *dnode;
    Scheme::AST::IdExprNode     *inode;
    Scheme::AST::ProgramNode    *pnode;
    Scheme::AST::PrimExprNode   *penode;

    std::list<Scheme::AST::ExprNode *>      *enodes;
    std::list<Scheme::AST::IdExprNode *>    *inodes;
    std::list<Scheme::AST::DefineNode *>    *dnodes;
}

%token          END                     0   "end of file"
%token          COMMA LP RP
%token          ADD SUB MUL DIV MOD
%token          PAIRQ NULLQ EQQ
%token          CONS CAR CDR
%token          LT GT EQ
%token          DEFINE
%token          LET IF NIL
%token  <ival>  ICONST
%token  <bval>  BCONST
%token  <sval>  SCONST ID

%type   <enodes>    list_of_exprs
%type   <dnodes>    list_of_defines
%type   <inodes>    list_of_identifiers

%type   <dnode>     define
%type   <pnode>     program
%type   <enode>     expr prim_app if_expr let_expr primapp_expr funapp_expr

%destructor { delete($$); }   expr define
%destructor { delete($$); }   ID BCONST ICONST SCONST
%destructor { delete($$); }   list_of_exprs list_of_defines list_of_identifiers
%destructor { delete($$); }   prim_app if_expr let_expr funapp_expr primapp_expr

%start program

%%

program : list_of_defines expr  { $$ = new Scheme::AST::ProgramNode($1, $2); driver.set_prog($$); }
        ;

list_of_defines : /* empty */               { $$ = new std::list<Scheme::AST::DefineNode *>(); }
                | list_of_defines define    { $$ = $1; $$->push_back($2); }
                ;

define : LP DEFINE LP ID list_of_identifiers RP expr RP { $$ = new Scheme::AST::DefineNode(new Scheme::AST::IdExprNode($4), $5, $7); }
       ;

list_of_identifiers : /* empty */               { $$ = new std::list<Scheme::AST::IdExprNode *>(); }
                    | list_of_identifiers ID    { $$ = $1; $$->push_back(new Scheme::AST::IdExprNode($2)); }
                    ;

expr : ID            { $$ = new Scheme::AST::IdExprNode($1); }
     | ICONST        { $$ = new Scheme::AST::IntConstExprNode($1); }
     | SCONST        { $$ = new Scheme::AST::StrConstExprNode($1); }
     | BCONST        { $$ = new Scheme::AST::BoolConstExprNode($1); }
     | NIL           { $$ = new Scheme::AST::NilConstExprNode(); }
     | if_expr       { $$ = $1; }
     | let_expr      { $$ = $1; }
     | primapp_expr  { $$ = $1; }
     | funapp_expr   { $$ = $1; }
     ;

if_expr : LP IF expr expr expr RP { $$ = new Scheme::AST::IfExprNode($3, $4, $5); }
        ;

let_expr : LP LET LP LP ID expr RP RP expr RP { $$ = new Scheme::AST::LetExprNode(new Scheme::AST::IdExprNode($5), $6, $9); }
         ;

list_of_exprs   : /* empty */           { $$ = new std::list<Scheme::AST::ExprNode *>(); }
                | list_of_exprs expr    { $$ = $1; $$->push_back($2); }
                ;

primapp_expr    : LP prim_app RP        { $$ = $2; }
                ;

funapp_expr : LP ID list_of_exprs RP    { $$ = new Scheme::AST::FuncExprNode(new Scheme::AST::IdExprNode($2), $3); }
            ;

prim_app    : PAIRQ expr        { $$ = new Scheme::AST::UnaryPrimExprNode("pair?", $2); }
            | NULLQ expr        { $$ = new Scheme::AST::UnaryPrimExprNode("null?", $2); }
            | EQQ expr expr     { $$ = new Scheme::AST::BinaryPrimExprNode("eq?", $2, $3); }
            | CAR expr          { $$ = new Scheme::AST::UnaryPrimExprNode("car", $2); }
            | CDR expr          { $$ = new Scheme::AST::UnaryPrimExprNode("cdr", $2); }
            | CONS expr expr    { $$ = new Scheme::AST::BinaryPrimExprNode("cons", $2, $3); }
            | ADD expr expr     { $$ = new Scheme::AST::BinaryPrimExprNode("+", $2, $3); }
            | SUB expr expr     { $$ = new Scheme::AST::BinaryPrimExprNode("-", $2, $3); }
            | MUL expr expr     { $$ = new Scheme::AST::BinaryPrimExprNode("*", $2, $3); }
            | DIV expr expr     { $$ = new Scheme::AST::BinaryPrimExprNode("/", $2, $3); }
            | MOD expr expr     { $$ = new Scheme::AST::BinaryPrimExprNode("mod", $2, $3); }
            | LT expr expr      { $$ = new Scheme::AST::BinaryPrimExprNode("<", $2, $3); }
            | GT expr expr      { $$ = new Scheme::AST::BinaryPrimExprNode(">", $2, $3); }
            | EQ expr expr      { $$ = new Scheme::AST::BinaryPrimExprNode("=", $2, $3); }
            ;

%%

void Scheme::SchemeParser::error(const std::string &err) {
    driver.getErrorStream() << "ERROR near Line " << scanner.get_yylineno() << " [" << err << "]: " << scanner.get_yytext() << std::endl;
}

#include "SchemeScanner.hpp"
static int yylex(Scheme::SchemeParser::semantic_type *yylval, Scheme::SchemeScanner &scanner, Scheme::SchemeDriver &driver) {
    return scanner.yylex(yylval);
}
