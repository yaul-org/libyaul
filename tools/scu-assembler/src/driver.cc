/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "driver.hh"

Lexer::Driver::Driver(const std::string &_filename) :
    filename(_filename),
    lexer(NULL),
    _parser(NULL),
    _err_count(0),
    _lineno(0)
{
}

Lexer::Driver::~Driver()
{
    if (_parser != NULL)
        delete _parser;

    if (lexer != NULL)
        delete lexer;
}

void Lexer::Driver::parse()
{
    std::istream *is;

    try {
        _fbuf.open(filename.c_str(), std::ios::in);
    } catch (std::fstream::failure e) {
        std::cerr << e.what() << std::endl;
        return;
    }

    try {
        is = new std::istream(&_fbuf);

        lexer = new Flex(is);
        lexer->init(is);

        _parser = new Bison(*this);
    } catch (std::bad_alloc& e) {
        std::cerr << e.what() << std::endl;
        return;
    }

    _parser->parse();
}

void Lexer::Driver::save_line()
{
    _lineno = lexer->lineno();
}

void Lexer::Driver::reset_line()
{
    _lineno = 0;
}

int Lexer::Driver::line() const
{
    return _lineno;
}

void Lexer::Driver::set_debug()
{
#if YYDEBUG
    _parser->set_debug_level(1);
#endif
}

int Lexer::Driver::error() const
{
    return _err_count;
}

void Lexer::Driver::error(const Bison::location_type &l, const std::string &s)
{
    _err_count++;
    std::cerr << *l.begin.filename << ":" << _lineno << ":" << s << std::endl;
}

void Lexer::Driver::error(const std::string &s)
{
    _err_count++;
    std::cerr << filename << ":" << lexer->lineno() << ":" << s << std::endl;
}
