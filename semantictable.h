
#ifndef SEMANTICTABLE_H
#define SEMANTICTABLE_H
#include <vector>



class SemanticTable
{
public:
    const int ERR = -1;
    const int OK_= 0;
    const int WAR = 1;

    const int INT = 0;
    const int FLO = 1;
    const int CHA = 2;
    const int STR = 3;
    const int BOO = 4;

    const int SUM = 0;
    const int SUB = 1;
    const int MUL = 2;
    const int DIV = 3;
    const int MOD = 4;
    const int REL = 5;


    const std::vector<std::vector<std::vector<int>>> expTable  =
                        {/*       INT           */ /*       FLOAT         */ /*        CHAR        */ /*        STRING        */ /*        BOOL          */
            /*   INT*/ {{INT,INT,INT,FLO,FLO,BOO},{FLO,FLO,FLO,FLO,FLO,BOO},{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR}},
            /* FLOAT*/ {{FLO,FLO,FLO,FLO,FLO,BOO},{FLO,FLO,FLO,FLO,FLO,BOO},{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR}},
            /*  CHAR*/ {{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR}},
            /*STRING*/ {{STR,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,ERR,BOO},{STR,ERR,ERR,ERR,ERR,ERR}},
            /*  BOOL*/ {{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR},{BOO,BOO,ERR,ERR,ERR,ERR}}
    };


    const std::vector<std::vector<int>> atribTable =
                 {/* INT FLO CHA STR BOO  */
            /*INT*/ {OK_,WAR,ERR,ERR,ERR},
            /*FLO*/ {OK_,OK_,ERR,ERR,ERR},
            /*CHA*/ {ERR,ERR,OK_,ERR,ERR},
            /*STR*/ {ERR,ERR,OK_,OK_,ERR},
            /*BOO*/ {ERR,ERR,ERR,ERR,OK_}
    };

    int resultType(int TP1,int TP2,int OP){
        return (expTable[TP1][TP2][OP]);
    }

    int atribType(int TP1,int TP2){
        return (atribTable[TP1][TP2]);
    }

};

#endif // SEMANTICTABLE_H
