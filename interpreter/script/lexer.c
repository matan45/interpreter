// lexer.c

#include "lexer.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Function to initialize the lexer with source code
void init_lexer(const char* source) {
    current_source = source;
}

// Function to skip over whitespace characters
void skip_whitespace() {
    while (*current_source == ' ' || *current_source == '\t' || *current_source == '\n' || *current_source == '\r') {
        current_source++;
    }
}


// Function to retrieve the next token from the source code
Token next_token() {
    skip_whitespace(); // Skip any whitespace

    Token token;
    token.value = NULL;

    if (*current_source == '\0') {
        token.type = TOKEN_END;
        token.value = "end";
        return token;
    }

    // Keyword and identifier matching
    if (strncmp(current_source, "class", 5) == 0 && !isalnum(current_source[5])) {
        current_source += 5;
        token.type = TOKEN_CLASS;
        token.value = "class";
    } else if (strncmp(current_source, "int", 3) == 0 && !isalnum(current_source[3])) {
        current_source += 3;
        token.type = TOKEN_INT;
        token.value = "int";
    } else if (strncmp(current_source, "float", 5) == 0 && !isalnum(current_source[5])) {
        current_source += 5;
        token.type = TOKEN_FLOAT;
        token.value = "float";
    } else if (strncmp(current_source, "void", 4) == 0 && !isalnum(current_source[4])) {
        current_source += 4;
        token.type = TOKEN_VOID;
        token.value = "void";
    } else if (strncmp(current_source, "for", 3) == 0 && !isalnum(current_source[3])) {
        current_source += 3;
        token.type = TOKEN_FOR;
        token.value = "for";
    } else if (strncmp(current_source, "if", 2) == 0 && !isalnum(current_source[2])) {
        current_source += 2;
        token.type = TOKEN_IF;
        token.value = "if";
    } else if (strncmp(current_source, "else", 4) == 0 && !isalnum(current_source[4])) {
        current_source += 4;
        token.type = TOKEN_ELSE;
        token.value = "else";
    }
	else if (strncmp(current_source, "switch", 6) == 0 && !isalnum(current_source[6])) {
		current_source += 6;
		token.type = TOKEN_SWITCH;
		token.value = "switch";
	}
	else if (strncmp(current_source, "case", 4) == 0 && !isalnum(current_source[4])) {
		current_source += 4;
		token.type = TOKEN_CASE;
		token.value = "case";
	}
	else if (strncmp(current_source, "default", 7) == 0 && !isalnum(current_source[7])) {
		current_source += 7;
		token.type = TOKEN_DEFAULT;
		token.value = "default";
	}
	else if (strncmp(current_source, "||", 2) == 0 && !isalnum(current_source[2])) {
		current_source += 2;
		token.type = TOKEN_OR;
		token.value = "||";
	}
	else if (strncmp(current_source, "&&", 2) == 0 && !isalnum(current_source[2])) {
		current_source += 2;
		token.type = TOKEN_AND;
		token.value = "&&";
	}
	else if (strncmp(current_source, "while", 5) == 0 && !isalnum(current_source[5])) {
		current_source += 5;
		token.type = TOKEN_WHILE;
		token.value = "while";
	}
	else if (strncmp(current_source, "do", 2) == 0 && !isalnum(current_source[2])) {
		current_source += 2;
		token.type = TOKEN_DO;
		token.value = "do";
	}
    // Symbol matching
    else if (*current_source == '{') {
        current_source++;
        token.type = TOKEN_LBRACE;
        token.value = "{";
    } else if (*current_source == '}') {
        current_source++;
        token.type = TOKEN_RBRACE;
        token.value = "}";
    } else if (*current_source == ';') {
        current_source++;
        token.type = TOKEN_SEMICOLON;
        token.value = ";";
	}
	else if (*current_source == ',') {
		current_source++;
		token.type = TOKEN_COMMA;
		token.value = ",";
	}
    else if (*current_source == '(') {
        current_source++;
        token.type = TOKEN_LPAREN;
        token.value = "(";
    } else if (*current_source == ')') {
        current_source++;
        token.type = TOKEN_RPAREN;
        token.value = ")";
    } else if (*current_source == '=') {
        current_source++;
        if (*current_source == '=') {
            current_source++;
            token.type = TOKEN_EQUAL; // ==
            token.value = "==";
        } else {
            token.type = TOKEN_ASSIGN; // =
            token.value = "=";
        }
    } else if (*current_source == '+') {
        current_source++;
        if (*current_source == '+') {
            current_source++;
            token.type = TOKEN_INCREMENT; // ++
            token.value = "++";
        } else {
            token.type = TOKEN_PLUS; // +
            token.value = "+";
        }
	}
	else if (*current_source == '<') {
        current_source++;
		if (*current_source == '=') {
            current_source++;
			token.type = TOKEN_LESS_EQUAL;
			token.value = "<=";
		}
		else {
			token.type = TOKEN_LESS;
			token.value = "<";
		}
	}
	else if (*current_source == '>') {
        current_source++;
		if (*current_source == '=') {
            current_source++;
			token.type = TOKEN_GREATER_EQUAL;
			token.value = ">=";
		}
		else {
			token.type = TOKEN_GREATER;
			token.value = ">";
		}
	}
    
    else if (*current_source == '-') {
        current_source++;
        if (*current_source == '-') {
            current_source++;
            token.type = TOKEN_DECREMENT; // --
            token.value = "--";
        } else {
            token.type = TOKEN_MINUS; // -
            token.value = "-";
        }
    } else if (isalpha(*current_source)) {
        // Identifiers (names of variables, functions, etc.)
        const char* start = current_source;
        while (isalnum(*current_source) || *current_source == '_') {
            current_source++;
        }
        size_t length = current_source - start;
        char* identifier = (char*)malloc(length + 1);
        strncpy(identifier, start, length);
        identifier[length] = '\0';
        token.type = TOKEN_IDENTIFIER;
        token.value = identifier;
    } else if (isdigit(*current_source)) {
        // Integer literals
        const char* start = current_source;
        while (isdigit(*current_source)) {
            current_source++;
        }
        size_t length = current_source - start;
        char* int_literal = (char*)malloc(length + 1);
        strncpy(int_literal, start, length);
        int_literal[length] = '\0';
        token.type = TOKEN_INT; // Treat as literal if appropriate
        token.value = int_literal;
    } else {
        // Unknown or error token
		char* unknown = (char*)malloc(2); // Allocate space for the unknown character and null terminator
        unknown[0] = *current_source;
        unknown[1] = '\0';
        token.type = TOKEN_UNKNOWN;
        token.value = unknown;
        current_source++; // Move to the next character
    }

    return token;
}

// Function to free memory used by tokens
void free_token(Token token) {
    if (token.type == TOKEN_IDENTIFIER || token.type == TOKEN_INT || token.type == TOKEN_UNKNOWN) {
        // Free the memory for tokens with dynamically allocated values
        free((char*)token.value);
    }
}

// Utility function to convert TokenType to a string representation (useful for debugging)
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_CLASS: return "TOKEN_CLASS";
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_INT: return "TOKEN_INT";
        case TOKEN_FLOAT: return "TOKEN_FLOAT";
        case TOKEN_VOID: return "TOKEN_VOID";
        case TOKEN_LBRACE: return "TOKEN_LBRACE";
        case TOKEN_RBRACE: return "TOKEN_RBRACE";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_LPAREN: return "TOKEN_LPAREN";
        case TOKEN_RPAREN: return "TOKEN_RPAREN";
        case TOKEN_COMMA: return "TOKEN_COMMA";
        case TOKEN_FOR: return "TOKEN_FOR";
        case TOKEN_IF: return "TOKEN_IF";
        case TOKEN_ELSE: return "TOKEN_ELSE";
        case TOKEN_ASSIGN: return "TOKEN_ASSIGN";
        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_MULTIPLY: return "TOKEN_MULTIPLY";
        case TOKEN_DIVIDE: return "TOKEN_DIVIDE";
        case TOKEN_INCREMENT: return "TOKEN_INCREMENT";
        case TOKEN_DECREMENT: return "TOKEN_DECREMENT";
        case TOKEN_LESS: return "TOKEN_LESS";
        case TOKEN_GREATER: return "TOKEN_GREATER";
        case TOKEN_LESS_EQUAL: return "TOKEN_LESS_EQUAL";
        case TOKEN_GREATER_EQUAL: return "TOKEN_GREATER_EQUAL";
        case TOKEN_EQUAL: return "TOKEN_EQUAL";
        case TOKEN_NOT_EQUAL: return "TOKEN_NOT_EQUAL";
        case TOKEN_END: return "TOKEN_END";
        case TOKEN_UNKNOWN: return "TOKEN_UNKNOWN";
        default: return "UNKNOWN_TOKEN_TYPE";
    }
}