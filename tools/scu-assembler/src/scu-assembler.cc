/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

#include "scu-assembler.hh"
#include "driver.hh"

static int usage(char *argv[])
{
    std::string progname;

    progname = argv[0];
    progname = progname.substr(1 + progname.find_last_of("/\\"));

    std::cerr << "usage:" << " " << progname << " "
              << "[file]" << std::endl;

    return EXIT_FAILURE;
}

static int compile(const std::string& file)
{
    int err_no;
    Lexer::Driver* driver;
    std::ifstream ifs;

    driver = new Lexer::Driver(file);
    err_no = 0;

    driver->parse();
    delete driver;

    return err_no;
}

int main(int argc, char* argv[])
{
    int ch;
    std::string file;

    while ((ch = getopt(argc, argv, "")) != -1) {
        switch(ch) {
        default:
            return usage(argv);
        }
    }

    int arg_offset;
    arg_offset = argc - 1;

    file = argv[arg_offset];
    if (file.empty() || (argc == 1))
        return usage(argv);

    return compile(file);
}
