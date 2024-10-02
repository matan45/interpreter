#include "interpreter.h"
#include "environment.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#define strdup _strdup
#endif

// Interpret an AST node
int interpret(ASTNode* node, Environment* env) {
	if (!node) return 0;

	switch (node->type) {
	case AST_NUMBER:
		return atoi(node->value); // Return the integer value

	case AST_BOOLEAN:
		return (strcmp(node->value, "true") == 0) ? 1 : 0;

	case AST_STRING:
		// Printing strings requires a special function (for example, print statements)
		printf("%s", node->value);
		return 0;

	case AST_VARIABLE: {
		Variable* var = lookup_variable(env, node->value);
		if (!var) {
			fprintf(stderr, "Error: Variable '%s' not found\n", node->value);
			exit(1);
		}
		return var->value;
	}

	case AST_ASSIGNMENT:
		interpret_assignment(node, env);
		return 0;

	case AST_BINARY_OP:
		return interpret_binary_op(node, env);

	case AST_IF:
		return interpret_if_statement(node, env);

	case AST_WHILE:
		return interpret_while_loop(node, env);

	case AST_DO_WHILE:
		return interpret_do_while_loop(node, env);

	case AST_FUNCTION_CALL:
		return interpret_function_call(node, env);

	case AST_RETURN:
		return interpret_return_statement(node, env);

	case AST_OBJECT_CONSTRUCT:
		return interpret_object_construction(node, env);

	case AST_OBJECT_DESTRUCT:
		return interpret_object_destruction(node, env);

	case AST_METHOD_CALL:
		return interpret_method_call(node, env);

	default:
		fprintf(stderr, "Error: Unsupported AST node type %d\n", node->type);
		exit(1);
	}

	return 0;
}

// Interpret an entire program represented by an AST
void interpret_program(ASTNode* program, Environment* env) {
	if (!program) return;

	// Check if the AST node represents a block of top-level nodes
	if (program->type == AST_BLOCK) {
		for (int i = 0; i < program->body_count; i++) {
			ASTNode* current_node = program->body[i];

			if (current_node->type == AST_FUNCTION_DEF) {
				// Store function definition in the environment
				define_function(env, current_node->value, current_node, current_node->parameters, current_node->param_count);
				printf("Function '%s' defined.\n", current_node->value);
			}
			else if (current_node->type == AST_CLASS_DEF) {
				// Handling class definition
				ClassDef* new_class = (ClassDef*)malloc(sizeof(ClassDef));
				new_class->name = strdup(current_node->value);
				new_class->methods = NULL;  // Initialize methods as NULL
				new_class->fields = NULL;   // Initialize fields as NULL
				new_class->next = env->classes;
				env->classes = new_class;

				// Add methods and fields to the class (if available)
				for (int j = 0; j < current_node->body_count; j++) {
					ASTNode* body_node = current_node->body[j];

					if (body_node->type == AST_FUNCTION_DEF) {
						// Add method to the class
						Function* method = (Function*)malloc(sizeof(Function));
						method->name = strdup(body_node->value);
						method->body = body_node;
						method->parameters = body_node->parameters;
						method->param_count = body_node->param_count;
						method->next = new_class->methods;
						new_class->methods = method;

						printf("Method '%s' added to class '%s'.\n", body_node->value, current_node->value);
					}
					else if (body_node->type == AST_DATA_MEMBER_DEF) {
						// Add field to the class
						ObjectField* field = (ObjectField*)malloc(sizeof(ObjectField));
						field->name = strdup(body_node->value);
						field->value = 0; // Default value; this could be set later when instances are created
						field->next = new_class->fields;
						new_class->fields = field;

						printf("Field '%s' added to class '%s'.\n", body_node->value, current_node->value);
					}
				}

				printf("Class '%s' defined with %d methods and %d fields.\n",
					new_class->name,
					count_methods(new_class->methods),
					count_fields(new_class->fields));
			}
			else {
				// Interpret any other statement
				interpret(current_node, env);
			}
		}
	}
	else {
		// If it's a single top-level node, handle it individually
		interpret(program, env);
	}
}

// Interpret an assignment operation
int interpret_assignment(ASTNode* node, Environment* env) {
	int value = interpret(node->right, env);
	assign_variable(env, node->value, value);
	return value;
}

// Interpret a binary operation
int interpret_binary_op(ASTNode* node, Environment* env) {
	int left_value = interpret(node->left, env);
	int right_value = interpret(node->right, env);

	switch (node->type) {
	case AST_BINARY_OP:
		if (strcmp(node->value, "+") == 0) return left_value + right_value;
		if (strcmp(node->value, "-") == 0) return left_value - right_value;
		if (strcmp(node->value, "*") == 0) return left_value * right_value;
		if (strcmp(node->value, "/") == 0) {
			if (right_value == 0) {
				fprintf(stderr, "Error: Division by zero\n");
				exit(1);
			}
			return left_value / right_value;
		}
		if (strcmp(node->value, "&&") == 0) return left_value && right_value;
		if (strcmp(node->value, "||") == 0) return left_value || right_value;
		break;

	default:
		fprintf(stderr, "Error: Unsupported binary operation\n");
		exit(1);
	}
	return 0;
}

// Interpret an if statement
int interpret_if_statement(ASTNode* node, Environment* env) {
	int condition_value = interpret(node->left, env);
	if (condition_value) {
		for (int i = 0; i < node->body_count; i++) {
			interpret(node->body[i], env);
		}
	}
	return 0;
}

// Interpret a while loop
int interpret_while_loop(ASTNode* node, Environment* env) {
	while (interpret(node->left, env)) {
		for (int i = 0; i < node->body_count; i++) {
			interpret(node->body[i], env);
		}
	}
	return 0;
}

// Interpret a do-while loop
int interpret_do_while_loop(ASTNode* node, Environment* env) {
	do {
		for (int i = 0; i < node->body_count; i++) {
			interpret(node->body[i], env);
		}
	} while (interpret(node->left, env));
	return 0;
}

// Interpret a function call
int interpret_function_call(ASTNode* node, Environment* env) {
	// First check if it is a native function
	NativeFunction native_func = lookup_native_function(env, node->value);
	if (native_func) {
		// Prepare arguments
		void** argv = (void**)malloc(node->argument_count * sizeof(void*));
		for (int i = 0; i < node->argument_count; i++) {
			argv[i] = (void*)(intptr_t)interpret(node->arguments[i], env);
		}

		// Call the native function
		int result = native_func(node->argument_count, argv);
		free(argv);
		return result;
	}

	// Otherwise, find the function in the environment
	Function* func = lookup_function(env, node->value);
	if (!func) {
		fprintf(stderr, "Error: Function '%s' not found\n", node->value);
		exit(1);
	}

	// Create a new environment for the function call
	Environment* func_env = create_environment(env);
	for (int i = 0; i < func->param_count; i++) {
		int argument_value = interpret(node->arguments[i], env);
		assign_variable(func_env, func->parameters[i], argument_value);
	}

	// Interpret the function body
	int return_value = 0;
	for (int i = 0; i < func->body->body_count; i++) {
		return_value = interpret(func->body->body[i], func_env);
	}

	free_environment(func_env);
	return return_value;
}

// Interpret a return statement
int interpret_return_statement(ASTNode* node, Environment* env) {
	return interpret(node->left, env);
}

// Interpret an object construction
int interpret_object_construction(ASTNode* node, Environment* env) {
	// Find the class definition in the environment
	ClassDef* class_def = lookup_class(env, node->class_name);
	if (!class_def) {
		fprintf(stderr, "Error: Class '%s' not found\n", node->class_name);
		exit(1);
	}

	// Create a new object of the class
	Object* new_object = (Object*)malloc(sizeof(Object));
	new_object->name = strdup(node->value); // The name given to the new object instance
	new_object->class_name = strdup(node->class_name);
	new_object->fields = NULL;  // Start with no fields (populate next)
	new_object->next = env->objects;
	env->objects = new_object;

	// Populate object fields based on the class definition
	ObjectField* class_field = class_def->fields;
	while (class_field) {
		ObjectField* new_field = (ObjectField*)malloc(sizeof(ObjectField));
		new_field->name = strdup(class_field->name);
		new_field->value = 0; // Default value; it can be initialized later
		new_field->next = new_object->fields;
		new_object->fields = new_field;
		class_field = class_field->next;
	}

	printf("Object '%s' of class '%s' constructed.\n", new_object->name, new_object->class_name);
	return 0;
}

// Interpret an object destruction
int interpret_object_destruction(ASTNode* node, Environment* env) {
	return delete_object(env, node->value);
}

// Interpret a method call
int interpret_method_call(ASTNode* node, Environment* env) {
	// Lookup the object
	Object* obj = lookup_object(env, node->value);
	if (!obj) {
		fprintf(stderr, "Error: Object '%s' not found\n", node->value);
		exit(1);
	}

	// Lookup the class definition to find the method
	ClassDef* class_def = lookup_class(env, obj->class_name);
	if (!class_def) {
		fprintf(stderr, "Error: Class '%s' not found for object '%s'\n", obj->class_name, obj->name);
		exit(1);
	}

	// Find the method in the class definition
	Function* method = class_def->methods;
	while (method) {
		if (strcmp(method->name, node->class_name) == 0) { // node->class_name actually represents the method name here
			// Found the method, now create a new environment for method call
			Environment* method_env = create_environment(env);

			// Add object fields to the method environment for context
			ObjectField* field = obj->fields;
			while (field) {
				assign_variable(method_env, field->name, field->value);
				field = field->next;
			}

			// Interpret the method body
			for (int i = 0; i < method->body->body_count; i++) {
				interpret(method->body->body[i], method_env);
			}

			// Update the object's fields after method execution
			field = obj->fields;
			while (field) {
				Variable* var = lookup_variable(method_env, field->name);
				if (var) {
					field->value = var->value;
				}
				field = field->next;
			}

			// Free the method environment
			free_environment(method_env);
			return 0;
		}
		method = method->next;
	}

	// Method not found
	fprintf(stderr, "Error: Method '%s' not found in class '%s'\n", node->class_name, obj->class_name);
	exit(1);
}

// Free the environment
void free_environment(Environment* env) {
	if (!env) return;

	// Free variables
	Variable* var = env->variables;
	while (var) {
		Variable* next_var = var->next;
		if (var->name) free(var->name);
		free(var);
		var = next_var;
	}

	// Free functions
	Function* func = env->functions;
	while (func) {
		Function* next_func = func->next;
		if (func->name) free(func->name);
		if (func->parameters) {
			for (int i = 0; i < func->param_count; i++) {
				if (func->parameters[i]) free(func->parameters[i]);
			}
			free(func->parameters);
		}
		free(func);
		func = next_func;
	}

	// Free native functions
	NativeFunctionEntry* native_func = env->native_functions;
	while (native_func) {
		NativeFunctionEntry* next_native_func = native_func->next;
		if (native_func->name) free(native_func->name);
		free(native_func);
		native_func = next_native_func;
	}

	// Free objects
	Object* obj = env->objects;
	while (obj) {
		Object* next_obj = obj->next;
		if (obj->name) free(obj->name);
		if (obj->class_name) free(obj->class_name);

		// Free object fields
		ObjectField* field = obj->fields;
		while (field) {
			ObjectField* next_field = field->next;
			if (field->name) free(field->name);
			free(field);
			field = next_field;
		}

		free(obj);
		obj = next_obj;
	}

	// Finally, free the environment structure itself
	free(env);
}

// Delete an object from the environment
int delete_object(Environment* env, const char* name) {
	if (!env || !name) return 0;

	Object* prev = NULL;
	Object* obj = env->objects;

	// Find the object
	while (obj) {
		if (strcmp(obj->name, name) == 0) {
			// Found the object, now remove it from the linked list
			if (prev) {
				prev->next = obj->next;
			}
			else {
				env->objects = obj->next;
			}

			// Free the object's fields
			ObjectField* field = obj->fields;
			while (field) {
				ObjectField* next_field = field->next;
				if (field->name) free(field->name);
				free(field);
				field = next_field;
			}

			// Free the object
			if (obj->name) free(obj->name);
			if (obj->class_name) free(obj->class_name);
			free(obj);

			return 1; // Successfully deleted
		}
		prev = obj;
		obj = obj->next;
	}

	// Object not found
	fprintf(stderr, "Error: Object '%s' not found for deletion\n", name);
	return 0;
}

// Create a new environment
Environment* create_environment(Environment* parent) {
	Environment* env = (Environment*)malloc(sizeof(Environment));
	env->variables = NULL;
	env->functions = NULL;
	env->native_functions = NULL;
	env->objects = NULL;
	env->parent = parent; // Link to parent for nested scopes
	return env;
}

// Count the number of methods in the class
int count_methods(Function* method) {
	int count = 0;
	while (method) {
		count++;
		method = method->next;
	}
	return count;
}

// Count the number of fields in the class
int count_fields(ObjectField* field) {
	int count = 0;
	while (field) {
		count++;
		field = field->next;
	}
	return count;
}