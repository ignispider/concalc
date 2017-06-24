#pragma once

#include <string>
#include <vector>
#include <sstream>

using std::string;
using std::vector;
using std::stringstream;

// Constantes arbitrariamente definidas para representar os tipos de tokens

const char number = 'n';			// N�mero literal
const char end_expression = ';';	// Termiandor de express�o
const char command = '#';			// Comando ao programa
const char variable = '$';			// Nome de vari�vel
const char function = ':';			// Nome de fun��o

/*
	Os operadores l�gicos e aritm�ticos n�o precisar�o
	ser representados por constantes. Esses literais
	s�o claros o suficiente para indicar sua fun��o

	ARITIM�TICOS
	'!' Fatorial
	'^' Pot�ncia
	'+' Adi��o ou Positivo
	'-' Subtra��o ou Negativo
	'*' Multiplica��o
	'/' Divis�o
	'%' M�dulo (resto da divis�o)

	L�GICOS
	'!' NOT
	'~' Complemento
	'&' AND
	'^' XOR
	'|' OR
*/


// Armazena os dados de um Token que contem o tipo de
// token, que pode ser um n�mero, nome, ou operador e
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

// Armazena e manipula uma sequ�ncia de Tokens.
class Token_stream
{
public:
	Token_stream ();

	// Retorna o pr�ximo Token da sequ�ncia
	Token get ();
	// Retorna o pr�ximo Token da sequ�ncia
	void operator >> (Token&);
	// Retorna o pr�ximo Token da sequ�ncia
	operator Token();
	// Retorna o pr�ximo Token da sequ�ncia e mant�m a posi��o
	Token peek ();
	// Coloca um Token de volta na fila
	void putback (Token&);
	// Retorna a express�o inserida no console como uma string
	string str () const;
	// Limpa o objeto Token_stream
	void clean ();

private:
	// L� o console e armazena a entrada
	void read_console ();
	
	// Armazena um Token tempor�rio
	Token buffer;
	// Indica se existe um Token dispon�vel no buffer tempor�rio
	bool empty_buffer;
	// Buffer que armazena a entrada lida do console
	stringstream input_stream;
};

// Obejto padr�o definido em token.cpp
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