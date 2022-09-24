digit           [0-9]
alpha           [a-zA-Z]
special         [?_-]

comment         ;.*\n

str_const       \"(\\.|[^"])*\"
int_const       -?{digit}+
bool_const      #[ftFT]

identifier      [_a-zA-Z!&*/:<=>?^][_a-zA-Z!0&*/:<=>?^0-9.+-]*

%{
    #include <cctype>
    /* The implementation of yyFlexScanner */
    #include "SchemeScanner.hpp"

    /* Typedef to make returns for tokens shorter */
    typedef Scheme::SchemeParser::token token;

    /* re-define yyterminate() as END token instead of NULL */
    #define yyterminate()   return ( token::END )
%}

%option c++
%option debug
%option batch
%option noinput
%option nounput
%option yylineno
%option noyywrap
%option nodefault
%option yyclass="SchemeScanner"

%%

"define"                { return( token::DEFINE ); }

"if"                    { return( token::IF ); }
"let"                   { return( token::LET ); }
"cons"                  { return( token::CONS ); }
"car"                   { return( token::CAR ); }
"cdr"                   { return( token::CDR ); }

"null?"                 { return( token::NULLQ ); }
"pair?"                 { return( token::PAIRQ ); }
"eq?"                   { return( token::EQQ ); }

"mod"                   { return( token::MOD ); }
"+"                     { return( token::ADD ); }
"-"                     { return( token::SUB ); }
"*"                     { return( token::MUL ); }
"/"                     { return( token::DIV ); }
"<"                     { return( token::LT ); }
">"                     { return( token::GT ); }
"="                     { return( token::EQ ); }

"nil"                   { return( token::NIL ); }
"'()"                   { return( token::NIL ); }
"()"                    { return( token::NIL ); }

"("                     { return( token::LP ); }
")"                     { return( token::RP ); }
","                     { return( token::COMMA ); }

{int_const}             { yylval->ival = new int(atoi(yytext));      return( token::ICONST ); }
{bool_const}            { yylval->bval = new bool(yytext[1] == 't'); return( token::BCONST ); }
{str_const}             { yylval->sval = new std::string(yytext);    return( token::SCONST ); }

{identifier}            { yylval->sval = new std::string(yytext);    return( token::ID ); }

{comment}               /* ignore all comments   */;
[\n\t ]+                /* ignore all whitespace */;

.                       /* ignore bad characters */

%%
