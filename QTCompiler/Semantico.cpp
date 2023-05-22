#include "Semantico.h"
#include "Constants.h"

#include <iostream>
#include <stack>
#include "semantictable.h"

using namespace std;

SemanticTable semanticTable;
Simbolo simbolo;
stack<int> stackEscopo;
Warning war;
int escopo = 0;
Simbolo * ptrSim;
Simbolo * lastSimbol;
Simbolo * ptrAtribuir;
list<Simbolo*> lstExp;
list<int> lstExpType;
list<int> lstOperators;
list<int>::iterator it;
int return_type = -1;

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

int ConvertType(string t)
{
    if(t == "int")
        return 0;
    if(t == "float")
        return 1;
    if(t == "char")
        return 2;
    if(t == "string")
        return 3;
    if(t == "bool")
        return 4;
    if(t == "double")
        return 5;
}

void Semantico::executeAction(int action, const Token *token) throw (SemanticError )
{
    std::cout << "Acao: " << action << ", Token: "  << token->getId()
              << ", Lexema: " << token->getLexeme() << std::endl;

    string lexema = token->getLexeme();
    stack<int> temp;
    bool setInitialized;
    int pos;
    int value_1;
    int value_2;
    int max;

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
            lstExpType.push_back(ConvertType( ptrSim->tipo ) );
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

        if( ptrAtribuir == nullptr )
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
                    //Tabela.setUsingUndefinedWarning(*lastSimbol, token->getLexeme());
                    Tabela.setUsingUndefinedWarning(*lastSimbol, "Utilizacao da variavel na atribuicao da mesma");
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
                    Tabela.setUsingUndefinedWarning(*ptrAtribuir, "Utilizacao da variavel na atribuicao da mesma");
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
        lstExp.clear();
        break;
    case 19:
        for( Simbolo * ptr : lstExp )
        {
            ptr->usado = true;
        }
        lstExp.clear();
        lstExpType.clear();
        lstOperators.clear();
        break;
    case 20:
        Tabela.setUnusedWarning();
        ResetaTabela();
        break;

    case 21:
        ptrSim = Tabela.Find(stackEscopo, token->getLexeme());
        if(ptrSim != nullptr)
        {
            ptrSim->usado = true;
            if( ptrSim->inicializado == false )
            {
                Tabela.setUsingUndefinedWarning(*ptrSim);
            }
        }
        else
        {
            Tabela.setUnusedWarning();
            throw SemanticError("Tentativa de leitura de variavel inexistente.", token->getPosition());
        }

        break;
    case 22:

        break;

        // Valores
    case 31:
        lstExpType.push_back(0); // Int
        break;
    case 32:
        lstExpType.push_back(1); // Float
        break;
    case 33:
        //lstExpType.push_back(3); // Double
        break;
    case 34:
        lstExpType.push_back(2); // Char
        break;
    case 35:
        lstExpType.push_back(3); // String
        break;
    case 36:
        lstExpType.push_back(4); // Boolean
        break;

        // Peso das Expressões
    case 40:
        lstOperators.push_back(0); // Mais ( + )
        break;
    case 41:
        lstOperators.push_back(1); // Menos ( - )
        break;
    case 42:
        lstOperators.push_back(2); // Vezes ( * )
        break;
    case 43:
        lstOperators.push_back(3); // Divisão ( / )
        break;
    case 44:
        lstOperators.push_back(4); // MOD ( % )
        break;
    case 45:
        lstOperators.push_back(5); // Relacionais ( >, <,  <=, >=, ==, != )
        break;
    case 46:
        // Negate
        //lstOperators.push_back(6);
        break;
    case 47:
        //ponteiro para lista
        // Parenteses
        break;

        // Leitura da expressão
    case 51:
        max = 0;
        return_type = 0;

        pos = 0;
        std::cout << "Lista de tipos: ";
        for(int i : lstExpType)
        {
            std::cout << "\nPosicao: " << pos << " Tipo: " << i << "\n";
            pos ++;
        }

        while(lstOperators.size() != 0)
        {
            std::cout << "\nSIZE = " << lstOperators.size();
            if(lstOperators.size() == 1)
            {
                value_1 = lstExpType.front();
                value_2 = lstExpType.back();
                max = lstOperators.front();
                return_type = semanticTable.resultType(value_1, value_2, max);
                lstOperators.clear();
                lstExpType.clear();
                if(return_type == -1)
                {
                    throw SemanticError("Erro na expressao, tipos invalidos.", token->getPosition());
                }
                lstExpType.push_back(return_type);
            }
            if(lstOperators.size() > 1)
            {
                // Pega a expressão com maior prioridade
                max = 0;
                for ( int i : lstOperators )
                {
                    if( i > max )
                        max = i;
                }
                // Pega a posição da primeira expressão com maior prioridade
                pos = 0;
                for( int i : lstOperators )
                {
                    if( max == i )
                        break;
                    pos++;
                }
                it = lstOperators.begin();
                advance(it, pos);
                lstOperators.erase(it);

                it = lstExpType.begin();
                advance(it, pos);
                value_1 = *it; // Pega o valor na mesma posição na lista de tipos
                it = lstExpType.erase(it);
                value_2 = *it; // Pega o proximo valor na lista de tipos
                it = lstExpType.erase(it);
                return_type = semanticTable.resultType(value_1, value_2, max); // Verifica a expressão ( valor_1, max (operador), valor_2 )

                if(return_type == -1)
                {
                    throw SemanticError("Erro na expressao, tipos invalidos.", token->getPosition());
                }

                if(return_type == 1)
                {
                    std::cout << "DEU WARNING";
                }

                lstExpType.insert(it, return_type);
            }
        }

        if( lstExpType.size() > 1 )
            std::cout << "DEU ALGO DE ERRADO, EXPRESSAO COM MAIS DE UM VALOR RESTANTE!";
        return_type = lstExpType.front();
        lstExpType.clear();
        std::cout << "\nRetorno: " << return_type << "\n";
        break;
    }
}
