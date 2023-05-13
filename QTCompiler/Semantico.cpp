#include "Semantico.h"
#include "Constants.h"

#include <iostream>

#include <stack>

using namespace std;

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
}

Simbolo simbolo;

void Semantico::executeAction(int action, const Token *token) throw (SemanticError )
{
    std::cout << "Acao: " << action << ", Token: "  << token->getId()
              << ", Lexema: " << token->getLexeme() << std::endl;

    string lexema = token->getLexeme();
    stack<int> temp;
    simbolo.funcao = false;
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


        simbolo.id = token->getLexeme();
        simbolo.escopo = Escopo.top();
        tabelaSimbolo.push_front(simbolo);
        break;

    case 10:
        escopo++;
        Escopo.push(escopo);
        cout << "\nValor novo escopo = " << escopo;
        cout << "\nTamanho novo Escopo = " << Escopo.size();
        cout << endl;
        break;
    case 11:
        Escopo.pop();
        cout << "\nValor novo escopo = " << escopo;
        cout << "\nTamanho novo Escopo = " << Escopo.size();
        cout << endl;
        break;
    case 20:
        for(Simbolo sim : tabelaSimbolo)
        {
            cout << "Escopo: " << sim.escopo << " Tipo: " << sim.tipo << " ID: " << sim.id << "Funcao? " << sim.funcao << endl;
        }


        /// Limpar tudo
        ResetaTabela();
        break;
    case 39:
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
        simbolo.id = token->getLexeme();
        simbolo.escopo = Escopo.top();
        tabelaSimbolo.push_front(simbolo);
        break;
    }
}
