/*
	TODO:
	- Implementar o reconhecimento de literais com vírgula
*/

#include <iostream>
#include "token.h"

using std::cout;
using std::cin;

// Objeto padrão usado no programa
Token_stream input;

bool isoperator (char c)
{
	// Lista de operadores disponíveis
	switch (c)
	{
	case '+':	// Adição ou positivo
	case '-':	// Subtração ou negativo
	case '/':	// Divisão
	case '*':	// Multipicação
	case '%':	// Módulo
	case '!':	// Fatorial
	case '^':	// Potência
	case '(':
	case ')':
	case '=':
	case end_expression:
	case variable:
	case function:
	case command:
		return true;
	default:
		return false;
	}
}

Token::Token ()
	: token (0), value (0.0)
{}

Token::Token (token_type c)
	: token (c), value (0.0)
{}

Token::Token (token_type c, double d)
	: token (c), value (d)
{}

Token::Token (token_type c, string &s)
	: token (c), value (0.0), name (s)
{}

Token_stream::Token_stream ()
	: empty_buffer (true)
{}

void Token_stream::read_console ()
{
	input_stream.clear ();
	cin.clear ();

	while (isspace (cin.peek ()))
		cin.ignore ();
	
	if (cin.peek () == command)
	{
		input_stream << (char)cin.get ();
		while (isalpha (cin.peek ()))
		{
			input_stream << (char)cin.get ();
		}
	}
	else
	{
		while (cin.peek () != end_expression &&
			   cin.peek () != '\n')
		{
			input_stream << (char)cin.get ();
		}

		if (cin.get () == end_expression)
			input_stream << end_expression;
		else
			cin.putback ('\n');
	}
}

Token Token_stream::get ()
{
	if (!empty_buffer)
	{
		empty_buffer = true;
		return buffer;
	}

	char c{};
	while (c == 0)
	{
		if (input_stream.peek () == EOF)
			read_console ();

		input_stream >> c;
	}

	Token t;
	switch (c)
	{
	case '+': case '-': case '*': case '/': case '%':
	case '(': case ')': case '=': case '!': case '^':
	case end_expression:
		t = Token (c);
		return t;
		
	case '.':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	{
		input_stream.putback (c);
		double d{};
		input_stream >> d;

		if (!input_stream.good ())
		{
			if (input_stream.fail ())
				throw InputError ("Literal invalido.", str ());
			else if (input_stream.bad ())
				throw InputError ("Falha na leitura da expressao.", str ());
		}
		
		t = Token (number, d);
	}
	return t;

	case command: case variable: case function:
		t = Token (c);
		{
			c = input_stream.get ();
			if (isalpha (c))
			{
				string s;
				while (isalnum (c) || c == '_')
				{
					s += c;
					c = input_stream.get ();
				}
				t.name = s;
				input_stream.putback (c);
				return t;
			}
			// else
			// Intencionalmente cai em default:
		}

	default:
	{
		string what = "Termo invalido: '";
		string s;
		while (!isoperator (c) && !isspace (c) && !isdigit (c) && c != EOF)
		{
			s += c;
			c = input_stream.get ();
		}
		if (s.size () == 0)
		{
			what = "Nenhum nome de ";
			switch (t.token)
			{
			case command:
				what += "comando";
				break;

			case variable:
				what += "variavel";
				break;

			case function:
				what += "funcao";
				break;
			}
			what += " fornecido.";
			throw InputError (what, str ());
		}
		what += s;
		what += "'.";
		throw InputError (what, str ());
	} // default:
	}
}

void Token_stream::operator>>(Token &t)
{
	t = get ();
}

Token_stream::operator Token()
{
	return get ();
}

void Token_stream::putback (Token &t)
{
	if (!empty_buffer)
		throw InputError ("Espaco indisponivel no buffer para receber o Token.", str ());

	buffer = t;
	empty_buffer = false;
}

Token Token_stream::peek ()
{
	if (!empty_buffer)
		return buffer;

	putback (get ());
	
	return peek ();
}

string Token_stream::str () const
{
	return input_stream.str ();
}

void Token_stream::clean ()
{
	empty_buffer = true;
	input_stream = stringstream ();
}
