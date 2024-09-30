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

	// Expect the "if" keyword
	if (current_token.type == TOKEN_IF) {
		next_token_wrapper();  // Move past "if"
	}
	else {
		printf("Error: Expected 'if' but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	// Expect '(' and then parse the condition
	match(TOKEN_LPAREN);
	if_node->condition = parse_expression();  // Parse the full condition expression
	match(TOKEN_RPAREN);

	// Parse the true block of the if statement
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
			for_node->initializer->variable = (char*)current_token.value;
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
		char* variable_name = (char*)current_token.value;
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


// Parse factor: handles numbers, variables, and expressions in parentheses
ExpressionNode* parse_factor() {
	ExpressionNode* node = (ExpressionNode*)malloc(sizeof(ExpressionNode));
	if (!node) {
		printf("Error: Memory allocation failed for ExpressionNode.\n");
		exit(1);
	}

	// Handle identifiers (variables)
	if (current_token.type == TOKEN_IDENTIFIER) {
		node->variable = current_token.value;  // Store variable name
		node->value = 0;  // Not a literal
		node->next = NULL;
		next_token_wrapper();  // Move past the identifier
	}
	// Handle integer literals
	else if (current_token.type == TOKEN_INT) {
		node->variable = NULL;  // No variable name for literals
		node->value = atoi(current_token.value);  // Convert token value to integer
		node->next = NULL;
		next_token_wrapper();  // Move past the integer literal
	}
	// Handle parenthesized expressions: (expression)
	else if (current_token.type == TOKEN_LPAREN) {
		next_token_wrapper();  // Move past '('
		node = parse_expression();  // Parse the expression inside parentheses
		if (current_token.type != TOKEN_RPAREN) {
			printf("Error: Expected TOKEN_RPAREN but found %s\n", token_type_to_string(current_token.type));
			exit(1);
		}
		next_token_wrapper();  // Move past ')'
	}
	// Handle unexpected end of input
	else if (current_token.type == TOKEN_END) {
		printf("Error: Unexpected end of input while parsing factor.\n");
		free(node);
		exit(1);
	}
	// Handle unknown token
	else {
		printf("Error: Unexpected token in factor: %s\n", token_type_to_string(current_token.type));
		free(node);
		exit(1);
	}

	return node;
}

// Parse terms: handles multiplication (*) and division (/)
ExpressionNode* parse_term() {
	ExpressionNode* left = parse_factor();  // Parse the first factor

	while (current_token.type == TOKEN_MULTIPLY || current_token.type == TOKEN_DIVIDE) {
		ExpressionNode* operator_node = (ExpressionNode*)malloc(sizeof(ExpressionNode));
		if (!operator_node) {
			printf("Error: Memory allocation failed for ExpressionNode.\n");
			exit(1);
		}

		// Set the operator value
		if (current_token.type == TOKEN_MULTIPLY) {
			operator_node->value = '*';
		}
		else if (current_token.type == TOKEN_DIVIDE) {
			operator_node->value = '/';
		}

		operator_node->variable = NULL;  // Operators do not have variable names
		next_token_wrapper();  // Move past the operator

		// Parse the right-hand side factor
		operator_node->next = parse_factor();

		// Link nodes
		operator_node->next->next = NULL;  // Ensure right-hand next is NULL
		operator_node->next = left;  // Operator points to left operand
		left = operator_node;  // Update left to point to operator
	}

	return left;
}

WhileNode* parse_while() {
	WhileNode* while_node = (WhileNode*)malloc(sizeof(WhileNode));
	if (!while_node) {
		printf("Error: Memory allocation failed for WhileNode.\n");
		exit(1);
	}

	// Expect "while" keyword
	if (current_token.type == TOKEN_WHILE) {
		next_token_wrapper();  // Move past "while"
	}
	else {
		printf("Error: Expected 'while' but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	// Expect '(' and parse condition
	match(TOKEN_LPAREN);
	while_node->condition = parse_expression();
	match(TOKEN_RPAREN);

	// Parse the body of the while loop
	match(TOKEN_LBRACE);
	while_node->body = parse_block();
	match(TOKEN_RBRACE);

	return while_node;
}

DoWhileNode* parse_do_while() {
	DoWhileNode* do_while_node = (DoWhileNode*)malloc(sizeof(DoWhileNode));
	if (!do_while_node) {
		printf("Error: Memory allocation failed for DoWhileNode.\n");
		exit(1);
	}

	// Expect "do" keyword
	if (current_token.type == TOKEN_DO) {
		next_token_wrapper();  // Move past "do"
	}
	else {
		printf("Error: Expected 'do' but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	// Parse the body of the do-while loop
	match(TOKEN_LBRACE);
	do_while_node->body = parse_block();
	match(TOKEN_RBRACE);

	// Expect "while" keyword and parse condition
	if (current_token.type == TOKEN_WHILE) {
		next_token_wrapper();  // Move past "while"
	}
	else {
		printf("Error: Expected 'while' but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}
	match(TOKEN_LPAREN);
	do_while_node->condition = parse_expression();
	match(TOKEN_RPAREN);
	match(TOKEN_SEMICOLON);

	return do_while_node;
}

SwitchNode* parse_switch() {
	SwitchNode* switch_node = (SwitchNode*)malloc(sizeof(SwitchNode));
	if (!switch_node) {
		printf("Error: Memory allocation failed for SwitchNode.\n");
		exit(1);
	}

	// Expect "switch" keyword
	if (current_token.type == TOKEN_SWITCH) {
		next_token_wrapper();  // Move past "switch"
	}
	else {
		printf("Error: Expected 'switch' but found %s\n", token_type_to_string(current_token.type));
		exit(1);
	}

	// Expect '(' and parse the switch expression
	match(TOKEN_LPAREN);
	switch_node->expression = parse_expression();
	match(TOKEN_RPAREN);

	// Parse the body of the switch statement
	match(TOKEN_LBRACE);
	CaseNode* current_case = NULL;

	while (current_token.type == TOKEN_CASE || current_token.type == TOKEN_DEFAULT) {
		CaseNode* case_node = (CaseNode*)malloc(sizeof(CaseNode));
		if (!case_node) {
			printf("Error: Memory allocation failed for CaseNode.\n");
			exit(1);
		}

		if (current_token.type == TOKEN_CASE) {
			next_token_wrapper();  // Move past "case"
			case_node->value = atoi(current_token.value);
			next_token_wrapper();  // Move past value
			match(TOKEN_COLON);
		}
		else if (current_token.type == TOKEN_DEFAULT) {
			next_token_wrapper();  // Move past "default"
			match(TOKEN_COLON);
			case_node->value = -1; // Use -1 or another marker for "default"
		}

		case_node->statements = parse_block();  // Parse statements in the case
		case_node->next = NULL;

		if (current_case == NULL) {
			switch_node->cases = case_node;
		}
		else {
			current_case->next = case_node;
		}
		current_case = case_node;
	}

	match(TOKEN_RBRACE);
	return switch_node;
}


// Parse an expression (a simplified version, handling only literals and identifiers)

ExpressionNode* parse_expression() {
	ExpressionNode* left = parse_term();  // Start by parsing a term (handles *, /)

	while (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS ||
		current_token.type == TOKEN_GREATER || current_token.type == TOKEN_LESS ||
		current_token.type == TOKEN_GREATER_EQUAL || current_token.type == TOKEN_LESS_EQUAL ||
		current_token.type == TOKEN_EQUAL || current_token.type == TOKEN_NOT_EQUAL ||
		current_token.type == TOKEN_AND || current_token.type == TOKEN_OR) {

		// Create a new operator node
		ExpressionNode* operator_node = (ExpressionNode*)malloc(sizeof(ExpressionNode));
		if (!operator_node) {
			printf("Error: Memory allocation failed for ExpressionNode.\n");
			exit(1);
		}

		// Set operator value based on token type
		if (current_token.type == TOKEN_PLUS) {
			operator_node->value = '+';
		}
		else if (current_token.type == TOKEN_MINUS) {
			operator_node->value = '-';
		}
		else if (current_token.type == TOKEN_GREATER) {
			operator_node->value = '>';
		}
		else if (current_token.type == TOKEN_LESS) {
			operator_node->value = '<';
		}
		else if (current_token.type == TOKEN_GREATER_EQUAL) {
			operator_node->value = '>=';  // Custom value for '>='
		}
		else if (current_token.type == TOKEN_LESS_EQUAL) {
			operator_node->value = '<=';  // Custom value for '<='
		}
		else if (current_token.type == TOKEN_EQUAL) {
			operator_node->value = '==';  // Custom value for '=='
		}
		else if (current_token.type == TOKEN_NOT_EQUAL) {
			operator_node->value = '==';  // Custom value for '!='
		}
		else if (current_token.type == TOKEN_AND) {
			operator_node->value = '&&';  // Representing logical AND (&&)
		}
		else if (current_token.type == TOKEN_OR) {
			operator_node->value = '||';  // Representing logical OR (||)
		}

		operator_node->variable = NULL;  // Operators do not represent variables
		next_token_wrapper();  // Move past the operator

		// Parse the right-hand side term
		ExpressionNode* right = parse_term();  // Get the right operand for the operator

		// Link nodes
		operator_node->next = right;  // Operator points to right-hand operand
		operator_node->next->next = NULL;  // Ensure right operand's next is NULL
		operator_node->next = left;  // Operator points back to left operand
		left = operator_node;  // Update left to point to operator node
	}

	return left;
}


