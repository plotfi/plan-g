#ifndef __OUTPUT_H__
#define __OUTPUT_H__ 1

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace Scheme {

    namespace output {

        struct options {
            std::string json_ast, plain_ast, scheme_ast,
                json_slice, plain_slice, scheme_slice;
            int ast_anf, slice_anf, ast_label, slice_label, dump_all;
        };

        extern options global_options;

        const std::string empty = "\u03D5";

        const std::string cdot = " \u00B7 ";
        const std::string vpipe = " \u2502 ";
        const std::string arrow = "  \u2192  ";
        const std::string piped = "<screen>";

        enum output_t { SCHEME = 0, PLAIN = 1, JSON = 2 };

        std::ostream & indentTill(std::ostream &, unsigned = 0);

    }

}

#endif
