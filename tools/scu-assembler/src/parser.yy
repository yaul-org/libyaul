/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

%skeleton "lalr1.cc"
%defines
%name-prefix "Lexer"
%define parser_class_name "Bison"
%parse-param { class Driver& driver }

%debug
%error-verbose

%locations
%initial-action
{
    @$.begin.filename = @$.end.filename = &driver.filename;
};

%code requires {
}

%token_table

%{
#include "driver.hh"

#ifdef yylex
#undef yylex
#define yylex driver.lexer->lex
#endif
%}

%union {
}

/* RULES */
%%
program :
        ;

%%

#include "lexer.hh"

void Lexer::Bison::error(const Lexer::Bison::location_type &l,
                         const std::string &s)
{
    driver.error(l, s);
}
