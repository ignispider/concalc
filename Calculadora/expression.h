#pragma once

#include "token.h"

// Dados de uma vari�vel definida no programa pelo usu�rio
struct Variable
{
	Variable (const string&, double);
	string name;
	double value;
};
// declara um tipo Variable_list, uma lista de objetos Variable
typedef vector<Variable> Variable_list;

// Armazena e manipula uma express�o na forma de uma sequ�ncia de Tokens
class Expression
{
	typedef vector<Token> Token_list;
	typedef vector<Token>::iterator Offset;
	typedef vector<string> History;
	//typedef unsigned int Bitflag;

	//		Sinalizadores bin�rio de erros de sintaxe
	/*
	const Bitflag good_state = 0x00;
	const Bitflag invalid_expression = 0x01;
	const Bitflag full_buffer = 0x02;
	const Bitflag operator_error = 0x04;
	const Bitflag primary_error = 0x08;
	const Bitflag left_operand_missing = 0x10;
	const Bitflag right_operand_missing = 0x20;
	const Bitflag operator_missing = 0x40;
	const Bitflag unknown_name = 0x80;
	const Bitflag all_flags = 0xFF;
	*/
public:
	Expression ();

	// Retorna o pr�ximo Token da sequ�ncia
	Token get ();
	// Retorna o pr�ximo Token da sequ�ncia
	void operator>> (Token&);
	// Retorna o pr�ximo Token da sequ�ncia e mantem a posi��o
	Token peek ();
	// Adiciona um Token � fila
	void pushback (const Token&);
	// Adiciona um Token � fila
	void operator<< (const Token&);
	// Coloca um Token de volta na fila 
	void putback (const Token&);
	// Retorna a express�o na forma de uma string
	string str ();
	// Adiciona uma string ao hist�rico
	void add_to_history (const string&);
	// Exibe o hist�rico de express�es na forma de string
	void print_history ();
	// Limpa o hist�rico
	void clear_history ();
	// Limpa a lista de express�es
	void clear ();
	// Avalia a sintaxe do Token
	void parse (const Token&);

private:
	Token_list expression_stream;
	Offset expression_index;
	History history;
	//Bitflag error_status;
};

// Objetos padr�o difinido em expression.cpp
extern Expression expression;
extern Variable_list variables;

// Tratamento de erro de express�o
class ExpressionError
	: public std::exception
{
public:
	ExpressionError (const string &what, const string &exp)
		: exception (what.c_str ()), input (exp)
	{}
	ExpressionError (const char *what, const string &exp)
		: exception (what), input (exp)
	{}
	string input;
};

string double_to_string (double);