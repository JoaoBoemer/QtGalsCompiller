# QtGalsCompiller

### #1
Tipo

### #2
Verifica ID existente e adiciona na tabela.
Adiciona ID no STORE.

### #3
Adiciona variável a lista da expressão.
Gera código de Load e ADD ou SUB.

### #4
Adiciona simbolo no ponteiro de atribuição, verifica sua existencia e coloca ID no Store.

### #5
Adiciona Warning caso variável não tenha sido inicializada

### #6
Inicializa variáveis que foram declaradas com valor;

### #7
Adiciona funções na tabela de simbolos e verifica se já há alguma função com mesmo nome no mesmo escopo.

### #8
Finaliza entrada de parametros da função.

### #9
Indica que a variável é um vetor na declaração de variáveis.

### #10
Abertura de escopo

### #11
Fechar escopo

### #12
Passa o tamanho do vetor para a lista de simbolos

### #13
Verifica se a função existe no escopo e determina como usada.

### #14
Gera o código de saida para strings e caracteres.

### #15
Geração de código de saída de dados de Vetor

### #16
Verifica na atribuição se o valor da expressão é compativel com o var.
Marca como inicializado e usado caso a expressão não esteja utilizando ela mesma na atribuição.
Limpa as expressões

### #17
Verifica se a chamada de função retorna o mesmo tipo e inicializa a variável

### #18
Verifica se o resultado da expressão é um Boolean.
(Utilizado em IF, DO, WHILE, FOR)

### #20
Encerra o programa e reinicia todas as vairáveis usadas no semantico.

### #21
Verifica se a saída é válida.
Gera o código de saída para variável.
Inidica como utilizado.

### #22
Verifica se a variável existe para realizar a entrada de dados.
Gera o código de entrada de dados para variáveis.
Indica como inicializado.

### #25
Gera o código de entrada de dados em vetor.

### #26
Identifica se está tentando utilizar uma variável em local de vetor.

### #27
Salva expressão global e limpa expressão atual.

### #28
Carrega expressão global e passa o valor do retorno para a expressão global.

### #29
Identifica se está tentando atribuir um vetor em local de variável.

### #30
Vetor nas expressões
Verifica se é realmente um vetor e existe.
Seta para usado.
Verifica se é a primeira variável da expressão.
Se não for pega um simbolo temporário para realizar cálculo posteriormente no 54.

# Valores

### #31
Int
Gera código de leitura ou ADDI/SUBI
### #32
Float
### #33
Dobule Comentado;
### #34
Char
### #35
String
### #36
Bool

# Simbolos

### #40
Mais
### #41
Menos
### #42
Multiplicação
### #43
Divisão
### #44
MOD (%)
### #45
Relacionais
### #46
Nada / Negate
### #47
Nada / Parenteses

### #51
Realiza a verificação se a expressão é válida.
Verifica a possibilidade da expressão e indica erros ( ex: int * string ) ou warnings.

### #52
Fim da expressão global, gera o STORE para vetor ou para variavel comum.

### #53
Armazena em STORE Temporário vetor a ser atribuido

### #54
Gera código de vetor de expressão, verificando se é ou não a primeira variável.
