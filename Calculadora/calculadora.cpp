/*
	Calculadora		Realiza operações matemáticas e lógicas no console
	Versão: 0.2

	TODO:
	- Implementar a parte de operações lógicas
	- Criar opcões de comandos para:
		- Encerrar o programa				[V]
		- Exibir resultado parcial			[X]
		- Criar variáveis					[V]
		- Exibir lista de variáveis			[X]
		- Limpar lista de variáveis			[X]
		- Exibir histórico de expressões	[V]
		- Limpar histórico de expressões	[X]

	Criado da criação: 01/06/2017
	Desenvolvido por: Ignispider

	Changelog:
	* 20/06/2017 *
	- Correção de bugs

	* 14/06/2017 *
	- Reestruturação e reorganização das classes e funções.
		- struct Token: Armazena o tipo e o valor de um Token.
		- class Token_stream: Lê o console, armazena a entrada
		  e cria um Token.
		- class Expression: Armazena uma lista de Token e realiza
		  diversas operações com Tokens.

	* 09/06/2017 *
	- Implementação das operações matemáticas.
	- Mudança do buffer da classe Token_stream de string para stringstream
	  e consequentemente todas as funções dentro do objeto que lidam com
	  entrada não manipulam o objeto cin diretamente, com exceção da
	  função read_input que lê o console e armazena a entrada no objeto
	  stringstream expression_string.
*/

#include <iostream>
#include "expression.h"

using std::cout;
using std::cin;

double calculate_expression ();

// Realiza a operação Mod '%' em valores inteiros e não inteiros
double modulo (double dividend, double divisor)
{
	long long quotient = static_cast<long long>(dividend / divisor);
	
	return dividend - (quotient * divisor);
}

double factorial (double n)
{

	if (n < 0.0)
	{
		//ts.setflag (invalid_expression);
		string exp = input.str ();
		input.clean ();
		expression.clean ();
		throw ExpressionError ("Fatorial de numero negativo.", exp);
	}
	else if (n == 0.0 || n == 1.0)
		return 1.0;

	double result = n;
	while(n > 1)
		result *= --n;

	return result;
}

bool get_variable (const string &s, double *d = nullptr)
{
	for each (Variable var in variables)
	{
		if (var.name == s)
		{
			if (d)
				*d = var.value;
			return true;
		}
	}

	return false;
}

bool get_function (const string &s, double *d = nullptr)
{
	throw ExpressionError ("", "A funcao is_function nao foi definida.");
}

double primary ()
{
	Token t;
	expression >> t;
	double value = 0.0;
	bool negative = false;

	switch (t.token)
	{
	case number:
		value = t.value;
		break;

	case '-':	// Negativo, não subtração
		negative = true;
		// Intencionalmente cai em case '+':
	case '+':	// Positivo, não adição
		value = primary ();
		break;

	case '(':
	{
		value = calculate_expression ();
		expression >> t;
		if (t.token != ')')
		{
			ExpressionError ee ("')' esperado.", input.str ());
			input.clean ();
			expression.clean ();
			throw ee;
		}
	}
	break;

	case ')':
	{
		ExpressionError ee ("'(' esperado.", input.str ());
		input.clean ();
		expression.clean ();
		throw ee;
	}

	case variable:
		if (!get_variable (t.name, &value))
		{
			ExpressionError ee ("Variavel '" + t.name + "' nao declarada.", input.str ());
			input.clean ();
			expression.clean ();
			throw ee;
		}
		break;

	case function:
		if (!get_function (t.name, &value))
		{
			ExpressionError ee ("Funcao '" + t.name + "' nao declarada.", input.str ());
			input.clean ();
			expression.clean ();
			throw ee;
		}
		break;
	}

	expression >> t;
	if (t.token == '!')
	{
		if (value - static_cast<long long>(value) != 0.0)
		{
			ExpressionError ee ("O operando de '!' deve ser de tipo inteiro.", input.str ());
			input.clean ();
			expression.clean ();
			throw ee;
		}
		value = factorial (value);
	}
	else
		expression.putback (t);

	return (negative ? -value : value);
}

double sub2 ()
{
	double value = primary ();

	Token t;
	while (true)
	{
		expression >> t;
		switch (t.token)
		{
		case '^':
			value = pow (value, primary ());
			break;

		default:
			expression.putback (t);
			return value;
		}
	}
}

double sub1 ()
{
	double value = sub2 ();

	Token t;
	while (true)
	{
		expression >> t;
		switch (t.token)
		{
		case '*':
			value *= sub2 ();
			break;

		case '/':
		{
			double d = sub2 ();
			if (d == 0)
			{
				ExpressionError ee ("Divisao por 0.", input.str ());
				input.clean ();
				expression.clean ();
				throw ee;
			}
			value /= d;
		}
		break;

		case '%':
			value = modulo (value, sub2 ());
			break;

		default:
			expression.putback (t);
			return value;
		}
	}
}


double calculate_expression ()
{
	double value = sub1 ();

	Token t;
	while (true)
	{
		expression >> t;
		switch (t.token)
		{
		case '+':
			value += sub1 ();
			break;

		case '-':
			value -= sub1 ();
			break;

		case end_expression:
			input.putback (t);
			return value;

		default:
			expression.putback (t);
			return value;
		}
	}
}

void prompt ()
{
	if (cin.peek () == '\n')
	{
		cin.ignore ();
		cout << ":> ";
	}
}

double declaration ()
{
	Token t;
	string expression_str = "#var $";

	input >> t;
	if(t.token != variable)
	{
		expression_str = "Token '$' esperado.";
	}
	else if (!get_variable (t.name))
	{
		string variable_name = t.name;
		expression_str += t.name;
		input >> t;
		if (t.token != '=')
			expression_str = "'=' esperado.";
		else
		{
			expression_str += " = ";
			double d = calculate_expression ();
			input >> t;
			if (t.token == end_expression)
			{
				expression_str += expression.str ();
				expression.add_to_history (expression_str);
				variables.push_back (Variable (variable_name, d));

				cout << "$" << variable_name << " = " << d << '\n';
				input.clean ();
				expression.clean ();
				
				prompt ();
				return d;
			}
			else
				expression_str = "Operador ';' esperado.";
		}
	}
	else
		expression_str = "A variavel '$" + t.name + "' ja foi declarada.";

	ExpressionError ee (expression_str, input.str ());
	input.clean ();
	expression.clean ();
	throw ee;
}

int main ()
{
	cout << "\tCalculadora Aritimetica\n"
		<< "Insira uma expressao aritimetica terminando com '" << end_expression << "'.\n"
		<< "Digite '" << command << "ajuda' para exibir uma tela de ajuda ou '"
		<< command <<"sair' para terminar.\n\n"
		<< ":> ";

	Token t;
	double result{};
	while (true)
	{
		try
		{
			input >> t;
			switch (t.token)
			{
			case command:
				if (t.name == "sair")
					return EXIT_SUCCESS;
				else if (t.name == "ajuda")
					cout << "Ajuda ainda nao disponivel... :(\n";
				else if (t.name == "var")
					result = declaration ();
				else if (t.name == "history")
					expression.print_history ();

				prompt ();
				break;

			case end_expression:
			{
				string expression_str = expression.str ();
				expression_str += " = ";
				expression_str += double_to_string (result);
				result = 0.0;
				expression.add_to_history (expression_str);
				cout << expression_str << '\n';
				input.clean ();
				expression.clean ();
				
				prompt ();
			}
				break;

			default:
				input.putback (t);
				result = calculate_expression ();
			}
			
		}
		catch (const InputError &e)
		{
			std::cerr << "Entrada invalida: " << e.input << '\n';
			std::cerr << e.what () << '\n';
			input.clean ();
			expression.clean ();
			
			prompt ();
		}
		catch (const ExpressionError &e)
		{
			std::cerr << "Erro na expressao: " << e.input << '\n';
			std::cerr << e.what () << '\n';
			
			prompt ();
		}
		catch (...)
		{
			std::cerr << "\nErro inesperado.\nO programa nao pode se "
				<< "recuperar desse erro e sera encerrado.\n";

			return EXIT_FAILURE;
		}
	}
}
