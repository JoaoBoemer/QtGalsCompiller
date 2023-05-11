#ifndef LEXICO_H
#define LEXICO_H

#include "Token.h"
#include "LexicalError.h"

#include <string>

class Lexico
{
public:
    Lexico(const char *input = "") { setInput(input); }

    void setInput(const char *input);
    void setPosition(unsigned pos) { position = pos; }
    Token *nextToken() throw (LexicalError);

private:
    unsigned position;
    std::string input;

    int nextState(unsigned char c, int state) const;
    TokenId tokenForState(int state) const;
    TokenId lookupToken(TokenId base, const std::string &key);

    bool hasInput() const { return position < input.size(); }
    char nextChar() { return hasInput() ? input[position++] : (char) -1; }
};

#endif
