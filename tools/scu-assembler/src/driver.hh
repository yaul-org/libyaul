/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Avik Das
 */

#ifndef _DRIVER_HH_
#define _DRIVER_HH_

#include <stack>

#include "parser.hh"
#include "lexer.hh"

namespace Lexer {
    class Driver {
    public:
        std::string filename;
        Flex *lexer;

        Driver(const std::string&);
        ~Driver();

        void parse();
        void save_line();
        void reset_line();
        int line() const;
        void set_debug();

        int error() const;
        void error(const Bison::location_type&, const std::string&);
        void error(const std::string&);

    private:
        Bison *_parser;
        std::filebuf _fbuf;
        int _err_count;
        int _lineno;
    };
}

#endif /* !_DRIVER_HH_ */
