// parser.h

#pragma once

#include "lexer.h"
#include "ast.h"
#include "types.h"

// Function to parse a class definition from the tokens
ClassNode* parse_class();

// Function to parse a field of a class
Field* parse_field();

// Function to parse a method of a class
Method* parse_method();

// Function to parse a block of code (e.g., method body)
BlockNode* parse_block();

// Function to parse a statement (e.g., assignment, if, for)
StatementNode* parse_statement();

// Function to parse an expression (e.g., arithmetic, assignment)
ExpressionNode* parse_expression();

// Utility function to handle token advancement
void next_token_wrapper();

// Utility function to ensure the current token is as expected
void expect(TokenType type);
