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

#pragma once
#include <string>
#include <deque>
#include <map>
#include <functional>

enum token_type
{
	token_open,
	token_close,
	token_pow,
	token_mult,
	token_div,
	token_add,
	token_sub,
	token_num,
	token_var,
	token_comma,
	token_function,
};

struct token
{
public:
	token_type type; // The type of token.
	std::string contents; // Actual string of the token.
	double value; // Numbers store their value here.
	bool isop; // Is it an operatator?
	bool isopleft; // Does the operator associate to the left or right?
	int oppres; // Order of operatations.
};

struct function_data
{
public:
	std::function<double(double*, int args)> func;
	int argcount;
};

class expression
{
private:
	std::deque<token> compiled;
	std::map<char, double> vars;
	std::map<std::string, function_data> funcs;

	std::deque<token> tokenize(std::string expr);
	std::deque<token> parse(std::deque<token> tokens);
public:
	expression();

	void compile(std::string expr);
	double run();

	void setvar(char var, double val);
	double getvar(char var);

	void addfunction(std::string name, std::function<double(double*, int args)> func, int argcount);
};