/*
	Calculadora		Realiza opera��es matem�ticas e bin�ria no console
	Vers�o: (alfa - 0.3)

	TODO:
	- Criar opc�es de comandos para:
		- Encerrar o programa				[V]
		- Exibir resultado parcial			[X]
		- Criar vari�veis					[V]
		- Modificar o valor de um vari�vel	[X]
		- Exibir a lista de vari�veis		[V]
		- Limpar a lista de vari�veis		[V]
		- Exibir hist�rico de express�es	[V]
		- Limpar hist�rico de express�es	[V]
	- Implementar a parte de opera��es l�gicas
	- Ampliar os resultados do tipo double para long double
	- Usar um conjunto de caracteres que suporte acentos

	Criado da cria��o: 01/06/2017
	Desenvolvido por: Ignispider

	Changelog:
	* 25/06/2017 *
	- Corre��o de bugs

	* 20/06/2017 *
	- Corre��o de bugs

	* 14/06/2017 *
	- Reestrutura��o e reorganiza��o das classes e fun��es.
		- struct Token: Armazena o tipo e o valor de um Token.
		- class Token_stream: L� o console, armazena a entrada
		  e cria um Token.
		- class Expression: Armazena uma lista de Token e realiza
		  diversas opera��es com Tokens.

	* 09/06/2017 *
	- Implementa��o das opera��es matem�ticas.
	- Mudan�a do buffer da classe Token_stream de string para stringstream
	  e consequentemente todas as fun��es dentro do objeto que lidam com
	  entrada n�o manipulam o objeto cin diretamente, com exce��o da
	  fun��o read_input que l� o console e armazena a entrada no objeto
	  stringstream expression_string
*/

#include <iostream>
#include "expression.h"

using std::cout;
using std::cin;



// Realiza a opera��o Mod '%' em valores inteiros e n�o inteiros
double modulo (double dividend, double divisor)
{
	long long quotient = static_cast<long long>(dividend / divisor);
	
	return dividend - (quotient * divisor);
}

// Apesar da fun��o factorial receber um double o c�lculo � baseado
// em n�meros inteiros. Para retornar um resultado correto a fun��o
// chamadora verifica se o argumento n�o possui parte decimal
double factorial (double n)
{
	// No futuro a fun��o pode ser melhorada para realizar o c�lculo
	// em n�meros n�o inteiros
	if (n > 1.0)
	{
		double result = n;
		while (n > 1.0)
			result *= --n;

		return result;
	}
	else if (n == 0.0 || n == 1.0)
	{
		return 1.0;
	}
	else
		throw ExpressionError ("Fatorial de numero negativo.", input.str ());

}

void print_variables ()
{
	for each (Variable var in variables)
	{
		cout << '$' << var.name << " = " << var.value << '\n';
	}
}

void clear_variables ()
{
	variables.clear ();
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

	case '-':	// Negativo, n�o subtra��o
		negative = true;
		// Intencionalmente cai em case '+':
	case '+':	// Positivo, n�o adi��o
		value = primary ();
		break;

	case '(':
	{
		value = arithimetic_additive ();
		expression >> t;
		if (t.token != ')')
			throw ExpressionError ("')' esperado.", input.str ());
	}
	break;

	case ')':
		throw ExpressionError ("'(' esperado.", input.str ());

	case variable:
		if (!get_variable (t.name, &value))
			throw ExpressionError ("Variavel '" + t.name + "' nao declarada.", input.str ());

		break;

	case function:
		if (!get_function (t.name, &value))
			throw ExpressionError ("Funcao '" + t.name + "' nao declarada.", input.str ());

		break;
	}

	expression >> t;
	if (t.token == '!')
	{
		if (value - static_cast<long long>(value) != 0.0)
			throw ExpressionError ("O operando de '!' deve ser de tipo inteiro.", input.str ());

		value = factorial (value);
	}
	else
		expression.putback (t);

	return (negative ? -value : value);
}

double arithmetic_multiplicative ()
{
	double value = primary ();

	Token t;
	while (true)
	{
		expression >> t;
		switch (t.token)
		{
		case '*':
			value *= primary ();
			break;

		case '/':
		{
			double d = primary ();
			if (d == 0)
			{
				throw ExpressionError ("Divisao por 0.", input.str ());
			}
			value /= d;
		}
		break;

		case '%':
			value = modulo (value, primary ());
			break;

		default:
			expression.putback (t);
			return value;
		}
	}
}

// Calcula soma e subtra��o
double arithimetic_additive ()
{
	double value = arithmetic_multiplicative ();

	Token t;
	while (true)
	{
		expression >> t;
		switch (t.token)
		{
		case '+':
			value += arithmetic_multiplicative ();
			break;

		case '-':
			value -= arithmetic_multiplicative ();
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
			double d = arithimetic_additive ();
			input >> t;
			if (t.token == end_expression)
			{
				expression_str += expression.str ();
				expression.add_to_history (expression_str);
				variables.push_back (Variable (variable_name, d));

				cout << "$" << variable_name << " = " << d << '\n';
				input.clear ();
				expression.clear ();
				
				prompt ();
				return d;
			}
			else
				expression_str = "Operador ';' esperado.";
		}
	}
	else
		expression_str = "A variavel '$" + t.name + "' ja foi declarada.";

	throw ExpressionError (expression_str, input.str ());
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
				else if (t.name == "decl")
					result = declaration ();
				else if (t.name == "hist")
					expression.print_history ();
				else if (t.name == "lsvar")
					print_variables ();
				else if (t.name == "clrvar")
					clear_variables ();
				else if (t.name == "clrhist")
					expression.clear_history ();
				else
					std::cerr << "Comando desconhecido: " << t.name << '\n';
				
				input.clear ();
				prompt ();
				break;

			case end_expression:
			{
				string expression_str = expression.str ();
				if (expression_str.size ())
				{
					expression_str += " = ";
					expression_str += double_to_string (result);
					expression.add_to_history (expression_str);
					cout << expression_str << '\n';
				}

				input.clear ();
				expression.clear ();
				result = 0.0;
				prompt ();
			}
				break;

			default:
				input.putback (t);
				result = arithimetic_additive ();
			}
			
		}
		catch (const InputError &e)
		{
			std::cerr << "Entrada invalida: " << e.input << '\n';
			std::cerr << e.what () << '\n';
			input.clear ();
			expression.clear ();
			
			prompt ();
		}
		catch (const ExpressionError &e)
		{
			std::cerr << "Erro na expressao: " << e.input << '\n';
			std::cerr << e.what () << '\n';
			expression.clear ();
			input.clear ();
			
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
