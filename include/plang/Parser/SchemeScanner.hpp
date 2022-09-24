#ifndef __SCHEME_SCANNER_HPP__
#define __SCHEME_SCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

/* Since we are using a namespace Scheme, override the yylex interface  */
#undef  YY_DECL
#define YY_DECL int  Scheme::SchemeScanner::yylex()

#include "scheme.tab.h"

namespace Scheme {

    class SchemeScanner : public yyFlexLexer {
      public:
        SchemeScanner(std::istream * in) : yyFlexLexer(in), yylval( NULL ) {};

        int yylex(Scheme::SchemeParser::semantic_type * lval) {
            yylval = lval;
            return( yylex() );
        }

        int get_yylineno() {
            return yylineno;
        }
        char * get_yytext() {
            return yytext;
        }

      private:
        int yylex();
        Scheme::SchemeParser::semantic_type * yylval;
    };

}

#endif
