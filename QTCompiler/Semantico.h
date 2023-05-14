#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "Token.h"
#include "SemanticError.h"
#include <list>

using namespace std;

class Simbolo{
public:
    string tipo;
    string id;
    int escopo;

    bool inicializado = false;
    bool usado = false;

    bool parametro = false;
    bool funcao = false;
    bool vetor = false;

    int posVetor = false;

    void DeclararTipo(std::string t);

};

class Semantico
{
public:
    void executeAction(int action, const Token *token) throw (SemanticError );
    list<Simbolo> tabelaSimbolo;
    list<Simbolo> tabelaSimboloFuncoes;
};

#endif
