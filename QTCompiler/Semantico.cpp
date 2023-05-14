#include "Semantico.h"
#include "Constants.h"

#include <iostream>

#include <stack>

using namespace std;

Simbolo simbolo;
stack<int> Escopo;
int escopo = 0;

void Simbolo::DeclararTipo(std::string t){
    tipo = t;
}

void ResetaTabela()
{
    while(!Escopo.empty())
    {
        Escopo.pop();
    }
    escopo = 0;
    simbolo.vetor = false;
    simbolo.parametro = false;
}

void Semantico::executeAction(int action, const Token *token) throw (SemanticError )
{
    std::cout << "Acao: " << action << ", Token: "  << token->getId()
              << ", Lexema: " << token->getLexeme() << std::endl;

    string lexema = token->getLexeme();
    stack<int> temp;
    simbolo.vetor = false;

    if(Escopo.empty())
        Escopo.push(escopo);

    switch (action) {
    case 1:
        simbolo.DeclararTipo(lexema);
        break;

    case 2:
        temp = Escopo;

        while(!temp.empty())
        {
            for(Simbolo sim : tabelaSimbolo)
            {
                if(sim.escopo == temp.top() && sim.id == token->getLexeme())
                {
                    ResetaTabela();

                    for(Simbolo sim : tabelaSimbolo)
                    {
                        cout << "Escopo: " << sim.escopo << " Tipo: " << sim.tipo << " ID: " << sim.id << endl;
                    }
                    throw SemanticError("Variavel declarada 2 vezes", token->getPosition());
                }
            }
            temp.pop();
        }

        simbolo.funcao = false;
        simbolo.id = token->getLexeme();
        simbolo.escopo = Escopo.top();

        if(simbolo.parametro)
        {
            simbolo.escopo = Escopo.top() + 1;
        }

        tabelaSimbolo.push_front(simbolo);

        simbolo.vetor =false;
        break;

    case 10:
        escopo++;
        Escopo.push(escopo);
        break;

    case 11:
        Escopo.pop();
        break;

    case 20:
        ResetaTabela();
        break;

    case 37:
        while(!temp.empty())
        {
            for(Simbolo sim : tabelaSimbolo)
            {
                if(sim.escopo == temp.top() && sim.id == token->getLexeme())
                {
                    ResetaTabela();

                    for(Simbolo sim : tabelaSimbolo)
                    {
                        cout << "Escopo: " << sim.escopo << " Tipo: " << sim.tipo << " ID: " << sim.id << endl;
                    }
                    throw SemanticError("Variavel declarada 2 vezes", token->getPosition());
                }
            }
            temp.pop();
        }

        simbolo.funcao = true;
        simbolo.parametro = false;
        simbolo.vetor = false;
        simbolo.id = token->getLexeme();
        simbolo.escopo = Escopo.top();
        tabelaSimbolo.push_front(simbolo);
        simbolo.parametro = true;
        break;

    case 39:
        simbolo.parametro = false;
        break;

    case 43:
        simbolo.vetor = true;
        break;
    }
}
