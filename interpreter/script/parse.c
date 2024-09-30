

#ifdef _MSC_VER
#define strdup _strdup
#endif

#include "parse.h"
#include "lexer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


const char* token_type_to_string(TokenType type) {
	switch (type) {
	case TOKEN_CLASS: return "TOKEN_CLASS";
	case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
	case TOKEN_INT: return "TOKEN_INT";
	case TOKEN_FLOAT: return "TOKEN_FLOAT";
	case TOKEN_VOID: return "TOKEN_VOID";
	case TOKEN_LBRACE: return "TOKEN_LBRACE";
	case TOKEN_RBRACE: return "TOKEN_RBRACE";
	case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
	case TOKEN_LPAREN: return "TOKEN_LPAREN";
	case TOKEN_RPAREN: return "TOKEN_RPAREN";
	case TOKEN_UNKNOWN: return "TOKEN_UNKNOWN";
	case TOKEN_COMMA: return "TOKEN_COMMA";
	case TOKEN_FOR: return "TOKEN_FOR";
	case TOKEN_IF: return "TOKEN_IF";
	case TOKEN_ELSE: return "TOKEN_ELSE";
	case TOKEN_LESS: return "TOKEN_LESS";  // <
	case TOKEN_GREATER: return "TOKEN_GREATER";  // >
	case TOKEN_LESS_EQUAL: return "TOKEN_LESS_EQUAL";  // <=
	case TOKEN_GREATER_EQUAL: return "TOKEN_GREATER_EQUAL";  // >=
	case TOKEN_EQUAL: return "TOKEN_EQUAL";  // ==
	case TOKEN_NOT_EQUAL: return "TOKEN_NOT_EQUAL";  // !=
	case TOKEN_END: return "TOKEN_END";
	case TOKEN_ASSIGN: return "TOKEN_ASSIGN";  // =
	case TOKEN_INCREMENT: return "TOKEN_INCREMENT";  // ++
	case TOKEN_DECREMENT: return "TOKEN_DECREMENT";  // --
	case TOKEN_MINUS: return "TOKEN_MINUS";  // --
	case TOKEN_PLUS: return "TOKEN_PLUS";  // --
	case TOKEN_DIVIDE: return "TOKEN_DIVIDE";  // --
	case TOKEN_MULTIPLY: return "TOKEN_MULTIPLY";  // --
	default: return "UNKNOWN_TOKEN_TYPE";
	}
}


void next_token_wrapper() {
	current_token = next_token(source);
	printf("Token Type: %s, Token Value: %s\n", token_type_to_string(current_token.type), current_token.value);

}

// Function to validate the current token type and move to the next one
void expect(TokenType type) {
	if (current_token.type != type) {
		// Print an error message if the current token is not as expected
		printf("Error: Expected token type %d but found '%s'\n", type, current_token.value);
		exit(1);  // Exit the program due to the parsing error
	}
	next_token_wrapper();  // Move to the next token
}

// Function to look up a variable in the symbol table
int lookup_variable(char* variable) {
	SymbolTable* current = symbol_table;
	while (current != NULL) {
		if (strcmp(current->variable_name, variable) == 0) {
			return current->value;
		}
		current = current->next;
	}
	printf("Error: Undefined variable %s\n", variable);
	exit(1);
}

// Function to update or add a variable to the symbol table
void update_variable(char* variable, int value) {
	SymbolTable* current = symbol_table;
	while (current != NULL) {
		if (strcmp(current->variable_name, variable) == 0) {
			current->value = value; // Update existing variable
			return;
		}
		current = current->next;
	}

	// If not found, add a new variable to the symbol table
	SymbolTable* new_variable = (SymbolTable*)malloc(sizeof(SymbolTable));
	new_variable->variable_name = strdup(variable); // Make a copy of the variable name
	new_variable->value = value;
	new_variable->next = symbol_table;
	symbol_table = new_variable;
}

// Function to free the symbol table
void free_symbol_table() {
	SymbolTable* current = symbol_table;
	while (current != NULL) {
		SymbolTable* temp = current;
		current = current->next;
		free(temp->variable_name);
		free(temp);
	}
	symbol_table = NULL;
}

// Function to execute an expression
void execute_expression(ExpressionNode* expr, Object* obj, LocalVariable* local_table) {
	if (expr == NULL) {
		printf("Error: Null expression.\n");
		return;
	}

	// Handle assignment operations: variable = value
	if (expr->variable != NULL && expr->next != NULL) {
		// This means we have an assignment expression
		// `expr->variable` is the variable to be assigned
		// `expr->next` is the value or expression that should be evaluated
		const char* variable_name = expr->variable;
		int value = evaluate_expression(expr->next, obj, local_table);  // Evaluate the right-hand side

		// First, check if this variable is in the local scope
		LocalVariable* local_var = local_table;
		while (local_var) {
			if (strcmp(local_var->variable_name, variable_name) == 0) {
				update_local_variable(&local_table, variable_name, value);
				return;
			}
			local_var = local_var->next;
		}

		// If not found in local scope, update it in the object fields
		update_object_field(obj, variable_name, value);
	}
	else if (expr->variable != NULL && expr->next == NULL) {
		// Simply accessing a variable without assigning
		// First, try to find the variable in the local table
		LocalVariable* local_var = local_table;
		while (local_var) {
			if (strcmp(local_var->variable_name, expr->variable) == 0) {
				printf("Variable %s has value: %d\n", expr->variable, local_var->value);
				return;
			}
			local_var = local_var->next;
		}

		// If not found in the local table, look up in the object fields
		int value = lookup_object_field(obj, expr->variable);
		printf("Variable %s has value: %d\n", expr->variable, value);
	}
	else if (expr->variable == NULL) {
		// For constant expressions
		printf("Constant value: %d\n", expr->value);
	}
	else {
		// Unsupported expression type
		printf("Error: Unsupported expression type.\n");
		exit(1);
	}
}


ClassNode* parse_class() {
	expect(TOKEN_CLASS);  // Expect 'class'

	const char* class_name = current_token.value;  // Store class name
	expect(TOKEN_IDENTIFIER);  // Expect class name
	expect(TOKEN_LBRACE);  // Expect '{'

	ClassNode* class_node = (ClassNode*)malloc(sizeof(ClassNode));
	if (!class_node) {
		printf("Error: Memory allocation failed for ClassNode.\n");
		exit(1);
	}

	class_node->class_name = strdup(class_name);
	class_node->fields = NULL;
	class_node->methods = NULL;

	// Parse class body (fields and methods)
	while (current_token.type != TOKEN_END) {
		if (current_token.type == TOKEN_INT || current_token.type == TOKEN_FLOAT) {
			// Parse field
			Field* field = parse_field();
			field->next = class_node->fields;  // Add field to the front of the list
			class_node->fields = field;
		}
		else if (current_token.type == TOKEN_VOID) {
			// Parse method
			Method* method = parse_method();
			method->next = class_node->methods;  // Add method to the front of the list
			class_node->methods = method;
		}
		else {
			// If an unexpected token is found in the class body, print an error message and exit
			printf("Error: Unexpected token in class body: %s\n", current_token.value);
			free_class_node(class_node);
			exit(1);
		}
	}

	return class_node;
}


// Parsing fields
Field* parse_field() {
	const char* type = current_token.value;  // Field type (e.g., 'int')
	expect(current_token.type);  // Expect data type (int, float, etc.)

	const char* name = current_token.value;  // Field name
	expect(TOKEN_IDENTIFIER);

	expect(TOKEN_SEMICOLON);  // Expect ';'

	Field* field = (Field*)malloc(sizeof(Field));
	field->type = type;
	field->name = name;
	field->next = NULL;
	return field;
}

// Parsing methods
Method* parse_method() {
	const char* return_type = current_token.value;  // Return type (e.g., "void")
	expect(current_token.type);  // Expect a valid return type like int, void, etc.

	Method* method = (Method*)malloc(sizeof(Method));
	method->return_type = return_type;

	// Expect method name (identifier)
	if (current_token.type != TOKEN_IDENTIFIER) {
		printf("Error: Expected method name but found '%s'\n", current_token.value);
		exit(1);
	}
	method->name = strdup(current_token.value);
	next_token_wrapper();  // Move to the next token after method name

	// Expect '(' to start parameter list
	expect(TOKEN_LPAREN);

	// Parse parameters (if any)
	method->parameters = NULL;
	ParameterNode* last_param = NULL;

	// Check if there are parameters or if we directly hit ')'
	if (current_token.type != TOKEN_RPAREN) {
		while (current_token.type != TOKEN_RPAREN) {
			const char* param_type = current_token.value;

			// Ensure valid parameter type
			if (current_token.type != TOKEN_INT && current_token.type != TOKEN_FLOAT) {
				printf("Error: Expected parameter type but found '%s'\n", current_token.value);
				exit(1);
			}

			next_token_wrapper();  // Move to parameter name

			if (current_token.type != TOKEN_IDENTIFIER) {
				printf("Error: Expected parameter name but found '%s'\n", current_token.value);
				exit(1);
			}

			const char* param_name = strdup(current_token.value);
			next_token_wrapper();  // Move to ',' or ')'

			ParameterNode* param = (ParameterNode*)malloc(sizeof(ParameterNode));
			param->type = param_type;
			param->name = param_name;
			param->next = NULL;

			// Add parameter to the list
			if (method->parameters == NULL) {
				method->parameters = param;
			}
			else {
				last_param->next = param;
			}
			last_param = param;

			// If there's a comma, move to the next parameter
			if (current_token.type == TOKEN_COMMA) {
				next_token_wrapper();  // Move to the next parameter
			}
			else if (current_token.type != TOKEN_RPAREN) {
				printf("Error: Expected ',' or ')' but found '%s'\n", current_token.value);
				exit(1);
			}
		}
	}

	// After parsing parameters, expect closing parenthesis ')'
	expect(TOKEN_RPAREN);

	// Now we should expect '{' for the method body
	if (current_token.type != TOKEN_LBRACE) {
		printf("Error: Expected '{' for method body but found '%s'\n", current_token.value);
		exit(1);
	}

	method->body = parse_block();  // Parse the method body

	// After parsing the method body, expect the closing '}'
	expect(TOKEN_RBRACE);

	return method;
}

ExpressionNode* parse_expression() {
	// Allocate memory for the left-hand side of the expression
	ExpressionNode* left = (ExpressionNode*)malloc(sizeof(ExpressionNode));
	if (!left) {
		printf("Error: Memory allocation failed for ExpressionNode.\n");
		exit(1);
	}

	// Parse the initial part of the expression (e.g., identifier or constant)
	if (current_token.type == TOKEN_IDENTIFIER) {
		left->variable = strdup(current_token.value);
		left->value = 0;
		left->next = NULL;
		next_token_wrapper();  // Move to the next token
	}
	else if (current_token.type == TOKEN_INT) {
		left->variable = NULL;
		left->value = atoi(current_token.value);
		left->next = NULL;
		next_token_wrapper();  // Move to the next token
	}
	else {
		printf("Error: Unexpected token in expression: %s\n", current_token.value);
		free(left);
		exit(1);
	}

	// Now handle possible comparison and arithmetic operators
	while (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS ||
		current_token.type == TOKEN_MULTIPLY || current_token.type == TOKEN_DIVIDE ||
		current_token.type == TOKEN_LESS || current_token.type == TOKEN_GREATER ||
		current_token.type == TOKEN_LESS_EQUAL || current_token.type == TOKEN_GREATER_EQUAL ||
		current_token.type == TOKEN_EQUAL || current_token.type == TOKEN_NOT_EQUAL) {

		// Create a new operator node
		ExpressionNode* operatorNode = (ExpressionNode*)malloc(sizeof(ExpressionNode));
		if (!operatorNode) {
			printf("Error: Memory allocation failed for operator ExpressionNode.\n");
			free(left);
			exit(1);
		}

		operatorNode->variable = strdup(current_token.value);  // Store the operator
		operatorNode->value = 0;
		operatorNode->next = left;  // Link the current left-hand side as part of the new operator node

		next_token_wrapper();  // Move past the operator

		// Parse the right-hand side operand
		ExpressionNode* right = (ExpressionNode*)malloc(sizeof(ExpressionNode));
		if (!right) {
			printf("Error: Memory allocation failed for right-hand side ExpressionNode.\n");
			free(operatorNode);
			free(left);
			exit(1);
		}

		if (current_token.type == TOKEN_IDENTIFIER) {
			right->variable = strdup(current_token.value);
			right->value = 0;
			right->next = NULL;
			next_token_wrapper();  // Move to the next token after identifier
		}
		else if (current_token.type == TOKEN_INT) {
			right->variable = NULL;
			right->value = atoi(current_token.value);
			right->next = NULL;
			next_token_wrapper();  // Move to the next token after integer
		}
		else {
			printf("Error: Expected an identifier or value after operator but found '%s'\n", current_token.value);
			free(right);
			free(operatorNode);
			free(left);
			exit(1);
		}

		// The new operator node becomes the root, with `left` and `right` operands attached
		operatorNode->next->next = right;  // Link right-hand operand to operator
		left = operatorNode;  // The operator becomes the new root of this expression
	}

	return left;  // Return the root of the constructed expression tree
}








// Parsing if statement
IfNode* parse_if_statement() {
	IfNode* if_node = (IfNode*)malloc(sizeof(IfNode));

	expect(TOKEN_IF);  // Expect 'if'
	expect(TOKEN_LPAREN);  // Expect '(' for condition
	if_node->condition = parse_expression();  // Parse condition expression
	expect(TOKEN_RPAREN);  // Expect ')' to close condition

	// Parse the true block (what happens if the condition is true)
	if_node->trueBlock = parse_block();

	// Check if there's an 'else' block
	if (current_token.type == TOKEN_ELSE) {
		expect(TOKEN_ELSE);  // Expect 'else'
		if_node->falseBlock = parse_block();  // Parse the false block
	}
	else {
		if_node->falseBlock = NULL;
	}

	return if_node;
}

ForNode* parse_for_loop() {
	ForNode* for_node = (ForNode*)malloc(sizeof(ForNode));
	if (!for_node) {
		printf("Error: Memory allocation failed for ForNode.\n");
		exit(1);
	}

	expect(TOKEN_FOR);  // Expect 'for' keyword
	expect(TOKEN_LPAREN);  // Expect '(' to start the for loop components

	// Parse initializer (e.g., int i = 0)
	if (current_token.type == TOKEN_INT || current_token.type == TOKEN_FLOAT) {
		// Recognize type declaration
		const char* type = current_token.value;
		next_token_wrapper();  // Move to the variable name

		if (current_token.type != TOKEN_IDENTIFIER) {
			printf("Error: Expected variable name but found '%s'\n", current_token.value);
			free(for_node);
			exit(1);
		}

		const char* variable_name = strdup(current_token.value);
		next_token_wrapper();  // Move to '=' or semicolon

		// Check if it's an assignment
		if (current_token.type == TOKEN_ASSIGN) {
			next_token_wrapper();  // Move past the '='

			if (current_token.type != TOKEN_INT && current_token.type != TOKEN_FLOAT) {
				printf("Error: Expected a value (int or float) but found '%s'\n", current_token.value);
				free(for_node);
				exit(1);
			}

			// Store the initializer value
			for_node->initializer = (ExpressionNode*)malloc(sizeof(ExpressionNode));
			if (!for_node->initializer) {
				printf("Error: Memory allocation failed for initializer ExpressionNode.\n");
				free(for_node);
				exit(1);
			}
			for_node->initializer->variable = strdup(variable_name);
			for_node->initializer->value = atoi(current_token.value);
			for_node->initializer->next = NULL;

			next_token_wrapper();  // Move to the semicolon after initialization

			if (current_token.type != TOKEN_SEMICOLON) {
				printf("Error: Expected ';' after initializer but found '%s'\n", current_token.value);
				free(for_node->initializer);
				free(for_node);
				exit(1);
			}
		}
		else {
			printf("Error: Expected '=' after variable name but found '%s'\n", current_token.value);
			free(for_node);
			exit(1);
		}

		next_token_wrapper();  // Move past the semicolon after initialization
	}
	else {
		printf("Error: Expected type (int/float) for loop initializer but found '%s'\n", current_token.value);
		free(for_node);
		exit(1);
	}

	// Parse condition (e.g., i < 3)
	for_node->condition = parse_expression();  // Parse the condition expression
	if (current_token.type != TOKEN_SEMICOLON) {
		printf("Error: Expected ';' after condition but found '%s'\n", current_token.value);
		free(for_node->initializer);
		free(for_node);
		exit(1);
	}
	next_token_wrapper();  // Move past the semicolon after condition

	// Parse update expression (e.g., i++, i--)
	for_node->update = (ExpressionNode*)malloc(sizeof(ExpressionNode));
	if (!for_node->update) {
		printf("Error: Memory allocation failed for update ExpressionNode.\n");
		free(for_node->initializer);
		free(for_node->condition);
		free(for_node);
		exit(1);
	}
	if (current_token.type == TOKEN_IDENTIFIER) {
		for_node->update->variable = strdup(current_token.value);
		next_token_wrapper();  // Move to the next token

		// Check for increment (++) or decrement (--)
		if (current_token.type == TOKEN_INCREMENT || current_token.type == TOKEN_DECREMENT) {
			for_node->update->value = (current_token.type == TOKEN_INCREMENT) ? 1 : -1;
			for_node->update->next = NULL;
			next_token_wrapper();  // Move past ++ or --
		}
		else {
			printf("Error: Expected '++' or '--' in update expression but found '%s'\n", current_token.value);
			free(for_node->initializer);
			free(for_node->condition);
			free(for_node->update);
			free(for_node);
			exit(1);
		}
	}
	else {
		printf("Error: Expected identifier in update expression but found '%s'\n", current_token.value);
		free(for_node->initializer);
		free(for_node->condition);
		free(for_node->update);
		free(for_node);
		exit(1);
	}

	// Ensure that the next token is a closing parenthesis
	if (current_token.type != TOKEN_RPAREN) {
		printf("Error: Expected ')' after update expression but found '%s'\n", current_token.value);
		free(for_node->initializer);
		free(for_node->condition);
		free(for_node->update);
		free(for_node);
		exit(1);
	}
	next_token_wrapper();  // Move past ')' to parse the for loop body

	// Parse the body of the loop
	for_node->body = parse_block();  // Parse the loop body, which is a block of statements

	return for_node;
}




StatementNode* parse_statement() {
	StatementNode* stmt = (StatementNode*)malloc(sizeof(StatementNode));

	if (current_token.type == TOKEN_IDENTIFIER) {
		// Handle assignment statement
		char* variable_name = strdup(current_token.value);  // Store the variable name
		next_token_wrapper();  // Move to next token (should be '=')

		if (current_token.type == TOKEN_ASSIGN) {
			next_token_wrapper();  // Move to the value being assigned

			ExpressionNode* value_expr = parse_expression();  // Parse the assigned value
			stmt->node_type = NODE_ASSIGNMENT;

			// Create a new expression node for the assignment
			ExpressionNode* assignment_expr = (ExpressionNode*)malloc(sizeof(ExpressionNode));
			assignment_expr->variable = variable_name;
			assignment_expr->next = value_expr;
			stmt->expression = assignment_expr;

			expect(TOKEN_SEMICOLON);  // Expect a semicolon after the assignment
		}
		else {
			printf("Error: Expected '=' for assignment after variable name but found '%s'\n", current_token.value);
			exit(1);
		}
	}
	else if (current_token.type == TOKEN_IF) {
		// Handle 'if' statement
		IfNode* if_node = parse_if_statement();
		stmt->node_type = NODE_IF;
		stmt->ifNode = if_node;
	}
	else if (current_token.type == TOKEN_FOR) {
		// Handle 'for' loop
		ForNode* for_node = parse_for_loop();
		stmt->node_type = NODE_FOR;
		stmt->forNode = for_node;
	}
	else {
		// Unsupported statement
		printf("Error: Unexpected token in statement: %s\n", current_token.value);
		free(stmt);
		exit(1);
	}

	return stmt;
}

BlockNode* parse_block() {
	expect(TOKEN_LBRACE);  // Expect '{'

	// Allocate memory for the first block node
	BlockNode* block = (BlockNode*)malloc(sizeof(BlockNode));
	if (!block) {
		printf("Error: Memory allocation failed for BlockNode.\n");
		exit(1);
	}

	block->next = NULL;
	BlockNode* current_block = block;

	// Parse each statement in the block
	while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_END) {
		StatementNode* stmt = parse_statement();  // Parse the statement

		// Allocate a new block node for the statement
		BlockNode* new_block = (BlockNode*)malloc(sizeof(BlockNode));
		if (!new_block) {
			printf("Error: Memory allocation failed for BlockNode.\n");
			exit(1);
		}

		new_block->node_type = stmt->node_type;

		// Assign the appropriate statement to the block node
		switch (stmt->node_type) {
		case NODE_IF:
			new_block->ifNode = stmt->ifNode;
			break;
		case NODE_FOR:
			new_block->forNode = stmt->forNode;
			break;
		case NODE_EXPRESSION:
			new_block->expression = stmt->expression;
			break;
		case NODE_ASSIGNMENT:
			new_block->expression = stmt->expression;  // Assign the assignment expression to the block node
			break;
		default:
			printf("Error: Unsupported node type in block.\n");
			free(new_block);
			free(stmt);
			exit(1);
		}

		// Link the new block node to the list
		current_block->next = new_block;
		current_block = new_block;
		current_block->next = NULL;

		// Free temporary statement node
		free(stmt);
	}

	expect(TOKEN_RBRACE);  // Expect '}'

	return block->next;  // Return the parsed block
}

void execute_block(BlockNode* block, Object* obj, LocalVariable* local_table) {
	BlockNode* current = block;
	while (current != NULL) {
		switch (current->node_type) {
		case NODE_IF:
			execute_if(current->ifNode, obj, local_table);
			break;
		case NODE_FOR:
			execute_for(current->forNode, obj);  // Pass the object and handle loop variables internally
			break;
		case NODE_ASSIGNMENT:
			execute_expression(current->expression, obj, local_table);  // Evaluate the assignment
			break;
		case NODE_EXPRESSION:
			execute_expression(current->expression, obj, local_table);
			break;
		default:
			printf("Error: Unsupported node type in block.\n");
			exit(1);
		}
		current = current->next;
	}
}



// Execute an if statement
void execute_if(IfNode* if_node, Object* obj, LocalVariable* local_table) {
	if (!if_node) {
		printf("Error: Null IfNode encountered.\n");
		return;
	}

	// Step 1: Evaluate the condition using both the object and the local scope
	int condition_value = evaluate_expression(if_node->condition, obj, local_table);

	// Step 2: Decide which block to execute based on the condition
	if (condition_value) {
		// If the condition is true, execute the true block
		execute_block(if_node->trueBlock, obj, local_table);
	}
	else if (if_node->falseBlock) {
		// If the condition is false and there's a false block, execute it
		execute_block(if_node->falseBlock, obj, local_table);
	}
}



void execute_for(ForNode* for_node, Object* obj) {
	if (!for_node) {
		printf("Error: Null ForNode encountered.\n");
		return;
	}

	// Create a local variable table for the scope of the for loop
	LocalVariable* local_table = NULL;

	// Step 1: Execute the initializer (e.g., int i = 0)
	if (for_node->initializer && for_node->initializer->variable != NULL) {
		// Add the variable to the local table
		update_local_variable(&local_table, for_node->initializer->variable, for_node->initializer->value);
	}

	// Step 2: Loop while the condition is true
	while (evaluate_expression(for_node->condition, obj, local_table)) {
		// Step 3: Execute the body of the loop
		execute_block(for_node->body, obj, local_table);

		// Step 4: Execute the update expression (e.g., i++)
		if (for_node->update && for_node->update->variable != NULL) {
			int current_value = lookup_local_variable(local_table, for_node->update->variable);
			if (for_node->update->value == 1) {  // Increment
				update_local_variable(&local_table, for_node->update->variable, current_value + 1);
			}
			else if (for_node->update->value == -1) {  // Decrement
				update_local_variable(&local_table, for_node->update->variable, current_value - 1);
			}
		}
	}
}



int evaluate_expression(ExpressionNode* expr, Object* obj, LocalVariable* local_table) {
	if (!expr) {
		printf("Error: Null expression encountered.\n");
		exit(1);
	}

	// If the expression is an operator, handle it as a binary expression
	if (expr->variable != NULL) {
		if (strcmp(expr->variable, "<") == 0 || strcmp(expr->variable, ">") == 0 ||
			strcmp(expr->variable, "<=") == 0 || strcmp(expr->variable, ">=") == 0 ||
			strcmp(expr->variable, "==") == 0 || strcmp(expr->variable, "!=") == 0 ||
			strcmp(expr->variable, "+") == 0 || strcmp(expr->variable, "-") == 0 ||
			strcmp(expr->variable, "*") == 0 || strcmp(expr->variable, "/") == 0) {

			// Handle binary operators
			if (!expr->next || !expr->next->next) {
				printf("Error: Invalid binary expression. Missing operand.\n");
				exit(1);
			}

			int left_value = evaluate_expression(expr->next, obj, local_table);    // Left operand
			int right_value = evaluate_expression(expr->next->next, obj, local_table);  // Right operand

			// Perform the operation based on the operator type
			if (strcmp(expr->variable, "<") == 0) {
				return left_value < right_value;
			}
			else if (strcmp(expr->variable, ">") == 0) {
				return left_value > right_value;
			}
			else if (strcmp(expr->variable, "<=") == 0) {
				return left_value <= right_value;
			}
			else if (strcmp(expr->variable, ">=") == 0) {
				return left_value >= right_value;
			}
			else if (strcmp(expr->variable, "==") == 0) {
				return left_value == right_value;
			}
			else if (strcmp(expr->variable, "!=") == 0) {
				return left_value != right_value;
			}
			else if (strcmp(expr->variable, "+") == 0) {
				return left_value + right_value;
			}
			else if (strcmp(expr->variable, "-") == 0) {
				return left_value - right_value;
			}
			else if (strcmp(expr->variable, "*") == 0) {
				return left_value * right_value;
			}
			else if (strcmp(expr->variable, "/") == 0) {
				if (right_value == 0) {
					printf("Error: Division by zero.\n");
					exit(1);
				}
				return left_value / right_value;
			}
		}
		else {
			// Handle non-operator variables (local variables or fields)
			// First, try to find the variable in the local table
			LocalVariable* local_var = local_table;
			while (local_var) {
				if (strcmp(local_var->variable_name, expr->variable) == 0) {
					return local_var->value;
				}
				local_var = local_var->next;
			}

			// If not found in the local table, look for it in the object fields
			return lookup_object_field(obj, expr->variable);
		}
	}

	// If the expression is a constant value, simply return the value
	if (expr->variable == NULL && expr->value != 0) {
		return expr->value;
	}

	// If we encounter an unexpected structure, print an error
	printf("Error: Unexpected expression structure.\n");
	exit(1);
}




// Free memory allocated for a class node
void free_class_node(ClassNode* class_node) {
	// Free fields
	Field* field = class_node->fields;
	while (field) {
		Field* temp = field;
		field = field->next;
		free(temp);
	}
	// Free methods
	Method* method = class_node->methods;
	while (method) {
		ParameterNode* param = method->parameters;
		while (param) {
			ParameterNode* temp = param;
			param = param->next;
			free(temp);
		}

		Method* temp = method;
		method = method->next;
		free(temp);
	}
	// Free class node
	free(class_node);
}

// Create an object from a class definition
Object* create_object(ClassNode* class_node) {
	// Allocate memory for the object
	Object* obj = (Object*)malloc(sizeof(Object));
	obj->class_type = class_node;

	// Initialize fields with default values
	obj->field_values = NULL;

	Field* class_field = class_node->fields;
	while (class_field != NULL) {
		Field* new_field = (Field*)malloc(sizeof(Field));
		new_field->type = class_field->type;
		new_field->name = class_field->name;

		// Initialize field values (0 for int, 0.0 for float)
		if (strcmp(class_field->type, "int") == 0) {
			new_field->value = malloc(sizeof(int));  // Allocate memory for an int
			*((int*)new_field->value) = 0;           // Initialize to zero
		}
		else if (strcmp(class_field->type, "float") == 0) {
			new_field->value = malloc(sizeof(float)); // Allocate memory for a float
			*((float*)new_field->value) = 0.0f;       // Initialize to zero
		}
		else {
			new_field->value = NULL;  // Default value for other types
		}

		new_field->next = obj->field_values; // Link to the list of field values
		obj->field_values = new_field;

		class_field = class_field->next; // Move to the next field
	}

	return obj; // Return the created object
}

// Execute a method on an object
void execute_method(Object* obj, const char* method_name) {
	// Validate input parameters
	if (obj == NULL) {
		printf("Error: obj is NULL.\n");
		return;
	}
	if (method_name == NULL) {
		printf("Error: method_name is NULL.\n");
		return;
	}

	// Find the method in the class definition
	Method* method = obj->class_type->methods;
	while (method != NULL) {
		if (strcmp(method->name, method_name) == 0) {
			printf("Executing method %s on object of class %s\n", method_name, obj->class_type->class_name);

			// Step 1: Create a local variable table for the method
			LocalVariable* local_table = NULL;

			// Step 2: Initialize method parameters in the local variable table
			ParameterNode* param = method->parameters;
			while (param != NULL) {
				// Initialize parameters with a default value (0 for simplicity)
				update_local_variable(&local_table, param->name, 0);
				param = param->next;
			}

			// Step 3: Execute the body of the method
			execute_block(method->body, obj, local_table);

			// Step 4: Clean up local variables after method execution
			LocalVariable* current = local_table;
			while (current != NULL) {
				LocalVariable* temp = current;
				current = current->next;
				free(temp->variable_name);  // Free the name string
				free(temp);                 // Free the local variable
			}

			return;
		}
		method = method->next;
	}

	// If we reach here, the method was not found
	printf("Error: Method %s not found in class %s\n", method_name, obj->class_type->class_name);
}


// Free memory allocated for an object
void free_object(Object* obj) {
	if (obj == NULL) return;

	Field* current_field = obj->field_values;
	while (current_field != NULL) {
		Field* next_field = current_field->next;
		if (current_field->value != NULL) {
			free(current_field->value); // Free field value
		}
		free(current_field); // Free field structure
		current_field = next_field;
	}

	free(obj); // Free the object itself
}

void update_object_field(Object* obj, const char* field_name, int value) {
	Field* field = obj->field_values;
	while (field) {
		if (strcmp(field->name, field_name) == 0) {
			*((int*)field->value) = value;
			return;
		}
		field = field->next;
	}
	printf("Error: Field %s not found in object.\n", field_name);
}

int lookup_object_field(Object* obj, const char* field_name) {
	Field* field = obj->field_values;
	while (field) {
		if (strcmp(field->name, field_name) == 0) {
			return *((int*)field->value);
		}
		field = field->next;
	}
	printf("Error: Field %s not found in object.\n", field_name);
	exit(1);
}

int lookup_local_variable(LocalVariable* local_table, const char* variable_name) {
	LocalVariable* current = local_table;
	while (current) {
		if (strcmp(current->variable_name, variable_name) == 0) {
			return current->value;
		}
		current = current->next;
	}
	printf("Error: Local variable %s not found.\n", variable_name);
	exit(1);
}

void update_local_variable(LocalVariable** local_table, const char* variable_name, int value) {
	LocalVariable* current = *local_table;
	while (current) {
		if (strcmp(current->variable_name, variable_name) == 0) {
			current->value = value; // Update existing variable
			return;
		}
		current = current->next;
	}

	// If not found, add a new variable to the local symbol table
	LocalVariable* new_variable = (LocalVariable*)malloc(sizeof(LocalVariable));
	new_variable->variable_name = strdup(variable_name); // Make a copy of the variable name
	new_variable->value = value;
	new_variable->next = *local_table;
	*local_table = new_variable;
}


// Free the symbol table when done with interpretation
void clean_up() {
	free_symbol_table();
}