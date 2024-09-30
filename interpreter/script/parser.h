// parser.h

#pragma once

#include "types.h"
#include "lexer.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// Static variables for parser state
static Token current_token;  // Stores the current token being processed

// Function prototypes for parsing

ClassNode* parse_class();     // Parses a class definition
Field* parse_field();         // Parses a field definition
Method* parse_method();       // Parses a method definition
BlockNode* parse_block();     // Parses a block of statements
StatementNode* parse_statement(); // Parses individual statements
ExpressionNode* parse_expression(); // Parses an expression
IfNode* parse_if();           // Parses an if statement
ForNode* parse_for();         // Parses a for loop
void match(TokenType expected_type);
void next_token_wrapper();
ExpressionNode* parse_factor();
ExpressionNode* parse_term();
WhileNode* parse_while();
DoWhileNode* parse_do_while();
SwitchNode* parse_switch();

