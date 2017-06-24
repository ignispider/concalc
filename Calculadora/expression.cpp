#include <iostream>
#include "expression.h"

using std::runtime_error;
using std::to_string;

Expression expression;
Variable_list variables;

// Converte um double em string e remove zeros
// a direita da parte fracionária
string double_to_string (double d)
{
	string s = to_string (d);
	
	while (s.back () == '0')
		s.pop_back ();

	if (s.back () == '.')
		s.pop_back ();

	return s;
}

Variable::Variable (const string &s, double d)
	:name (s), value (d)
{}

Expression::Expression ()
{
	expression_index = expression_stream.begin ();
}

Token Expression::get ()
try
{
	Token t;
	if (expression_index == expression_stream.end ())
	{
		input >> t;
		pushback (t);
		++expression_index;
	}
	else
		t = *expression_index++;

	return t;
}
catch (const InputError &ie)
{
	ExpressionError ee (ie.what(), ie.input);
	input.clean ();
	clean ();
	throw ee;
}

void Expression::operator>> (Token &t)
{
	t = get ();
}

Token Expression::peek ()
try
{
	Token t;
	if (expression_index == expression_stream.end ())
	{
		input >> t;
		pushback (t);
	}
	else
		t = *expression_index;

	return t;
}
catch (const InputError &ie)
{
	ExpressionError ee (ie.what (), ie.input);
	input.clean ();
	clean ();
	throw ee;
}

void Expression::pushback (const Token &t)
{
	parse (t);
	int offset = expression_index - expression_stream.begin ();
	expression_stream.push_back (t);
	expression_index = expression_stream.begin () + offset;
}

void Expression::operator<< (const Token &t)
{
	pushback (t);
}

void Expression::putback (const Token &t)
{
	if (expression_index == expression_stream.begin ())
	{
		expression_stream.insert (expression_index, t);
		expression_index = expression_stream.begin ();
		return;
	}

	*--expression_index = t;
}

void Expression::parse (const Token &t)
{
	string exp = input.str ();
	if (t.token == '=' ||
		t.token == command)
	{
		input.clean ();
		clean ();
		string what = "Operador inesperado: '";
		what += t.token;
		what += "'.";
		throw ExpressionError (what, exp);
	}

	string what;

	if (expression_stream.size () == 0)
		switch (t.token)
		{
		case '+': case '-': case '(': case end_expression:
		case number: case variable: case function:
			return;

		// case '*': case '/': case '%': case ')':
		default:
			what = "Primario esperado a esquerda do operador '";
			what += t.token;
			what += "'.";

			input.clean ();
			clean ();
			throw ExpressionError (what, exp);
		}

	// Guia a criação da mensagem de erro

	// operator_missing = true	- Operador esperado
	// operator_missing = false	- Primário esperado
	bool operator_missing = false;
	// left_operand_missing = true	- Operando esquerdo esperado
	// left_operand_missing = false - Operando direito esperado
	bool left_operand_missing = false;

	const Offset previous = expression_index - 1;
	switch (t.token)
	{
	case number: case variable: case function:
		switch (previous->token)
		{
		case ')': case '!': case number: case variable: case function:
			what = "Operador esperado a esquerda do primario '";
			if (t.token == variable ||
				t.token == function)
				what += t.name;
			else
				what += double_to_string (t.value);

			what += "'.";

			input.clean ();
			clean ();
			throw ExpressionError (what, exp);

		//case '+': case '-': case '*': case '/':
		//case '%': case '(': case '^':
		default:
			return;
		}
		break;

	case '+': case '-':
		switch (previous->token)
		{
		case '(': case ')': case '!':
		case number: case variable: case function:
			return;

		// case '+': case '-': case '*': case '/': case '%': case '^':
			// Primario esperado a direita do operador anterior
			// operator_missing = false;
			// left_opernad_missing = false;
		}
		break;

	case '*': case '/': case '%': case '^':
		switch (previous->token)
		{
		case ')': case '!': case number: case variable: case function:
			return;

		case '*': case '/': case '%': case '^':
			// Primario esperado a direita do operador anterior
			// operator_missing = false;
			// left_operand_mnissing = false;
			break;

		// case '+': case '-': case '(':
		default:
			// Primario esperado a esquerda do operador atual
			// operator_missing = false;
			left_operand_missing = true;
		}
		break;

	case '(':
		switch (previous->token)
		{
		case ')': case '!': case number: case variable: case function:
			// Operador esperado a esquerda do operado atual
			operator_missing = true;
			left_operand_missing = true;
			break;

		//case '*': case '/': case '%': case '^':
		//case '+': case '-': case '(':
		default:
			return;
		}
		break;

	case ')':
		switch (previous->token)
		{
		case ')': case '!': case number: case variable: case function:
			return;

		//case '*': case '/': case '%': case '^':
		//case '+': case '-': case '(':
		default:
			// Primario esperado a direita do operador anterior
			// operator_missing = false;
			// left_operand_missing = false;
			break;
		}
		break;

	case '!':
		switch (previous->token)
		{
		case ')': case number: case variable: case function:
			return;

		//case '*': case '/': case '%': case '^':
		//case '+': case '-': case '(': case '!':
		default:
			// Primario esperado a esquerda do operador atual
			// operator_missing = false;
			left_operand_missing = true;
		}
		break;

	case end_expression:
		switch (previous->token)
		{
		case ')': case '!': case number: case variable: case function:
			return;

		//case '+': case '-': case '*': case '/':
		//case '%': case '^': case '(':
		default:
			// Primario esperado a direita do operador anterior
			// operator_missing = false;
			//left_operand_missing = false;
			break;
		}
		break;
	}

	what = (operator_missing ? "Operador" : "Primario");
	what += " esperado a ";
	what += (left_operand_missing ? "esquerda" : "direita");
	what += " do operador '";
	what += (left_operand_missing ? t.token : previous->token);
	what += "'.";

	input.clean ();
	clean ();
	throw ExpressionError (what, exp);
}

string Expression::str ()
{
	if (expression_stream.size() == 0 ||
		expression_stream.size () == 1 &&
		expression_stream [0].token == end_expression)
		return "";

	string s;
	Offset current = expression_stream.begin ();

	if (current->token == number)
		s += double_to_string (current->value);
	else if (current->token == variable ||
			 current->token == function)
		s += current->name;
	else
		s += current->token;

	++current;
	Offset previous = current - 1;

	while (current != expression_stream.end ())
	{
		switch (current->token)
		{
		case number: case variable: case function:
			switch (previous->token)
			{
			case '(':
				break;

			case '+':	case '-':
				if (previous == expression_stream.begin () ||
					(previous - 1)->token == '(')
					break;
				// else
				//	Intencionalmente cai em default:

			default:
				s += ' ';
			}
			if (current->token == number)
				s += double_to_string (current->value);
			else
				s += current->name;
			break;

		case '+':	case '-':
			switch (previous->token)
			{
			case '(':
				break;

			default:
				s += ' ';
			}
			s += current->token;
			break;

		case '*': case '/': case '%': case '^':
			s += ' ';
			s += current->token;
			break;

		case '(':
			switch (previous->token)
			{
			case '(':
				break;

			default:
				s += ' ';
			}
			s += current->token;
			break;

		case ')':	case '!':
			s += current->token;
			break;

		case end_expression:
			return s;
		}

		++current;
		++previous;
	}

	return s;
}

void Expression::add_to_history (const string &s)
{
	if (s.size() == 0)
		return;

	history.push_back (s);
}

void Expression::print_history ()
{
	if (history.size () == 0)
	{
		std::cout << "Nenhuma expressao inserida.\n";
		return;
	}

	for each (string exp in history)
	{
		std::cout << exp << '\n';
	}
}

void Expression::clean ()
{
	expression_stream.clear ();
	expression_index = expression_stream.begin ();
}
