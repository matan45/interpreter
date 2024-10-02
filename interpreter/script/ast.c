#include "ast.h"
#include <stdlib.h>
#include <string.h>

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

// Create a do-while node
ASTNode* create_do_while_node(ASTNode* condition, ASTNode** body, int body_count) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_DO_WHILE;
	node->left = condition;
	node->body = body;
	node->body_count = body_count;
	return node;
}

// Create an assignment node
ASTNode* create_assignment_node(const char* name, ASTNode* right) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_ASSIGNMENT;
	node->value = strdup(name);
	node->right = right;
	return node;
}

// Create an if node
ASTNode* create_if_node(ASTNode* condition, ASTNode** body, int body_count) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_IF;
	node->left = condition;
	node->body = body;
	node->body_count = body_count;
	return node;
}

// Create a while node
ASTNode* create_while_node(ASTNode* condition, ASTNode** body, int body_count) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_WHILE;
	node->left = condition;
	node->body = body;
	node->body_count = body_count;
	return node;
}

// Create a function node
ASTNode* create_function_node(const char* name, char** parameters, char** parameter_types, int param_count, ASTNode** body, int body_count, AccessModifier access, const char* return_type) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_FUNCTION_DEF;
	node->value = strdup(name);
	node->parameters = parameters;
	node->parameter_types = parameter_types;
	node->param_count = param_count;
	node->body = body;
	node->body_count = body_count;
	node->access_modifier = access;
	node->return_type = strdup(return_type);
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
ASTNode* create_block_node(ASTNode** body, int body_count) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = AST_BLOCK;
	node->body = body;
	node->body_count = body_count;
	node->value = NULL;
	node->left = NULL;
	node->right = NULL;
	return node;
}

// Free an AST node and all its children
void free_ast_node(ASTNode* node) {
	if (!node) return;

	// Free the value if it exists
	if (node->value) free(node->value);

	// Free parameters and parameter types if they exist
	if (node->parameters) {
		for (int i = 0; i < node->param_count; i++) {
			if (node->parameters[i]) free(node->parameters[i]);
		}
		free(node->parameters);
	}

	if (node->parameter_types) {
		for (int i = 0; i < node->param_count; i++) {
			if (node->parameter_types[i]) free(node->parameter_types[i]);
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

	// Free the node itself
	free(node);
}

