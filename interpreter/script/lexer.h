// lexer.h

#pragma once

#include "types.h" // Include types shared across the project

// Token structure
typedef struct {
    TokenType type;   // Type of the token
    const char* value; // Pointer to the value (used for identifiers and literals)
} Token;

// Function prototypes
void init_lexer(const char* source);
Token next_token();
void free_token(Token token);
void skip_whitespace();
const char* token_type_to_string(TokenType type);


