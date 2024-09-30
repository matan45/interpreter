#include "lexer.h"




void push(StackNode** top, char value) {
	StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
	newNode->data = value;
	newNode->next = *top;
	*top = newNode;
}

char pop(StackNode** top) {
	if (*top == NULL) return '\0';
	StackNode* temp = *top;
	char popped = temp->data;
	*top = (*top)->next;
	free(temp);
	return popped;
}

int is_empty(StackNode* top) {
	return top == NULL;
}

int is_matching_pair(char open, char close) {
	return (open == '(' && close == ')') ||
		(open == '{' && close == '}') ||
		(open == '[' && close == ']');
}

// Check for balanced parentheses, braces, and brackets
int check_balance(const char** src) {
	StackNode* stack = NULL;

	while (**src != '\0') {
		if (**src == '(' || **src == '{' || **src == '[') {
			push(&stack, **src);
		}
		else if (**src == ')' || **src == '}' || **src == ']') {
			if (is_empty(stack)) {
				return 0; // Unbalanced
			}
			char top = pop(&stack);
			if (!is_matching_pair(top, **src)) {
				return 0; // Unbalanced
			}
		}
		(*src)++;
	}

	return is_empty(stack); // If stack is empty, parentheses are balanced
}

// Skip whitespace
void skip_whitespace(const char** src) {
	while (**src == ' ' || **src == '\n' || **src == '\t') {
		(*src)++;
	}
}

// Tokenizing the source code
Token next_token(const char** src) {
	skip_whitespace(src);

	Token token; // Create a Token variable
	token.value = NULL; // Initialize value to NULL

	// Keyword and symbol matching
	if (strncmp(*src, "class", 5) == 0 && !isalnum((*src)[5])) {
		*src += 5;
		token.type = TOKEN_CLASS;
		token.value = "class";
	}
	else if (strncmp(*src, "int", 3) == 0 && !isalnum((*src)[3])) {
		*src += 3;
		token.type = TOKEN_INT;
		token.value = "int";
	}
	else if (strncmp(*src, "float", 5) == 0 && !isalnum((*src)[5])) {
		*src += 5;
		token.type = TOKEN_FLOAT;
		token.value = "float";
	}
	else if (strncmp(*src, "void", 4) == 0 && !isalnum((*src)[4])) {
		*src += 4;
		token.type = TOKEN_VOID;
		token.value = "void";
	}
	else if (strncmp(*src, "for", 3) == 0 && !isalnum((*src)[3])) {
		*src += 3;
		token.type = TOKEN_FOR;
		token.value = "for";
	}
	else if (strncmp(*src, "if", 2) == 0 && !isalnum((*src)[2])) {
		*src += 2;
		token.type = TOKEN_IF;
		token.value = "if";
	}
	else if (strncmp(*src, "else", 4) == 0 && !isalnum((*src)[4])) {
		*src += 4;
		token.type = TOKEN_ELSE;
		token.value = "else";
	}
	else if (**src == '{') {
		(*src)++;
		token.type = TOKEN_LBRACE;
		token.value = "{";
	}
	else if (**src == '}') {
		(*src)++;
		token.type = TOKEN_RBRACE;
		token.value = "}";
	}
	else if (**src == ';') {
		(*src)++;
		token.type = TOKEN_SEMICOLON;
		token.value = ";";
	}
	else if (**src == ',') {
		(*src)++;
		token.type = TOKEN_COMMA;
		token.value = ",";
	}
	else if (**src == '(') {
		(*src)++;
		token.type = TOKEN_LPAREN;
		token.value = "(";
	}
	else if (**src == ')') {
		(*src)++;
		token.type = TOKEN_RPAREN;
		token.value = ")";
	}
	// Recognize comparison operators
	else if (**src == '<') {
		(*src)++;
		if (**src == '=') {
			(*src)++;
			token.type = TOKEN_LESS_EQUAL;
			token.value = "<=";
		}
		else {
			token.type = TOKEN_LESS;
			token.value = "<";
		}
	}
	else if (**src == '>') {
		(*src)++;
		if (**src == '=') {
			(*src)++;
			token.type = TOKEN_GREATER_EQUAL;
			token.value = ">=";
		}
		else {
			token.type = TOKEN_GREATER;
			token.value = ">";
		}
	}
	else if (**src == '==') {
		(*src)++;
		if (**src == '==') {
			(*src)++;
			token.type = TOKEN_EQUAL;
			token.value = "==";
		}
		else {
			// Handle assignment '=' if needed
		}
	}
	else if (**src == '=') {
		(*src)++;
		if (**src == '=') {
			(*src)++;
			token.type = TOKEN_EQUAL;  // ==
			token.value = "==";
		}
		else {
			token.type = TOKEN_ASSIGN;  // =
			token.value = "=";
		}
	}
	else if (**src == '!') {
		(*src)++;
		if (**src == '=') {
			(*src)++;
			token.type = TOKEN_NOT_EQUAL;
			token.value = "!=";
		}
		else {
			token.type = TOKEN_UNKNOWN;
			token.value = "!";
		}
	}
	else if (**src == '+') {
		(*src)++;
		if (**src == '+') {
			(*src)++;
			token.type = TOKEN_INCREMENT;  // ++
			token.value = "++";
		}
		else {
			token.type = TOKEN_PLUS;
			token.value = "+";
		}
	}
	else if (**src == '*') {
		(*src)++;
		token.type = TOKEN_MULTIPLY;  // *
		token.value = "*";
	}
	else if (**src == '/') {
		(*src)++;
		token.type = TOKEN_DIVIDE;  // /
		token.value = "/";
	}
	else if (**src == '-') {
		(*src)++;
		if (**src == '-') {
			(*src)++;
			token.type = TOKEN_DECREMENT;  // --
			token.value = "--";
		}
		else {
			token.type = TOKEN_MINUS;
			token.value = "-";
		}
	}
	else if (isalpha(**src)) {
		const char* start = *src;
		while (isalnum(**src) || **src == '_') (*src)++;
		size_t length = *src - start;
		char* value = (char*)malloc(length + 1);
		strncpy(value, start, length);
		value[length] = '\0';
		token.type = TOKEN_IDENTIFIER;
		token.value = value;
	}
	// Recognize integers
	else if (isdigit(**src)) {
		const char* start = *src;
		while (isdigit(**src)) (*src)++;
		size_t length = *src - start;
		char* value = (char*)malloc(length + 1);
		strncpy(value, start, length);
		value[length] = '\0';
		token.type = TOKEN_INT;
		token.value = value;
	}
	else if (**src == '\0') {
		token.type = TOKEN_END;
		token.value = "end";
	}
	else {
		char* unknown_value = (char*)malloc(2);
		unknown_value[0] = **src;
		unknown_value[1] = '\0';
		token.type = TOKEN_UNKNOWN;
		token.value = unknown_value;
		(*src)++;
	}

	return token; // Return the token variable
}

// Free memory for tokens that are dynamically allocated
void free_token(Token token) {
	if (token.type == TOKEN_IDENTIFIER || token.type == TOKEN_UNKNOWN) {
		free((char*)token.value);
	}
}