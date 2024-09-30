// types.h

#pragma once

#include <stddef.h> // Include for standard definitions like NULL, size_t

// Token types (used by lexer)
typedef enum {
    TOKEN_CLASS,         // "class" keyword
    TOKEN_IDENTIFIER,    // Identifier (variable, method, class names)
    TOKEN_INT,           // int keyword or integer literal
    TOKEN_FLOAT,         // float keyword or floating-point literal
    TOKEN_VOID,          // void keyword
    TOKEN_LBRACE,        // {
    TOKEN_RBRACE,        // }
    TOKEN_SEMICOLON,     // ;
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
    TOKEN_COMMA,         // ,
    TOKEN_FOR,           // "for" keyword
    TOKEN_IF,            // "if" keyword
    TOKEN_ELSE,          // "else" keyword
    TOKEN_ASSIGN,        // =
    TOKEN_PLUS,          // +
    TOKEN_MINUS,         // -
    TOKEN_MULTIPLY,      // *
    TOKEN_DIVIDE,        // /
    TOKEN_INCREMENT,     // ++
    TOKEN_DECREMENT,     // --
    TOKEN_LESS,          // <
    TOKEN_GREATER,       // >
    TOKEN_LESS_EQUAL,    // <=
    TOKEN_GREATER_EQUAL, // >=
    TOKEN_EQUAL,         // ==
    TOKEN_NOT_EQUAL,     // !=
    TOKEN_END,           // End of input
    TOKEN_UNKNOWN        // Unknown or error token
} TokenType;

// Abstract Syntax Tree Node Types (used by parser and interpreter)
typedef enum {
    NODE_CLASS,          // Represents a class definition
    NODE_FIELD,          // Represents a field of a class
    NODE_METHOD,         // Represents a method of a class
    NODE_IF,             // Represents an if statement
    NODE_FOR,            // Represents a for loop
    NODE_ASSIGNMENT,     // Represents an assignment
    NODE_EXPRESSION,     // Represents an expression (e.g., x = 5)
    NODE_RETURN          // Represents a return statement
} NodeType;

// Access types for fields and methods (e.g., public, private)
typedef enum {
	ACCESS_PUBLIC,
	ACCESS_PRIVATE
} AccessType;

typedef struct Method {
	const char* return_type;     // Return type of the method (e.g., void, int)
	const char* name;            // Name of the method
	struct ParameterNode* parameters; // Parameters for the method
	struct BlockNode* body;      // Body of the method (block of statements)
	struct Method* next;         // Pointer to the next method (linked list for multiple methods)
} Method;

typedef struct Field {
	const char* type;         // Data type of the field (e.g., int, float)
	const char* name;         // Name of the field
	void* value;              // Value of the field (initial value, if any)
	struct Field* next;       // Pointer to the next field (linked list for multiple fields)
} Field;

typedef struct ClassNode {
	const char* class_name;  // Name of the class
	Field* fields;           // Pointer to the first field in the linked list of fields
	Method* methods;         // Pointer to the first method in the linked list of methods
} ClassNode;

typedef struct ExpressionNode {
	char* variable;  // Variable name (for identifier expressions)
	int value;       // Literal value (for constant integer expressions)
	struct ExpressionNode* next; // Used to chain multiple parts of an expression
} ExpressionNode;

typedef struct IfNode {
	ExpressionNode* condition;   // Condition expression
	struct BlockNode* trueBlock; // Statements to execute if true
	struct BlockNode* falseBlock; // Statements to execute if false (optional)
} IfNode;

// For loop node
typedef struct ForNode {
	ExpressionNode* initializer; // Initialization expression (e.g., int i = 0)
	ExpressionNode* condition;   // Loop condition (e.g., i < 10)
	ExpressionNode* update;      // Update expression (e.g., i++)
	struct BlockNode* body;      // Body of the loop
} ForNode;

// Statement node (represents individual statements inside a block)
typedef struct StatementNode {
	NodeType node_type;  // Type of statement (e.g., assignment, if, for)
	union {
		Field* field;          // Field declaration
		Method* method;        // Method
		IfNode* ifNode;        // If statement
		ForNode* forNode;      // For loop
		ExpressionNode* expression; // Assignment or expressions
	};
} StatementNode;

typedef struct Object {
	ClassNode* class_type;  // Assuming ClassNode is defined elsewhere
	Field* field_values;    // Assuming Field is defined elsewhere
} Object;

// Block node representing a list of statements
typedef struct BlockNode {
	NodeType node_type;        // Type of node (if it's part of a block)
	struct BlockNode* next;    // Linked list of statements in a block
	union {
		Field* field;           // Field declaration
		Method* method;         // Method
		IfNode* ifNode;         // If statement
		ForNode* forNode;       // For loop
		ExpressionNode* expression; // Assignment or expressions
	};
} BlockNode;



// Symbol table representation (used to track variables during interpretation)
typedef struct SymbolTable {
    char* variable_name;       // Name of the variable
    int value;                 // Value of the variable (currently supporting int)
    struct SymbolTable* next;  // Pointer to the next symbol in the linked list
} SymbolTable;



// Field structure representing a class's member variables


// Parameter node for method parameters
typedef struct ParameterNode {
	const char* type;            // Type of the parameter (e.g., int, float)
	const char* name;            // Name of the parameter
	struct ParameterNode* next;  // Pointer to the next parameter
} ParameterNode;

// Method structure representing a class's methods


// Class structure representing a class


// General linked list node for type parameters, generics, etc. (used for generics and more complex structures)
typedef struct GenericType {
    const char* type_name;         // Base type name (e.g., "List", "Map")
    struct GenericType* parameters; // Generic parameters (e.g., int, String)
    struct GenericType* next;      // For multiple parameters (used in Maps for key/value pairs)
} GenericType;



