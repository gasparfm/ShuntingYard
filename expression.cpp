/*
MIT License

Copyright (c) 2016 Philip Picard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <deque>
#include <stack>
#include <cmath>
#include <functional>
#include <exception>
#include <iostream>
#include "expression.hpp"

expression::expression()
{
	vars = std::map<char, double>();
}

// Simply loops through and breaks the expression intos tokens.
std::deque<token> expression::tokenize(std::string expr)
{
	int i = 0;
	std::deque<token> list = std::deque<token>();

	while (i != expr.length())
	{
		char c = expr[i++]; // Get the character and move to the next character. After
		// this i will be pointing to the next character not that current one.
		token t;
		t.contents = "";
		t.value = 0;
		t.isop = false;
		t.oppres = 0;
		t.isopleft = true;

		switch (c)
		{
			// Operations and stuff.
		case '(':
			t.type = token_type::token_open;
			t.contents = "(";
			break;
		case ')':
			t.type = token_type::token_close;
			t.contents = ")";
			break;
		case '^':
			t.type = token_type::token_pow;
			t.contents = "^";
			t.isop = true;
			t.oppres = 3;
			t.isopleft = false;
			break;
		case '*':
			t.type = token_type::token_mult;
			t.contents = "*";
			t.isop = true;
			t.oppres = 2;
			break;
		case '/':
			t.type = token_type::token_div;
			t.contents = "/";
			t.isop = true;
			t.oppres = 2;
			break;
		case '+':
			t.type = token_type::token_add;
			t.contents = "+";
			t.isop = true;
			t.oppres = 1;
			break;
			// No negative case because it could be a negative number so it needs special handling
			// in the default case.
		case ',':
			t.type = token_type::token_comma;
			t.contents = ",";
			break;
		default:
			if (isdigit(c) || c == '.' || (c == '-' && (isdigit(expr[i]) || expr[i] == '.'))) // Number
			{
				// Read the entire number into the buffer.
				std::string buffer;
				i--; // Shift back one because the loop will shift one extra.
				while (isdigit(c) || c == '.' || c == '-')
				{
					buffer += c;
					c = expr[++i];
				}

				t.type = token_type::token_num;
				t.contents = buffer;
				t.value = std::atof(buffer.c_str());
			}
			else if (c == '-') // Negative operation.
			{
				t.type = token_type::token_sub;
				t.contents = "-";
				t.isop = true;
				t.oppres = 1;
			}
			else if (isspace(c)) // Skip.
				continue; // Skip adding the token since we have no token.
			else if (isalpha(c)) // Variable or function.
			{
				t.type = token_type::token_var;
				t.contents = c;

				// Functions can have a name longer than one character so if it has multiple
				// characters is must be a function.
				if (isalpha(expr[i]))
				{
					t.type = token_type::token_function;
					t.contents = "";
					t.isop = false;
					t.oppres = 4; // Functions always come before anything else.
					i--; // Shift back one because the loop will shift one extra.
					while (isalpha(c))
					{
						t.contents += c;
						c = expr[++i];
					}
				}
			}
			else // Unkown token. Error!
				throw std::exception("Unkown token!");
			break;
		}

		list.push_back(t); // Finally add the token to the output.
	}

	return list;
}

// Actually parse the tokens from a regular expression into reverse polish notation.
// This is done using the Shunting Yard algorithm.
std::deque<token> expression::parse(std::deque<token> tokens)
{
	std::deque<token> list = std::deque<token>();
	std::stack<token> opstack = std::stack<token>();

	// Loop all the tokens.
	token t;
	while (tokens.size() != 0)
	{
		t = tokens.front();
		tokens.pop_front();

		// Move numbers directly to the output.
		if (t.type == token_type::token_num || t.type == token_type::token_var)
			list.push_back(t);
		else if (t.type == token_type::token_function) // Simply gets the value of the variable and treats it like a number.
			opstack.push(t);
		else if (t.type == token_type::token_comma)
		{
			while (!opstack.empty() && opstack.top().type != token_type::token_open)
			{
				token t = opstack.top();
				opstack.pop();
				list.push_back(t);
			}

			if (opstack.empty()) throw std::exception("Bad function call syntax.");
		}
		else if (t.isop) // Operators handling.
		{
			// Shift things around on the stack to get order of operations right.
			// We have to check and make sure there is an operator on the stack and not a function.
			while (!opstack.empty() && opstack.top().isop)
			{
				if ((t.isopleft && (t.oppres <= opstack.top().oppres)) || (!t.isopleft && (t.oppres < opstack.top().oppres)))
				{
					token t = opstack.top();
					opstack.pop();
					list.push_back(t);
				}
				else
					break;
			}

			opstack.push(t);
		}
		// Handles brackets.
		else if (t.type == token_type::token_open)
			opstack.push(t);
		else if (t.type == token_type::token_close)
		{
			while (!opstack.empty() && opstack.top().type != token_type::token_open)
			{
				token t = opstack.top();
				opstack.pop();
				list.push_back(t);
			}

			if (opstack.empty()) throw std::exception("Bracket mismatch!");

			opstack.pop();
			// Function handling.
			if (!opstack.empty() && opstack.top().type == token_type::token_function)
			{
				token t = opstack.top();
				opstack.pop();
				list.push_back(t);
			}
		}
	}

	// Move anything left on the stack to the output list.
	while (!opstack.empty())
	{
		token t = opstack.top();
		if (t.type == token_type::token_open || t.type == token_type::token_close) throw std::exception("Bracket mismatch!");
		opstack.pop();
		list.push_back(t);
	}

	return list;
}

void expression::compile(std::string expr)
{
	compiled = parse(tokenize(expr));
}

double expression::run()
{
	// Runs the compiled tokens. They
	// are in reverse polish notation.

	std::stack<double> numbers = std::stack<double>();

	for (int i = 0; i < compiled.size(); i++)
	{
		token t = compiled[i];

		if (t.type == token_type::token_num)
			numbers.push(t.value);
		else if (t.type == token_type::token_var)
		{
			if (vars.count(t.contents[0]) == 0) vars[t.contents[0]] = 0;
			numbers.push(vars[t.contents[0]]);
		}
		else if (t.type == token_type::token_function)
		{
			if (funcs.count(t.contents) == 0) throw std::exception("Could not find function of that name.");

			double* args = new double[funcs[t.contents].argcount];
			for (int i = 0; i < funcs[t.contents].argcount; i++)
			{
				if (numbers.empty())
					throw std::exception("Not enough arguments!");
				args[i] = numbers.top();
				numbers.pop();
			}

			double r = funcs[t.contents].func(args, funcs[t.contents].argcount);
			numbers.push(r);
			delete[] args;
		}
		else
		{
			// 2nd argument comes first and then the first argument.
			if (numbers.size() < 2) throw std::exception("Bad expression format.");
			double b = numbers.top();
			numbers.pop();
			double a = numbers.top();
			numbers.pop();

			switch (t.type)
			{
			case token_type::token_pow:
				numbers.push(std::pow(a, b));
				break;
			case token_type::token_mult:
				numbers.push(a * b);
				break;
			case token_type::token_div:
				numbers.push(a / b);
				break;
			case token_type::token_add:
				numbers.push(a + b);
				break;
			case token_type::token_sub:
				numbers.push(a - b);
				break;
			}
		}
	}

	// The top of the stack now has the result.
	if (numbers.empty()) numbers.push(0);
	if (numbers.size() > 1) throw std::exception("Bad expression format.");

	return numbers.top();
}

void expression::setvar(char var, double val)
{
	vars[var] = val;
}

double expression::getvar(char var)
{
	if (vars.count(var) == 0)
		vars[var] = 0;
	return vars[var];
}

void expression::addfunction(std::string name, std::function<double(double*, int args)> func, int argcount)
{
	funcs[name].func = func;
	funcs[name].argcount = argcount;
}