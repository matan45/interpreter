#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifdef _MSC_VER
#define strdup _strdup
#endif

// Utility function to print a token
void print_token(Token* token) {
	if (!token) {
		printf("Token: NULL\n");
		return;
	}

	printf("Token Type: %d", token->type);

	// Print the token's value if it exists
	if (token->value) {
		printf(", Value: %s\n", token->value);
	}
	else {
		printf(", Value: NULL\n");
	}
}

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

ASTNode* parse_variable_declaration(Parser* parser) {
	// Expect the type (e.g., 'int', 'boolean', 'float')
	Token* type_token = current_token(parser);
	if (type_token->type != TOKEN_INT && type_token->type != TOKEN_BOOLEAN && type_token->type != TOKEN_FLOAT) {
		fprintf(stderr, "Error: Expected a type declaration (int, boolean, float), but got %d\n", type_token->type);
		exit(1);
	}
	next_token(parser); // Consume the type token

	// Expect the variable name
	Token* var_name_token = current_token(parser);
	if (var_name_token->type != TOKEN_IDENTIFIER) {
		fprintf(stderr, "Error: Expected variable name, but got %d\n", var_name_token->type);
		exit(1);
	}
	next_token(parser); // Consume the variable name

	// Expect the assignment operator
	expect(parser, TOKEN_ASSIGN);

	// Parse the initialization value
	ASTNode* value_node = parse_expression(parser);

	// Expect the semicolon at the end
	expect(parser, TOKEN_SEMICOLON);

	// Create an AST node for the variable declaration
	return create_assignment_node(var_name_token->value, value_node);
}


// Get the current token
Token* current_token(Parser* parser) {
	if (parser->current_position < parser->token_count) {
		Token* token = parser->tokens[parser->current_position];
		print_token(token);  // Print the current token for debugging purposes
		return token;
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
	else if (match(parser, TOKEN_DO)) {
		return parse_do_while_statement(parser);
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
	else if (match(parser, TOKEN_FUNCTION)) {
		return parse_function_definition(parser);
	}
	// Handle variable declarations for int, float, boolean
	else if (current_token(parser)->type == TOKEN_INT || current_token(parser)->type == TOKEN_FLOAT || current_token(parser)->type == TOKEN_BOOLEAN) {
		Token* type_token = current_token(parser); // Store the type token (e.g., 'int', 'float', 'boolean')
		next_token(parser);

		expect(parser, TOKEN_IDENTIFIER); // Parse the variable name
		Token* var_name_token = current_token(parser);
		

		ASTNode* value_node = NULL;

		// Check if the next token is '=' for an assignment
		if (next_token(parser) && current_token(parser)->type == TOKEN_ASSIGN) {
			next_token(parser); // Consume '='
			value_node = parse_expression(parser); // Parse the right-hand side value/expression
		}

		match(parser, TOKEN_SEMICOLON); // Expect a semicolon to end the statement
		// Create and return the variable declaration or assignment node
		return create_variable_declaration_node(type_token->value, var_name_token->value, value_node);
	}
	else if (match(parser, TOKEN_IDENTIFIER)) {
		// Possible assignment or function call
		Token* identifier_token = current_token(parser);  // Store the current identifier
		next_token(parser); // Consume the identifier

		// Check if the next token is '(' to determine if it's a function call
		if (current_token(parser)->type == TOKEN_LEFT_PAREN) {
			next_token(parser); // Consume '('

			// Parse the function arguments
			ASTNode** arguments = (ASTNode**)malloc(10 * sizeof(ASTNode*));
			int arg_count = 0;

			// If the next token is not ')', then there are arguments to parse
			if (current_token(parser)->type != TOKEN_RIGHT_PAREN) {
				do {
					if (arg_count >= 10) {
						arguments = realloc(arguments, (arg_count + 10) * sizeof(ASTNode*)); // Resize if more arguments are present
					}
					arguments[arg_count++] = parse_expression(parser);
				} while (match(parser, TOKEN_COMMA));
			}

			expect(parser, TOKEN_RIGHT_PAREN); // Expect and consume ')'
			expect(parser, TOKEN_SEMICOLON);   // Expect and consume ';' at the end of the function call

			// Create and return a function call node
			return create_function_call_node(identifier_token->value, arguments, arg_count);
		}
		// Otherwise, handle assignment
		else if (current_token(parser)->type == TOKEN_ASSIGN) {
			next_token(parser); // Consume '='

			// Parse the right-hand side value/expression
			ASTNode* value_node = parse_expression(parser);

			// Expect a semicolon to end the statement
			expect(parser, TOKEN_SEMICOLON);

			// Create and return the assignment node
			return create_assignment_node(identifier_token->value, value_node);
		}
		else {
			// Handle unexpected tokens after identifier
			fprintf(stderr, "Error: Unexpected token after identifier '%s'\n", identifier_token->value);
			exit(1);
		}
	}
	else {
		ASTNode* expr = parse_expression(parser);
		expect(parser, TOKEN_SEMICOLON);
		return expr;
	}
}

// Parse a function definition
ASTNode* parse_function_definition(Parser* parser) {
	// Expect and consume 'function' keyword (already consumed by match in parse_statement)
	Token* return_type_token = current_token(parser);
	// Expect a return type (can be int, float, boolean, or an object type)
	if (!(match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) ||
		match(parser, TOKEN_BOOLEAN) || match(parser, TOKEN_IDENTIFIER))) {
		fprintf(stderr, "Error: Expected return type (int, float, boolean, or class name).\n");
		exit(1);
	}

	Token* function_name_token = current_token(parser);
	match(parser, TOKEN_IDENTIFIER); // Parse function name (e.g., 'main')

	// Parse function parameters
	match(parser, TOKEN_LEFT_PAREN);

	char** parameters = (char**)malloc(10 * sizeof(char*));
	char** parameter_types = (char**)malloc(10 * sizeof(char*));
	int param_count = 0;

	while (!match(parser, TOKEN_RIGHT_PAREN)) {
		Token* param_type = current_token(parser);
		// Expect a return type (can be int, float, boolean, or an object type)
		if (!(match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) ||
			match(parser, TOKEN_BOOLEAN) || match(parser, TOKEN_IDENTIFIER))) {
			fprintf(stderr, "Error: Expected return type (int, float, boolean, or class name).\n");
			exit(1);
		}

		Token* param_name = current_token(parser);
		expect(parser, TOKEN_IDENTIFIER); // Parameter name

		if (param_count >= 10) {
			parameters = realloc(parameters, (param_count + 10) * sizeof(char*));
			parameter_types = realloc(parameter_types, (param_count + 10) * sizeof(char*));
		}

		parameter_types[param_count] = strdup(param_type->value);
		parameters[param_count] = strdup(param_name->value);
		param_count++;

		if (!match(parser, TOKEN_COMMA)) {
			break;
		}
	}

	// Parse the function body
	ASTNode* body = parse_block(parser);

	// Create the function node
	return create_function_node(function_name_token->value, parameters, parameter_types, param_count, body->body, body->body_count, ACCESS_PUBLIC, return_type_token->value);
}


// Parse a do-while statement
ASTNode* parse_do_while_statement(Parser* parser) {
	// Expect the 'do' keyword
	expect(parser, TOKEN_DO);

	ASTNode** body;
	int body_count = 0;

	// Parse the body of the do-while loop, which could be a block or a single statement
	if (current_token(parser)->type == TOKEN_LEFT_BRACE) {
		body = parse_block(parser)->body;  // Parse a block and get the statements
		body_count = parse_block(parser)->body_count;  // Get the number of statements in the block
	}
	else {
		// Allocate space for a single statement to create a consistent array-like structure
		body = (ASTNode**)malloc(sizeof(ASTNode*));
		if (!body) {
			fprintf(stderr, "Error: Memory allocation failed for do-while body\n");
			exit(1);
		}
		body[0] = parse_statement(parser);
		body_count = 1;
	}

	// Expect "while" keyword and parse the loop condition
	expect(parser, TOKEN_WHILE);
	expect(parser, TOKEN_LEFT_PAREN);
	ASTNode* condition = parse_expression(parser);
	expect(parser, TOKEN_RIGHT_PAREN);
	expect(parser, TOKEN_SEMICOLON);  // Expect a semicolon to end the 'do-while' statement

	// Create and return the 'do-while' AST node
	return create_do_while_node(condition, body, body_count);
}

// Parse a for statement
ASTNode* parse_for_statement(Parser* parser) {
	// Expect the 'for' keyword
	expect(parser, TOKEN_FOR);
	expect(parser, TOKEN_LEFT_PAREN);

	// Parse the initialization part
	ASTNode* initialization = NULL;
	if (current_token(parser)->type != TOKEN_SEMICOLON) {
		initialization = parse_statement(parser);
	}
	expect(parser, TOKEN_SEMICOLON);

	// Parse the condition part
	ASTNode* condition = NULL;
	if (current_token(parser)->type != TOKEN_SEMICOLON) {
		condition = parse_expression(parser);
	}
	expect(parser, TOKEN_SEMICOLON);

	// Parse the iteration part
	ASTNode* iteration = NULL;
	if (current_token(parser)->type != TOKEN_RIGHT_PAREN) {
		iteration = parse_expression(parser);
	}
	expect(parser, TOKEN_RIGHT_PAREN);

	// Parse the body of the for loop
	ASTNode** body = NULL;
	int body_count = 0;

	if (current_token(parser)->type == TOKEN_LEFT_BRACE) {
		// Parse a block of statements enclosed in '{' and '}'
		ASTNode* block_node = parse_block(parser);
		body = block_node->body;
		body_count = block_node->body_count;
	}
	else {
		// Parse a single statement as the loop body
		body = (ASTNode**)malloc(sizeof(ASTNode*));
		if (!body) {
			fprintf(stderr, "Error: Memory allocation failed for for-loop body\n");
			exit(1);
		}
		body[0] = parse_statement(parser);
		body_count = 1;
	}

	// Create and return the for loop AST node
	return create_for_node(initialization, condition, iteration, body, body_count);
}


// Parse an if statement
ASTNode* parse_if_statement(Parser* parser) {
	// Expect and consume the 'if' keyword
	expect(parser, TOKEN_IF);

	// Expect and consume the left parenthesis '('
	expect(parser, TOKEN_LEFT_PAREN);

	// Parse the condition inside the parentheses
	ASTNode* condition = parse_expression(parser);

	// Expect and consume the right parenthesis ')'
	expect(parser, TOKEN_RIGHT_PAREN);

	// Parse the true branch of the if statement
	ASTNode** true_branch = NULL;
	int true_branch_count = 0;

	if (current_token(parser) && current_token(parser)->type == TOKEN_LEFT_BRACE) {
		ASTNode* true_block = parse_block(parser);
		true_branch = true_block->body;
		true_branch_count = true_block->body_count;
	}
	else {
		// If the true branch is a single statement
		true_branch = (ASTNode**)malloc(sizeof(ASTNode*));
		if (!true_branch) {
			fprintf(stderr, "Error: Memory allocation failed for true branch in if statement\n");
			exit(1);
		}
		true_branch[0] = parse_statement(parser);
		true_branch_count = 1;
	}

	// Parse the optional false branch
	ASTNode* false_branch = NULL;

	if (match(parser, TOKEN_ELSE)) {
		if (current_token(parser) && current_token(parser)->type == TOKEN_LEFT_BRACE) {
			false_branch = parse_block(parser);  // Parse the entire block for the else branch
		}
		else {
			// If the false branch is a single statement
			false_branch = parse_statement(parser);
		}
	}

	// Create and return the if statement AST node
	return create_if_node(condition, true_branch, true_branch_count, false_branch);
}



// Parse a while statement
// Parse a while statement
ASTNode* parse_while_statement(Parser* parser) {
	// Expect the 'while' keyword and the left parenthesis '('
	expect(parser, TOKEN_WHILE);
	expect(parser, TOKEN_LEFT_PAREN);

	// Parse the condition of the while loop
	ASTNode* condition = parse_expression(parser);

	// Expect the right parenthesis ')'
	expect(parser, TOKEN_RIGHT_PAREN);

	// Parse the body of the while loop
	ASTNode** body = NULL;
	int body_count = 0;

	if (current_token(parser)->type == TOKEN_LEFT_BRACE) {
		// Parse a block of statements enclosed in '{' and '}'
		ASTNode* block_node = parse_block(parser);
		body = block_node->body;
		body_count = block_node->body_count;
	}
	else {
		// Parse a single statement as the loop body
		body = (ASTNode**)malloc(sizeof(ASTNode*));
		if (!body) {
			fprintf(stderr, "Error: Memory allocation failed for while loop body\n");
			exit(1);
		}
		body[0] = parse_statement(parser);
		body_count = 1;
	}

	// Create and return the while loop AST node
	return create_while_node(condition, body, body_count);
}


// Parse a block of statements enclosed by braces
ASTNode* parse_block(Parser* parser) {
	if (!match(parser, TOKEN_LEFT_BRACE)) {
		fprintf(stderr, "Error: Expected '{' to begin a block\n");
		exit(1);
	}

	int capacity = 10;  // Initial capacity for block statements
	ASTNode** statements = (ASTNode**)malloc(capacity * sizeof(ASTNode*));
	if (!statements) {
		fprintf(stderr, "Error: Memory allocation failed for block statements\n");
		exit(1);
	}

	int count = 0;

	Token* token = current_token(parser);
	// Parse until we encounter the closing brace
	while (token  && token->type != TOKEN_RIGHT_BRACE) {
		// Resize the array if needed
		if (count <= capacity) {
			capacity += 10;  // Increase capacity as needed
			ASTNode** temp = (ASTNode**)realloc(statements, capacity * sizeof(ASTNode*));
			if (!temp) {
				fprintf(stderr, "Error: Memory allocation failed while resizing block statements array\n");
				free(statements);
				exit(1);
			}
			statements = temp;
		}

		// Parse the next statement and add it to the list
		ASTNode* statement = parse_statement(parser);
		if (statement) {
			statements[count++] = statement;
		}
		token = current_token(parser);
	}

	// Ensure we consume the closing brace
	match(parser, TOKEN_RIGHT_BRACE);

	return create_block_node(statements, count);
}

ASTNode* parse_factor(Parser* parser) {
	Token* token = current_token(parser);

	ASTNode* node = NULL;

	// Handle numbers
	if (token->type == TOKEN_NUMBER) {
		next_token(parser);
		return create_number_node(token->value);
	}
	// Handle strings
	else if (token->type == TOKEN_STRING) {
		next_token(parser);
		return create_string_node(token->value);
	}
	// Handle expressions in parentheses
	else if (token->type == TOKEN_LEFT_PAREN) {
		next_token(parser); // Consume '('
		ASTNode* expr = parse_expression(parser);
		if (current_token(parser)->type != TOKEN_RIGHT_PAREN) {
			fprintf(stderr, "Error: Expected closing parenthesis\n");
			exit(1);
		}
		next_token(parser); // Consume ')'
		return expr;
	}
	// Handle identifiers, which could potentially have member access
	else if (token->type == TOKEN_IDENTIFIER) {
		// Parse the initial identifier as a variable or object
		node = create_variable_node(token->value);
		next_token(parser); // Consume the identifier

		// Handle member access with the dot operator
		while (current_token(parser) && current_token(parser)->type == TOKEN_DOT) {
			next_token(parser); // Consume the dot token
			Token* member_token = current_token(parser);

			if (member_token->type != TOKEN_IDENTIFIER) {
				fprintf(stderr, "Error: Expected member name after '.'\n");
				exit(1);
			}

			// Create a new member access node
			node = create_member_access_node(node, member_token->value);
			next_token(parser); // Consume the member name identifier
		}

		return node;
	}

	// If we reach here, we encountered an unexpected token
	fprintf(stderr, "Error: Unexpected token '%s'\n", token->value);
	exit(1);
	return NULL; // Should never reach here
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
		TokenType unary_type = token->type; // Store the type of the unary operator
		next_token(parser); // Consume the unary operator
		ASTNode* operand = parse_term(parser);
		return create_unary_op_node(unary_type, operand);
	}

	// Handle integers
	if (token->type == TOKEN_NUMBER) {
		next_token(parser);
		return create_number_node(token->value); // Number literals are treated as int by default
	}

	else if (token->type == TOKEN_STRING) {
		next_token(parser);
		return create_string_node(token->value); // Create a node for strings
	}

	// Handle boolean literals (true, false)
	else if (token->type == TOKEN_TRUE) {
		next_token(parser);
		return create_boolean_node(1); // true as 1
	}
	else if (token->type == TOKEN_FALSE) {
		next_token(parser);
		return create_boolean_node(0); // false as 0
	}

	// Handle identifiers (variables, function calls, member access)
	else if (token->type == TOKEN_IDENTIFIER) {
		// Parse the initial identifier as a variable or function name
		ASTNode* node = create_variable_node(token->value);
		next_token(parser); // Consume the identifier

		// Handle function calls
		if (current_token(parser) && current_token(parser)->type == TOKEN_LEFT_PAREN) {
			next_token(parser); // Consume '('

			// Parse arguments
			ASTNode** arguments = (ASTNode**)malloc(10 * sizeof(ASTNode*)); // Allocate space for up to 10 arguments initially
			if (!arguments) {
				fprintf(stderr, "Error: Memory allocation failed for function arguments.\n");
				exit(1);
			}

			int arg_count = 0;
			int arg_capacity = 10;

			// If there are arguments, parse them
			if (current_token(parser)->type != TOKEN_RIGHT_PAREN) {
				do {
					if (arg_count >= arg_capacity) {
						arg_capacity += 10;
						ASTNode** temp_args = realloc(arguments, arg_capacity * sizeof(ASTNode*));
						if (!temp_args) {
							fprintf(stderr, "Error: Memory allocation failed while resizing arguments.\n");
							free(arguments);
							exit(1);
						}
						arguments = temp_args;
					}
					arguments[arg_count++] = parse_expression(parser);
				} while (match(parser, TOKEN_COMMA));
			}

			expect(parser, TOKEN_RIGHT_PAREN); // Expect and consume ')'

			// Create a function call node
			return create_function_call_node(node->value, arguments, arg_count);
		}

		// Handle member access with the dot operator
		while (current_token(parser) && current_token(parser)->type == TOKEN_DOT) {
			next_token(parser); // Consume the dot token
			Token* member_token = current_token(parser);

			if (member_token->type != TOKEN_IDENTIFIER) {
				fprintf(stderr, "Error: Expected member name after '.'\n");
				exit(1);
			}

			// Create a new member access node
			node = create_member_access_node(node, member_token->value);
			next_token(parser); // Consume the member name identifier
		}

		return node;
	}

	// Handle grouped expressions with parentheses
	else if (token->type == TOKEN_LEFT_PAREN) {
		next_token(parser); // Consume '('
		ASTNode* node = parse_expression(parser);
		expect(parser, TOKEN_RIGHT_PAREN); // Expect and consume ')'
		return node;
	}

	// If we reach here, unexpected token
	fprintf(stderr, "Error: Unexpected token type %d and %s in term\n", token->type, token->value);
	exit(1);
}


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
	int param_capacity = 10;  // Initial parameter capacity
	char** parameters = (char**)malloc(param_capacity * sizeof(char*));
	char** parameter_types = (char**)malloc(param_capacity * sizeof(char*));
	int param_count = 0;

	while (current_token(parser)->type != TOKEN_RIGHT_PAREN) {
		if (param_count >= param_capacity) {
			param_capacity += 5;  // Increase parameter capacity if needed
			parameters = (char**)realloc(parameters, param_capacity * sizeof(char*));
			parameter_types = (char**)realloc(parameter_types, param_capacity * sizeof(char*));
			if (!parameters || !parameter_types) {
				fprintf(stderr, "Error: Memory allocation failed while resizing function parameters array\n");
				exit(1);
			}
		}

		Token* param_type = current_token(parser);
		expect(parser, TOKEN_IDENTIFIER);

		Token* param_name = current_token(parser);
		expect(parser, TOKEN_IDENTIFIER);

		parameter_types[param_count] = strdup(param_type->value);
		parameters[param_count] = strdup(param_name->value);
		param_count++;

		if (current_token(parser)->type != TOKEN_RIGHT_PAREN) {
			expect(parser, TOKEN_COMMA);
		}
	}
	expect(parser, TOKEN_RIGHT_PAREN);

	// Parse the function body
	ASTNode* body = parse_block(parser);

	return create_function_node(function_name->value, parameters, parameter_types, param_count, &body, 1, access, return_type->value);
}
