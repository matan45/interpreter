#pragma once

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>  // Use standard library for malloc/free

typedef enum {
	TOKEN_CLASS,       // class keyword
	TOKEN_IDENTIFIER,  // class name or variable/method names
	TOKEN_INT,         // int keyword
	TOKEN_FLOAT,       // float keyword
	TOKEN_VOID,        // void keyword
	TOKEN_LBRACE,      // {
	TOKEN_RBRACE,      // }
	TOKEN_SEMICOLON,   // ;
	TOKEN_LPAREN,      // (
	TOKEN_RPAREN,      // )
	TOKEN_UNKNOWN,     // for error handling
	TOKEN_COMMA,
	TOKEN_FOR,
	TOKEN_IF,
	TOKEN_ELSE,
	TOKEN_ASSIGN,
	TOKEN_PLUS,       // +
	TOKEN_MINUS,      // -
	TOKEN_MULTIPLY,   // *
	TOKEN_DIVIDE,     // /
	TOKEN_INCREMENT,  // ++
	TOKEN_DECREMENT,  // --
	TOKEN_LESS,           // <
	TOKEN_GREATER,        // >
	TOKEN_LESS_EQUAL,     // <=
	TOKEN_GREATER_EQUAL,  // >=
	TOKEN_EQUAL,          // ==
	TOKEN_NOT_EQUAL,      // !=
	TOKEN_END          // for end of file
} TokenType;

typedef struct {
	TokenType type;
	const char* value; // Useful for identifiers or literal strings
} Token;

// Stack for balance checking
typedef struct StackNode {
	char data;
	struct StackNode* next;
} StackNode;

// Function prototypes (ensure these are before any function implementation)
void push(StackNode** top, char value);
char pop(StackNode** top);
int is_empty(StackNode* top);
int is_matching_pair(char open, char close);
int check_balance(const char** src);
void skip_whitespace(const char** src);
Token next_token(const char** src);
void free_token(Token token);
