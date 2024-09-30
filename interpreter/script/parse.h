#pragma once
#include "lexer.h"  // Include lexer.h to access Token structure and functions

// Field structure representing a class's member variables
typedef struct Field {
	const char* type;         // Data type of the field (e.g., int, float)
	const char* name;         // Name of the field
	void* value;              // Value of the field
	struct Field* next;       // Pointer to the next field (linked list for multiple fields)
} Field;

// Parameter node for method parameters
typedef struct ParameterNode {
	const char* type;            // Type of the parameter
	const char* name;            // Name of the parameter
	struct ParameterNode* next;  // Pointer to the next parameter
} ParameterNode;

// Method structure representing a class's methods
typedef struct Method {
	const char* return_type;  // Return type of the method (e.g., void, int)
	const char* name;         // Name of the method
	struct ParameterNode* parameters;  // Parameters for the method
	struct BlockNode* body;   // Body of the method (block of statements)
	struct Method* next;      // Pointer to the next method (linked list for multiple methods)
} Method;

// Class structure representing a class
typedef struct ClassNode {
	const char* class_name;  // Name of the class
	Field* fields;           // Pointer to the first field in the linked list of fields
	Method* methods;         // Pointer to the first method in the linked list of methods
} ClassNode;

// Object structure representing an instance of a class
typedef struct Object {
	ClassNode* class_type;   // Pointer to the class definition
	Field* field_values;     // Stores the current values of the object's fields
} Object;

// Symbol table for storing variables and their values
typedef struct SymbolTable {
	char* variable_name;  // Name of the variable
	int value;            // Value of the variable (only supporting int for simplicity)
	struct SymbolTable* next;  // Pointer to the next variable
} SymbolTable;

typedef enum {
	NODE_CLASS,       // Represents a class
	NODE_FIELD,       // Represents a field
	NODE_METHOD,      // Represents a method
	NODE_IF,          // Represents an if statement
	NODE_FOR,         // Represents a for loop
	NODE_RETURN,      // Represents a return statement
	NODE_ASSIGNMENT,  // Represents an assignment
	NODE_EXPRESSION,  // Represents an expression (e.g., x = 5)
} NodeType;

// Expression node for simple expressions (variable or constant values)
typedef struct ExpressionNode {
	char* variable;  // Variable name (if it's a variable)
	int value;       // Constant value (if it's a constant)
	struct ExpressionNode* next;
} ExpressionNode;

// If statement node
typedef struct IfNode {
	ExpressionNode* condition;     // Condition expression
	struct BlockNode* trueBlock;   // Statements to execute if true
	struct BlockNode* falseBlock;  // Statements to execute if false
} IfNode;

// For loop node
typedef struct ForNode {
	ExpressionNode* initializer;  // Initialization expression (e.g., int i = 0)
	ExpressionNode* condition;    // Loop condition (e.g., i < 10)
	ExpressionNode* update;       // Update expression (e.g., i++)
	struct BlockNode* body;       // Body of the loop
} ForNode;

// Method node for representing method definitions
typedef struct MethodNode {
	char* return_type;               // Return type of the method
	char* name;                      // Name of the method
	struct ParameterNode* parameters;  // Parameters for the method
	struct BlockNode* body;          // Block of statements inside the method
} MethodNode;

// Statement node (can be used to represent individual statements inside a block)
typedef struct StatementNode {
	NodeType node_type;  // Type of statement (e.g., assignment, method call)
	union {
		Field* field;           // Field declaration
		MethodNode* method;     // Method
		IfNode* ifNode;         // If statement
		ForNode* forNode;       // For loop
		ExpressionNode* expression; // Assignment or expressions
	};
} StatementNode;

// Block node representing a list of statements
typedef struct BlockNode {
	NodeType node_type;  // Type of node
	struct BlockNode* next;  // Linked list of statements in a block
	union {
		Field* field;           // Field declaration
		MethodNode* method;     // Method
		IfNode* ifNode;         // If statement
		ForNode* forNode;       // For loop
		ExpressionNode* expression; // Assignment or expressions
	};
} BlockNode;

typedef struct LocalVariable {
	char* variable_name;  // Name of the variable
	int value;            // Value of the variable
	struct LocalVariable* next;  // Pointer to the next variable in the local scope
} LocalVariable;

// Global variables for token management
Token current_token;  // Current token being processed
const char** source;  // Source code being parsed
static SymbolTable* symbol_table = NULL; // Head of the symbol table linked list

// Function declarations for parsing and interpretation

// Parsing functions
ClassNode* parse_class();
Field* parse_field();
Method* parse_method();
ForNode* parse_for_loop();
IfNode* parse_if_statement();
BlockNode* parse_block();
ExpressionNode* parse_expression();
StatementNode* parse_statement();

void next_token_wrapper();
void expect(TokenType type);

// Object functions
Object* create_object(ClassNode* class_node);
void execute_method(Object* obj, const char* method_name);
void free_object(Object* obj);

// AST Node execution functions
void execute_if(IfNode* if_node, Object* obj, LocalVariable* local_table);
void execute_for(ForNode* for_node, Object* obj);
int evaluate_expression(ExpressionNode* expr, Object* obj, LocalVariable* local_table);
void execute_block(BlockNode* block, Object* obj, LocalVariable* local_table);

// Utility functions
void free_class_node(ClassNode* class_node);
void update_variable(char* variable, int value);
int lookup_object_field(Object* obj, const char* field_name);
// Function to update or add a variable to the symbol table
void update_object_field(Object* obj, const char* field_name, int value);
void free_symbol_table();
void clean_up();
int lookup_local_variable(LocalVariable* local_table, const char* variable_name);
void update_local_variable(LocalVariable** local_table, const char* variable_name, int value);