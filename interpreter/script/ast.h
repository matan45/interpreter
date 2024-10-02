#ifndef AST_H
#define AST_H

// Enumeration for different types of AST nodes
typedef enum {
	AST_NUMBER,           // Represents a number
	AST_STRING,           // Represents a string
	AST_BOOLEAN,          // Represents a boolean literal (true or false)
	AST_BINARY_OP,        // Represents a binary operation (+, -, *, /, &&, ||)
	AST_ASSIGNMENT,       // Represents variable assignment
	AST_VARIABLE,         // Represents a variable reference
	AST_IF,               // Represents an if statement
	AST_FOR,            // Represents a for loop
	AST_WHILE,            // Represents a while loop
	AST_DO_WHILE,         // Represents a do-while loop
	AST_FUNCTION_DEF,     // Represents a function definition
	AST_FUNCTION_CALL,    // Represents a function call
	AST_RETURN,           // Represents a return statement
	AST_CLASS_DEF,        // Represents a class definition
	AST_OBJECT_CONSTRUCT, // Represents object construction (new keyword)
	AST_OBJECT_DESTRUCT,  // Represents object destruction (delete keyword)
	AST_CONSTRUCTOR_CALL, // Represents a call to a constructor
	AST_METHOD_CALL,      // Represents a call to a class method
	AST_DATA_MEMBER_DEF,  // Represents data member definition in a class
	AST_LOGICAL_AND,      // Represents logical AND (&&)
	AST_LOGICAL_OR,       // Represents logical OR (||)
	AST_UNARY_OP,
	AST_MEMBER_ACCESS,
	AST_VARIABLE_DECLARATION,
	AST_BLOCK             // Represents a block of statements
} ASTNodeType;

// Access modifier enumeration
typedef enum {
	ACCESS_PUBLIC,
	ACCESS_PRIVATE
} AccessModifier;

// Structure to represent a node in the AST
typedef struct ASTNode {
	ASTNodeType type;
	struct ASTNode* left;            // Left child (e.g., condition for loops)
	struct ASTNode* right;           // Right child (e.g., body for loops)
	char* value;                     // General value field (e.g., string, variable name)
	struct ASTNode** body;           // Body for compound statements
	int body_count;                  // Count of nodes in the body
	char* return_type;               // Function return type (e.g., "int", "void")
	char* class_name;                // Class name (for constructors and class definitions)
	char** parameters;               // List of parameter names for functions
	char** parameter_types;          // List of parameter types
	int param_count;                 // Number of parameters
	int is_final;                    // Flag for if the data member is final
	AccessModifier access_modifier;  // Access control (public or private)
	struct ASTNode** arguments;      // Arguments for function calls
	int argument_count;              // Count of arguments
} ASTNode;

// Function prototypes for creating different AST nodes
ASTNode* create_number_node(const char* value);
ASTNode* create_boolean_node(int value);
ASTNode* create_string_node(const char* value);
ASTNode* create_binary_op_node(ASTNodeType type, ASTNode* left, ASTNode* right);
ASTNode* create_variable_node(const char* name);
ASTNode* create_assignment_node(const char* variable_name, ASTNode* value);
ASTNode* create_if_node(ASTNode* condition, ASTNode** true_branch, int true_branch_count, ASTNode* false_branch);
ASTNode* create_do_while_node(ASTNode* condition, ASTNode** body, int body_count);
ASTNode* create_while_node(ASTNode* condition, ASTNode** body, int body_count);
ASTNode* create_for_node(ASTNode* initialization, ASTNode* condition, ASTNode* iteration, ASTNode** body, int body_count);
ASTNode* create_function_node(const char* name, char** parameters, char** parameter_types, int param_count, ASTNode** body, int body_count, AccessModifier access, const char* return_type);
ASTNode* create_function_call_node(const char* name, char** parameters, char** parameter_types, int argument_count);
ASTNode* create_class_node(const char* name, ASTNode** body, int body_count, AccessModifier access);
ASTNode* create_object_construct_node(const char* class_name, ASTNode** arguments, int argument_count);
ASTNode* create_logical_op_node(ASTNodeType type, ASTNode* left, ASTNode* right);
ASTNode* create_return_node(ASTNode* return_value);
ASTNode* create_data_member_def_node(const char* name, const char* type, AccessModifier access, int is_final);
ASTNode* create_method_call_node(const char* object_name, const char* method_name, ASTNode** arguments, int argument_count);
ASTNode* create_unary_op_node(ASTNodeType type, ASTNode* operand);
ASTNode* create_object_destruct_node(const char* object_name);
ASTNode* create_block_node(ASTNode** statements, int statement_count);
ASTNode* create_member_access_node(ASTNode* object, const char* member_name);
ASTNode* create_variable_declaration_node(const char* type, const char* name, ASTNode* value);


void free_ast_node(ASTNode* node);

#endif // AST_H
