/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

%{

#include "driver.hh"

/*-
 * Add debugging messages for rules
 */
/* #define DEBUG */
%}

/* DEFINITIONS SECTION */
S [\r\t\f\v\ ]
NEWLINE "\n"
DIGIT [0-9]
UPPERCASE [A-Z]
LOWERCASE [a-z]

%option yywrap
/* 'yylineno' is not part of the POSIX specification */
%option yylineno

/* Change the name of the lexer class. */
%option prefix="Generated"

/* Enable C++ scanner class generation. */
%option c++

/* The manual says "somewhat more optimized" */
%option batch

/* Enable scanner to generate debug output. Disable this for release
 * versions */
%option debug

/*
 * Suppresses inclusion of the non-ANSI header file 'unistd.h'. This
 * option is meant to target environments in which 'unistd.h' does not
 * exist.
 */
%option nounistd

/*
 * Causes the default rule (that unmatched scanner input is echoed to
 * 'stdout') to be suppressed.  If the scanner encounters input that
 * does not match any of its rules, it aborts with an error.  This
 * option is useful for finding holes in a scanner's rule set.
 */
/* %option nodefault */

/*
 * Warn about certain things. In particular, if the default rule can be
 * matched but no default rule has been given, flex will warn you.
 */
%option warn

/* Turn on maximum compatibility with POSIX 1003.2-1992 definition of
 * 'lex' */
/* %option posix */

/* A fix for a compilation error. See the manual. */
%option yymore

/*-
 * (%s) declares inclusive start.
 * (%x) declares exclusive start.
 *
 * If it is exclusive, then only rules qualified with the start
 * condition will be active.
 *
 * If it is inclusive, then rules with no start conditions at all will
 * also be active.
 */
%x START_CONDITION_INDENTATION
%x START_CONDITION_IMPLICIT_LINE_JOINING
%x START_CONDITION_SHORTSTRING1
%x START_CONDITION_SHORTSTRING2
%x START_CONDITION_LONGSTRING1
%x START_CONDITION_LONGSTRING2

/* RULES */
%%
. {
    ECHO;
}
%%

/* USER CODE */

void Lexer::Flex::init(std::istream *f)
{
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(yy_create_buffer(f, YY_BUF_SIZE));
}

void Lexer::Flex::set_debug()
{
    yy_flex_debug = 1;
}

#ifdef yylex
#undef yylex
#endif

/*
 * VERY IMPORTANT
 * This implementation of 'GeneratedFlexLexer::yylex()' is required to
 * fill the 'vtable' of the class 'GeneratedFlexLexer'. Limitations of
 * GCC?
 *
 * We define the scanner's main 'yylex()' function via 'YY_DECL' to
 * reside in the 'Flex' class instead.
 */
int GeneratedFlexLexer::yylex()
{
    return 0;
}

/*
 * When the scanner receives an end-of-file indication from 'YY_INPUT',
 * it then checks the 'yywrap()' function.  If 'yywrap()' returns false
 * (zero), then it is assumed that the function has gone ahead and set
 * up 'yyin' to point to another input file, and scanning continues.  If
 * it returns true (non-zero), then the scanner terminates, returning 0
 * to its caller.
 */
int GeneratedFlexLexer::yywrap()
{
    return 1;
}
