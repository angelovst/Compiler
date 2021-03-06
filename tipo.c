#include "tipo.h"
#include <iostream>

using namespace std;

Tipo tipo_float = { TIPO_FLOAT_ID, sizeof(float), TIPO_FLOAT_TRAD, &castPadrao, NULL, NULL, NULL };
Tipo tipo_int = { TIPO_INT_ID, sizeof(int), TIPO_INT_TRAD, &castPadrao, NULL, NULL, NULL };
Tipo tipo_bool = { TIPO_BOOL_ID, sizeof(unsigned char), TIPO_BOOL_TRAD, &castPadrao, NULL, NULL, NULL };
Tipo tipo_char = { TIPO_CHAR_ID, sizeof(char), TIPO_CHAR_TRAD, &castPadrao, NULL, NULL, NULL };
Tipo tipo_ptr = { GROUP_PTR, sizeof(char*), TIPO_PTR_TRAD, &castPadrao, NULL, NULL, NULL	};
Tipo tipo_str = { GROUP_STRING, sizeof(char*), TIPO_PTR_TRAD, &castPadrao, NULL, NULL, NULL	};

Tipo tipo_arithmetic_operator = { TIPO_INF_OP_ID, 0, TIPO_INF_OP_TRAD, NULL, &traducaoLAPadrao, NULL, NULL };
Tipo tipo_logic_operator = { TIPO_INF_OP_ID, 0, TIPO_INF_OP_TRAD, NULL, &traducaoLAPadrao, new std::vector<Tipo*>({&tipo_bool}), NULL };
Tipo tipo_atrib_operator = { TIPO_INF_OP_ID, 0, TIPO_INF_OP_TRAD, NULL, &traducaoAtribuicao, NULL, NULL };

//mapa de tipos de ponteiros
std::map<Tipo*, Tipo> tipo_ptrs;
std::map<unsigned int, Tipo> tipos;

//Pilha de variaveis
std::list<Context> contextStack;
unsigned int contextDepth = 0;

string generateVarLabel (void) {
	static unsigned int i = 0;
	return "_tmp" + to_string(i++);
}

string ident (void) {
	std::string identation = "";
	for (unsigned int i = 0; i < contextDepth; i++) {
		identation += "\t";
	}
	return identation;	
}

string newLine (const string &line) {
	return ident() + line + ";\n";
}

int getGroup (Tipo *tipo) {
	return tipo->id&0xFF000000;
}

bool belongsTo (Tipo *tipo, int group) {
	return (tipo->id&group) == group;
}

Tipo* resolverTipo (Tipo *a, Tipo *b) {
	if (a == NULL) {
		return b;
	} else if (b == NULL) {
		return a;
	} else if (a->id < b->id && !belongsTo(b, GROUP_PTR)) {	//b e o tipo do retorno
		return b;
	}
	return a;	//a e o tipo do retorno
}

Tipo* newPtr (Tipo *pointsTo) {
	if (tipo_ptrs.count(pointsTo) == 0) {
		tipo_ptrs[pointsTo] = *pointsTo;
		tipo_ptrs[pointsTo].id |= GROUP_PTR;
		tipo_ptrs[pointsTo].trad = TIPO_PTR_TRAD;
	}

	return &tipo_ptrs[pointsTo];
}

Tipo* nonPtr (Tipo *ptr) {
	Tipo t = *ptr;
	t.id = t.id & ~GROUP_PTR;
	if (tipos.count(t.id) == 0) {
		tipos[t.id] = t;
	}

	return &tipos[t.id];
}

string implicitCast (atributos *var1, atributos *var2, string *label1, string *label2) {
	if (var1->tipo->id == var2->tipo->id) {
		*label1 = var1->label;
		*label2 = var2->label;
		return "";
	}
	if ((getGroup(var1->tipo)&getGroup(var2->tipo)) == 0 || belongsTo(var1->tipo, GROUP_UNCASTABLE) || belongsTo(var2->tipo, GROUP_UNCASTABLE)) {
		return INVALID_CAST;
	}
	//int cast = var1->tipo->id - var2->tipo->id;

	if (var1->tipo->id < var2->tipo->id && !belongsTo(var2->tipo, GROUP_PTR) ) {	//cast var1 para var2
		*label1 = generateVarLabel();
		declararLocal(var2->tipo, *label1);
		*label2 = var2->label;
		return var2->tipo->cast(*label1, var2->tipo, var1->tipo, var1->label);
	}
	//cast var2 para var1
	*label1 = var1->label;
	*label2 = generateVarLabel();
	declararLocal(var1->tipo, *label2);
	return var1->tipo->cast(*label2, var1->tipo, var2->tipo, var2->label);
}

/*FUNCOES DE CAST*/
string castPadrao (std::string &dst, struct _Tipo *selfT, struct _Tipo *fromT, std::string &fromL) {
	string traducao;
	string dstAddr;
	if (belongsTo(fromT, GROUP_PTR)) {
		dstAddr = generateVarLabel();
		declararLocal(&tipo_ptr, dstAddr);
		traducao = newLine(dstAddr + " = (" + TIPO_PTR_TRAD + ")&" + dst) + newLine("memcpy("+dstAddr+", "+fromL+", "+to_string(selfT->size)+")");
	} else {
		traducao = newLine(dst + " = (" + selfT->trad + ")" + fromL);
	}
	return traducao;
}

/*FUNCOES DE OPERADORES*/
string traducaoLAPadrao (void *args)  {
	atributos **atribs = (atributos**)args;
	string **labels = (string**)((atributos**)args+2);
	
	string *retorno = labels[0];
	string *operador = labels[1];
	
	string varALabel;	//labels[1]
	string varBLabel;	//labels[3]
	string ptr;
	
	string cast, traducao = "";

	if (atribs[0]->tipo == NULL || atribs[1]->tipo == NULL) {
		return VAR_UNDECLARED;
	}
	
	if (belongsTo(atribs[0]->tipo, GROUP_PTR)) {
		Tipo *t = nonPtr(atribs[0]->tipo);
		
		varALabel = generateVarLabel();
		ptr = generateVarLabel();
		
		declararLocal(t, varALabel);
		declararLocal(&tipo_ptr, ptr);
		
		traducao += newLine(ptr+" = ("+TIPO_PTR_TRAD+")&"+varALabel);
		traducao += newLine("memcpy("+ptr+", "+atribs[0]->label+", "+std::to_string(t->size)+")");
		atribs[0]->label = varALabel;
		atribs[0]->tipo = t;
	}
	
	if (belongsTo(atribs[1]->tipo, GROUP_PTR)) {
		Tipo *t = nonPtr(atribs[1]->tipo);
		
		varALabel = generateVarLabel();
		ptr = generateVarLabel();
		
		declararLocal(t, varALabel);
		declararLocal(&tipo_ptr, ptr);
		
		traducao += newLine(ptr+" = ("+TIPO_PTR_TRAD+")&"+varALabel);
		traducao += newLine("memcpy("+ptr+", "+atribs[1]->label+", "+std::to_string(t->size)+")");
		atribs[1]->label = varALabel;
		atribs[1]->tipo = t;
	}
	
	cast = implicitCast (atribs[0], atribs[1], &varALabel, &varBLabel);
	if (cast == INVALID_CAST) {
		return INVALID_CAST;
	}	
	
	return traducao + cast + newLine(*retorno + " = " + varALabel + *operador + varBLabel);
}

string traducaoAtribuicao (void *args) {
	atributos **atribs = (atributos**)args;
	atributos *lvalue = atribs[0];
	atributos *rvalue = atribs[1];
	string *retorno = *((string**)((atributos**)args+2));
	string traducao = "";
	string cast;
	
	if (lvalue->tipo == NULL) lvalue->tipo = findVar(lvalue->label);
	//declarar variavel caso ainda nao tenha sido declarada
	if (lvalue->tipo == NULL) {
		//cout << rvalue->tipo->trad << endl;	//debug
		if (!declararLocal(rvalue->tipo, lvalue->label)) {
			return VAR_ALREADY_DECLARED;
		}
		lvalue->tipo = rvalue->tipo;
	}
	
	//cout << hex << rvalue->tipo->id << endl;	//debug
	//if (!belongsTo(lvalue->tipo, GROUP_PTR)) cout << "is not ptr" << endl;	//debug
	
	if (!belongsTo(lvalue->tipo, GROUP_PTR)) {
		Tipo *t = nonPtr(rvalue->tipo);
		int rightGroup = getGroup(lvalue->tipo)&getGroup(t);
		string rlabel, llabel;
		//std::cout << std::hex << lvalue->tipo->id << std::endl;	//debug
		//std::cout << std::hex << t->id << std::endl;	//debug
		//std::cout << std::hex << rightGroup << std::endl;	//debug
		if (rightGroup == 0 || rightGroup != 0 && lvalue->tipo->id < t->id) {
			//std::cout << "here" << std::endl;	//debug
			return INVALID_CAST;
		}
		cast = implicitCast(lvalue, rvalue, &llabel, &rlabel);
		traducao += cast + newLine(llabel + " = " + rlabel) + ((retorno != NULL) ? newLine(*retorno + " = " + llabel) : "");
	} else {
		string rvalueAddr, lvalueAddr;
		Tipo *a, *b;
		string rlabel;
		
		a = lvalue->tipo;
		b = rvalue->tipo;
		
		lvalue->tipo = nonPtr(a);
		rvalue->tipo = nonPtr(b);
		
		if (lvalue->tipo < rvalue->tipo) {
			//return INVALID_CAST;
		}
		
		cast = implicitCast(lvalue, rvalue, &lvalue->label, &rlabel);
		if (cast == INVALID_CAST) {
			return INVALID_CAST;
		}
		
		lvalue->tipo = a;
		rvalue->tipo = b;
		
		traducao += cast + ident() + "//ATTR\n";
		if (!belongsTo(lvalue->tipo, GROUP_STRUCT)) {
			if (!belongsTo(rvalue->tipo, GROUP_PTR)) {
				rvalueAddr = generateVarLabel();
				declararLocal(&tipo_ptr, rvalueAddr);
				traducao += newLine(rvalueAddr + " = (" + TIPO_PTR_TRAD + ")&" + rlabel);
			} else {
				/*
				if (rvalue->tipo->size == 0) {
					return VOID_POINTER;
				}
				*/
				rvalueAddr = rlabel;
			}
			traducao += newLine("memcpy("+lvalue->label+", "+rvalueAddr+", "+to_string(rvalue->tipo->size)+")");
		} else {
			if (!belongsTo(rvalue->tipo, GROUP_STRUCT) || rvalue->tipo->id != lvalue->tipo->id) {
				return INVALID_CAST;
			}
			
			if (belongsTo(rvalue->tipo, GROUP_PTR) && !belongsTo(rvalue->tipo, GROUP_STRUCT)) {
				lvalueAddr = generateVarLabel();
				declararLocal(&tipo_ptr, lvalueAddr);
			
				traducao += newLine(lvalueAddr+" = ("+TIPO_PTR_TRAD+")&"+lvalue->label);
				traducao += newLine("memcpy("+lvalueAddr+", "+rvalue->label+", "+to_string(tipo_ptr.size)+")");
			} else {
				traducao += newLine(lvalue->label+" = "+rvalue->label);
			} 
		}
		
		
	}	

	return traducao;
}

string traducaoOperadores( atributos atr1, atributos atr2, atributos atr3, atributos *atrRetorno) {
	//cout << "Funcao traducaoOperadores : " << atr1.label << " " << atr2.label << " " << atr3.label << endl;	//debug
	void *args[4];
	string traducao, trdParcial;
	
	atrRetorno->label = generateVarLabel();	//retorno
	
	args[0] = &atr1;
	args[1] = &atr3;
	args[2] = &atrRetorno->label;
	args[3] = &atr2.label;
	
	trdParcial = atr2.tipo->traducaoParcial((void*)args);

	if (trdParcial == INVALID_CAST || trdParcial == VAR_ALREADY_DECLARED || trdParcial == VAR_UNDECLARED) {
		return trdParcial;
	}
	
	if (atr2.tipo->retornos == NULL) {	//caso retorno nao seja especificado inferir o tipo
		atrRetorno->tipo = resolverTipo(atr1.tipo, atr3.tipo);
	} else {
		atrRetorno->tipo = (*atr2.tipo->retornos)[0];
	}
	declararLocal(atrRetorno->tipo, atrRetorno->label);
	traducao = atr1.traducao + atr3.traducao;

	return traducao + trdParcial;	
}

//CONTEXTO
void empContexto (void) {
	contextStack.push_front({map<string, Tipo*>(), "", ""});
	contextDepth++;
}

void desempContexto (void) {
	contextStack.pop_front();
	contextDepth--;
}

Tipo* findVar(const string &label) {
	list<Context>::iterator i = contextStack.begin();
	while (i != contextStack.end() && !i->vars.count(label)) {
		i++;
	}		
	return (i == contextStack.end()) ? NULL : i->vars[label];
}

bool declararGlobal (Tipo *tipo, const std::string &label) {
	list<Context>::iterator bottom = contextStack.end();
	bottom--;
	if (bottom->vars.count(label)) {
		return false;
	}
	bottom->declar += tipo->trad + " " + label + ";\n\t";
	bottom->vars[label] = tipo;
	return true;
}

bool declararLocal (Tipo *tipo, const std::string &label) {
	list<Context>::iterator top = contextStack.begin();

	if (top->vars.count(label)) {
		return false;
	}

	top->declar += newLine(tipo->trad + " " + label);	
	top->vars[label] = tipo;
	//cout << "declaracao " << tipo->trad << " " << label << endl;	//debug
	return true;
}
