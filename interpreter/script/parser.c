#include "parser.h"

void next_token_wrapper() {
	current_token = next_token();
	printf("Token Type: %s, Token Value: %s\n", token_type_to_string(current_token.type), current_token.value);

}



// Match a specific token type, error if not matched
void match(TokenType expected_type) {
	if (current_token.type == expected_type) {
		next_token_wrapper(); // Move to the next token
	}
	else {
		printf("Error: Expected %s but found %s\n", token_type_to_string(expected_type), token_type_to_string(current_token.type));
		exit(1);
	}
}

IfNode* parse_if() {
	IfNode* if_node = (IfNode*)malloc(sizeof(IfNode));
	if (!if_node) {
		printf("Error: Memory allocation failed for IfNode.\n");
		exit(1);
	}

	// Expect "if" keyword
	if (current_token.type == TOKEN_IF) {
		next_token_wrapper();  // Move past "if"
	}
	else {
		printf("Error: Expected 'if' but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	// Expect '(' for the condition
	match(TOKEN_LPAREN);

	// Parse the condition inside the parentheses (e.g., `x > 5`)
	if_node->condition = parse_expression();

	// Expect ')' after the condition
	match(TOKEN_RPAREN);

	// Parse the block of the if statement (true block)
	match(TOKEN_LBRACE);
	if_node->trueBlock = parse_block();

	// Parse the optional else block
	if (current_token.type == TOKEN_ELSE) {
		next_token_wrapper();  // Move past "else"
		match(TOKEN_LBRACE);
		if_node->falseBlock = parse_block();
	}
	else {
		if_node->falseBlock = NULL;  // No else block present
	}

	return if_node;
}


// Parse a `for` loop: `for (init; condition; update) { ... }`
ForNode* parse_for() {
	// Allocate memory for ForNode
	ForNode* for_node = (ForNode*)malloc(sizeof(ForNode));
	if (!for_node) {
		printf("Error: Memory allocation failed for ForNode.\n");
		exit(1);
	}

	// Expect "for" keyword
	if (current_token.type == TOKEN_FOR) {
		next_token_wrapper();  // Move past "for"
	}
	else {
		printf("Error: Expected 'for' but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	// Expect '(' to start the for loop components
	match(TOKEN_LPAREN);

	// Parse initialization (e.g., `int i = 0;`)
	if (current_token.type == TOKEN_INT || current_token.type == TOKEN_FLOAT) {
		// Allocate memory for the initialization ExpressionNode
		for_node->initializer = (ExpressionNode*)malloc(sizeof(ExpressionNode));
		if (!for_node->initializer) {
			printf("Error: Memory allocation failed for ExpressionNode.\n");
			exit(1);
		}

		// Set the data type
		const char* type = (current_token.type == TOKEN_INT) ? "int" : "float";
		next_token_wrapper();  // Move past the data type

		// Expect an identifier for the variable name
		if (current_token.type == TOKEN_IDENTIFIER) {
			for_node->initializer->variable = current_token.value;
			for_node->initializer->value = 0;  // Default value for the identifier
			next_token_wrapper();  // Move past the identifier
		}
		else {
			printf("Error: Expected variable name in initialization but found %s\n", token_type_to_string(current_token.type));
			exit(1);
		}

		// Handle optional assignment (e.g., `= 0`)
		if (current_token.type == TOKEN_ASSIGN) {
			next_token_wrapper();  // Move past '='
			if (current_token.type == TOKEN_INT) {
				for_node->initializer->value = atoi(current_token.value);  // Set the initial value
				next_token_wrapper();  // Move past the integer literal
			}
			else {
				printf("Error: Expected integer literal after '=' in initialization.\n");
				exit(1);
			}
		}

		// Expect a semicolon after the initialization statement
		match(TOKEN_SEMICOLON);
	}
	else {
		printf("Error: Expected type specifier in for loop initialization but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	// Parse the condition (e.g., `i < 10`)
	for_node->condition = parse_expression();

	// Expect a semicolon after the condition
	match(TOKEN_SEMICOLON);

	// Parse the update expression (e.g., `i++`)
	for_node->update = parse_expression();

	next_token_wrapper();
	// Expect ')' to close the for loop components
	match(TOKEN_RPAREN);

	// Parse the body of the for loop
	match(TOKEN_LBRACE);
	for_node->body = parse_block();

	return for_node;
}


// Parse a class definition: `class ClassName { ... }`
ClassNode* parse_class() {
	ClassNode* class_node = (ClassNode*)malloc(sizeof(ClassNode));

	match(TOKEN_CLASS);  // Expect 'class' keyword

	if (current_token.type == TOKEN_IDENTIFIER) {
		class_node->class_name = current_token.value; // Store class name
		next_token();  // Move past class name
	}
	else {
		printf("Error: Expected class name but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	match(TOKEN_LBRACE); // Expect '{'

	// Initialize fields and methods pointers to NULL
	class_node->fields = NULL;
	class_node->methods = NULL;

	Field* last_field = NULL;
	Method* last_method = NULL;

	// Parse fields and methods inside the class
	while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_END) {
		if (current_token.type == TOKEN_INT || current_token.type == TOKEN_FLOAT) {
			Field* field = parse_field();
			if (last_field == NULL) {
				class_node->fields = field;
			}
			else {
				last_field->next = field;
			}
			last_field = field;
		}
		else if (current_token.type == TOKEN_VOID || current_token.type == TOKEN_IDENTIFIER) {
			Method* method = parse_method();
			if (last_method == NULL) {
				class_node->methods = method;
			}
			else {
				last_method->next = method;
			}
			last_method = method;
		}
		else {
			printf("Error: Unexpected token in class body: %s\n", token_type_to_string(current_token.type));
			exit(1);
		}
	}

	match(TOKEN_RBRACE); // Expect '}'

	return class_node;
}

// Parse a field definition: `int fieldName;`
Field* parse_field() {
	Field* field = (Field*)malloc(sizeof(Field));

	// Set the type of the field
	if (current_token.type == TOKEN_INT) {
		field->type = "int";
	}
	else if (current_token.type == TOKEN_FLOAT) {
		field->type = "float";
	}
	else {
		printf("Error: Expected type specifier but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}
	next_token_wrapper();  // Move past type

	// Set the name of the field
	if (current_token.type == TOKEN_IDENTIFIER) {
		field->name = current_token.value;
		next_token_wrapper();  // Move past identifier
	}
	else {
		printf("Error: Expected identifier but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	match(TOKEN_SEMICOLON);  // Expect ';'

	field->value = NULL;  // Default to no initial value
	field->next = NULL;

	return field;
}

// Parse a method definition: `void methodName() { ... }`
// Parse a method definition: `void methodName(int a, float b) { ... }`
Method* parse_method() {
	Method* method = (Method*)malloc(sizeof(Method));
	if (!method) {
		printf("Error: Memory allocation failed for Method.\n");
		exit(1);
	}

	// Set the return type of the method
	if (current_token.type == TOKEN_VOID) {
		method->return_type = "void";
	}
	else if (current_token.type == TOKEN_INT) {
		method->return_type = "int";
	}
	else if (current_token.type == TOKEN_FLOAT) {
		method->return_type = "float";
	}
	else {
		printf("Error: Expected return type but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}
	next_token_wrapper();  // Move past return type

	// Set the name of the method
	if (current_token.type == TOKEN_IDENTIFIER) {
		method->name = current_token.value;
		next_token_wrapper();  // Move past method name
	}
	else {
		printf("Error: Expected method name but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	// Expect '(' and then parse parameters if present
	match(TOKEN_LPAREN);

	// Initialize parameters list as empty
	method->parameters = NULL;

	// If the next token is not a closing parenthesis, there are parameters to parse
	if (current_token.type != TOKEN_RPAREN) {
		ParameterNode* head = NULL;
		ParameterNode* last_param = NULL;

		// Loop to parse all parameters until we reach ')'
		while (current_token.type != TOKEN_RPAREN) {
			ParameterNode* param = (ParameterNode*)malloc(sizeof(ParameterNode));
			if (!param) {
				printf("Error: Memory allocation failed for ParameterNode.\n");
				exit(1);
			}

			// Parse parameter type
			if (current_token.type == TOKEN_INT) {
				param->type = "int";
			}
			else if (current_token.type == TOKEN_FLOAT) {
				param->type = "float";
			}
			else {
				printf("Error: Expected type specifier for parameter but found %s\n", token_type_to_string(current_token.type));
				exit(1);
			}
			next_token_wrapper();  // Move past the parameter type

			// Parse parameter name
			if (current_token.type == TOKEN_IDENTIFIER) {
				param->name = current_token.value;
				next_token_wrapper();  // Move past the parameter name
			}
			else {
				printf("Error: Expected identifier for parameter but found %s\n", token_type_to_string(current_token.type));
				exit(1);
			}

			// Add parameter to the list
			param->next = NULL;

			if (head == NULL) {
				head = param;  // Set the first parameter as the head of the list
			}
			else {
				last_param->next = param;  // Link the new parameter to the previous one
			}

			last_param = param;

			// If there's a comma, there are more parameters
			if (current_token.type == TOKEN_COMMA) {
				next_token_wrapper();  // Move past the comma
			}
			else if (current_token.type != TOKEN_RPAREN) {
				printf("Error: Expected ',' or ')' in parameter list but found %s\n", token_type_to_string(current_token.type));
				exit(1);
			}
		}

		method->parameters = head;
	}

	// Expect closing parenthesis ')'
	match(TOKEN_RPAREN);

	// Expect '{' and parse the method body
	match(TOKEN_LBRACE);
	method->body = parse_block();  // Parse the method body as a block of statements

	method->next = NULL;

	return method;
}


// Parse a block of statements enclosed by `{ ... }`
BlockNode* parse_block() {
	BlockNode* block = NULL;
	BlockNode* last_block = NULL;

	while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_END) {
		StatementNode* statement = parse_statement();

		BlockNode* new_block_node = (BlockNode*)malloc(sizeof(BlockNode));
		new_block_node->node_type = statement->node_type;
		new_block_node->next = NULL;

		// Copy the statement to the block's union member
		new_block_node->expression = statement->expression; // Copying for simplicity

		if (block == NULL) {
			block = new_block_node;
		}
		else {
			last_block->next = new_block_node;
		}
		last_block = new_block_node;
	}

	match(TOKEN_RBRACE); // Expect '}'

	return block;
}

// Parse an individual statement
StatementNode* parse_statement() {
	StatementNode* statement = (StatementNode*)malloc(sizeof(StatementNode));
	if (!statement) {
		printf("Error: Memory allocation failed for StatementNode.\n");
		exit(1);
	}

	// Handle return statement
	if (current_token.type == TOKEN_IDENTIFIER && strcmp(current_token.value, "return") == 0) {
		statement->node_type = NODE_RETURN;
		next_token_wrapper();  // Move past 'return'

		// Parse the return expression (if any)
		if (current_token.type != TOKEN_SEMICOLON) {
			statement->expression = parse_expression();
		}
		else {
			statement->expression = NULL;  // No return value (e.g., `return;`)
		}

		// Match the semicolon at the end of the return statement
		match(TOKEN_SEMICOLON);
	}
	// Handle assignment statement (like `x = 10;`)
	else if (current_token.type == TOKEN_IDENTIFIER) {
		// Capture the identifier (variable name)
		char* variable_name = current_token.value;
		next_token_wrapper();  // Move past the identifier

		// Check for assignment operator '='
		if (current_token.type == TOKEN_ASSIGN) {
			next_token_wrapper();  // Move past '='

			// Parse the right-hand side expression
			ExpressionNode* value = parse_expression();

			// Fill in the statement node for the assignment
			statement->node_type = NODE_ASSIGNMENT;
			statement->expression = (ExpressionNode*)malloc(sizeof(ExpressionNode));
			if (!statement->expression) {
				printf("Error: Memory allocation failed for ExpressionNode.\n");
				exit(1);
			}

			// Set the left-hand side variable and link the right-hand side value
			statement->expression->variable = variable_name;  // The variable being assigned to
			statement->expression->value = 0;  // Placeholder since this node represents a variable
			statement->expression->next = value;  // The right-hand side value parsed from the expression

			// Ensure the assignment statement ends with a semicolon
			match(TOKEN_SEMICOLON);
		}
		else {
			printf("Error: Expected '=' after identifier in assignment, but found %s\n", token_type_to_string(current_token.type));
			exit(1);
		}
	}
	// Handle variable declarations (e.g., `int y = 10;`)
	else if (current_token.type == TOKEN_INT || current_token.type == TOKEN_FLOAT || current_token.type == TOKEN_VOID) {
		statement->node_type = NODE_FIELD;
		Field* field = (Field*)malloc(sizeof(Field));
		if (!field) {
			printf("Error: Memory allocation failed for Field.\n");
			exit(1);
		}

		// Set the type of the variable (e.g., int, float)
		if (current_token.type == TOKEN_INT) {
			field->type = "int";
		}
		else if (current_token.type == TOKEN_FLOAT) {
			field->type = "float";
		}
		else {
			printf("Error: Unexpected type for variable declaration.\n");
			exit(1);
		}

		next_token_wrapper();  // Move past the type

		// Parse the variable name
		if (current_token.type == TOKEN_IDENTIFIER) {
			field->name = current_token.value;
			next_token_wrapper();  // Move past the identifier
		}
		else {
			printf("Error: Expected identifier for variable name but found %s\n", token_type_to_string(current_token.type));
			exit(1);
		}

		// Handle optional initialization (e.g., `= 10`)
		if (current_token.type == TOKEN_ASSIGN) {
			next_token_wrapper();  // Move past '='
			ExpressionNode* value = parse_expression();  // Parse the value being assigned
			field->value = value;  // Store the value in the field structure
		}
		else {
			field->value = NULL;  // No initial value assigned
		}

		// Match the semicolon at the end of the declaration
		match(TOKEN_SEMICOLON);

		// Set the statement's field to the parsed field
		statement->field = field;
	}
	else {
		printf("Error: Unexpected token in statement: %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	return statement;
}



// Parse an expression (a simplified version, handling only literals and identifiers)
ExpressionNode* parse_expression() {
	// Parse the left-hand side of the expression
	ExpressionNode* left = (ExpressionNode*)malloc(sizeof(ExpressionNode));
	if (!left) {
		printf("Error: Memory allocation failed for ExpressionNode.\n");
		exit(1);
	}

	// Parse either an identifier or a literal as the left operand
	if (current_token.type == TOKEN_IDENTIFIER) {
		left->variable = current_token.value;
		left->value = 0; // Placeholder for literals when it's an identifier
		left->next = NULL;
		next_token_wrapper();  // Move past the identifier
	}
	else if (current_token.type == TOKEN_INT) {
		left->variable = NULL;
		left->value = atoi(current_token.value);
		left->next = NULL;
		next_token_wrapper();  // Move past the integer literal
	}
	else {
		printf("Error: Unexpected token in expression: %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	// Handle binary operators like `>`, `<`, `>=`, `<=`, `+`, `-`
	while (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS ||
		current_token.type == TOKEN_GREATER || current_token.type == TOKEN_LESS ||
		current_token.type == TOKEN_GREATER_EQUAL || current_token.type == TOKEN_LESS_EQUAL ||
		current_token.type == TOKEN_EQUAL || current_token.type == TOKEN_NOT_EQUAL) {

		// Create a new operator node
		ExpressionNode* operator_node = (ExpressionNode*)malloc(sizeof(ExpressionNode));
		if (!operator_node) {
			printf("Error: Memory allocation failed for ExpressionNode.\n");
			exit(1);
		}

		// Set the operator type
		if (current_token.type == TOKEN_PLUS) {
			operator_node->value = '+';  // Use ASCII value for plus
		}
		else if (current_token.type == TOKEN_MINUS) {
			operator_node->value = '-';  // Use ASCII value for minus
		}
		else if (current_token.type == TOKEN_GREATER) {
			operator_node->value = '>';  // Use ASCII value for greater-than
		}
		else if (current_token.type == TOKEN_LESS) {
			operator_node->value = '<';  // Use ASCII value for less-than
		}
		else if (current_token.type == TOKEN_GREATER_EQUAL) {
			operator_node->value = '>=';   // Custom value for greater-equal (using placeholder values)
		}
		else if (current_token.type == TOKEN_LESS_EQUAL) {
			operator_node->value = '<=';   // Custom value for less-equal
		}
		else if (current_token.type == TOKEN_EQUAL) {
			operator_node->value = '==';   // Custom value for equality
		}
		else if (current_token.type == TOKEN_NOT_EQUAL) {
			operator_node->value = '!=';   // Custom value for not-equal
		}
		operator_node->variable = NULL;  // Operators do not have variable names
		next_token_wrapper();  // Move past the operator

		// Parse the right-hand side of the operator
		ExpressionNode* right = (ExpressionNode*)malloc(sizeof(ExpressionNode));
		if (!right) {
			printf("Error: Memory allocation failed for ExpressionNode.\n");
			exit(1);
		}

		if (current_token.type == TOKEN_IDENTIFIER) {
			right->variable = current_token.value;  // Assign the variable name
			right->value = 0;                       // No direct value since this is an identifier
			right->next = NULL;
			next_token_wrapper();  // Move past the identifier
		}
		else if (current_token.type == TOKEN_INT) {
			right->variable = NULL;                 // Not an identifier, so no variable name
			right->value = atoi(current_token.value);  // Assign the literal value
			right->next = NULL;
			next_token_wrapper();  // Move past the literal
		}
		else {
			printf("Error: Expected literal or identifier but found %s\n", token_type_to_string(current_token.type));
			exit(1);
		}

		// Link the operator node and right operand to the left side
		operator_node->next = right;  // Operator points to the right-hand operand
		left->next = operator_node;   // Left operand points to the operator
		left = operator_node;         // Update left to continue parsing if more operators follow
	}

	return left;
}
