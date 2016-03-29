# ShuntingYard
A C++ library for parsing and running math expressions without all the hard work. Allows you to add your own functions and variables.

## Example:
This code simply executes the expression:
  
  ```c++
  expression e = expression();
	e.compile("5 * (8 - 3)");
	e.run();
  ```
The compile function allows you to parse an expression only once and run it multiple times. Now you might ask why because the same expression gives the same results right? Well in this library you can expose your own variables and functions and you can change their values without having to recompile. For example I could have a equation I wanted to graph and compile it once and then change the x variable for each point I want to graph without having to recompile.
  
  A more detailed example:
  ```c++
  #include <cmath>
  #include <iostream>
  #include "expression.hpp"

  double sine(double args[], int argcount)
  {
	  return std::sin(args[0]);
  }

  int main(int argc, char* argv[])
  {
	  expression e = expression();
	  e.setvar('a', 5.3);
	  e.addfunction("sin", sine, 1);
	  e.compile("sin(a) * 3");
	  std::cout << e.run() << std::endl;
	  return 0;
  }
  ```
  
  In this example we expose the variable *a* (all variables are a single character) and the function *sin*. Then we compile and run. Very easy!
  
## Compiling
Only uses the standard C++ library so it should compile and run on any platform.
  
## How it works
Uses the Shunting Yard algorithm to compile the expression into reverse polish notation (the compiled part). This reverse polish notation is then run using a stack based system. The expression is not actually compiled into machine code but it is compiled into a token type system.
