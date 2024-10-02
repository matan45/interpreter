#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

typedef enum {
	TOKEN_NUMBER,            // 0
	TOKEN_IDENTIFIER,        // 1
	TOKEN_STRING,            // 2
	TOKEN_TRUE,              // 3
	TOKEN_FALSE,             // 4
	TOKEN_PLUS,              // 5
	TOKEN_MINUS,             // 6
	TOKEN_MULTIPLY,          // 7
	TOKEN_DIVIDE,            // 8
	TOKEN_ASSIGN,            // 9
	TOKEN_EQUAL,             // 10
	TOKEN_LESS_THAN,         // 11
	TOKEN_GREATER_THAN,      // 12
	TOKEN_LEFT_PAREN,        // 13
	TOKEN_RIGHT_PAREN,       // 14
	TOKEN_LEFT_BRACE,        // 15
	TOKEN_DOT,               // 16
	TOKEN_RIGHT_BRACE,       // 17
	TOKEN_SEMICOLON,         // 18
	TOKEN_COMMA,             // 19
	TOKEN_LOGICAL_AND,       // 20
	TOKEN_LOGICAL_OR,        // 21
	TOKEN_IF,                // 22
	TOKEN_DO,                // 23
	TOKEN_LOGICAL_NOT,       // 24
	TOKEN_ELSE,              // 25
	TOKEN_WHILE,             // 26
	TOKEN_FOR,               // 27
	TOKEN_RETURN,            // 28
	TOKEN_FUNCTION,          // 29
	TOKEN_CLASS,             // 30
	TOKEN_CONSTRUCTOR,       // 31
	TOKEN_DESTRUCTOR,        // 32
	TOKEN_PUBLIC,            // 33
	TOKEN_PRIVATE,           // 34
	TOKEN_FINAL,             // 35
	TOKEN_NEW,               // 36
	TOKEN_INCREMENT,         // 37
	TOKEN_DECREMENT,         // 38
	TOKEN_DELETE,            // 39
	TOKEN_INT,               // 40
	TOKEN_FLOAT,             // 41
	TOKEN_BOOLEAN,           // 42
	TOKEN_VOID,              // 43
	TOKEN_EOF                // 44
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

