#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>  // Required for fprintf and stderr

#ifdef _MSC_VER
#define strdup _strdup
#endif

// Create a number node
ASTNode* create_number_node(const char* value) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_NUMBER;
	node->value = strdup(value);
	node->left = NULL;
	node->right = NULL;
	node->body = NULL;
	node->body_count = 0;
	return node;
}

// Create a boolean node
ASTNode* create_boolean_node(int value) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_BOOLEAN;
	node->value = value ? strdup("true") : strdup("false");
	node->left = NULL;
	node->right = NULL;
	node->body = NULL;
	node->body_count = 0;
	return node;
}

// Create a string node
ASTNode* create_string_node(const char* value) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_STRING;
	node->value = strdup(value);
	node->left = NULL;
	node->right = NULL;
	node->body = NULL;
	node->body_count = 0;
	return node;
}

// Create a binary operation node
ASTNode* create_binary_op_node(ASTNodeType type, ASTNode* left, ASTNode* right) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = type;
	node->left = left;
	node->right = right;
	node->value = NULL;
	return node;
}

// Create a variable node
ASTNode* create_variable_node(const char* name) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_VARIABLE;
	node->value = strdup(name);
	node->left = NULL;
	node->right = NULL;
	return node;
}

// Create a for loop AST node
ASTNode* create_for_node(ASTNode* initialization, ASTNode* condition, ASTNode* iteration, ASTNode** body, int body_count) {
	// Allocate memory for the new AST node
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	if (!node) {
		fprintf(stderr, "Error: Memory allocation failed for for-loop AST node\n");
		exit(1);
	}

	// Set the type of the node to represent a 'for' loop
	node->type = AST_FOR;

	// Assign the initialization, condition, and iteration parts of the for loop
	node->left = initialization;    // Initialization part of the for loop (e.g., 'int i = 0')
	node->right = condition;        // Condition part of the for loop (e.g., 'i < 10')

	// Allocate memory for the body of the for loop, which includes the iteration statement and loop body
	node->body = (ASTNode**)malloc((body_count + 1) * sizeof(ASTNode*));
	if (!node->body) {
		fprintf(stderr, "Error: Memory allocation failed for for-loop body nodes\n");
		free(node);  // Free previously allocated memory to avoid memory leak
		exit(1);
	}

	// Assign the iteration statement to the first element of the body array
	node->body[0] = iteration;  // Iteration part of the for loop (e.g., 'i++')

	// Copy the rest of the loop body to the body array
	for (int i = 0; i < body_count; i++) {
		node->body[i + 1] = body[i];  // Copy each statement in the body of the loop
	}

	// Set the body count to represent the number of elements in the body array
	node->body_count = body_count + 1;  // Includes the iteration and the body statements

	return node;
}



// Create a while AST node
ASTNode* create_while_node(ASTNode* condition, ASTNode** body, int body_count) {
	// Allocate memory for the new AST node
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	if (!node) {
		fprintf(stderr, "Error: Memory allocation failed for while AST node\n");
		exit(1);
	}

	// Set the type of the node to represent a 'while' loop
	node->type = AST_WHILE;

	// Assign the condition of the while loop
	node->left = condition;  // The condition to determine if the loop should continue

	// Allocate memory for the body of the while loop
	node->body = (ASTNode**)malloc(body_count * sizeof(ASTNode*));
	if (!node->body) {
		fprintf(stderr, "Error: Memory allocation failed for while body nodes\n");
		free(node);  // Free previously allocated memory to avoid memory leak
		exit(1);
	}

	// Copy body pointers to the node's body array
	node->body = body;


	// Set the number of statements in the body
	node->body_count = body_count;

	return node;
}



// Create a do-while node
ASTNode* create_do_while_node(ASTNode* condition, ASTNode** body, int body_count) {
	// Allocate memory for the new AST node
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	if (!node) {
		fprintf(stderr, "Error: Memory allocation failed for do-while AST node\n");
		exit(1);
	}

	// Set the type of the node to represent a 'do-while' loop
	node->type = AST_DO_WHILE;

	// Assign the condition and the body of the do-while loop
	node->left = condition;  // Condition of the do-while loop

	// Allocate memory for the body of the loop
	node->body = (ASTNode**)malloc(body_count * sizeof(ASTNode*));
	if (!node->body) {
		fprintf(stderr, "Error: Memory allocation failed for do-while body nodes\n");
		free(node);
		exit(1);
	}

	// Copy the statements into the body's array
	for (int i = 0; i < body_count; i++) {
		node->body[i] = body[i];
	}
	node->body_count = body_count;  // Set the count of the body statements

	return node;
}

// Create an assignment node
ASTNode* create_assignment_node(const char* variable_name, ASTNode* value) {
	// Allocate memory for the new AST node
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	if (!node) {
		fprintf(stderr, "Error: Memory allocation failed for assignment AST node\n");
		exit(1);
	}

	// Set the type of the node to represent an assignment
	node->type = AST_ASSIGNMENT;

	// Assign the variable name and the value expression to the AST node
	node->value = strdup(variable_name); // The variable name being assigned to (e.g., "x")
	if (!node->value) {
		fprintf(stderr, "Error: Memory allocation failed for variable name in assignment node\n");
		free(node); // Free the node to prevent memory leaks
		exit(1);
	}

	node->left = value; // The value to be assigned (could be an expression or a literal)

	node->body_count = 0;
	node->param_count = 0;
	node->is_final = 0;
	node->access_modifier = ACCESS_PUBLIC; // Default access for assignment nodes
	node->argument_count = 0;

	return node;
}

// Create an if statement AST node

ASTNode* create_if_node(ASTNode* condition, ASTNode** true_branch, int true_branch_count, ASTNode* false_branch) {
	// Allocate memory for the new AST node
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	if (!node) {
		fprintf(stderr, "Error: Memory allocation failed for if AST node\n");
		exit(1);
	}

	// Set the type to represent an if statement
	node->type = AST_IF;

	// Assign the condition of the if statement
	node->left = condition;  // Condition expression for 'if (condition)'

	// Assign the true branch
	if (true_branch_count > 0) {
		node->body = (ASTNode**)malloc(true_branch_count * sizeof(ASTNode*));
		if (!node->body) {
			fprintf(stderr, "Error: Memory allocation failed for true branch of if statement\n");
			free(node);
			exit(1);
		}
		for (int i = 0; i < true_branch_count; i++) {
			node->body[i] = true_branch[i];
		}
	}
	else {
		node->body = NULL;
	}
	node->body_count = true_branch_count;

	// Assign the false branch (if it exists)
	node->right = false_branch;

	return node;
}





// Create a function node
ASTNode* create_function_node(const char* name, char** parameters, char** parameter_types, int param_count, ASTNode** body, int body_count, AccessModifier access, const char* return_type) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	if (!node) {
		fprintf(stderr, "Error: Memory allocation failed for function ASTNode\n");
		exit(1);
	}

	// Set initial values
	node->type = AST_FUNCTION_DEF;
	node->value = strdup(name);
	node->access_modifier = access;
	node->return_type = strdup(return_type);
	node->body = body;
	node->body_count = body_count;
	node->parameters = NULL;         // Initialize to NULL
	node->parameter_types = NULL;    // Initialize to NULL

	// Allocate memory for parameters if param_count > 0
	if (param_count > 0) {
		node->parameters = (char**)malloc(param_count * sizeof(char*));
		node->parameter_types = (char**)malloc(param_count * sizeof(char*));

		if (!node->parameters || !node->parameter_types) {
			fprintf(stderr, "Error: Memory allocation failed for parameters or parameter types\n");
			exit(1);
		}

		for (int i = 0; i < param_count; i++) {
			node->parameters[i] = strdup(parameters[i]);
			node->parameter_types[i] = strdup(parameter_types[i]);
		}
	}

	node->param_count = param_count;

	return node;
}

// Create a function call node
ASTNode* create_function_call_node(const char* name, ASTNode** arguments, int argument_count) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_FUNCTION_CALL;
	node->value = strdup(name);
	node->arguments = arguments;
	node->argument_count = argument_count;
	return node;
}

// Create a class node
ASTNode* create_class_node(const char* name, ASTNode** body, int body_count, AccessModifier access) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_CLASS_DEF;
	node->value = strdup(name);
	node->body = body;
	node->body_count = body_count;
	node->access_modifier = access;
	return node;
}

// Create an object construction node
ASTNode* create_object_construct_node(const char* class_name, ASTNode** arguments, int argument_count) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_OBJECT_CONSTRUCT;
	node->class_name = strdup(class_name);
	node->arguments = arguments;
	node->argument_count = argument_count;
	return node;
}

// Create a logical operation node
ASTNode* create_logical_op_node(ASTNodeType type, ASTNode* left, ASTNode* right) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = type;
	node->left = left;
	node->right = right;
	return node;
}

// Create a return node
ASTNode* create_return_node(ASTNode* return_value) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_RETURN;
	node->left = return_value;
	return node;
}

// Create a data member definition node
ASTNode* create_data_member_def_node(const char* name, const char* type, AccessModifier access, int is_final) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_DATA_MEMBER_DEF;
	node->value = strdup(name);
	node->return_type = strdup(type);
	node->access_modifier = access;
	node->is_final = is_final;
	return node;
}

// Create a method call node
ASTNode* create_method_call_node(const char* object_name, const char* method_name, ASTNode** arguments, int argument_count) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_METHOD_CALL;
	node->value = strdup(object_name);
	node->class_name = strdup(method_name);
	node->arguments = arguments;
	node->argument_count = argument_count;
	return node;
}

// Create a unary operation node
ASTNode* create_unary_op_node(ASTNodeType type, ASTNode* operand) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = type;
	node->left = operand;
	return node;
}

// Create an object destruction node
ASTNode* create_object_destruct_node(const char* object_name) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_OBJECT_DESTRUCT;
	node->value = strdup(object_name);
	return node;
}

// Create a block node (a sequence of statements)
ASTNode* create_block_node(ASTNode** statements, int statement_count) {
	// Allocate memory for the new AST node
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	if (!node) {
		fprintf(stderr, "Error: Memory allocation failed for block AST node\n");
		exit(1);
	}

	// Set the type of the node to represent a block
	node->type = AST_BLOCK;

	// Assign the list of statements and the count of statements to the node
	node->body = statements;         // The array of statements in the block
	node->body_count = statement_count;  // Number of statements in the block

	// Initialize other fields that are not relevant for a block node
	node->param_count = 0;
	node->is_final = 0;
	node->access_modifier = ACCESS_PUBLIC; // Default for blocks
	node->argument_count = 0;

	return node;
}

// Create a member access node, e.g., object.field
ASTNode* create_member_access_node(ASTNode* object, const char* member_name) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_MEMBER_ACCESS;
	node->left = object; // The object (e.g., 'object' in 'object.field')
	node->value = strdup(member_name); // The member name being accessed (e.g., 'field')
	node->right = NULL; // No right child needed
	node->body = NULL;
	node->body_count = 0;
	return node;
}

ASTNode* create_variable_declaration_node(const char* type, const char* name, ASTNode* value) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	if (!node) {
		fprintf(stderr, "Error: Memory allocation failed for variable declaration AST node\n");
		exit(1);
	}

	node->type = AST_VARIABLE_DECLARATION;
	node->value = strdup(name); // Variable name
	node->return_type = strdup(type); // Variable type (e.g., "int", "float", "boolean")
	node->left = value; // The value being assigned (if any)

	// Set unused fields to NULL or 0
	node->body_count = 0;
	node->param_count = 0;
	node->is_final = 0;
	node->access_modifier = ACCESS_PUBLIC;
	node->argument_count = 0;

	return node;
}


// Free an AST node and all its children
void free_ast_node(ASTNode* node) {
	// Free the value if it exists
	if (node->value) {
		free(node->value);
	}

	// Free parameters and parameter types if they exist
	if (node->parameters) {
		for (int i = 0; i < node->param_count; i++) {
			if (node->parameters[i]) {
				free(node->parameters[i]);
			}
		}
		free(node->parameters);
	}

	if (node->parameter_types) {
		for (int i = 0; i < node->param_count; i++) {
			if (node->parameter_types[i]) {
				free(node->parameter_types[i]);
			}
		}
		free(node->parameter_types);
	}

	// Free arguments if they exist
	if (node->arguments) {
		for (int i = 0; i < node->argument_count; i++) {
			free_ast_node(node->arguments[i]);
		}
		free(node->arguments);
	}

	// Free body nodes if they exist
	if (node->body) {
		for (int i = 0; i < node->body_count; i++) {
			free_ast_node(node->body[i]);
		}
		free(node->body);
	}

	if (node->left) {
		free_ast_node(node->left);
	}
	if (node->right) {
		free_ast_node(node->right);
	}

	// Free the node itself
	free(node);
}

