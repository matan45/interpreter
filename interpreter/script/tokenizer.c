#include "tokenizer.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define strdup _strdup
#endif

Stack stack;


// Stack operations
void init_stack(Stack * stack) {
    stack->top = -1;
}

void push(Stack* stack, char item) {
    if (stack->top < STACK_MAX - 1) {
        stack->data[++stack->top] = item;
    }
    else {
        fprintf(stderr, "Error: Stack overflow while pushing %c\n", item);
        exit(1);
    }
}

char pop(Stack* stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top--];
    }
    else {
        fprintf(stderr, "Error: Stack underflow while popping\n");
        exit(1);
    }
}

int is_empty(Stack* stack) {
    return stack->top == -1;
}

// Create a new tokenizer
Tokenizer* create_tokenizer(char* input) {
    Tokenizer* tokenizer = (Tokenizer*)malloc(sizeof(Tokenizer));
    tokenizer->input = input;
    tokenizer->current_pos = 0;
    tokenizer->input_length = (size_t)strlen(input);
    init_stack(&stack);
    return tokenizer;
}

// Create a token
Token* create_token(TokenType type, char* value) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->value = strdup(value);
    return token;
}

// Free a token
void free_token(Token* token) {
    if (token) {
        if (token->value) {
            free(token->value);
        }
        free(token);
    }
}

// Free the tokenizer
void free_tokenizer(Tokenizer* tokenizer) {
    free(tokenizer);
}

// Helper function to skip whitespace
void skip_whitespace(Tokenizer* tokenizer) {
    while (tokenizer->current_pos < tokenizer->input_length &&
        isspace(tokenizer->input[tokenizer->current_pos])) {
        tokenizer->current_pos++;
    }
}

// Helper function to skip comments
void skip_comments(Tokenizer* tokenizer) {
    if (tokenizer->current_pos + 1 < tokenizer->input_length) {
        char current_char = tokenizer->input[tokenizer->current_pos];
        char next_char = tokenizer->input[tokenizer->current_pos + 1];

        // Single-line comment (starts with //)
        if (current_char == '/' && next_char == '/') {
            tokenizer->current_pos += 2; // Skip "//"
            while (tokenizer->current_pos < tokenizer->input_length &&
                tokenizer->input[tokenizer->current_pos] != '\n') {
                tokenizer->current_pos++;
            }
        }
        // Multi-line comment (starts with /*)
        else if (current_char == '/' && next_char == '*') {
            tokenizer->current_pos += 2; // Skip "/*"
            while (tokenizer->current_pos < tokenizer->input_length) {
                if (tokenizer->input[tokenizer->current_pos] == '*' &&
                    tokenizer->current_pos + 1 < tokenizer->input_length &&
                    tokenizer->input[tokenizer->current_pos + 1] == '/') {
                    tokenizer->current_pos += 2; // Skip "*/"
                    break;
                }
                tokenizer->current_pos++;
            }
        }
    }
}

// Helper function to read a string
char* read_string(Tokenizer* tokenizer) {
	tokenizer->current_pos++;  // Skip the initial quote
	size_t start = tokenizer->current_pos;

	while (tokenizer->current_pos < tokenizer->input_length &&
		tokenizer->input[tokenizer->current_pos] != '"') {
		tokenizer->current_pos++;
	}

	if (tokenizer->current_pos >= tokenizer->input_length) {
		fprintf(stderr, "Error: Unterminated string literal\n");
		exit(1);
	}

	size_t length = tokenizer->current_pos - start;
	char* string = (char*)malloc(length + 1);
	strncpy(string, &tokenizer->input[start], length);
	string[length] = '\0';

	tokenizer->current_pos++;  // Skip the closing quote
	return string;
}

// Helper function to read an identifier or keyword
char* read_identifier(Tokenizer* tokenizer) {
    size_t start = tokenizer->current_pos;
    while (tokenizer->current_pos < tokenizer->input_length &&
        (isalnum(tokenizer->input[tokenizer->current_pos]) || tokenizer->input[tokenizer->current_pos] == '_')) {
        tokenizer->current_pos++;
    }
    size_t length = tokenizer->current_pos - start;
    char* identifier = (char*)malloc(length + 1);
    strncpy(identifier, &tokenizer->input[start], length);
    identifier[length] = '\0';
    return identifier;
}

// Helper function to read a number
char* read_number(Tokenizer* tokenizer) {
    size_t start = tokenizer->current_pos;
    while (tokenizer->current_pos < tokenizer->input_length &&
        isdigit(tokenizer->input[tokenizer->current_pos])) {
        tokenizer->current_pos++;
    }
    size_t length = tokenizer->current_pos - start;
    char* number = (char*)malloc(length + 1);
    strncpy(number, &tokenizer->input[start], length);
    number[length] = '\0';
    return number;
}

// Function to get the next token from the tokenizer
Token* get_next_token(Tokenizer* tokenizer) {

	while (tokenizer->current_pos < tokenizer->input_length) {
		char current_char = tokenizer->input[tokenizer->current_pos];

		// Skip whitespace
		skip_whitespace(tokenizer);

		// Skip comments
		skip_comments(tokenizer);

		// Update current character after skipping whitespace and comments
		if (tokenizer->current_pos >= tokenizer->input_length) {
			break;
		}
		current_char = tokenizer->input[tokenizer->current_pos];

		// Handle numbers
		if (isdigit(current_char)) {
			char* number = read_number(tokenizer);
			return create_token(TOKEN_NUMBER, number);
		}

		// Handle identifiers, keywords, and boolean literals
		if (isalpha(current_char) || current_char == '_') {
			char* identifier = read_identifier(tokenizer);

			// Check for keywords and boolean literals
			if (strcmp(identifier, "if") == 0) return create_token(TOKEN_IF, identifier);
			if (strcmp(identifier, "do") == 0) return create_token(TOKEN_DO, identifier);
			if (strcmp(identifier, "else") == 0) return create_token(TOKEN_ELSE, identifier);
			if (strcmp(identifier, "while") == 0) return create_token(TOKEN_WHILE, identifier);
			if (strcmp(identifier, "for") == 0) return create_token(TOKEN_FOR, identifier);
			if (strcmp(identifier, "return") == 0) return create_token(TOKEN_RETURN, identifier);
			if (strcmp(identifier, "function") == 0) return create_token(TOKEN_FUNCTION, identifier);
			if (strcmp(identifier, "class") == 0) return create_token(TOKEN_CLASS, identifier);
			if (strcmp(identifier, "constructor") == 0) return create_token(TOKEN_CONSTRUCTOR, identifier);
			if (strcmp(identifier, "destructor") == 0) return create_token(TOKEN_DESTRUCTOR, identifier);
			if (strcmp(identifier, "public") == 0) return create_token(TOKEN_PUBLIC, identifier);
			if (strcmp(identifier, "private") == 0) return create_token(TOKEN_PRIVATE, identifier);
			if (strcmp(identifier, "final") == 0) return create_token(TOKEN_FINAL, identifier);
			if (strcmp(identifier, "new") == 0) return create_token(TOKEN_NEW, identifier);
			if (strcmp(identifier, "delete") == 0) return create_token(TOKEN_DELETE, identifier);
			if (strcmp(identifier, "float") == 0) return create_token(TOKEN_FLOAT, identifier);
			if (strcmp(identifier, "boolean") == 0) return create_token(TOKEN_BOOLEAN, identifier);
			if (strcmp(identifier, "int") == 0) return create_token(TOKEN_INT, identifier);
			if (strcmp(identifier, "string") == 0) return create_token(TOKEN_STRING, identifier);
			if (strcmp(identifier, "void") == 0) return create_token(TOKEN_VOID, identifier);
			if (strcmp(identifier, "true") == 0) return create_token(TOKEN_TRUE, identifier);
			if (strcmp(identifier, "false") == 0) return create_token(TOKEN_FALSE, identifier);

			// Otherwise, it's an identifier
			return create_token(TOKEN_IDENTIFIER, identifier);
		}

		// Handle logical operators
		if (current_char == '&' && tokenizer->input[tokenizer->current_pos + 1] == '&') {
			tokenizer->current_pos += 2;
			return create_token(TOKEN_LOGICAL_AND, "&&");
		}
		if (current_char == '|' && tokenizer->input[tokenizer->current_pos + 1] == '|') {
			tokenizer->current_pos += 2;
			return create_token(TOKEN_LOGICAL_OR, "||");
		}

		// Handle parentheses and braces with stack operations
		if (current_char == '(') {
			push(&stack, '(');
			tokenizer->current_pos++;
			return create_token(TOKEN_LEFT_PAREN, "(");
		}
		if (current_char == ')') {
			if (is_empty(&stack) || pop(&stack) != '(') {
				fprintf(stderr, "Error: Unmatched closing parenthesis ')'\n");
				exit(1);
			}
			tokenizer->current_pos++;
			return create_token(TOKEN_RIGHT_PAREN, ")");
		}
		if (current_char == '{') {
			push(&stack, '{');
			tokenizer->current_pos++;
			return create_token(TOKEN_LEFT_BRACE, "{");
		}
		if (current_char == '}') {
			if (is_empty(&stack) || pop(&stack) != '{') {
				fprintf(stderr, "Error: Unmatched closing brace '}'\n");
				exit(1);
			}
			tokenizer->current_pos++;
			return create_token(TOKEN_RIGHT_BRACE, "}");
		}

		// Handle increment and decrement operators (++ and --)
		if (current_char == '+' && tokenizer->input[tokenizer->current_pos + 1] == '+') {
			tokenizer->current_pos += 2;
			return create_token(TOKEN_INCREMENT, "++");
		}
		if (current_char == '-' && tokenizer->input[tokenizer->current_pos + 1] == '-') {
			tokenizer->current_pos += 2;
			return create_token(TOKEN_DECREMENT, "--");
		}

		// Handle strings
		if (current_char == '"') {
			char* string = read_string(tokenizer);
			return create_token(TOKEN_STRING, string);
		}

		// Handle single-character tokens
		tokenizer->current_pos++;
		switch (current_char) {
		case '+': return create_token(TOKEN_PLUS, "+");
		case '-': return create_token(TOKEN_MINUS, "-");
		case '*': return create_token(TOKEN_MULTIPLY, "*");
		case '/': return create_token(TOKEN_DIVIDE, "/");
		case '=': return create_token(TOKEN_ASSIGN, "=");
		case ',': return create_token(TOKEN_COMMA, ",");
		case ';': return create_token(TOKEN_SEMICOLON, ";");
		case '<': return create_token(TOKEN_LESS_THAN, "<");
		case '>': return create_token(TOKEN_GREATER_THAN, ">");
		case '!': return create_token(TOKEN_LOGICAL_NOT, "!");
		case '.': return create_token(TOKEN_DOT, ".");
		default:
			fprintf(stderr, "Error: Unrecognized character '%c'\n", current_char);
			exit(1);
		}
	}

	return create_token(TOKEN_EOF, "");
}
