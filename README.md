# QtGalsCompiller

### #1
Tipo

### #2
Verifica ID e adiciona na tabela

### #3
Marca utilização de variável e dispara warning se não foi inicializada anteriormente

### #4
Marca a initicalização da variável

### #5
Adiciona Warning caso variável não tenha sido inicializada

### #6
Inicializa variáveis que foram declaradas com valor;

### #7
Adiciona função na tabela

### #8
Finaliza entrada de parametros

### #9
Indica que a variável é um vetor

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
Aleatorio

### #16
Verifica na atribuição se o valor da expressão é compativel com o var.
Marca como inicializado e usado caso a expressão não esteja utilizando ela mesma na atribuição.
Limpa as expressões

### #17
Verifica se a chamada de função retorna o mesmo tipo e inicializa a variável

### #18
Verifica se o resultado da expressão é um Boolean.
(Utilizado em IF, DO, WHILE, FOR)

### #19 
Limpa os valores da expressão e indica que foram utilizados.

### #20
Encerra o programa

### #21
Verifica se a saída é válida.
Gera o código de saída para variável.
Inidica como utilizado.

### #22
Verifica se a variável existe para realizar a entrada de dados.
Gera o código de entrada de dados para variáveis.
Indica como inicializado.

# Valores

### #31
Int
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

### #51
Realiza a verificação e geração de código da expressão.
Verifica a possibilidade da expressão e indica erros ( ex: int * string ) ou warnings.
