#include "Semantico.h"
#include "Constants.h"

#include <iostream>
#include <stack>
#include "semantictable.h"

using namespace std;

SemanticTable semanticTable;
Simbolo simbolo;
stack<int> stackEscopo;
int escopo = 0;
Simbolo * ptrSim;
Simbolo * lastSimbol;
Simbolo * ptrAtribuir;
int return_type = -1;

list<Simbolo*> lstExp;
list<int> lstExpType;
list<int> lstExpTypeSave;
list<int>::iterator it;
list<pair<int, int>> lstOperators;
list<pair<int, int>> lstOperatorsSave;
list<pair<int, int>>::iterator it_par;
pair<int, int> intPar;

bool vectorExp = false;
bool firstVar = true;
bool flagOp = false;

char oper;
string store;
string vector_load;
string lastTemp;
Temp * temporario;
Temp * temporarioVetor = nullptr;
Temp * temporarioAux;
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
    lstExp.clear();
    lstExpType.clear();
    lstOperators.clear();
    vectorExp = false;
    firstVar = true;
    flagOp = false;
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
    std::cout << "Acao: " << action << ", Token: "  << token->getId();
    std::cout << "Ação: " << action << ", Token: "  << token->getId()
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
                throw SemanticError("Variavel com mesmo nome declarada.", token->getPosition());
            }
            break;

        case 3:
            ptrSim = Tabela.Find( stackEscopo, token->getLexeme() );
            if( ptrSim != nullptr )
            {
                if( ptrSim->vetor ){
                    ResetaTabela();
                    throw SemanticError("Tentativa de utilizar vetor como variavel.", token->getPosition());
                }

                lstExp.push_back(ptrSim);
                lstExpType.push_back(ConvertType( ptrSim->tipo ) );

                if (!flagOp)
                    Tabela.gera_cod ("LD", token->getLexeme());
                else
                {
                    if (oper == '+')
                        Tabela.gera_cod ("ADD", token->getLexeme());
                    if (oper == '-')
                        Tabela.gera_cod ("SUB", token->getLexeme());
                    flagOp = false;
                }
                firstVar = false;
            }
            else
            {
                ResetaTabela();
                throw SemanticError("Tentativa de utilizacao de variavel nao existe no escopo.", token->getPosition());
            }
            break;

        case 4:
            ptrAtribuir = Tabela.Find( stackEscopo, token->getLexeme() );
            if( ptrAtribuir == nullptr )
            {
                ResetaTabela();
                throw SemanticError("Tentativa de atribuicao de variavel nao existente.", token->getPosition());
            }
            lstExp.clear();
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
                throw SemanticError("Tentativa de utilizar variavel nao existente.", token->getPosition());
            }

            break;

        case 6:
            setInitialized = true;

            switch(semanticTable.atribType( ConvertType( lastSimbol->tipo) , return_type)){

                case -1:
                    ResetaTabela();
                    throw SemanticError("Erro na atribuicao de variavel.", token->getPosition());
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
            if(stackEscopo.empty())
            {
                ResetaTabela();
                throw SemanticError("} inesperado", token->getPosition());
            }
            stackEscopo.pop();
            break;

        case 12:
            if(lastSimbol == nullptr)
            {
                ResetaTabela();
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
            ResetaTabela();
            throw SemanticError("Funcao inexistene no escopo. ", token->getPosition());
            break;

        case 14: // GERACAO DE CODIGO ASSEMBLY - Saida de dados
            Tabela.gera_cod("LDI", token->getLexeme());
            Tabela.gera_cod("STO", "$out_port");
            break;

        case 15:
            if( !ptrAtribuir->vetor )
            {
                ResetaTabela();
                throw SemanticError("Tentativa de leitura de vetor em outro tipo de variavel.", token->getPosition());
            }
            ptrAtribuir->usado = true;
            Tabela.gera_cod("STO", "$indr");
            Tabela.gera_cod("LDV", ptrAtribuir->id);
            Tabela.gera_cod("STO", "$out_port");

            vectorExp = false;
            break;

        case 16:
            setInitialized = true;


            switch(semanticTable.atribType( ConvertType( ptrAtribuir->tipo) , return_type)){
                case -1:
                    ResetaTabela();
                    throw SemanticError("Erro na atribuicao de variavel.", token->getPosition());
                case 1:
                    Tabela.setWarning(*lastSimbol, "Perda de precisao" );
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
                ResetaTabela();
                throw SemanticError("Erro ao tentar atribuir funcao em variavel", token->getPosition());

            case 1:
                Tabela.setWarning( *ptrAtribuir, "Perda de precisao");
            }

            break;

        case 18:
            if(return_type != 4)
            {
                ResetaTabela();
                throw SemanticError("Boolean esperado na expressao", token->getPosition());
            }
            break;
        case 19:
            break;
        case 20:
            Tabela.setUnusedWarning();
            ResetaTabela();

            Tabela.data.clear();
            Tabela.data.append(".data\n");
            for (auto it = Tabela.lstSimbolos.rbegin(); it != Tabela.lstSimbolos.rend(); ++it)
            {
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

                if( ptrSim->vetor )
                {
                    ResetaTabela();
                    throw SemanticError("Tentativa de leitura de vetor", token->getPosition());
                }

                if( ptrSim->inicializado == false )
                {
                    Tabela.setWarning(*ptrSim);
                }
                Tabela.gera_cod("LD", token->getLexeme());
                Tabela.gera_cod("STO", "$out_port");
            }
            else
            {
                ResetaTabela();
                throw SemanticError("Tentativa de leitura de variavel inexistente.", token->getPosition());
            }
            break;

        case 22:
            ptrSim = Tabela.Find( stackEscopo, token->getLexeme() );

            if( ptrSim->vetor )
            {
                ResetaTabela();
                throw SemanticError("Tentativa de entrada de dados em Vetor", token->getPosition());
            }
            if( ptrSim != nullptr )
            {
                ptrSim->inicializado = true;
                Tabela.gera_cod("LD", "$in_port");
                Tabela.gera_cod("STO", ptrSim->id);
            }
            else
            {
                ResetaTabela();
                throw SemanticError("Entrada de dados em variavel inexistente.", token->getPosition());
            }
            break;

        case 25:
            Tabela.gera_cod("STO", "$indr");
            Tabela.gera_cod("LD", "$in_port");
            Tabela.gera_cod("STOV", store);
            break;

        case 26:
            if( !ptrAtribuir->vetor )
            {
                ResetaTabela();
                throw SemanticError("Tentativa de atribuir variavel como vetor.", token->getPosition());
            }
            vectorExp = true;
            break;

        case 27:
            lstExpTypeSave = lstExpType;
            lstOperatorsSave = lstOperators;
            lstExp.clear();
            lstExpType.clear();
            lstOperators.clear();
            break;

        case 28:
            lstExpType = lstExpTypeSave;
            lstOperators = lstOperatorsSave;
            lstExpTypeSave.clear();
            lstOperatorsSave.clear();
            lstExpType.push_back(return_type);
            break;

        case 29:
            if( ptrAtribuir->vetor )
            {
                ResetaTabela();
                throw SemanticError("Tentativa de atribuir vetor como variavel.", token->getPosition());
            }
            break;

        case 30:
            ptrSim = Tabela.Find( stackEscopo, token->getLexeme() );
            if( ptrSim == nullptr ){
                ResetaTabela();
                throw SemanticError("Tentativa de utilizar vetor inexistente.", token->getPosition());
            }
            if( !ptrSim->vetor ){
                ResetaTabela();
                throw SemanticError("Tentativa de utilizar variavel como vetor.", token->getPosition());
            }
            if( !ptrSim->inicializado )
                Tabela.setWarning( *ptrSim, "Uso de variavel nao inicializada." );
            ptrSim->usado = true;
            flagOp = false;

            if(!firstVar)
            {
                temporarioAux = Tabela.GetTemp();
                temporarioAux->livre = false;
                Tabela.gera_cod("STO", temporarioAux->name);
            }

            vector_load = token->getLexeme();
            break;

            // Valores
        case 31:
            firstVar = false;
            lstExpType.push_back(0); // Int
            if (!flagOp)
            {
                Tabela.gera_cod("LDI", token->getLexeme());
            }
            else
            {
                if (oper == '+')
                    Tabela.gera_cod ("ADDI", token->getLexeme());
                if (oper == '-')
                    Tabela.gera_cod ("SUBI", token->getLexeme());
                flagOp = false;
            }
            break;
        case 32:
            firstVar = false;
            lstExpType.push_back(1); // Float
            break;
        case 33:
            firstVar = false;
            //lstExpType.push_back(3); // Double
            break;
        case 34:
            firstVar = false;
            lstExpType.push_back(2); // Char
            break;
        case 35:
            firstVar = false;
            lstExpType.push_back(3); // String
            break;
        case 36:
            firstVar = false;
            lstExpType.push_back(4); // Boolean
            break;

            // Peso das Expressões
        case 40:
            intPar.first = 0;
            intPar.second = 0;
            lstOperators.push_back(intPar); // Mais ( + )
            oper = '+';
            flagOp = true;
            break;
        case 41:
            intPar.first = 1;
            intPar.second = 0;
            lstOperators.push_back(intPar); // Menos ( - )
            oper = '-';
            flagOp = true;
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
            std::cout << "Lista de tipos: \n";
            for(int i : lstExpType)
            {
                std::cout << "Posicao: " << pos << " Tipo: " << i << "\n";
                pos ++;
            }

            pos = 0;
            std::cout << "Lista de operadores: \n";
            for(pair<int, int> i : lstOperators)
            {
                std::cout << "Operador: " << pos << " Operador: " << i.first << "\n";
                pos ++;
            }

            while(lstOperators.size() != 0)
            {
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
                        ResetaTabela();
                        throw SemanticError("Erro na expressao, tipos invalidos.", token->getPosition());
                    }
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
                        ResetaTabela();
                        throw SemanticError("Erro na expressao, tipos invalidos.", token->getPosition());
                    }
                    lstExpType.insert(it, return_type);
                }
            }

            if( lstExpType.size() > 1 )
            {
                ResetaTabela();
                throw SemanticError("Erro inexperado na expressao, mais de um valor no retorno", token->getPosition());
            }
            firstVar = true;
            lstExpType.clear();
            break;

        case 52:
            firstVar = true;
            if(vectorExp)
            {
                temporario = Tabela.GetTemp();
                Tabela.gera_cod("STO", temporario->name);
                Tabela.gera_cod("LD", temporarioVetor->name);
                Tabela.gera_cod("STO", "$indr");
                Tabela.gera_cod("LD", temporario->name);
                Tabela.gera_cod("STOV", store);
            }
            else
            {
                Tabela.gera_cod("STO", store);
            }
            if(temporarioVetor != nullptr )
                temporarioVetor->livre = true;

            vectorExp = false;
            break;

        case 53:

            for( Simbolo * ptr : lstExp )
            {
                if(ptr->inicializado == false )
                {
                    Tabela.setWarning(*ptr, "Uso de variavel nao inicializada!");
                }
                ptr->usado = true;
            }
            temporarioVetor = Tabela.GetTemp();
            Tabela.gera_cod("STO", temporarioVetor->name); // expressao da esquerda
            temporarioVetor->livre = false;
            break;

        case 54:
            for( Simbolo * ptr : lstExp )
            {
                if(ptr->inicializado == false )
                {
                    Tabela.setWarning(*ptr, "Uso de variavel nao inicializada!");
                }
                ptr->usado = true;
            }
            Tabela.gera_cod("STO", "$indr");
            Tabela.gera_cod("LDV", vector_load);
            if( !firstVar )
            {
                temporario = Tabela.GetTemp();
                Tabela.gera_cod("STO", temporario->name);
                Tabela.gera_cod("LD", temporarioAux->name);
                if(oper == '+')
                {
                    Tabela.gera_cod("ADD", temporario->name);
                }
                if(oper == '-')
                {
                    Tabela.gera_cod("SUB", temporario->name);
                }
                temporario->livre = true;
                temporarioAux->livre = true;
            }
            firstVar = false;
            break;
        }
}
