#include "Semantico.h"
#include "Constants.h"

#include <iostream>

#include <stack>

using namespace std;

Simbolo simbolo;
stack<int> stackEscopo;
Warning war;
int escopo = 0;
Simbolo * ptrSim;

void Simbolo::DeclararTipo(std::string t){
    tipo = t;
}

void ResetaTabela()
{
    while(!stackEscopo.empty())
    {
        stackEscopo.pop();
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

    if(stackEscopo.empty())
        stackEscopo.push(escopo);

    switch (action) {
    case 1:
        simbolo.DeclararTipo(lexema);
        break;

    case 2:
        ptrSim = Tabela.Find(stackEscopo, token->getLexeme());

        if(ptrSim == nullptr)
        {
            simbolo.funcao = false;
            simbolo.id = token->getLexeme();
            simbolo.escopo = stackEscopo.top();

            if(simbolo.parametro)
            {
                simbolo.escopo = stackEscopo.top() + 1;
            }

            Tabela.lstSimbolos.push_front(simbolo);

            ptrSim = &Tabela.lstSimbolos.front();
        }
        else
        {
            throw SemanticError("Variavel com mesmo nome declarada.", token->getPosition());
        }
        break;

    case 3:
        ptrSim = Tabela.Find( stackEscopo, token->getLexeme() );

        if( ptrSim != nullptr )
        {
            if( ptrSim->inicializado == false )
                Tabela.setUsingUndefinedWarning(*ptrSim);

            ptrSim->usado = true;
        }
        else
        {
            Tabela.setUnusedWarning();
            throw SemanticError("Tentativa de utilizacao de variavel nao existe no escopo.", token->getPosition());
        }
        break;

    case 4:
        ptrSim = Tabela.Find( stackEscopo, token->getLexeme() ) ;

        if( ptrSim != nullptr )
        {
            ptrSim->inicializado = true;
        }
        else
        {
            Tabela.setUnusedWarning();
            throw SemanticError("Tentativa de atribuicao de variavel nao existente.", token->getPosition());
        }
        break;

    case 5:
        ptrSim = Tabela.Find( stackEscopo, token->getLexeme() ) ;

        if( ptrSim != nullptr )
        {
            if( ptrSim->inicializado == false )
            {
                Tabela.setUsingUndefinedWarning(*ptrSim, "Sera utilizado lixo de memoria em acao ( var++ )");
            }
        }
        break;

    case 6:
        ptrSim->inicializado = true;
        break;

    case 7:

        if( Tabela.Procurar(stackEscopo, token->getLexeme()) )
        {
            Tabela.setUnusedWarning();
            throw SemanticError("Variavel com mesmo nome declarada", token->getPosition());
        }

        simbolo.funcao = true;
        simbolo.parametro = false;
        simbolo.vetor = false;
        simbolo.id = token->getLexeme();
        simbolo.escopo = stackEscopo.top();
        Tabela.lstSimbolos.push_front(simbolo);
        simbolo.parametro = true;
        break;

    case 8:
        simbolo.parametro = false;
        break;

    case 9:
        ptrSim->vetor = true;
        break;

    case 10:
        escopo++;
        stackEscopo.push(escopo);
        break;

    case 11:
        stackEscopo.pop();
        break;

    case 12:
        ptrSim->posVetor = stoi(token->getLexeme());
        break;

    case 13 :
        temp = stackEscopo;
        while(!temp.empty())
        {
            for(Simbolo &sim : Tabela.lstSimbolos)
            {
                if(sim.escopo == temp.top() && sim.id == lexema && sim.funcao == true)
                {
                    sim.usado = true;
                    return;
                }
            }
            temp.pop();
        }
        Tabela.setUnusedWarning();
        throw SemanticError("Funcao inexistene no escopo. ", token->getPosition());
        break;
    case 20:
        Tabela.setUnusedWarning();
        ResetaTabela();
        break;
    }
}
