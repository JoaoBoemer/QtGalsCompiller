#include "Semantico.h"
#include "Constants.h"

#include <iostream>
#include <stack>
#include "semantictable.h"

using namespace std;

string busca_nome_funcao;

SemanticTable semanticTable;
Simbolo simbolo;
stack<int> stackEscopo;

int escopo = 0;
Simbolo * ptrSim;
Simbolo * lastSimbol;
Simbolo * ptrAtribuir;
Simbolo * ptrFunc;
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
string operl;
stack<string> stackRot;
string rotIf;
string rotFim;
int contIf;

int contparFunc = 0;
int contpar = 0;
string nome_call;
string nome;
string func;
string store;
string vector_load;
string lastTemp;
Temp * temporario;
Temp * temporarioVetor = nullptr;
Temp * temporarioAux;

string func_aux;

void Simbolo::DeclararTipo(std::string t){
    tipo = t;
}

string newRotulo()
{
    contIf ++;
    string rot = "ROT" + to_string(contIf);
    return rot;
}
/*
string getParname(string nome_call, int contpar)
{
    for ( auto ptr : Tabela.lstSimbolos ) {
        if ( ptr.id.substring( 0, nome_call.size() ) == nome_call && ptr.posParam == contpar )
            return ptr.id;
    }

    //Tabela.setWar
    return "";
}
*/
void ResetaTabela()
{
    while(!stackEscopo.empty())
    {
        stackEscopo.pop();
    }
    while(!stackRot.empty())
    {
        stackRot.pop();
    }
    contIf = 0;
    escopo = 0;
    contpar = 0;
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
            if(func != "")
                ptrSim = Tabela.Find(stackEscopo, func + "_" + token->getLexeme());
            else
                ptrSim = Tabela.Find(stackEscopo, token->getLexeme());

            if(ptrSim == nullptr)
            {
                simbolo.funcao = false;
                if(func != "")
                    simbolo.id = func + "_" + token->getLexeme();
                else
                    simbolo.id = token->getLexeme();
                simbolo.escopo = stackEscopo.top();

                if(simbolo.parametro)
                {
                    simbolo.escopo = stackEscopo.top() + 1;
                }

                simbolo.posParam = contparFunc;

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
            func = token->getLexeme();
            Tabela.lstSimbolos.push_front(simbolo);
            ptrFunc = &Tabela.lstSimbolos.front();

            simbolo.parametro = true;
            break;

        case 8:
            simbolo.parametro = false;
            ptrFunc->posParam = contparFunc;
            contparFunc = 0;
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
                        ptrFunc = Tabela.Find(temp, sim.id);
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
            for ( Simbolo * x : lstExp)
            {
                x->usado = true;
            }
            break;
        case 20:
            Tabela.setUnusedWarning();
            ResetaTabela();

            Tabela.data.clear();
            Tabela.data.append(".data\n");

            for(auto t : Tabela.temp )
            {
                Tabela.data.append(t.name);
                Tabela.data.append(" : 0\n");
            }

            for (auto it = Tabela.lstSimbolos.rbegin(); it != Tabela.lstSimbolos.rend(); ++it)
            {
                Simbolo sim = *it;
                if(sim.vetor)
                {
                    Tabela.data.append( sim.id );
                    Tabela.data.append(":");
                    for(int i = 0; i < sim.posVetor; i++)
                    {
                        Tabela.data.append(" 0,");
                    }
                    Tabela.data = Tabela.data.substr(0, Tabela.data.size()-1);
                    //Tabela.data.erase(Tabela.data.back());
                    Tabela.data.append("\n");
                }
                else
                {
                    Tabela.data.append( sim.id );
                    Tabela.data.append(": 0\n");
                }
            }

            Tabela.data.append("\n.text\n");
            Tabela.data.append("JMP main\n" );
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
            operl = token->getLexeme();
            Tabela.gera_cod("STO", "TEMP_ESQ");
            intPar.first = 5;
            intPar.second = 2;
            lstOperators.push_back(intPar); // Relacionais ( >, <,  <=, >=, ==, != )
            break;
        case 46:
            Tabela.gera_cod("STO", "TEMP_DIR");
            Tabela.gera_cod("LD", "TEMP_ESQ");
            Tabela.gera_cod("SUB", "TEMP_DIR");
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

    case 55:

        rotIf = newRotulo();
        stackRot.push(rotIf);

        if(operl == ">")
        {
            Tabela.gera_cod("BLE", rotIf);
        }
        if(operl == "<")
        {
            Tabela.gera_cod("BGE", rotIf);
        }
        if(operl == "==")
        {
            Tabela.gera_cod("BNE", rotIf);
        }
        if(operl == "!=")
        {
            Tabela.gera_cod("BEQ", rotIf);
        }
        if(operl == ">=")
        {
            Tabela.gera_cod("BLT", rotIf);
        }
        if(operl == "<=")
        {
            Tabela.gera_cod("BGT", rotIf);
        }

        break;

    case 56:
        rotIf = stackRot.top();
        stackRot.pop();
        Tabela.gera_cod(rotIf + ":", " ");
        break;

    case 57:
        if(stackRot.empty())
        {
            throw SemanticError("Erro no else ", token->getPosition());
        }
        rotIf = stackRot.top();
        stackRot.pop();
        //rotIf = pop ();
        rotFim = newRotulo();
        Tabela.gera_cod("JMP", rotFim);
        stackRot.push(rotFim);
        Tabela.gera_cod(rotIf + ":", " ");
        break;

    case 58:
        rotIf = newRotulo();
        stackRot.push(rotIf);
        Tabela.gera_cod(rotIf + ":", "");
        break;

    case 59:
        rotFim = newRotulo();
        stackRot.push(rotFim);

        if(operl == ">")
        {
            Tabela.gera_cod("BLE", rotFim);
        }
        if(operl == "<")
        {
            Tabela.gera_cod("BGE", rotFim);
        }
        if(operl == "==")
        {
            Tabela.gera_cod("BNE", rotFim);
        }
        if(operl == "!=")
        {
            Tabela.gera_cod("BEQ", rotFim);
        }
        if(operl == ">=")
        {
            Tabela.gera_cod("BLT", rotFim);
        }
        if(operl == "<=")
        {
            Tabela.gera_cod("BGT", rotFim);
        }

        break;

    case 60:
        rotIf = stackRot.top();
        stackRot.pop();
        rotFim= stackRot.top();
        stackRot.pop();
        Tabela.gera_cod("JMP", rotFim);
        Tabela.gera_cod(rotIf + ":", "");
        break;

    case 61:
        rotIf = newRotulo();
        stackRot.push(rotIf);
        Tabela.gera_cod(rotIf + ":", "");
        break;

    case 62:

        rotFim = stackRot.top();
        stackRot.pop();
        if(operl == ">")
        {
            Tabela.gera_cod("BLE", rotFim);
        }
        if(operl == "<")
        {
            Tabela.gera_cod("BGE", rotFim);
        }
        if(operl == "==")
        {
            Tabela.gera_cod("BNE", rotFim);
        }
        if(operl == "!=")
        {
            Tabela.gera_cod("BEQ", rotFim);
        }
        if(operl == ">=")
        {
            Tabela.gera_cod("BLT", rotFim);
        }
        if(operl == "<=")
        {
            Tabela.gera_cod("BGT", rotFim);
        }
        break;

    case 63: // - Gera o rótulo de início da rotina

        nome = token->getLexeme();
        Tabela.gera_cod("ROT_"+nome, "");
        break;

    case 64: // - Gera retorno ao ponto de chamada da rotina
        Tabela.gera_cod("RETURN", "0");
        break;

    case 65: // – Armazena o nome da rotina a ser chamada e inicializa o
        //contador de parâmetros
        nome_call = token->getLexeme();
        contpar=1;
        break;

    case 66: // – Copia os valores passados por parâmetros para as
        for ( auto ptr : Tabela.lstSimbolos ) {
            func_aux = ptr.id;
            func_aux = func_aux.substr(0, nome_call.size());
            cout << ptr.id;
            if ( func_aux == nome_call && contpar == ptr.posParam && ptr.funcao == 0)
            {
                busca_nome_funcao = ptr.id;
            }
        }

        ptrSim = Tabela.Find( stackEscopo, token->getLexeme() );

        if(busca_nome_funcao != "")
        {
            if( ptrSim == nullptr )
            {
                Tabela.gera_cod("LDI", token->getLexeme() ); // ver se é valor ou id
            }
            else
            {
                Tabela.gera_cod("LD", token->getLexeme() ); // ver se é valor ou id
            }
            Tabela.gera_cod("STO", busca_nome_funcao );
        }

        func_aux == "";
        busca_nome_funcao = "";
        contpar++;
        break;

    case 67: // Faz a chamada da rotina
        if(contpar-1 < ptrFunc->posParam)
        {
            throw SemanticError( "Faltou parametros na funcao " + ptrFunc->id, token->getPosition() );
        }
        if( contpar-1 > ptrFunc->posParam )
        {
            Tabela.setWarning(*ptrFunc, "Muitos parametros na funcao");
        }
        Tabela.gera_cod("CALL", "_"+nome_call);
        break;

    case 68:
        func = "";
        Tabela.gera_cod("main: ", "");
        break;

    case 69:
        contparFunc += 1;
        break;

        }
}
