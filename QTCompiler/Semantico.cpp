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
pair<string, bool> par;
pair<int, int> intPar;
list<int> lstExpType;
list<pair<int, int>> lstOperators;
list<pair<string, bool>> lstExpValor;
list<int>::iterator it;
list<pair<int, int>>::iterator it_par;
list<pair<string, bool>>::iterator it_val;
int return_type = -1;

bool firstVar = true;

string store = "";

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

            store = lastSimbol->id;
        }
        else
        {
            ResetaTabela();
            //Tabela.setUnusedWarning();
            throw SemanticError("Variavel com mesmo nome declarada.", token->getPosition());
        }
        break;

    case 3:
        ptrSim = Tabela.Find( stackEscopo, token->getLexeme() );

        if( ptrSim != nullptr )
        {
            lstExp.push_back(ptrSim);
            lstExpType.push_back(ConvertType( ptrSim->tipo ) );
            par.first = ptrSim->id;
            par.second = true;
            lstExpValor.push_back(par);
        }
        else
        {
            ResetaTabela();
            //Tabela.setUnusedWarning();
            throw SemanticError("Tentativa de utilizacao de variavel nao existe no escopo.", token->getPosition());
        }
        break;

    case 4:
        ptrAtribuir = Tabela.Find( stackEscopo, token->getLexeme() );
        lstExp.clear();

        if( ptrAtribuir == nullptr )
        {
            ResetaTabela();
            //Tabela.setUnusedWarning();
            throw SemanticError("Tentativa de atribuicao de variavel nao existente.", token->getPosition());
        }

        store = token->getLexeme();

        break;

    case 5:
        ptrSim = Tabela.Find( stackEscopo, token->getLexeme() ) ;

        if( ptrSim != nullptr )
        {
            if( ptrSim->inicializado == false )
            {
                Tabela.setWarning(*ptrSim, "Sera utilizado lixo de memoria em acao ( var++ ou var += var )");
            }
        }
        else
        {
            ResetaTabela();
            //Tabela.setUnusedWarning();
            throw SemanticError("Tentativa de utilizar variavel nao existente.", token->getPosition());
        }

        break;

    case 6:
        setInitialized = true;

        switch(semanticTable.atribType( ConvertType( lastSimbol->tipo) , return_type)){

        case -1:
            throw SemanticError("Erro na atribuicao de variavel.", token->getPosition());
            break;
        case 1:
            Tabela.setWarning(*lastSimbol, "Perda de precisao" );
            break;
        }

        for( Simbolo * ptr : lstExp )
        {
            if( ptr->escopo == lastSimbol->escopo && ptr->id == lastSimbol->id )
            {
                if( lastSimbol->inicializado == false ){
                    Tabela.setWarning(*lastSimbol, "Utilizacao da variavel na atribuicao da mesma");
                }
                setInitialized = false;
            }
            else
            {
                ptr->usado = true;
                if( !ptr->inicializado )
                    Tabela.setWarning(*ptr);
            }
        }
        if( setInitialized )
        {
            lastSimbol->inicializado = true;
        }

        lstExp.clear();
        break;

    case 7:

        if( Tabela.Procurar(stackEscopo, token->getLexeme()) )
        {
            ResetaTabela();
            //Tabela.setUnusedWarning();
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
        //Tabela.setUnusedWarning();
        ResetaTabela();
        throw SemanticError("Funcao inexistene no escopo. ", token->getPosition());
        break;

    case 14: // GERACAO DE CODIGO ASSEMBLY - Saida de dados
        Tabela.gera_cod("LDI", token->getLexeme());
        Tabela.gera_cod("STO", "$out_port");
        break;

    case 15:

        break;

    case 16:
        setInitialized = true;


        switch(semanticTable.atribType( ConvertType( ptrAtribuir->tipo) , return_type)){
            case -1:
                throw SemanticError("Erro na atribuicao de variavel.", token->getPosition());
                break;
            case 1:
                Tabela.setWarning(*lastSimbol, "Perda de precisao" );
                break;
        }

        for( Simbolo * ptr : lstExp )
        {
            if( ptrAtribuir->escopo == ptr->escopo && ptrAtribuir->id == ptr->id )
            {
                if ( ptrAtribuir->inicializado == false )
                {
                    Tabela.setWarning(*ptrAtribuir, "Utilizacao da variavel na atribuicao da mesma");
                    setInitialized = false;
                }
            }
            else
            {
                if( !ptr->inicializado )
                    Tabela.setWarning(*ptr);
                ptr->usado = true;
            }
        }

        if( setInitialized )
        {
            ptrAtribuir->inicializado = true;
        }
        lstExp.clear();

        break;

    case 17:
        switch ( semanticTable.atribType( ConvertType(ptrAtribuir->tipo), return_type) )
        {
        case -1:
            throw SemanticError("Erro ao tentar atribuir funcao em variavel", token->getPosition());

        case 1:
            Tabela.setWarning( *ptrAtribuir, "Perda de precisao");
        }

        break;

    case 18:
        if(return_type != 4)
        {
            throw SemanticError("Boolean esperado na expressao", token->getPosition());
        }
        break;
    case 19:
        for( Simbolo * ptr : lstExp )
        {
            if(ptr->inicializado == false )
            {
                Tabela.setWarning(*ptr, "Uso de variavel nao inicializada!");
            }
            ptr->usado = true;
        }

        lstExp.clear();
        lstExpType.clear();
        lstOperators.clear();

        break;
    case 20:
        Tabela.setUnusedWarning();
        ResetaTabela();

        Tabela.data.clear();
        Tabela.data.append(".data\n");
        for (auto it = Tabela.lstSimbolos.rbegin(); it != Tabela.lstSimbolos.rend(); ++it) {
            Simbolo sim = *it;
            if(sim.vetor)
            {
                Tabela.data.append(sim.id);
                Tabela.data.append(":");
                for(int i = 0; i < sim.posVetor; i++)
                {
                    Tabela.data.append(" 0");
                }
                Tabela.data.append("\n");
            }
            else
            {
                Tabela.data.append(sim.id);
                Tabela.data.append(": 0\n");
            }
        }

        Tabela.data.append("\n.text\n");
        Tabela.data.append(Tabela.assembly);
        break;

    case 21:
        ptrSim = Tabela.Find(stackEscopo, token->getLexeme());
        if(ptrSim != nullptr)
        {
            ptrSim->usado = true;
            if( ptrSim->inicializado == false )
            {
                Tabela.setWarning(*ptrSim);
            }
            Tabela.gera_cod("LD", token->getLexeme());
            Tabela.gera_cod("STO", "$out_port");
        }
        else
        {
            throw SemanticError("Tentativa de leitura de variavel inexistente.", token->getPosition());
        }
        break;

    case 22:
        ptrSim = Tabela.Find( stackEscopo, token->getLexeme() );

        if( ptrSim != nullptr )
        {
            ptrSim->inicializado = true;
            Tabela.gera_cod("LD", "$in_port");
            Tabela.gera_cod("STO", ptrSim->id);
        }
        else
        {
            throw SemanticError("Entrada de dados em variavel inexistente.", token->getPosition());
        }
        break;

        // Valores
    case 31:
        lstExpType.push_back(0); // Int
        par.first = token->getLexeme();
        par.second = false;
        lstExpValor.push_back(par);
        break;
    case 32:
        lstExpType.push_back(1); // Float
        par.first = token->getLexeme();
        par.second = false;
        lstExpValor.push_back(par);
        break;
    case 33:
        //lstExpType.push_back(3); // Double
        //lstExpValor.push_back(token->getLexeme());
        break;
    case 34:
        lstExpType.push_back(2); // Char
        par.first = token->getLexeme();
        par.second = false;
        lstExpValor.push_back(par);
        break;
    case 35:
        lstExpType.push_back(3); // String
        par.first = token->getLexeme();
        par.second = false;
        lstExpValor.push_back(par);
        break;
    case 36:
        lstExpType.push_back(4); // Boolean
        par.first = token->getLexeme();
        par.second = false;
        lstExpValor.push_back(par);
        break;

        // Peso das Expressões
    case 40:
        intPar.first = 0;
        intPar.second = 0;
        lstOperators.push_back(intPar); // Mais ( + )
        break;
    case 41:
        intPar.first = 1;
        intPar.second = 0;
        lstOperators.push_back(intPar); // Menos ( - )
        break;
    case 42:
        intPar.first = 2;
        intPar.second = 1;
        lstOperators.push_back(intPar); // Vezes ( * )
        break;
    case 43:
        intPar.first = 3;
        intPar.second = 1;
        lstOperators.push_back(intPar); // Divisão ( / )
        break;
    case 44:
        intPar.first = 4;
        intPar.second = 1;
        lstOperators.push_back(intPar); // MOD ( % )
        break;
    case 45:
        intPar.first = 5;
        intPar.second = 2;
        lstOperators.push_back(intPar); // Relacionais ( >, <,  <=, >=, ==, != )
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

        pos = 0;
        std::cout << "\nLista de operadores: ";
        for(pair<int, int> i : lstOperators)
        {
            std::cout << "\nOperador: " << pos << " Operador: " << i.first << "\n";
            pos ++;
        }

        if(lstExpType.size() == 1 ) // SE FOR SO UM NUMERO
        {
            par = lstExpValor.front();
            if( par.second )
            {
                Tabela.gera_cod("LD", par.first);
            }
            else
            {
                Tabela.gera_cod("LDI", par.first);
            }

        }

        while(lstOperators.size() != 0)
        {
            std::cout << "SIZE = " << lstOperators.size() << "\n";
            if(lstOperators.size() == 1) // ULTIMA ITERACAO
            {
                value_1 = lstExpType.front();
                value_2 = lstExpType.back();
                intPar = lstOperators.front();
                return_type = semanticTable.resultType(value_1, value_2, intPar.first);
                lstOperators.clear();
                lstExpType.clear();
                if(return_type == -1)
                {
                    throw SemanticError("Erro na expressao, tipos invalidos.", token->getPosition());
                }

                if(firstVar)
                {
                    par = lstExpValor.front();
                    if(par.second)
                    {
                        Tabela.gera_cod("LD", par.first);
                    }
                    else
                    {
                        Tabela.gera_cod("LDI", par.first);
                    }

                    par = lstExpValor.back();

                    if( intPar.first == 0 ) // ADICAO
                    {
                        if(par.second) // VARIAVEL?
                        {
                            Tabela.gera_cod("ADD", par.first); // SIM
                        }
                        else
                        {
                            Tabela.gera_cod("ADDI", par.first); // NAO
                        }
                    }
                    else if( intPar.first == 1) // SUBTRACAO
                    {
                        if(par.second) // VARIAVEL?
                        {
                            Tabela.gera_cod("SUB", par.first); // SIM
                        }
                        else
                        {
                            Tabela.gera_cod("SUBI", par.first); // NAO
                        }
                    }

                    firstVar = false;
                }
                else
                {
                    if( intPar.first == 0 ) // ADICAO
                    {
                        par = lstExpValor.front();
                        if(par.second) // VARIAVEL?
                        {
                                Tabela.gera_cod("ADD", par.first); // SIM
                        }
                        else
                        {
                                Tabela.gera_cod("ADDI", par.first); // NAO
                        }
                    }
                    else if( intPar.first == 1) // SUBTRACAO
                    {
                        par = lstExpValor.front();
                        if(par.second) // VARIAVEL?
                        {
                                Tabela.gera_cod("SUB", par.first); // SIM
                        }
                        else
                        {
                                Tabela.gera_cod("SUBI", par.first); // NAO
                        }
                    }
                }

                lstExpValor.clear();
                lstExpType.push_back(return_type);
            }
            if(lstOperators.size() > 1)
            {
                // Pega a expressão com maior prioridade
                intPar.first = -1;
                intPar.second = -1;
                for ( pair<int, int> i : lstOperators )
                {
                    if( i.second > intPar.second )
                        intPar = i;
                }
                // Pega a posição da primeira expressão com maior prioridade
                pos = 0;
                for( pair<int, int> i : lstOperators )
                {
                    if( intPar.second == i.second )
                        break;
                    pos++;
                }
                it_par = lstOperators.begin();
                advance(it_par, pos);
                lstOperators.erase(it_par);

                it = lstExpType.begin();
                advance(it, pos);
                value_1 = *it; // Pega o valor na mesma posição na lista de tipos
                it = lstExpType.erase(it);
                value_2 = *it; // Pega o proximo valor na lista de tipos
                it = lstExpType.erase(it);
                return_type = semanticTable.resultType(value_1, value_2, intPar.first); // Verifica a expressão ( valor_1, max (operador), valor_2 )

                if(return_type == -1)
                {
                    throw SemanticError("Erro na expressao, tipos invalidos.", token->getPosition());
                }

                // GERACAO DE CODIGO ( apos a validacao apenas )
                it_val = lstExpValor.begin();
                advance(it_val, pos);
                par = *it_val;
                it_val = lstExpValor.erase(it_val);

                if(firstVar) // PRIMEIRA ITERACAO?
                {
                    if(par.second) // VARIAVEl?
                    {
                        Tabela.gera_cod("LD", par.first); // SIM
                    }
                    else
                    {
                        Tabela.gera_cod("LDI", par.first); // NAO
                    }
                    par = *it_val;

                    if ( intPar.first == 0 ) // ADICAO
                    {
                        if(par.second) // VARIAVEL?
                        {
                                Tabela.gera_cod("ADD", par.first); // SIM
                        }
                        else
                        {
                                Tabela.gera_cod("ADDI", par.first); // NAO
                        }
                    }
                    else if( intPar.first == 1) // SUBTRACAO
                    {
                        if(par.second) // VARIAVEL?
                        {
                                Tabela.gera_cod("SUB", par.first); // SIM
                        }
                        else
                        {
                                Tabela.gera_cod("SUBI", par.first); // NAO
                        }
                    }
                    it_val = lstExpValor.erase(it_val);

                    firstVar = false; // JA FOI
                }
                else // NAO EH A PRIMEIRA ITERACAO
                {
                    if ( intPar.first == 0 ) // ADICAO
                    {
                        if(par.second) // VARIAVEL?
                        {
                            Tabela.gera_cod("ADD", par.first); // SIM
                        }
                        else
                        {
                            Tabela.gera_cod("ADDI", par.first); // NAO
                        }
                    }
                    else if( intPar.first == 1) // SUBTRACAO
                    {
                        if(par.second) // VARIAVEL?
                        {
                            Tabela.gera_cod("SUB", par.first); // SIM
                        }
                        else
                        {
                            Tabela.gera_cod("SUBI", par.first); // NAO
                        }
                    }
                }


                lstExpType.insert(it, return_type);
            }
        }

        if( lstExpType.size() > 1 )
            throw SemanticError("Erro inexperado na expressao, mais de um valor no retorno", token->getPosition());


        lstExpValor.clear();
        lstExpType.clear();
        return_type = lstExpType.front();
        firstVar = true;
        Tabela.gera_cod("STO", store);
        break;
    }
}
