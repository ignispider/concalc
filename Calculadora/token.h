#pragma once

#include <string>
#include <vector>
#include <sstream>

using std::string;
using std::vector;
using std::stringstream;

// Constantes arbitrariamente definidas para representar os tipos de tokens

const char number = 'n';			// Número literal
const char end_expression = ';';	// Termiandor de expressão
const char command = '#';			// Comando ao programa
const char variable = '$';			// Nome de variável
const char function = ':';			// Nome de função

/*
	Os operadores lógicos e aritméticos não precisarão
	ser representados por constantes. Esses literais
	são claros o suficiente para indicar sua função

	ARITIMÉTICOS
	'!' Fatorial
	'^' Potência
	'+' Adição ou Positivo
	'-' Subtração ou Negativo
	'*' Multiplicação
	'/' Divisão
	'%' Módulo (resto da divisão)

	LÓGICOS
	'!' NOT
	'~' Complemento
	'&' AND
	'^' XOR
	'|' OR
*/


// Armazena os dados de um Token que contem o tipo de
// token, que pode ser um número, nome, ou operador e
// seu respectivo valor
struct Token
{
	typedef char token_type;

	Token ();
	Token (token_type);
	Token (token_type, double);
	Token (token_type, string&);

	token_type token{};
	double value{};
	string name;
};

// Armazena e manipula uma sequência de Tokens.
class Token_stream
{
public:
	Token_stream ();

	// Retorna o próximo Token da sequência
	Token get ();
	// Retorna o próximo Token da sequência
	void operator >> (Token&);
	// Retorna o próximo Token da sequência
	operator Token();
	// Retorna o próximo Token da sequência e mantém a posição
	Token peek ();
	// Coloca um Token de volta na fila
	void putback (Token&);
	// Retorna a expressão inserida no console como uma string
	string str () const;
	// Limpa o objeto Token_stream
	void clean ();

private:
	// Lê o console e armazena a entrada
	void read_console ();
	
	// Armazena um Token temporário
	Token buffer;
	// Indica se existe um Token disponível no buffer temporário
	bool empty_buffer;
	// Buffer que armazena a entrada lida do console
	stringstream input_stream;
};

// Obejto padrão definido em token.cpp
extern Token_stream input;

// Tratamento de erro de entrada
class InputError
	: public std::exception
{
public:
	InputError (const string &what, const string &exp)
		: exception (what.c_str ()), input(exp)
	{}
	InputError (const char *what, const string &exp)
		: exception (what), input (exp)
	{}
	string input;
};