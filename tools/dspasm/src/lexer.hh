#ifndef _LEXER_HH_
#define _LEXER_HH_

#ifndef YY_DECL
#define YY_DECL                                                         \
    int Lexer::Flex::lex(Lexer::Bison::semantic_type* yylval,           \
    Lexer::location*)
#endif

#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer GeneratedFlexLexer
#include <FlexLexer.h>
#endif

#include <stack>
#include <fstream>
#include <sstream>
#include <climits>
#include <cstdlib>

#include "parser.hh"

typedef Lexer::Bison::token token;
typedef Lexer::Bison::token_type token_type;

namespace Lexer {
    class Flex: public GeneratedFlexLexer {
    private:
        std::istream* is;
        std::stack<int> indent_s;

        std::string *strip_string(char, const int);
        std::string *strip_single_string(const int);
        std::string *strip_double_string(const int);

    public:
        Flex(std::istream* _is) :
            GeneratedFlexLexer(_is),
            is(_is) {
            /* Always have a 0 at the stack. Never pop. */
            indent_s.push(0);
        }

        virtual int lex(Bison::semantic_type* yylval, location*);

        void init(std::istream*);
        void set_debug();

        void match_stringliteral(const char);

        /**
         * Check if the integer is valid; within the range [0,2^31].
         */
        token_type check_integer(std::string **, int);

        std::string *strip_shortstring1();
        std::string *strip_shortstring2();
        std::string *strip_longstring1();
        std::string *strip_longstring2();
    };
}

#endif /* !_LEXER_HH_ */
