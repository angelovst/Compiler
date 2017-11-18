#ifndef HELPER_INCLUDED_H
#define HELPER_INCLUDED_H

#include <string>
#include <vector>
#include <map>
#include <list>
#include <stdlib.h>

#define TIPO_INT_TRAD "int"
#define TIPO_FLOAT_TRAD "float"
#define TIPO_BOOL_TRAD "unsigned char"
#define TIPO_CHAR_TRAD "char"
#define TIPO_LIST_TRAD "void*"
#define TIPO_INF_OP_TRAD ""

//id consiste em duas partes, primeiros 8bits definem a qual grupo o tipo pertence, os demais definem qual nivel ele esta
//tipos podem ser convertidos de um para outro caso pertencam ao mesmo grupo, um tipo pode ser implicitamente convertido para outro caso tenha nivel menor
#define GROUP_NUMBER		0x01000000
#define GROUP_BOOL			0x02000000
#define GROUP_CHAR			0x04000000
#define GROUP_CONTAINER 	0x08000000
#define GROUP_FUNCTION		0x10000000
#define GROUP_STRUCT		0x20000000
#define GROUP_UNCASTABLE	0x40000000	

#define TIPO_INT_ID		GROUP_NUMBER|0x01
#define TIPO_FLOAT_ID	GROUP_NUMBER|0x04

#define TIPO_BOOL_ID	GROUP_BOOL|0x01

#define TIPO_CHAR_ID	GROUP_CHAR|0x01

#define TIPO_LIST_ID	GROUP_CONTAINER|0x01

#define TIPO_INF_OP_ID	GROUP_FUNCTION|0x01

#define TIPO_STRUCT_ID	GROUP_STRUCT|GROUP_UNCASTABLE|0x00

#define INVALID_CAST "invalid cast"
#define VAR_ALREADY_DECLARED "already declared"

#define BOOL_TRUE "true"
#define BOOL_FALSE "false"

typedef struct _CustomType {
	Tipo tipo;
	std::map<std::string, Tipo *tipo> signature;
} CustomType;

typedef struct _Tipo {
	unsigned int id;
	size_t size;
	std::string trad;
	std::string (*traducaoParcial)(void *args);
	std::vector<struct _Tipo*> *retornos;		//usado em funcoes
	std::vector<struct _Tipo*> *argumentos;	//usado em funcoes
	CustomType &customType;
} Tipo;

typedef struct _LoopLabel {
	std::string inicio;		
	std::string progressao;
	std::string fim;
} LoopLabel;

typedef struct _Context {
	std::map<std::string, Tipo*> vars;
	std::string declar;
	std::map<std::string, CustomType> customTypes;
	std::list<void*> garbageCollector;
} Context;

typedef struct atributos {
	std::string label;
	std::string traducao;
	Tipo *tipo;
} atributos;

std::string ident (void);
std::string newLine (const std::string &line);	//escreve nova linha do codigo intermediario

//VARIAVEIS
std::string generateVarLabel (void);
int getGroup (Tipo *tipo);
bool belongsTo (Tipo *tipo, int group);
Tipo* resolverTipo (Tipo *a, Tipo *b);	//decide implicitamente o tipo do retorno entre uma operacao envolvendo a e b
std::string implicitCast (atributos *var1, atributos *var2, std::string *label1, std::string *label2);	//faz cast implicito dos tipos var1 e var2 e atribui os labels das variaveis em label1 e label2

//CONTEXTO
void empContexto (void);
void desempContexto (void);
Tipo* findVar(std::string &label);
bool declararGlobal (Tipo *tipo, std::string &label);
bool declararLocal (Tipo *tipo, std::string &label);

//LOOP
void empLoop (void);
void desempLoop (void);
LoopLabel* getLoop (unsigned int out);
LoopLabel* getOuterLoop (void);
std::string generateLabel (void);

//FUNCOES DE OPERADORES
std::string traducaoLAPadrao (void *args);	//args = (atributos *varA, atributos *varB, string *retorno, string *operador)
std::string traducaoAtribuicao (void *args);	//args = (atributos *varA, atributos *varB, string *retorno)

//VARIAVEIS GLOBAIS
extern unsigned int line;	//linha na qual o parser esta, usado para erros

//Pilha de variaveis
extern unsigned int contextDepth;
extern std::list<Context> contextStack;

//Pilha de labels
extern std::list<LoopLabel> loopStack;

//String para declaracao de var
extern std::string varDeclar;

//Tipos
extern Tipo tipo_float;
extern Tipo tipo_int;
extern Tipo tipo_bool;
extern Tipo tipo_char;
extern Tipo tipo_list;
extern Tipo tipo_ref;

extern Tipo tipo_arithmetic_operator;
extern Tipo tipo_logic_operator;
extern Tipo tipo_atrib_operator;

#endif
