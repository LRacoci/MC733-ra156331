/**
 * @file      mips_isa.cpp
 * @author    Sandro Rigo
 *            Marcus Bartholomeu
 *            Alexandro Baldassin (acasm information)
 *
 *            The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   1.0
 * @date      Mon, 19 Jun 2006 15:50:52 -0300
 * 
 * @brief     The ArchC i8051 functional model.
 * 
 * @attention Copyright (C) 2002-2006 --- The ArchC Team
 *
 */

#include  "mips_isa.H"
#include  "mips_isa_init.cpp"
#include  "mips_bhv_macros.H"

/* Inclui a biblioteca do dinero */
extern "C" {
#include "d4-7/d4.h"
}
#include <iostream>
#include <vector>

//If you want debug information for this model, uncomment next line
#define DEBUG_MODEL
#include "ac_debug_model.H"


//!User defined macros to reference registers.
#define Ra 31
#define Sp 29

// 'using namespace' statement to allow access to all
// mips-specific datatypes
using namespace mips_parms;

static int processors_started = 0;
#define DEFAULT_STACK_SIZE (256*1024)


/* Variáveis que indicam se o branch atual foi tomado e os dois bits */
bool tomado = false;
enum BP2bits {SNT, WNT, WT, ST};
BP2bits preditor = SNT;

/* Define qual preditor será utilizado, descomente aquele que será utilizado */
//#define PREDITOR_ALWAYS_TAKEN
#define PREDITOR_ALWAYS_NOT_TAKEN
//#define PREDITOR_2_BITS

/* Define tipo do pipeline, apenas defina uma das abaixo */
#define SUPERESCALAR
//#define PIPE5
//#define PIPE7
//#define PIPE13


#ifdef PIPE5

#define PIPELINE_SIZE 5
#define MISSPREDICT_PENALITY 1

#else
#ifdef PIPE7

#define PIPELINE_SIZE 7
#define MISSPREDICT_PENALITY 2

#else
#ifdef PIPE13

#define PIPELINE_SIZE 13
#define MISSPREDICT_PENALITY 13

#else
#ifdef SUPERESCALAR

#define PIPELINE_SIZE 5
#define MISSPREDICT_PENALITY 1

#endif
#endif
#endif
#endif

/* Contadores de ciclos */
unsigned long int instrucoes = 0;
unsigned long int ciclos_arit = 0;
unsigned long int ciclos_load = 0;
unsigned long int ciclos_branch = 0;
unsigned long int ciclos_total = PIPELINE_SIZE-1;
unsigned long int ciclos_cache = 0;
unsigned long int ciclos_jump = 0;
unsigned long int ciclos_misspredict = 0;

/* Espaço de nomes do C++ */
using namespace std;

/* Variáveis do dinero */
d4cache *Mem, *L2, *L1i, *L1d;

/* Enumerado que representa o tipo da instrução */
enum Tipo {OUTRAS, LOAD, STORE, BRANCH, JUMP, BUBBLE};

/* Classe que representa uma instrução */
class Instrucao {
public:
    Tipo t;
    int rs, rt, rd, dest;

    Instrucao(Tipo t, int rs = -1, int rt = -1, int rd = -1, int dest = -1) {
        this->t = t;
        this->rs = rs;
        this->rt = rt;
        this->rd = rd;
        this->dest = dest;
    }

    /* Imprime a instrução */
    void print(){
        cout << "Tipo: " << t;
        cout << " (rs: " << rs;
        cout << ", rt: " << rt;
        cout << ", rd: " << rd << ")" << endl; 
    }

    void dbg_print(){
        if (dest != -1){
            dbg_printf("r%d := %s(rs:%d, rt:%d)", dest,
                (t == OUTRAS) ? "OUTRAS":
                (t ==  LOAD ) ?  "LOAD" :
                (t == STORE ) ? "STORE" :
                (t == BRANCH) ? "BRANCH": 
                (t ==  JUMP ) ? "JUMP"  : 
                (t == BUBBLE) ? "BUBBLE": 
                "ABSURDO",
            rs,rt);
        }else{
            dbg_printf("%s(rs:%d, rt:%d)", 
                t == OUTRAS ? "OUTRAS":
                t ==  LOAD  ?  "LOAD" :
                t == STORE  ? "STORE" :
                t == BRANCH ? "BRANCH": 
                t ==  JUMP  ? "JUMP"  : 
                t == BUBBLE ? "BUBBLE": 
                "ABSURDO",
            rs,rt);
        }
        
    }
};

ostream& operator<<(ostream& os, const Instrucao& i) {  
    if(i.dest != -1) {
        os << 'r' << i.dest << " := ";
    }
    if (i.t == OUTRAS) os << "OUTRAS"; else
    if (i.t ==  LOAD ) os <<  "LOAD";  else
    if (i.t == STORE ) os << "STORE";  else
    if (i.t == BRANCH) os << "BRANCH"; else
    if (i.t ==  JUMP ) os << "JUMP";   else
    if (i.t == BUBBLE) os << "BUBBLE"; else os << "ABSURDO";

    os << "(rs:" << i.rs << " rt:" << i.rt << ')';
    return os;
} 


/* Classe que representa um histórico de instruções */
class Pipeline {
public:
    vector<Instrucao> p;

/* Se for o processador superescalar é necessário um segundo vetor de instrução */
#ifdef SUPERESCALAR
    vector<Instrucao> p2;
    int last_inserted = 2;
#endif

    Pipeline() {

        p.resize(PIPELINE_SIZE, Instrucao(BUBBLE));
#ifdef SUPERESCALAR
        p2.resize(PIPELINE_SIZE, Instrucao(BUBBLE));
#endif
    }

    unsigned stall(unsigned pos, unsigned cicles){
        pos++;
        for (int i = PIPELINE_SIZE - 1; i > pos; i--) {
            p[i] = p[i - cicles];
        }
        for (int i = 0; i < cicles; i++){
            p[pos+i] = Instrucao(BUBBLE);
        }
        dbg_printf("------------- Stall %d ciclo -------------\n", cicles);
        return cicles;
    }

#ifdef SUPERESCALAR
    unsigned stall_p2(unsigned pos, unsigned cicles){
        pos++;
        for (int i = PIPELINE_SIZE - 1; i > pos; i--) {
            p2[i] = p2[i - cicles];
        }
        for (int i = 0; i < cicles; i++){
            p2[pos+i] = Instrucao(BUBBLE);
        }
        dbg_printf("------------- Stall %d ciclo -------------\n", cicles);
        return cicles;
    }
#endif

    /* Contabiliza os ciclos devido a hazards e erros do branch */
    void update() {
        int i;

#ifdef PIPE5
        /* Checa stall no load seguido por instrução que usa load, e insere uma bolha */
        if (
            (p[2].t == LOAD) and 
            (p[1].t == OUTRAS or p[1].t == STORE) and 
            ((p[2].dest == p[1].rs) or (p[2].dest == p[1].rt))
        ) {
            ciclos_load += stall(1,1);
        } else if (
            (p[2].t == LOAD) and
            (p[1].t == LOAD) and
            (p[2].dest == p[1].rs)
        ) {
            ciclos_load += stall(1,1);
        }

        /* Checa se jump e branches que possuem um único registrador dão stall,
         * ou depois apenas branches que possuem os dois registradores */
        if (
            ((p[0].t == JUMP) and (p[0].rs != -1)) or (
                (p[0].t == BRANCH) and (p[0].rt == -1)
            )
        ) {
            /* Checa load seguido de branch, outras instruções seguidas de branch
             * e branch precedido por qualquer coisa que é precedida por branch */
            if ((p[1].t == LOAD) and (p[0].rs == p[1].dest)) {
                ciclos_branch += stall(0,2);
            } else if (
                (p[1].t == OUTRAS) and (
                    (p[0].rs == p[1].dest)
                )
            ) {
                ciclos_branch += stall(0,1);
            } else if ((p[2].t == LOAD) and (p[0].rs == p[2].dest)) {
                ciclos_branch += stall(0,1);
            }
        } else if (p[0].t == BRANCH) {
            /* Checa load seguido de branch, outras instruções seguidas de branch
             * e branch precedido por qualquer coisa que é precedida por branch */
            if (
                (p[1].t == LOAD) and (
                    (p[0].rs == p[1].dest) or (p[0].rt == p[1].dest)
                )
            ) {
                ciclos_branch += stall(0,2);
            } else if (
                (p[1].t == OUTRAS) and (
                    (p[0].rs == p[1].dest) or (p[0].rt == p[1].dest)
                )
            ) {
                ciclos_branch += stall(0,1);
            } else if (
                (p[2].t == LOAD) and (
                    (p[0].rs == p[2].dest) or (p[0].rt == p[2].dest)
                )
            ) {
                ciclos_branch += stall(1,1);
            }
        }
#else

#ifdef PIPE7
        
        if (p[2].t == LOAD) {
            // Caso novo: 
            /* Checa stall no load seguido qualquer coisa seguido por 
             * instrução que usa o load, insere 1 bolha*/
            if (
                (p[0].t == OUTRAS or p[0].t == STORE) and 
                ((p[2].dest == p[0].rs) or (p[2].dest == p[0].rt))
            ){
                ciclos_load += stall(0,1);
            } else if (
                (p[0].t == LOAD) and
                (p[2].dest == p[0].rs)
            ) {
                ciclos_load += stall(0,1);
            // Caso adaptado do de 5 estágios
            /* Checa stall no load seguido por instrução que usa load, e insere 2 bolhas */
            } else if (
                (p[1].t == OUTRAS or p[1].t == STORE) and 
                ((p[2].dest == p[1].rs) or (p[2].dest == p[1].rt))
            ) {
                ciclos_load += stall(1,2);
            } else if (
                (p[1].t == LOAD) and
                (p[2].dest == p[1].rs)
            ) {
                ciclos_load += stall(1,2);
            }
        }

        /* Checa se jump e branches que possuem um único registrador dão stall,
         * ou depois apenas branches que possuem os dois registradores */
        if (
            ((p[0].t ==  JUMP ) and (p[0].rs != -1)) or 
            ((p[0].t == BRANCH) and (p[0].rt == -1))
        ) {
            /* Checa load seguido de branch, outras instruções seguidas de branch
             * e branch precedido por qualquer coisa que é precedida por branch */
            if (
                (p[1].t == LOAD) and (
                    p[0].rs == p[1].dest
                )
            ) {
                ciclos_branch += stall(0,3);
            } else if (
                (p[1].t == OUTRAS) and (
                    p[0].rs == p[1].dest 
                )
            ){
                ciclos_branch += stall(0,1); // Aqui fica igual o de 5 estagios
            } else if (
                (p[2].t == LOAD) and (
                    p[0].rs == p[2].dest
                )
            ) {
                ciclos_branch += stall(0,2);
            /* Caso novo : Load seguido de duas instruções quaisquer 
             * seguido de jump com dependencia do Load */
            } else if (
                (p[3].t == LOAD) and (
                    p[0].rs == p[3].dest
                )
            ) {
                ciclos_branch += stall(0,1);
            }
        } else if (p[0].t == BRANCH) {
            /* Checa load seguido de branch, outras instruções seguidas de branch
             * e branch precedido por qualquer coisa que é precedida por branch */
            if (
                (p[1].t == LOAD) and (
                    (p[0].rs == p[1].dest) or (p[0].rt == p[1].dest)
                )
            ) {
                ciclos_branch += stall(0,3);
            } else if (
                (p[1].t == OUTRAS) and (
                    (p[0].rs == p[1].dest) or (p[0].rt == p[1].dest)
                )
            ) {
                ciclos_branch += stall(0,1); // Aqui fica igual o de 5 estagios
            } else if (
                (p[2].t == LOAD) and (
                    (p[0].rs == p[2].dest) or (p[0].rt == p[2].dest)
                )
            ) {
                ciclos_branch += stall(0,2);
            /* Caso novo: Load seguido de duas instruções quaisquer 
             * seguido de Branch com dpendencia do Load */
            } else if (
                (p[3].t == LOAD) and (
                    (p[0].rs == p[3].dest) or (p[0].rt == p[3].dest)
                )
            ) {
                ciclos_branch += stall(0,1);
            }
        }
#else

#ifdef PIPE13
        
#endif

#endif

#endif

    }

#ifdef SUPERESCALAR
    void update_superescalar() {

        // HAZARDS PARA A INSTRUÇÃO NO PIPELINE 1

        /* Checa se uma instrução ocorreu logo depois de um load no pipeline 1 ou no 2 e ocorre dependência de um dos seus registradores */
        if (((p[2].t == LOAD) and (p[1].t == OUTRAS or p[1].t == STORE) and ((p[2].dest == p[1].rs) or (p[2].dest == p[1].rt))) or
            ((p2[2].t == LOAD) and (p[1].t == OUTRAS or p[1].t == STORE) and ((p2[2].dest == p[1].rs) or (p2[2].dest == p[1].rt)))
        ) {
            ciclos_load += stall(1,1);
            stall_p2(1,1);
        /* Checa se ocorreu um load após o outro no pipeline 1 ou 2 e existe dependência de registrador */
        } else if (((p[2].t == LOAD) and (p[1].t == LOAD) and (p[2].dest == p[1].rs)) or
            ((p2[2].t == LOAD) and (p[1].t == LOAD) and (p2[2].dest == p[1].rs))
        ) {
            ciclos_load += stall(1,1);
            stall_p2(1,1);
        }

        /* Checa se jump e branches que possuem um único registrador dão stall,
        ou depois apenas branches que possuem os dois registradores */
        if (
            ((p[0].t == JUMP) and (p[0].rs != -1)) or (
            (p[0].t == BRANCH) and (p[0].rt == -1)
            )
        ) {
            /* Checa se a instrução no próximo estágio do pipeline 1 ou 2 é load ou outras instruções e existe dependência.
            Depois checa se a instrução no estágio após o próximo do pipeline 1 ou 2 é load e existe dependência */
            if (((p[1].t == LOAD) and (p[0].rs == p[1].dest)) or ((p2[1].t == LOAD) and (p[0].rs == p2[1].dest))) {
                ciclos_branch += stall(0,2);
                stall_p2(0,2);
            } else if (
                ((p[1].t == OUTRAS) and ((p[0].rs == p[1].dest))) or ((p2[1].t == OUTRAS) and ((p[0].rs == p2[1].dest)))
            ) {
                ciclos_branch += stall(0,1);
                stall_p2(0,1);
            } else if (((p[2].t == LOAD) and (p[0].rs == p[2].dest)) or ((p2[2].t == LOAD) and (p[0].rs == p2[2].dest))) {
                ciclos_branch += stall(1,1);
                stall_p2(1,1);
            }

        } else if(p[0].t == BRANCH) {
            /* Checa se a instrução no próximo estágio do pipeline 1 ou 2 é load ou outra e existe dependência para algum dos
            registradores usados no branch */
            if (
                ((p[1].t == LOAD) and ((p[0].rs == p[1].dest) or (p[0].rt == p[1].dest))) or
                ((p2[1].t == LOAD) and ((p[0].rs == p2[1].dest) or (p[0].rt == p2[1].dest)))
            ) {
                ciclos_branch += stall(0,2);
                stall_p2(0,2);

            } else if (
                ((p[1].t == OUTRAS) and ((p[0].rs == p[1].dest) or (p[0].rt == p[1].dest))) or
                ((p2[1].t == OUTRAS) and ((p[0].rs == p2[1].dest) or (p[0].rt == p2[1].dest)))
            ) {
                ciclos_branch += stall(0,1);
                stall_p2(0,1);

            /* Checa se a instrução no estágio após o próximo do pipeline 1 ou 2 é load e existe dependência para algum dos
            registradores usados no branch */
            } else if (
                ((p[2].t == LOAD) and ((p[0].rs == p[2].dest) or (p[0].rt == p[2].dest))) or 
                ((p2[2].t == LOAD) and ((p[0].rs == p2[2].dest) or (p[0].rt == p2[2].dest)))
            ) {
                ciclos_branch += stall(1,1);
                stall_p2(1,1);
            }
        }

        // HAZARDS PARA A INSTRUÇÃO NO PIPELINE 2

        /* Se uma instrução OUTRAS ou STORE entrou no pipeline 2 junto com uma de LOAD no pipeline 1 e ocorre dependência de registrador */
        if ((p[1].t == LOAD) and (p2[1].t == OUTRAS or p2[1].t == STORE) and ((p[1].dest == p2[1].rs) or (p[1].dest == p2[1].rt))) {
            ciclos_load += stall_p2(1,2);
            stall(0,2);
        /* Se uma instrução OUTRAS ou STORE entrou no pipeline 2 junto com uma de OUTRAS no pipeline 1 e ocorre dependência de registrador */
        } else if ((p[1].t == OUTRAS) and (p2[1].t == OUTRAS or p2[1].t == STORE) and ((p[1].dest == p2[1].rs) or (p[1].dest == p2[1].rt))) {
            ciclos_arit += stall_p2(1,1);
            stall(0,1);
        /* Se uma instrução de LOAD entrou no pipeline 2 junto com uma de LOAD no pipeline 1 e ocorre dependências de registrador */
        } else if ((p[1].t == LOAD) and (p2[1].t == LOAD) and ((p[1].dest == p2[1].rs))) {
            ciclos_load += stall_p2(1,2);
            stall(0,2);
        }

        /* Checa se uma instrução ocorreu logo depois de um LOAD no pipeline 1 ou no 2 e ocorre dependência de um dos seus registradores */
        if (((p[2].t == LOAD) and (p2[1].t == OUTRAS or p2[1].t == STORE) and ((p[2].dest == p2[1].rs) or (p[2].dest == p2[1].rt))) or
            ((p2[2].t == LOAD) and (p2[1].t == OUTRAS or p2[1].t == STORE) and ((p2[2].dest == p2[1].rs) or (p2[2].dest == p2[1].rt)))
        ) {
            ciclos_load += stall_p2(1,1);
            stall(1,1);

        /* Checa se ocorreu um LOAD após o outro no pipeline 1 ou 2 e existe dependência de registrador */
        } else if (((p[2].t == LOAD) and (p2[1].t == LOAD) and (p[2].dest == p2[1].rs)) or
            ((p2[2].t == LOAD) and (p2[1].t == LOAD) and (p2[2].dest == p2[1].rs))
        ) {
            ciclos_load += stall_p2(1,1);
            stall(1,1);
        }

        /* Checa se jump e branches que possuem um único registrador dão stall,
        ou depois apenas branches que possuem os dois registradores */
        if (
            ((p2[0].t == JUMP) and (p2[0].rs != -1)) or (
            (p2[0].t == BRANCH) and (p2[0].rt == -1)
            )
        ) {
            /* Checa se a instrução que entrou junto com o branch ou jump no pipeline 1 é um LOAD e ocorre dependência */
            if ((p[0].t == LOAD) and (p2[0].rs == p[0].dest)) {
                ciclos_branch += stall_p2(0,3);

                /* É necessário dar stall a partir da posíção zero (incluindo ela), inserindo instruções de bolha simulando para fazer isso */
                for(int i = 0; i < 3; i++) {
                    insere_instr(BUBBLE);
                    last_inserted = 2;
                }

            /* Checa se a instrução que entrou junto com o branch ou jump no pipeline 1 escreve no registrador que o branch ou jump utiliza */
            } else if ((p[0].t == OUTRAS) and (p2[0].rs == p[0].dest)) {
                ciclos_branch += stall_p2(0,2);

                /* É necessário dar stall a partir da posíção zero (incluindo ela), inserindo instruções de bolha simulando para fazer isso */
                for(int i = 0; i < 2; i++) {
                    insere_instr(BUBBLE);
                    last_inserted = 2;
                }

            }

            /* Checa se a instrução no próximo estágio do pipeline 1 ou 2 é load ou outras instruções e existe dependência.
            Depois checa se a instrução no estágio após o próximo do pipeline 1 ou 2 é load e existe dependência */
            if (((p[1].t == LOAD) and (p2[0].rs == p[1].dest)) or ((p2[1].t == LOAD) and (p2[0].rs == p2[1].dest))) {

                ciclos_branch += stall_p2(0,2);
                stall(0,2);

            } else if (
                ((p[1].t == OUTRAS) and ((p2[0].rs == p[1].dest))) or ((p2[1].t == OUTRAS) and ((p2[0].rs == p2[1].dest)))
            ) {
                ciclos_branch += stall_p2(0,1);
                stall(0,1);

            } else if (((p[2].t == LOAD) and (p[0].rs == p[2].dest)) or ((p2[2].t == LOAD) and (p2[0].rs == p2[2].dest))) {

                ciclos_branch += stall_p2(1,1);
                stall(1,1);
            }

        } else if(p2[0].t == BRANCH) {

            /* Checa se a instrução que entrou junto com o branch no pipeline 1 é um load */
            if (
                (p[0].t == LOAD) and ((p2[0].rs == p[0].dest) or (p2[0].rt == p[0].dest))
            ) {
                ciclos_branch += stall_p2(0,3);

            /* É necessário dar stall a partir da posíção zero (incluindo ela), inserindo instruções de bolha simulando para fazer isso */
            for(int i = 0; i < 3; i++) {
                insere_instr(BUBBLE);
                last_inserted = 2;
            }

            /* Checa se a instrução que entrou junto com o branch no pipeline 1 escreve em registrador que o branch utiliza */
            } else if (
                (p[0].t == OUTRAS) and ((p2[0].rs == p[0].dest) or (p2[0].rt == p[0].dest))
            ) {
                ciclos_branch += stall_p2(0,2);  
                for(int i = 0; i < 3; i++) {
                    insere_instr(BUBBLE);
                    last_inserted = 2;
                }


            /* Checa se a instrução no próximo estágio do pipeline 1 ou 2 é do tipo load e tem dependência de registrador */
            } else if (
                ((p[1].t == LOAD) and ((p2[0].rs == p[1].dest) or (p2[0].rt == p[1].dest))) or
                ((p2[1].t == LOAD) and ((p2[0].rs == p2[1].dest) or (p2[0].rt == p2[1].dest)))
            ) {
                ciclos_branch += stall_p2(0,2);
                stall(0,2);

            /* Checa se a instrução no próximo estágio do pipeline 1 ou 2 escreve em um registrador que o branch utiliza */
            } else if (
                ((p[1].t == OUTRAS) and ((p2[0].rs == p[1].dest) or (p2[0].rt == p[1].dest))) or
                ((p2[1].t == OUTRAS) and ((p2[0].rs == p2[1].dest) or (p2[0].rt == p2[1].dest)))
            ) {
                ciclos_branch += stall_p2(0,1);
                stall(0,1);

            /* Checa se a instrução no estágio após o próximo no pipeline 1 ou 2 é load e existe dependência para algum dos
            registradores que o branch utiliza */
            } else if (
                ((p[2].t == LOAD) and ((p2[0].rs == p[2].dest) or (p2[0].rt == p[2].dest))) or 
                ((p2[2].t == LOAD) and ((p2[0].rs == p2[2].dest) or (p2[0].rt == p2[2].dest)))
            ) {
                ciclos_branch += stall_p2(1,1);
                stall(1,1);
            }
        }
        
    }
#endif
    /* Insere uma instrução no pipeline e faz os testes de ciclos */
    void insere_instr(Instrucao instr) {
        int i;

#ifndef SUPERESCALAR
        for (i = PIPELINE_SIZE - 1; i > 0; i--) {
            p[i] = p[i - 1];
        }
        p[0] = instr;
#else
        if (last_inserted == 2) {
            for (i = PIPELINE_SIZE - 1; i > 0; i--) {
                p[i] = p[i - 1];
            }
            p[0] = instr;
            last_inserted = 1;
        } else {
            for (i = PIPELINE_SIZE - 1; i > 0; i--) {
                p2[i] = p2[i - 1];
            }
            p2[0] = instr;
            last_inserted = 2;
        }
#endif

        /* Aumenta os stalls de jumps */
        if (instr.t == JUMP) {
#ifdef PIPE5
            ciclos_jump += 1;
#else

#ifdef PIPE7
            ciclos_jump += 2;
#else

#ifdef PIPE13
            ciclos_jump += 13;

#else

#ifdef SUPERESCALAR
            ciclos_jump += 1;
#endif

#endif

#endif

#endif      
        }

        /* Realiza as predições dos branches */
        if (instr.t == BRANCH) {
#ifdef PREDITOR_ALWAYS_TAKEN
            if (tomado == false) {
                ciclos_misspredict+=MISSPREDICT_PENALITY;
            }
#else

#ifdef PREDITOR_ALWAYS_NOT_TAKEN
            if (tomado == true) {
                ciclos_misspredict+=MISSPREDICT_PENALITY;
            }
#else

#ifdef PREDITOR_2_BITS
            if (tomado == true) {
                if (preditor == SNT) {
                    preditor = WNT;
                    ciclos_misspredict+=MISSPREDICT_PENALITY;
                } else if (preditor == WNT) {
                    preditor = WT;
                    ciclos_misspredict+=MISSPREDICT_PENALITY;
                } else {
                    preditor = ST;
                }
            } else {
                if (preditor == ST) {
                    preditor = WT;
                    ciclos_misspredict+=MISSPREDICT_PENALITY;
                } else if (preditor == WT) {
                    preditor = WNT;
                    ciclos_misspredict+=MISSPREDICT_PENALITY;
                } else {
                    preditor = SNT;
                }
            }
#endif

#endif

#endif
        }
#ifndef SUPERESCALAR
        update();
#else
        if (last_inserted == 2) {
            update_superescalar();
        }
#endif
    }

    /* Imprime o pipeline */
    void print(){
        int i;
        cout << "Histórico de instruções:" << endl;
        for (i = 0; i < PIPELINE_SIZE; i++) {
            p[i].print();
        }
        cout << endl;
    }
    /* Imprime o pipeline na dpuração */
    void dbg_print(){
        dbg_printf("Histórico de instruções:");
        for (int i = 0; i < PIPELINE_SIZE; i++) {
            p[i].dbg_print();
            dbg_printf("\n");
        }
    }

};

// Joga Pipeline pra uma stream de saída
ostream& operator<<(ostream& os, const Pipeline& p) {  
    cout << "Histórico de instruções:" << endl;
    cout << "Pipeline 1" << endl;
    for (int i = 0; i < PIPELINE_SIZE; i++) {
        os << p.p[i] << endl;
    }
#ifdef SUPERESCALAR
    cout << "Pipeline 2" << endl;
    for (int i = 0; i < PIPELINE_SIZE; i++) {
        os << p.p2[i] << endl;
    }
#endif
    return os;
} 

/* Pipeline */
Pipeline pip;

/* Parâmetros das Cache L1 e L2 */
#define CACHE_L1_SIZE 12
#define CACHE_L1_ASSOC 4
#define CACHE_L1_BLOCK_SIZE 4
#define CACHE_L2_SIZE 18
#define CACHE_L2_ASSOC 4
#define CACHE_L2_BLOCK_SIZE 4

/* Stalls das caches */
#define CACHE_L1_STALL 15
#define CACHE_L2_STALL 100

/* Função que faz a leitura da cache com o dinero */
void doread(unsigned int addr, d4cache* Cache) {
    d4memref R;
    R.address = (d4addr)addr;
    R.size = 4;
    R.accesstype = D4XREAD;
    d4ref(Cache, R);
}

/* Função que faz a escrita da cache com o dinero */
void dowrite(unsigned int addr, d4cache* Cache) {
    d4memref R;
    R.address = (d4addr)addr;
    R.size = 4;
    R.accesstype = D4XWRITE;
    d4ref(Cache, R);
}

//!Generic instruction behavior method.
void ac_behavior( instruction ) { 
     dbg_printf("----- PC=%#x ----- %lld\n", (int) ac_pc, ac_instr_counter);
    //  dbg_printf("----- PC=%#x NPC=%#x ----- %lld\n", (int) ac_pc, (int)npc, ac_instr_counter);
    instrucoes++;

    ciclos_total++;

    /* Simula leitura de instrução da cache L1i */
    doread((unsigned int) ac_pc, L1i);

#ifndef NO_NEED_PC_UPDATE
    ac_pc = npc;
    npc = ac_pc + 4;
#endif 
};
 
//! Instruction Format behavior methods.
void ac_behavior( Type_R ){}
void ac_behavior( Type_I ){}
void ac_behavior( Type_J ){}
 
//!Behavior called before starting simulation
void ac_behavior(begin) {
    dbg_printf("@@@ begin behavior @@@\n");
    RB[0] = 0;
    npc = ac_pc + 4;
    
    /* Inicializa as caches de instrução e de dados do dinero */
    Mem = d4new(0);
    L2 = d4new(Mem);
    L2->name = (char*)"L2";
    L2->lg2blocksize = CACHE_L2_BLOCK_SIZE;
    L2->lg2subblocksize = 0;
    L2->lg2size = CACHE_L2_SIZE;
    L2->assoc = CACHE_L2_ASSOC;
    L2->replacementf = d4rep_lru;
    L2->prefetchf = d4prefetch_none;
    L2->wallocf = d4walloc_always;
    L2->wbackf = d4wback_always;
    L2->name_replacement = L2->name_prefetch = L2->name_walloc = L2->name_wback = (char*)"L2";
    L1i = d4new(L2);
    L1i->name = (char*)"L1i";
    L1i->lg2blocksize = CACHE_L1_BLOCK_SIZE;
    L1i->lg2subblocksize = 0;
    L1i->lg2size = CACHE_L1_SIZE;
    L1i->assoc = CACHE_L1_ASSOC;
    L1i->replacementf = d4rep_lru;
    L1i->prefetchf = d4prefetch_none;
    L1i->wallocf = d4walloc_always;
    L1i->wbackf = d4wback_always;
    L1i->name_replacement = L1i->name_prefetch = L1i->name_walloc = L1i->name_wback = (char*)"L1i";
    L1d = d4new(L2);
    L1d->name = (char*)"L1d";
    L1d->lg2blocksize = CACHE_L1_BLOCK_SIZE;
    L1d->lg2subblocksize = 0;
    L1d->lg2size = CACHE_L1_SIZE;
    L1d->assoc = CACHE_L1_ASSOC;
    L1d->replacementf = d4rep_lru;
    L1d->prefetchf = d4prefetch_none;
    L1d->wallocf = d4walloc_always;
    L1d->wbackf = d4wback_always;
    L1d->name_replacement = L1d->name_prefetch = L1d->name_walloc = L1d->name_wback = (char*)"L1d";
    int r;
    if (0 != (r = d4setup())) {
        cerr << "Failed\n";
        abort();
    }
    
    // Is is not required by the architecture, but makes debug really easier
    for (int regNum = 0; regNum < 32; regNum ++)
        RB[regNum] = 0;
    hi = 0;
    lo = 0;

    RB[29] =  AC_RAM_END - 1024 - processors_started++ * DEFAULT_STACK_SIZE;
}

//!Behavior called after finishing simulation
void ac_behavior(end) {
    ciclos_cache += (L1d->miss[D4XREAD] + L1d->miss[D4XWRITE] + L1i->miss[D4XREAD]) * CACHE_L1_STALL + (L2->miss[D4XREAD] + L2->miss[D4XWRITE]) * CACHE_L2_STALL;

#ifdef SUPERESCALAR
    ciclos_total = (ciclos_total / 2) + 2;
#endif
    ciclos_total += ciclos_arit + ciclos_load + ciclos_branch + ciclos_jump + ciclos_cache + ciclos_misspredict;
    cout << "Número total de instruções: " << instrucoes << endl;
    cout << "Número total de ciclos totais: " << ciclos_total << endl;
    cout << "Número total de ciclos de hazards aritméticos: " << ciclos_arit << endl;
    cout << "Número total de ciclos de hazards de load: " << ciclos_load << endl;
    cout << "Número total de ciclos de hazards de branch: " << ciclos_branch << endl;
    cout << "Número total de ciclos de stalls de jumps: " << ciclos_jump << endl;
    cout << "Número total de ciclos de stalls de branches errados: " << ciclos_misspredict << endl;
    cout << "Número total de ciclos de stalls de cache: " << ciclos_cache << endl;
    cout << "L1i read miss/fetch: " << L1i->miss[D4XREAD] << "/" << L1i->fetch[D4XREAD] << endl;
    cout << "L1d read miss/fetch: " << L1d->miss[D4XREAD] << "/" << L1d->fetch[D4XREAD] << endl;
    cout << "L1d write miss/fetch: " << L1d->miss[D4XWRITE] << "/" << L1d->fetch[D4XWRITE] << endl;
    cout << "L2 read miss/fetch: " << L2->miss[D4XREAD] << "/" << L2->fetch[D4XREAD] << endl;
    cout << "L2 write miss/fetch: " << L2->miss[D4XWRITE] << "/" << L2->fetch[D4XWRITE] << endl;

    //cout << pip << endl;

    dbg_printf("@@@ end behavior @@@\n");
}


//!Instruction lb behavior method.
void ac_behavior( lb ) {
    char byte;
    dbg_printf("lb r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    byte = DATA_PORT->read_byte(RB[rs]+ imm);
    doread(RB[rs]+ imm, L1d);
    RB[rt] = (ac_Sword)byte ;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(LOAD, rs, rt, -1, rt));
};

//!Instruction lbu behavior method.
void ac_behavior( lbu ) {
    unsigned char byte;
    dbg_printf("lbu r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    byte = DATA_PORT->read_byte(RB[rs]+ imm);
    doread(RB[rs]+ imm, L1d);
    RB[rt] = byte ;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(LOAD, rs, rt, -1, rt));
};

//!Instruction lh behavior method.
void ac_behavior( lh ) {
    short int half;
    dbg_printf("lh r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    half = DATA_PORT->read_half(RB[rs]+ imm);
    doread(RB[rs]+ imm, L1d);
    RB[rt] = (ac_Sword)half ;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(LOAD, rs, rt, -1, rt));
};

//!Instruction lhu behavior method.
void ac_behavior( lhu ) {
    unsigned short int  half;
    half = DATA_PORT->read_half(RB[rs]+ imm);
    doread(RB[rs]+ imm, L1d);
    RB[rt] = half ;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(LOAD, rs, rt, -1, rt));
};

//!Instruction lw behavior method.
void ac_behavior( lw ) {
    dbg_printf("lw r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    RB[rt] = DATA_PORT->read(RB[rs]+ imm);
    doread(RB[rs]+ imm, L1d);
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(LOAD, rs, rt, -1, rt));
};

//!Instruction lwl behavior method.
void ac_behavior( lwl ) {
    dbg_printf("lwl r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    unsigned int addr, offset;
    ac_Uword data;

    addr = RB[rs] + imm;
    offset = (addr & 0x3) * 8;
    data = DATA_PORT->read(addr & 0xFFFFFFFC);
    doread(addr & 0xFFFFFFFC, L1d);
    data <<= offset;
    data |= RB[rt] & ((1<<offset)-1);
    RB[rt] = data;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(LOAD, rs, rt, -1, rt));
};

//!Instruction lwr behavior method.
void ac_behavior( lwr ) {
    dbg_printf("lwr r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    unsigned int addr, offset;
    ac_Uword data;

    addr = RB[rs] + imm;
    offset = (3 - (addr & 0x3)) * 8;
    data = DATA_PORT->read(addr & 0xFFFFFFFC);
    doread(addr & 0xFFFFFFFC, L1d);
    data >>= offset;
    data |= RB[rt] & (0xFFFFFFFF << (32-offset));
    RB[rt] = data;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(LOAD, rs, rt, -1, rt));
};

//!Instruction sb behavior method.
void ac_behavior( sb ) {
    unsigned char byte;
    dbg_printf("sb r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    byte = RB[rt] & 0xFF;
    DATA_PORT->write_byte(RB[rs] + imm, byte);
    dowrite(RB[rs] + imm, L1d);
    dbg_printf("Result = %#x\n", (int) byte);
    pip.insere_instr(Instrucao(STORE, rs, rt, -1, -1));
};

//!Instruction sh behavior method.
void ac_behavior( sh ) {
    unsigned short int half;
    dbg_printf("sh r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    half = RB[rt] & 0xFFFF;
    DATA_PORT->write_half(RB[rs] + imm, half);
    dowrite(RB[rs] + imm, L1d);
    dbg_printf("Result = %#x\n", (int) half);
    pip.insere_instr(Instrucao(STORE, rs, rt, -1, -1));
};

//!Instruction sw behavior method.
void ac_behavior( sw ) {
    dbg_printf("sw r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    DATA_PORT->write(RB[rs] + imm, RB[rt]);
    dowrite(RB[rs] + imm, L1d);
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(STORE, rs, rt, -1, -1));
};

//!Instruction swl behavior method.
void ac_behavior( swl ) {
    /* Aumenta em mais um ciclo por causa das operações de memória */
    ciclos_total++;
    dbg_printf("swl r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    unsigned int addr, offset;
    ac_Uword data;

    addr = RB[rs] + imm;
    offset = (addr & 0x3) * 8;
    data = RB[rt];
    data >>= offset;
    data |= DATA_PORT->read(addr & 0xFFFFFFFC) & (0xFFFFFFFF << (32-offset));
    doread(addr & 0xFFFFFFFC, L1d);
    DATA_PORT->write(addr & 0xFFFFFFFC, data);
    dowrite(addr & 0xFFFFFFFC, L1d);
    dbg_printf("Result = %#x\n", data);
    pip.insere_instr(Instrucao(STORE, rs, rt, -1, -1));
};

//!Instruction swr behavior method.
void ac_behavior( swr ) {
    /* Aumenta em mais um ciclo por causa das operações de memória */
    ciclos_total++;
    dbg_printf("swr r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    unsigned int addr, offset;
    ac_Uword data;

    addr = RB[rs] + imm;
    offset = (3 - (addr & 0x3)) * 8;
    data = RB[rt];
    data <<= offset;
    data |= DATA_PORT->read(addr & 0xFFFFFFFC) & ((1<<offset)-1);
    doread(addr & 0xFFFFFFFC, L1d);
    DATA_PORT->write(addr & 0xFFFFFFFC, data);
    dowrite(addr & 0xFFFFFFFC, L1d);
    dbg_printf("Result = %#x\n", data);
    pip.insere_instr(Instrucao(STORE, rs, rt, -1, -1));
};

//!Instruction addi behavior method.
void ac_behavior( addi ) {
    dbg_printf("addi r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    RB[rt] = RB[rs] + imm;
    dbg_printf("Result = %#x\n", RB[rt]);
    //Test overflow
    if ( ((RB[rs] & 0x80000000) == (imm & 0x80000000)) and
             ((imm & 0x80000000) != (RB[rt] & 0x80000000)) ) {
        fprintf(stderr, "EXCEPTION(addi): integer overflow.\n"); exit(EXIT_FAILURE);
    }
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, rt));
};

//!Instruction addiu behavior method.
void ac_behavior( addiu ) {
    dbg_printf("addiu r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    RB[rt] = RB[rs] + imm;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, rt));
};

//!Instruction slti behavior method.
void ac_behavior( slti ) {
    dbg_printf("slti r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    // Set the RD if RS< IMM
    if( (ac_Sword) RB[rs] < (ac_Sword) imm )
        RB[rt] = 1;
    // Else reset RD
    else
        RB[rt] = 0;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, rt));
};

//!Instruction sltiu behavior method.
void ac_behavior( sltiu ) {
    dbg_printf("sltiu r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    // Set the RD if RS< IMM
    if( (ac_Uword) RB[rs] < (ac_Uword) imm )
        RB[rt] = 1;
    // Else reset RD
    else
        RB[rt] = 0;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, rt));
};

//!Instruction andi behavior method.
void ac_behavior( andi ) {  
    dbg_printf("andi r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    RB[rt] = RB[rs] & (imm & 0xFFFF) ;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, rt));
};

//!Instruction ori behavior method.
void ac_behavior( ori ) {   
    dbg_printf("ori r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    RB[rt] = RB[rs] | (imm & 0xFFFF) ;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, rt));
};

//!Instruction xori behavior method.
void ac_behavior( xori ) {  
    dbg_printf("xori r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    RB[rt] = RB[rs] ^ (imm & 0xFFFF) ;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, rt));
};

//!Instruction lui behavior method.
void ac_behavior( lui ) {   
    dbg_printf("lui r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    // Load a constant in the upper 16 bits of a register
    // To achieve the desired behaviour, the constant was shifted 16 bits left
    // and moved to the target register ( rt )
    RB[rt] = imm << 16;
    dbg_printf("Result = %#x\n", RB[rt]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, rt));
};

//!Instruction add behavior method.
void ac_behavior( add ) {
    dbg_printf("add r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] + RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
    //Test overflow
    if ( ((RB[rs] & 0x80000000) == (RB[rd] & 0x80000000)) and
             ((RB[rd] & 0x80000000) != (RB[rt] & 0x80000000)) ) {
        fprintf(stderr, "EXCEPTION(add): integer overflow.\n"); exit(EXIT_FAILURE);
    }
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction addu behavior method.
void ac_behavior( addu ) {
    dbg_printf("addu r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] + RB[rt];
    //cout << "  RS: " << (unsigned int)RB[rs] << " RT: " << (unsigned int)RB[rt] << endl;
    //cout << "  Result =  " <<  (unsigned int)RB[rd] <<endl;
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction sub behavior method.
void ac_behavior( sub ) {
    dbg_printf("sub r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] - RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
    //TODO: test integer overflow exception for sub
};

//!Instruction subu behavior method.
void ac_behavior( subu ) {
    dbg_printf("subu r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] - RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction slt behavior method.
void ac_behavior( slt ) {   
    dbg_printf("slt r%d, r%d, r%d\n", rd, rs, rt);
    // Set the RD if RS< RT
    if( (ac_Sword) RB[rs] < (ac_Sword) RB[rt] )
        RB[rd] = 1;
    // Else reset RD
    else
        RB[rd] = 0;
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction sltu behavior method.
void ac_behavior( sltu ) {
    dbg_printf("sltu r%d, r%d, r%d\n", rd, rs, rt);
    // Set the RD if RS < RT
    if( RB[rs] < RB[rt] )
        RB[rd] = 1;
    // Else reset RD
    else
        RB[rd] = 0;
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction instr_and behavior method.
void ac_behavior( instr_and ) {
    dbg_printf("instr_and r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] & RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction instr_or behavior method.
void ac_behavior( instr_or ) {
    dbg_printf("instr_or r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] | RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction instr_xor behavior method.
void ac_behavior( instr_xor ) {
    dbg_printf("instr_xor r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] ^ RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction instr_nor behavior method.
void ac_behavior( instr_nor ) {
    dbg_printf("nor r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = ~(RB[rs] | RB[rt]);
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction nop behavior method.
void ac_behavior( nop ) {  
    dbg_printf("nop\n");
    pip.insere_instr(Instrucao(BUBBLE, -1, -1, -1));
};

//!Instruction sll behavior method.
void ac_behavior( sll ) {  
    dbg_printf("sll r%d, r%d, %d\n", rd, rs, shamt);
    RB[rd] = RB[rt] << shamt;
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, -1, rt, rd, rd));
};

//!Instruction srl behavior method.
void ac_behavior( srl ) {
    dbg_printf("srl r%d, r%d, %d\n", rd, rs, shamt);
    RB[rd] = RB[rt] >> shamt;
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, -1, rt, rd, rd));
};

//!Instruction sra behavior method.
void ac_behavior( sra ) {
    dbg_printf("sra r%d, r%d, %d\n", rd, rs, shamt);
    RB[rd] = (ac_Sword) RB[rt] >> shamt;
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, -1, rt, rd, rd));
};

//!Instruction sllv behavior method.
void ac_behavior( sllv ) {
    dbg_printf("sllv r%d, r%d, r%d\n", rd, rt, rs);
    RB[rd] = RB[rt] << (RB[rs] & 0x1F);
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction srlv behavior method.
void ac_behavior( srlv ) {
    dbg_printf("srlv r%d, r%d, r%d\n", rd, rt, rs);
    RB[rd] = RB[rt] >> (RB[rs] & 0x1F);
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction srav behavior method.
void ac_behavior( srav ) {
    dbg_printf("srav r%d, r%d, r%d\n", rd, rt, rs);
    RB[rd] = (ac_Sword) RB[rt] >> (RB[rs] & 0x1F);
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, rd, rd));
};

//!Instruction mult behavior method.
void ac_behavior( mult ) {
    dbg_printf("mult r%d, r%d\n", rs, rt);

    long long result;
    int half_result;

    result = (ac_Sword) RB[rs];
    result *= (ac_Sword) RB[rt];

    half_result = (result & 0xFFFFFFFF);
    // Register LO receives 32 less significant bits
    lo = half_result;

    half_result = ((result >> 32) & 0xFFFFFFFF);
    // Register HI receives 32 most significant bits
    hi = half_result ;

    dbg_printf("Result = %#llx\n", result);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, -1));
};

//!Instruction multu behavior method.
void ac_behavior( multu ) {
    dbg_printf("multu r%d, r%d\n", rs, rt);

    unsigned long long result;
    unsigned int half_result;

    result  = RB[rs];
    result *= RB[rt];

    half_result = (result & 0xFFFFFFFF);
    // Register LO receives 32 less significant bits
    lo = half_result;

    half_result = ((result>>32) & 0xFFFFFFFF);
    // Register HI receives 32 most significant bits
    hi = half_result ;

    dbg_printf("Result = %#llx\n", result);
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, -1));
};

//!Instruction div behavior method.
void ac_behavior( div ) {
    dbg_printf("div r%d, r%d\n", rs, rt);
    // Register LO receives quotient
    lo = (ac_Sword) RB[rs] / (ac_Sword) RB[rt];
    // Register HI receives remainder
    hi = (ac_Sword) RB[rs] % (ac_Sword) RB[rt];
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, -1));
};

//!Instruction divu behavior method.
void ac_behavior( divu ) {
    dbg_printf("divu r%d, r%d\n", rs, rt);
    // Register LO receives quotient
    lo = RB[rs] / RB[rt];
    // Register HI receives remainder
    hi = RB[rs] % RB[rt];
    pip.insere_instr(Instrucao(OUTRAS, rs, rt, -1, -1));
};

//!Instruction mfhi behavior method.
void ac_behavior( mfhi ) {
    dbg_printf("mfhi r%d\n", rd);
    RB[rd] = hi;
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, -1, -1, rd, rd));
};

//!Instruction mthi behavior method.
void ac_behavior( mthi ) {
    dbg_printf("mthi r%d\n", rs);
    hi = RB[rs];
    dbg_printf("Result = %#x\n", (unsigned int) hi);
    pip.insere_instr(Instrucao(OUTRAS, rs, -1, -1, -1));
};

//!Instruction mflo behavior method.
void ac_behavior( mflo ) {
    dbg_printf("mflo r%d\n", rd);
    RB[rd] = lo;
    dbg_printf("Result = %#x\n", RB[rd]);
    pip.insere_instr(Instrucao(OUTRAS, -1, -1, rd, rd));
};

//!Instruction mtlo behavior method.
void ac_behavior( mtlo ) {
    dbg_printf("mtlo r%d\n", rs);
    lo = RB[rs];
    dbg_printf("Result = %#x\n", (unsigned int) lo);
    pip.insere_instr(Instrucao(OUTRAS, rs, -1, -1, -1));
};

//!Instruction j behavior method.
void ac_behavior( j ) {
    dbg_printf("j %d\n", addr);
    addr = addr << 2;
#ifndef NO_NEED_PC_UPDATE
    npc =  (ac_pc & 0xF0000000) | addr;
#endif 
    dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr );
    pip.insere_instr(Instrucao(JUMP, -1, -1, -1, -1));
};

//!Instruction jal behavior method.
void ac_behavior( jal ) {
    dbg_printf("jal %d\n", addr);
    // Save the value of PC + 8 (return address) in $ra ($31) and
    // jump to the address given by PC(31...28)or(addr<<2)
    // It must also flush the instructions that were loaded into the pipeline
    RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
    
    addr = addr << 2;
#ifndef NO_NEED_PC_UPDATE
    npc = (ac_pc & 0xF0000000) | addr;
#endif 
    
    dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr );
    dbg_printf("Return = %#x\n", ac_pc+4);
    pip.insere_instr(Instrucao(JUMP, -1, -1, -1, -1));
};

//!Instruction jr behavior method.
void ac_behavior( jr ) {
    dbg_printf("jr r%d\n", rs);
    // Jump to the address stored on the register reg[RS]
    // It must also flush the instructions that were loaded into the pipeline
#ifndef NO_NEED_PC_UPDATE
    npc = RB[rs], 1;
#endif 
    dbg_printf("Target = %#x\n", RB[rs]);
    pip.insere_instr(Instrucao(JUMP, rs, -1, -1, -1));
};

//!Instruction jalr behavior method.
void ac_behavior( jalr ) {
    dbg_printf("jalr r%d, r%d\n", rd, rs);
    // Save the value of PC + 8(return address) in rd and
    // jump to the address given by [rs]

#ifndef NO_NEED_PC_UPDATE
    npc = RB[rs], 1;
#endif 
    dbg_printf("Target = %#x\n", RB[rs]);

    if( rd == 0 )  //If rd is not defined use default
        rd = Ra;
    RB[rd] = ac_pc+4;
    dbg_printf("Return = %#x\n", ac_pc+4);
    pip.insere_instr(Instrucao(JUMP, rs, -1, rd, rd));
};

//!Instruction beq behavior method.
void ac_behavior( beq ) {
    dbg_printf("beq r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    if( RB[rs] == RB[rt] ){
        tomado = true;
#ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
#endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    } else {
        tomado = false;
    }
    pip.insere_instr(Instrucao(BRANCH, rs, rt, -1, -1));
};

//!Instruction bne behavior method.
void ac_behavior( bne ) {   
    dbg_printf("bne r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    if( RB[rs] != RB[rt] ){
        tomado = true;
#ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
#endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    } else {
        tomado = false;
    }
    pip.insere_instr(Instrucao(BRANCH, rs, rt, -1, -1));
};

//!Instruction blez behavior method.
void ac_behavior( blez ) {
    dbg_printf("blez r%d, %d\n", rs, imm & 0xFFFF);
    if( (RB[rs] == 0 ) or (RB[rs]&0x80000000 ) ){
        tomado = true;
#ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2), 1;
#endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    } else {
        tomado = false;
    }
    pip.insere_instr(Instrucao(BRANCH, rs, -1, -1, -1));
};

//!Instruction bgtz behavior method.
void ac_behavior( bgtz ) {
    dbg_printf("bgtz r%d, %d\n", rs, imm & 0xFFFF);
    if( !(RB[rs] & 0x80000000) and (RB[rs]!=0) ){
        tomado = true;
#ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
#endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    } else {
        tomado = false;
    }
    pip.insere_instr(Instrucao(BRANCH, rs, -1, -1, -1));
};

//!Instruction bltz behavior method.
void ac_behavior( bltz ) {
    dbg_printf("bltz r%d, %d\n", rs, imm & 0xFFFF);
    if( RB[rs] & 0x80000000 ){
        tomado = true;
#ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
#endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    } else {
        tomado = false;
    }
    pip.insere_instr(Instrucao(BRANCH, rs, -1, -1, -1));
};

//!Instruction bgez behavior method.
void ac_behavior( bgez ) {
    dbg_printf("bgez r%d, %d\n", rs, imm & 0xFFFF);
    if( !(RB[rs] & 0x80000000) ){
        tomado = true;
#ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
#endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    } else {
        tomado = false;
    }
    pip.insere_instr(Instrucao(BRANCH, rs, -1, -1, -1));
};

//!Instruction bltzal behavior method.
void ac_behavior( bltzal ) {
    dbg_printf("bltzal r%d, %d\n", rs, imm & 0xFFFF);
    RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
    if( RB[rs] & 0x80000000 ){
        tomado = true;
#ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
#endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    } else {
        tomado = false;
    }
    dbg_printf("Return = %#x\n", ac_pc+4);
    pip.insere_instr(Instrucao(BRANCH, rs, -1, -1, -1));
};

//!Instruction bgezal behavior method.
void ac_behavior( bgezal ) {
    dbg_printf("bgezal r%d, %d\n", rs, imm & 0xFFFF);
    RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
    if( !(RB[rs] & 0x80000000) ){
        tomado = true;
#ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
#endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    } else {
        tomado = false;
    }
    dbg_printf("Return = %#x\n", ac_pc+4);
    pip.insere_instr(Instrucao(BRANCH, rs, -1, -1, -1));
};

//!Instruction sys_call behavior method.
void ac_behavior( sys_call ) {
    dbg_printf("syscall\n");
    stop();
}

//!Instruction instr_break behavior method.
void ac_behavior( instr_break ) {
    fprintf(stderr, "instr_break behavior not implemented.\n"); 
    exit(EXIT_FAILURE);
}
