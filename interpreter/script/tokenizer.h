#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

// Define the possible token types
typedef enum {
	TOKEN_NUMBER,
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_TRUE,            // true literal
	TOKEN_FALSE,           // false literal
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_MULTIPLY,
	TOKEN_DIVIDE,
	TOKEN_ASSIGN,
	TOKEN_EQUAL,
	TOKEN_LESS_THAN,
	TOKEN_GREATER_THAN,
	TOKEN_LEFT_PAREN,
	TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE,
	TOKEN_RIGHT_BRACE,
	TOKEN_SEMICOLON,
	TOKEN_COMMA,
	TOKEN_LOGICAL_AND,     // &&
	TOKEN_LOGICAL_OR,      // ||
	TOKEN_IF,
	TOKEN_DO,
	TOKEN_LOGICAL_NOT,
	TOKEN_ELSE,
	TOKEN_WHILE,
	TOKEN_FOR,
	TOKEN_RETURN,
	TOKEN_FUNCTION,
	TOKEN_CLASS,
	TOKEN_CONSTRUCTOR,
	TOKEN_DESTRUCTOR,
	TOKEN_PUBLIC,
	TOKEN_PRIVATE,
	TOKEN_FINAL,
	TOKEN_NEW,
	TOKEN_INCREMENT,
	TOKEN_DECREMENT,
	TOKEN_DELETE,
	TOKEN_INT,
	TOKEN_FLOAT,
	TOKEN_BOOLEAN,
	TOKEN_VOID,
	TOKEN_EOF              // End of file/input
} TokenType;

// Define a structure to represent a token
typedef struct {
	TokenType type;
	char* value;
} Token;


// Define a structure to represent a stack for parentheses/braces matching
#define STACK_MAX 100

typedef struct {
	char data[STACK_MAX];
	int top;
} Stack;

// Function prototypes for stack operations
void init_stack(Stack* stack);
void push(Stack* stack, char item);
char pop(Stack* stack);
int is_empty(Stack* stack);

// Define a structure to represent a tokenizer
typedef struct {
	char* input;
	size_t current_pos;
	size_t input_length;
} Tokenizer;

// Function prototypes
Tokenizer* create_tokenizer(char* input);
void free_tokenizer(Tokenizer* tokenizer);

Token* get_next_token(Tokenizer* tokenizer);
Token* create_token(TokenType type, char* value);
void free_token(Token* token);
char* read_number(Tokenizer* tokenizer);

#endif // TOKENIZER_H

