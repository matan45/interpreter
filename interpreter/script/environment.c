#include "environment.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define strdup _strdup
#endif

// Lookup a variable in the environment
Variable* lookup_variable(Environment* env, const char* name) {
	while (env) {
		Variable* var = env->variables;
		while (var) {
			if (strcmp(var->name, name) == 0) {
				return var;
			}
			var = var->next;
		}
		env = env->parent; // Check the parent environment
	}
	return NULL; // Variable not found
}

// Assign a variable in the environment
void assign_variable(Environment* env, const char* name, int value) {
	Variable* var = lookup_variable(env, name);
	if (var) {
		if (var->is_final) {
			fprintf(stderr, "Error: Variable %s is declared as final and cannot be modified\n", name);
			exit(1);
		}
		var->value = value;
	}
	else {
		Variable* new_var = (Variable*)malloc(sizeof(Variable));
		new_var->name = strdup(name);
		new_var->value = value;
		new_var->is_final = 0; // Default to not final
		new_var->next = env->variables;
		env->variables = new_var;
	}
}

// Lookup a function in the environment
Function* lookup_function(Environment* env, const char* name) {
	while (env) {
		Function* func = env->functions;
		while (func) {
			if (strcmp(func->name, name) == 0) {
				return func;
			}
			func = func->next;
		}
		env = env->parent; // Check the parent environment
	}
	return NULL; // Function not found
}

// Define a function in the environment
void define_function(Environment* env, const char* name, ASTNode* body, char** parameters, int param_count) {
	Function* new_func = (Function*)malloc(sizeof(Function));
	new_func->name = strdup(name);
	new_func->body = body;
	new_func->parameters = parameters;
	new_func->param_count = param_count;
	new_func->next = env->functions;
	env->functions = new_func;
}

// Lookup a native function in the environment
NativeFunction lookup_native_function(Environment* env, const char* name) {
	while (env) {
		NativeFunctionEntry* entry = env->native_functions;
		while (entry) {
			if (strcmp(entry->name, name) == 0) {
				return entry->function;
			}
			entry = entry->next;
		}
		env = env->parent; // Check the parent environment
	}
	return NULL; // Native function not found
}

// Register a native function in the environment
void register_native_function(Environment* env, const char* name, NativeFunction function) {
	NativeFunctionEntry* new_entry = (NativeFunctionEntry*)malloc(sizeof(NativeFunctionEntry));
	new_entry->name = strdup(name);
	new_entry->function = function;
	new_entry->next = env->native_functions;
	env->native_functions = new_entry;
}


// Lookup a class in the environment
ClassDef* lookup_class(Environment* env, const char* name) {
	while (env) {
		ClassDef* class_def = env->classes;
		while (class_def) {
			if (strcmp(class_def->name, name) == 0) {
				return class_def;
			}
			class_def = class_def->next;
		}
		env = env->parent; // Check parent environment for class definitions
	}
	return NULL; // Class not found
}


// Lookup an object in the environment
Object* lookup_object(Environment* env, const char* name) {
	while (env) {
		Object* obj = env->objects;
		while (obj) {
			if (strcmp(obj->name, name) == 0) {
				return obj;
			}
			obj = obj->next;
		}
		env = env->parent; // Check the parent environment
	}
	return NULL; // Object not found
}

// Assign an object to the environment
void assign_object(Environment* env, const char* name, Object* object) {
	object->name = strdup(name);
	object->next = env->objects;
	env->objects = object;
}

// Lookup an object field
ObjectField* lookup_object_field(Object* obj, const char* field_name) {
	ObjectField* field = obj->fields;
	while (field) {
		if (strcmp(field->name, field_name) == 0) {
			return field;
		}
		field = field->next;
	}
	return NULL; // Field not found
}

// Set an object field
void set_object_field(Object* obj, const char* field_name, int value) {
	ObjectField* field = lookup_object_field(obj, field_name);
	if (field) {
		field->value = value;
	}
	else {
		ObjectField* new_field = (ObjectField*)malloc(sizeof(ObjectField));
		new_field->name = strdup(field_name);
		new_field->value = value;
		new_field->next = obj->fields;
		obj->fields = new_field;
	}
}
