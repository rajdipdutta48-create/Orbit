#include <iostream>

#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"

int main()
{
  std::string source = R"(

20 + 30

)";

  // Step 1: Convert source code into tokens
  Lexer lexer(source);

  std::vector<Token> tokens = lexer.scanTokens();

  // Step 2: Convert tokens into an AST
  Parser parser(tokens);

  std::unique_ptr<Expr> tree = parser.parse();

  // Step 3: Evaluate the AST
  Interpreter interpreter;

  std::cout << interpreter.evaluate(tree.get()) << std::endl;

  return 0;
}