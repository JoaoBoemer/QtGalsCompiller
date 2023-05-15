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
Simbolo * lastSimbol;
Simbolo * ptrAtribuir;
list<Simbolo*> lstExp;

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
    bool setInitialized;

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
            lastSimbol = &Tabela.lstSimbolos.front();
        }
        else
        {
            ResetaTabela();
            Tabela.setUnusedWarning();
            throw SemanticError("Variavel com mesmo nome declarada.", token->getPosition());
        }
        break;

    case 3:
        ptrSim = Tabela.Find( stackEscopo, token->getLexeme() );

        if( ptrSim != nullptr )
        {
            lstExp.push_back(ptrSim);
            /*
            if( ptrSim->inicializado == false ){

                Tabela.setUsingUndefinedWarning(*ptrSim);
            }

            if( ptrSim != ptrAtribuir )
                ptrSim->usado = true;
            */
        }
        else
        {
            ResetaTabela();
            Tabela.setUnusedWarning();
            throw SemanticError("Tentativa de utilizacao de variavel nao existe no escopo.", token->getPosition());
        }
        break;

    case 4:
        ptrAtribuir = Tabela.Find( stackEscopo, token->getLexeme() );
        lstExp.clear();

        if( ptrAtribuir != nullptr )
        {

        }
        else
        {
            ResetaTabela();
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
                Tabela.setUsingUndefinedWarning(*ptrSim, "Sera utilizado lixo de memoria em acao ( var++ ou var += var )");
            }
        }
        else
        {
            ResetaTabela();
            Tabela.setUnusedWarning();
            throw SemanticError("Tentativa de utilizar variavel nao existente.", token->getPosition());
        }

        break;

    case 6:
        setInitialized = true;

        for( Simbolo * ptr : lstExp )
        {
            if( ptr->escopo == lastSimbol->escopo && ptr->id == lastSimbol->id )
            {
                if( lastSimbol->inicializado == false ){
                    Tabela.setUsingUndefinedWarning(*lastSimbol, token->getLexeme());
                    //Tabela.setUsingUndefinedWarning(*lastSimbol, "Utilizacao da variavel na atribuicao da mesma");
                }
                setInitialized = false;
            }
            else
            {
                ptr->usado = true;
                if( !ptr->inicializado )
                    Tabela.setUsingUndefinedWarning(*ptr);
            }
        }
        if( setInitialized )
        {
            lastSimbol->inicializado = true;
        }

        lstExp.clear();
        /*
        if( token->getLexeme() == lastSimbol->id && lastSimbol->inicializado == false )
        {
            lastSimbol->inicializado = false;
            lastSimbol->usado = false;
        }
        else
        {
            lastSimbol->inicializado = true;
        }
        */
        break;

    case 7:

        if( Tabela.Procurar(stackEscopo, token->getLexeme()) )
        {
            ResetaTabela();
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
        /// dar visoiada;
        ptrSim->vetor = true;
        break;

    case 10:
        escopo++;
        stackEscopo.push(escopo);
        break;

    case 11:
        if(stackEscopo.empty())
        {
            throw SemanticError("} inesperado", token->getPosition());
        }
        stackEscopo.pop();
        break;

    case 12:
        if(lastSimbol == nullptr)
        {
            throw SemanticError("Vetor inesperado", token->getPosition());
        }
        lastSimbol->posVetor = stoi(token->getLexeme());
        //ptrSim->posVetor = stoi(token->getLexeme());
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
        ResetaTabela();
        throw SemanticError("Funcao inexistene no escopo. ", token->getPosition());
        break;

    case 16:
        setInitialized = true;

        for( Simbolo * ptr : lstExp )
        {
            if( ptrAtribuir->escopo == ptr->escopo && ptrAtribuir->id == ptr->id )
            {
                if ( ptrAtribuir->inicializado == false )
                {
                    Tabela.setUsingUndefinedWarning(*lastSimbol, token->getLexeme());
                    Tabela.setUsingUndefinedWarning(*lastSimbol, "Utilizacao da variavel na atribuicao da mesma");
                    setInitialized = false;
                }
            }
            else
            {
                if( !ptr->inicializado )
                    Tabela.setUsingUndefinedWarning(*ptr);
                ptr->usado = true;
            }
        }

        if( setInitialized )
        {
            ptrAtribuir->inicializado = true;
        }
        /*
        if( ptrAtribuir->id == token->getLexeme() )
        {
            Tabela.setUsingUndefinedWarning(*ptrSim);
            ptrSim->inicializado = false;
        }
        else
        {
            ptrSim->inicializado = true;
        }
        */
        lstExp.clear();
        break;
    case 19:
        for( Simbolo * ptr : lstExp )
        {
            ptr->usado = true;
        }
        lstExp.clear();
        break;
    case 20:
        Tabela.setUnusedWarning();
        ResetaTabela();
        break;
    }
}
