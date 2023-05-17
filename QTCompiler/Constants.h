#ifndef CONSTANTS_H
#define CONSTANTS_H

enum TokenId 
{
    EPSILON  = 0,
    DOLLAR   = 1,
    t_BOOLEAN_VALUE = 2,
    t_ID = 3,
    t_USING = 4,
    t_CONST = 5,
    t_BREAK = 6,
    t_RETURN = 7,
    t_COUT = 8,
    t_CIN = 9,
    t_DEFAULT = 10,
    t_CONTINUE = 11,
    t_PRINTF = 12,
    t_SCANF = 13,
    t_FUNCTION = 14,
    t_IMPORT = 15,
    t_INT = 16,
    t_FLOAT = 17,
    t_CHAR = 18,
    t_STRING = 19,
    t_BOOL = 20,
    t_DOUBLE = 21,
    t_VOID = 22,
    t_MAIN = 23,
    t_IF = 24,
    t_ELSE = 25,
    t_SWITCH = 26,
    t_CASE = 27,
    t_FOR = 28,
    t_WHILE = 29,
    t_DO = 30,
    t_OR = 31,
    t_AND = 32,
    t_MAIOR = 33,
    t_MENOR = 34,
    t_MENOR_IGUAL = 35,
    t_MAIOR_IGUAL = 36,
    t_MAIS = 37,
    t_MENOS = 38,
    t_DIVISAO = 39,
    t_MOD = 40,
    t_MULTIPLICACAO = 41,
    t_ATRIBUICAO = 42,
    t_IGUAL = 43,
    t_INVERTER = 44,
    t_DIFERENTE = 45,
    t_RIGHTSHIFT = 46,
    t_LEFTSHIFT = 47,
    t_BITWISE_AND = 48,
    t_BITWISE_OR = 49,
    t_BITWISE_NOT = 50,
    t_BITWISE_XOR = 51,
    t_PONTO = 52,
    t_VIRGULA = 53,
    t_DOIS_PONTOS = 54,
    t_PONTO_E_VIRGULA = 55,
    t_ABRIR_COLCHETES = 56,
    t_FECHAR_COLCHETES = 57,
    t_ABRIR_PARENTESES = 58,
    t_FECHAR_PARENTESES = 59,
    t_ABRIR_CHAVES = 60,
    t_FECHAR_CHAVES = 61,
    t_CONTRABARRA = 62,
    t_INTERROGACAO = 63,
    t_INT_VALUE = 64,
    t_FLOAT_VALUE = 65,
    t_DOUBLE_VALUE = 66,
    t_BIN_VALUE = 67,
    t_HEX_VALUE = 68,
    t_STRING_VALUE = 69,
    t_CHAR_VALUE = 70
};

const int STATES_COUNT = 62;

extern int SCANNER_TABLE[STATES_COUNT][256];

extern int TOKEN_STATE[STATES_COUNT];

extern int SPECIAL_CASES_INDEXES[72];

extern const char *SPECIAL_CASES_KEYS[27];

extern int SPECIAL_CASES_VALUES[27];

extern const char *SCANNER_ERROR[STATES_COUNT];

const int FIRST_SEMANTIC_ACTION = 137;

const int SHIFT  = 0;
const int REDUCE = 1;
const int ACTION = 2;
const int ACCEPT = 3;
const int GO_TO  = 4;
const int ERROR  = 5;

extern const int PARSER_TABLE[339][189][2];

extern const int PRODUCTIONS[148][2];

extern const char *PARSER_ERROR[339];

#endif
