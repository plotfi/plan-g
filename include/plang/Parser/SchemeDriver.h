#ifndef __SCHEME_DRIVER_H__
#define __SCHEME_DRIVER_H__ 1

#include <fstream>
#include <sstream>
#include <iostream>

#include "scheme.tab.h"
#include "SchemeScanner.hpp"
#include "DemandStructure.h"

namespace Scheme {

    class SchemeDriver {
      public:
        SchemeDriver();
        virtual ~SchemeDriver();

        long process();
        std::pair<bool, long> parse(const char *);

        std::string getErrors() const;
        std::ostream & getErrorStream();

        void set_prog(AST::ProgramNode *);
        AST::ProgramNode* get_prog() {return program;}
        AST::ProgramNode* get_anf_prog() {return anf_program;}

        bool printAST(std::ostream &, std::ostream &);
        bool printGrammar(std::ostream &, std::ostream &);

      private:
        SchemeParser * parser;
        SchemeScanner * scanner;

        AST::ProgramNode * program;
        AST::ProgramNode * anf_program;

        // Scheme::Demands::demand_grammar * approx_grammar;
        // Scheme::Demands::demand_grammar * combined_grammar;
        Demands::LivenessInformation program_grammars;

        std::stringstream error_stream;
    };

}

#endif
