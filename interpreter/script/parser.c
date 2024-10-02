#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifdef _MSC_VER
#define strdup _strdup
#endif

// Create a new parser
Parser* create_parser(Token** tokens, int token_count) {
	Parser* parser = (Parser*)malloc(sizeof(Parser));
	parser->tokens = tokens;
	parser->current_position = 0;
	parser->token_count = token_count;
	return parser;
}

// Free the parser
void free_parser(Parser* parser) {
	free(parser);
}

// Get the current token
Token* current_token(Parser* parser) {
	if (parser->current_position < parser->token_count) {
		return parser->tokens[parser->current_position];
	}
	return NULL;  // End of input
}

// Move to the next token
Token* next_token(Parser* parser) {
	parser->current_position++;
	return current_token(parser);
}

// Match and consume a token of a given type
int match(Parser* parser, TokenType type) {
	if (current_token(parser) && current_token(parser)->type == type) {
		next_token(parser);
		return 1;
	}
	return 0;
}

// Expect a specific token type and move to the next
void expect(Parser* parser, TokenType type) {
	if (current_token(parser)->type != type) {
		fprintf(stderr, "Error: Expected token type %d, but got %d\n", type, current_token(parser)->type);
		exit(1);
	}
	next_token(parser);
}

// Parse a program (sequence of statements)
ASTNode* parse_program(Parser* parser, int* statement_count) {
	ASTNode** statements = (ASTNode**)malloc(100 * sizeof(ASTNode*));  // Assume max 100 statements
	*statement_count = 0;

	while (current_token(parser) && current_token(parser)->type != TOKEN_EOF) {
		statements[(*statement_count)++] = parse_statement(parser);
	}

	ASTNode* program_node = (ASTNode*)malloc(sizeof(ASTNode));
	program_node->type = AST_FUNCTION_DEF;  // Treat the program as a function body
	program_node->body = statements;
	program_node->body_count = *statement_count;

	return program_node;
}

// Parse a statement
ASTNode* parse_statement(Parser* parser) {
	if (match(parser, TOKEN_IF)) {
		return parse_if_statement(parser);
	}
	else if (match(parser, TOKEN_WHILE)) {
		return parse_while_statement(parser);
	}
	else if (match(parser, TOKEN_RETURN)) {
		ASTNode* return_value = parse_expression(parser);
		expect(parser, TOKEN_SEMICOLON);
		return create_return_node(return_value);
	}
	else if (match(parser, TOKEN_DELETE)) {
		Token* object_token = current_token(parser);
		expect(parser, TOKEN_IDENTIFIER);
		expect(parser, TOKEN_SEMICOLON);
		return create_object_destruct_node(object_token->value);
	}
	else {
		ASTNode* expr = parse_expression(parser);
		expect(parser, TOKEN_SEMICOLON);
		return expr;
	}
}

// Parse a do-while statement
ASTNode* parse_do_while_statement(Parser* parser) {
	expect(parser, TOKEN_DO);

	ASTNode** body = parse_block(parser);

	expect(parser, TOKEN_WHILE);
	expect(parser, TOKEN_LEFT_PAREN);
	ASTNode* condition = parse_expression(parser);
	expect(parser, TOKEN_RIGHT_PAREN);
	expect(parser, TOKEN_SEMICOLON);

	return create_do_while_node(condition, body, 1);
}

// Parse an if statement
ASTNode* parse_if_statement(Parser* parser) {
	expect(parser, TOKEN_LEFT_PAREN);
	ASTNode* condition = parse_expression(parser);
	expect(parser, TOKEN_RIGHT_PAREN);
	ASTNode** body = parse_block(parser);
	return create_if_node(condition, body, 1);
}

// Parse a while statement
ASTNode* parse_while_statement(Parser* parser) {
	expect(parser, TOKEN_LEFT_PAREN);
	ASTNode* condition = parse_expression(parser);
	expect(parser, TOKEN_RIGHT_PAREN);
	ASTNode** body = parse_block(parser);
	return create_while_node(condition, body, 1);
}

// Parse a block of statements enclosed by braces
ASTNode* parse_block(Parser* parser) {
	if (!match(parser, TOKEN_LEFT_BRACE)) {
		fprintf(stderr, "Error: Expected '{' to begin a block\n");
		exit(1);
	}

	ASTNode** statements = malloc(10 * sizeof(ASTNode*)); // Allocate space for up to 10 statements initially
	int count = 0;

	while (!match(parser, TOKEN_RIGHT_BRACE)) {
		if (count >= 10) {
			statements = realloc(statements, (count + 10) * sizeof(ASTNode*)); // Increase space if necessary
		}

		ASTNode* statement = parse_statement(parser);
		if (statement) {
			statements[count++] = statement;
		}
	}

	return create_block_node(statements, count);
}

// Parse an expression
ASTNode* parse_expression(Parser* parser) {
	ASTNode* left = parse_term(parser);

	// Parse logical operations like && and ||
	while (current_token(parser) &&
		(current_token(parser)->type == TOKEN_LOGICAL_AND || current_token(parser)->type == TOKEN_LOGICAL_OR)) {
		Token* operator = current_token(parser);
		next_token(parser);
		ASTNode* right = parse_term(parser);

		if (operator->type == TOKEN_LOGICAL_AND) {
			left = create_logical_op_node(AST_LOGICAL_AND, left, right);
		}
		else if (operator->type == TOKEN_LOGICAL_OR) {
			left = create_logical_op_node(AST_LOGICAL_OR, left, right);
		}
	}

	return left;
}

// Parse a term (basic units of expressions like variables, numbers, unary operators, etc.)
ASTNode* parse_term(Parser* parser) {
	Token* token = current_token(parser);

	// Handle unary operations
	if (token->type == TOKEN_MINUS || token->type == TOKEN_LOGICAL_NOT) {
		next_token(parser); // Consume the unary operator
		ASTNode* operand = parse_term(parser);
		return create_unary_op_node(AST_UNARY_OP, operand);
	}

	// Handle numbers, identifiers, booleans, and parentheses
	if (token->type == TOKEN_NUMBER) {
		next_token(parser);
		return create_number_node(token->value);
	}
	else if (token->type == TOKEN_IDENTIFIER) {
		next_token(parser);
		return create_variable_node(token->value);
	}
	else if (token->type == TOKEN_TRUE) {
		next_token(parser);
		return create_boolean_node(1); // true as 1
	}
	else if (token->type == TOKEN_FALSE) {
		next_token(parser);
		return create_boolean_node(0); // false as 0
	}
	else if (token->type == TOKEN_LEFT_PAREN) {
		next_token(parser);
		ASTNode* node = parse_expression(parser);
		expect(parser, TOKEN_RIGHT_PAREN);
		return node;
	}

	fprintf(stderr, "Error: Unexpected token type %d in term\n", token->type);
	exit(1);
}

// Parse a function definition
ASTNode* parse_function(Parser* parser) {
	// Handle access modifier (public or private)
	AccessModifier access = ACCESS_PUBLIC;
	if (match(parser, TOKEN_PUBLIC)) {
		access = ACCESS_PUBLIC;
	}
	else if (match(parser, TOKEN_PRIVATE)) {
		access = ACCESS_PRIVATE;
	}

	// Parse return type and function name
	Token* return_type = current_token(parser);
	expect(parser, TOKEN_IDENTIFIER);

	Token* function_name = current_token(parser);
	expect(parser, TOKEN_IDENTIFIER);

	// Parse function parameters
	expect(parser, TOKEN_LEFT_PAREN);
	char** parameters = (char**)malloc(10 * sizeof(char*));
	char** parameter_types = (char**)malloc(10 * sizeof(char*));
	int param_count = 0;

	while (!match(parser, TOKEN_RIGHT_PAREN)) {
		Token* param_type = current_token(parser);
		expect(parser, TOKEN_IDENTIFIER);

		Token* param_name = current_token(parser);
		expect(parser, TOKEN_IDENTIFIER);

		parameter_types[param_count] = strdup(param_type->value);
		parameters[param_count++] = strdup(param_name->value);
		match(parser, TOKEN_COMMA);
	}

	// Parse the function body
	ASTNode** body = parse_block(parser);
	return create_function_node(function_name->value, parameters, parameter_types, param_count, body, 1, access, return_type->value);
}
