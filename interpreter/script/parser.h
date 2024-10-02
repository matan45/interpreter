#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "ast.h"

// Define a structure for the parser
typedef struct {
	Token** tokens;           // Array of tokens
	int current_position;     // Current position in the token list
	int token_count;          // Total number of tokens
} Parser;

// Function prototypes for parser operations
Parser* create_parser(Token** tokens, int token_count);
void free_parser(Parser* parser);

// Parsing functions
ASTNode* parse_program(Parser* parser, int* statement_count);
ASTNode* parse_statement(Parser* parser);
ASTNode* parse_expression(Parser* parser);
ASTNode* parse_term(Parser* parser);
ASTNode* parse_block(Parser* parser);
ASTNode* parse_factor(Parser* parser);
ASTNode* parse_if_statement(Parser* parser);
ASTNode* parse_while_statement(Parser* parser);
ASTNode* parse_do_while_statement(Parser* parser);
ASTNode* parse_for_statement(Parser* parser);
ASTNode* parse_function_definition(Parser* parser);
ASTNode* parse_variable_declaration(Parser* parser);
ASTNode* parse_class(Parser* parser);


// Utility functions
Token* current_token(Parser* parser);
Token* next_token(Parser* parser);
int match(Parser* parser, TokenType type);
void expect(Parser* parser, TokenType type);

#endif // PARSER_H
