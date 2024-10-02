#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "ast.h"

// Define the type for a native function
typedef int (*NativeFunction)(int argc, void** argv);

// Structure to represent a variable
typedef struct Variable {
	char* name;
	int value;
	int is_final;
	struct Variable* next;
} Variable;

// Structure to represent an object field
typedef struct ObjectField {
	char* name;
	int value;                 // Field value if it's an integer
	struct ObjectField* next;
} ObjectField;

// Structure to represent an object
typedef struct Object {
	char* name;
	char* class_name;
	ObjectField* fields;       // Linked list of object fields
	struct Object* next;
} Object;

// Structure to represent a function
typedef struct Function {
	char* name;
	ASTNode* body;
	char** parameters;
	int param_count;
	struct Function* next;
} Function;

// Structure to represent a class definition
typedef struct ClassDef {
	char* name;
	Function* methods;       // Linked list of methods in the class
	ObjectField* fields;     // Linked list of fields in the class
	struct ClassDef* next;   // Linked list pointer to next class
} ClassDef;

// Structure to represent a native function
typedef struct NativeFunctionEntry {
	char* name;
	NativeFunction function;
	struct NativeFunctionEntry* next;
} NativeFunctionEntry;

// Updated structure to represent an environment
typedef struct Environment {
	Variable* variables;
	Function* functions;
	NativeFunctionEntry* native_functions;
	Object* objects;
	ClassDef* classes;       // Add a linked list of class definitions to the environment
	struct Environment* parent;
} Environment;
// Function prototypes for environment operations
Variable* lookup_variable(Environment* env, const char* name);
void assign_variable(Environment* env, const char* name, int value);

Function* lookup_function(Environment* env, const char* name);
void define_function(Environment* env, const char* name, ASTNode* body, char** parameters, int param_count);

NativeFunction lookup_native_function(Environment* env, const char* name);
void register_native_function(Environment* env, const char* name, NativeFunction function);

ClassDef* lookup_class(Environment* env, const char* name);


Object* lookup_object(Environment* env, const char* name);
void assign_object(Environment* env, const char* name, Object* object);
ObjectField* lookup_object_field(Object* obj, const char* field_name);
void set_object_field(Object* obj, const char* field_name, int value);

#endif // ENVIRONMENT_H
