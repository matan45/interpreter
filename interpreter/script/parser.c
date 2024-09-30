// parser.c

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Static variable to hold the current token being processed
static Token current_token;

// Static source pointer to handle parsing
static const char** source;

// Function to advance to the next token
void next_token_wrapper() {
    current_token = next_token();
    printf("Token Type: %s, Token Value: %s\n", token_type_to_string(current_token.type), current_token.value);
}

// Function to validate that the current token matches the expected type, otherwise throw an error
void expect(TokenType type) {
    if (current_token.type != type) {
        fprintf(stderr, "Error: Expected token type %d but found '%s'\n", type, current_token.value);
        exit(1);
    }
    next_token_wrapper();  // Advance to the next token
}

// Function to parse a class definition
ClassNode* parse_class() {
    expect(TOKEN_CLASS);  // Expect 'class' keyword

    const char* class_name = current_token.value;  // Store class name
    expect(TOKEN_IDENTIFIER);  // Expect class name

    ClassNode* class_node = (ClassNode*)malloc(sizeof(ClassNode));
    class_node->class_name = strdup(class_name);
    class_node->fields = NULL;
    class_node->methods = NULL;

    expect(TOKEN_LBRACE);  // Expect '{' to start class body

    // Parse the fields and methods within the class
    while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_END) {
        if (current_token.type == TOKEN_INT || current_token.type == TOKEN_FLOAT) {
            // Parse a field
            Field* field = parse_field();
            field->next = class_node->fields;  // Add to the front of the list
            class_node->fields = field;
        } else if (current_token.type == TOKEN_VOID) {
            // Parse a method
            Method* method = parse_method();
            method->next = class_node->methods;  // Add to the front of the list
            class_node->methods = method;
        } else {
            fprintf(stderr, "Error: Unexpected token in class body: %s\n", current_token.value);
            free_class_node(class_node);
            exit(1);
        }
    }

    expect(TOKEN_RBRACE);  // Expect '}' to close class body
    return class_node;
}

// Function to parse a class field
Field* parse_field() {
    const char* type = current_token.value;  // Data type (e.g., "int", "float")
    expect(current_token.type);  // Consume data type token

    const char* name = current_token.value;  // Field name
    expect(TOKEN_IDENTIFIER);  // Expect field name

    expect(TOKEN_SEMICOLON);  // Expect semicolon to end the field declaration

    Field* field = (Field*)malloc(sizeof(Field));
    field->type = strdup(type);
    field->name = strdup(name);
    field->next = NULL;
    return field;
}

// Function to parse a method definition
Method* parse_method() {
    const char* return_type = current_token.value;  // Return type (e.g., "void")
    expect(current_token.type);  // Consume return type token

    const char* method_name = current_token.value;  // Method name
    expect(TOKEN_IDENTIFIER);  // Expect method name

    expect(TOKEN_LPAREN);  // Expect '(' to start parameter list

    // Parse parameters (currently skipping for simplicity)
    expect(TOKEN_RPAREN);  // Expect ')' to close parameter list

    expect(TOKEN_LBRACE);  // Expect '{' to start method body

    Method* method = (Method*)malloc(sizeof(Method));
    method->return_type = strdup(return_type);
    method->name = strdup(method_name);
    method->body = parse_block();  // Parse method body

    expect(TOKEN_RBRACE);  // Expect '}' to end method body

    method->next = NULL;
    return method;
}

// Function to parse a block of statements
BlockNode* parse_block() {
    BlockNode* block = NULL;
    BlockNode* last_block = NULL;

    while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_END) {
        StatementNode* stmt = parse_statement();

        BlockNode* new_block = (BlockNode*)malloc(sizeof(BlockNode));
        new_block->node_type = stmt->node_type;
        new_block->next = NULL;

        // Link the statement to the block node
        switch (stmt->node_type) {
            case NODE_IF:
                new_block->ifNode = stmt->ifNode;
                break;
            case NODE_FOR:
                new_block->forNode = stmt->forNode;
                break;
            case NODE_EXPRESSION:
                new_block->expression = stmt->expression;
                break;
            default:
                fprintf(stderr, "Error: Unsupported node type in block.\n");
                exit(1);
        }

        // Add the new block node to the linked list of blocks
        if (block == NULL) {
            block = new_block;
        } else {
            last_block->next = new_block;
        }
        last_block = new_block;

        // Free the statement node (since the data has already been moved)
        free(stmt);
    }

    return block;
}

// Function to parse an individual statement
StatementNode* parse_statement() {
    StatementNode* stmt = (StatementNode*)malloc(sizeof(StatementNode));

    if (current_token.type == TOKEN_IDENTIFIER) {
        // Assume it's an assignment or expression
        ExpressionNode* expr = parse_expression();
        stmt->node_type = NODE_EXPRESSION;
        stmt->expression = expr;
        expect(TOKEN_SEMICOLON);  // Expect a semicolon after an expression
    } else if (current_token.type == TOKEN_IF) {
        // Parse an if statement
        IfNode* if_node = (IfNode*)malloc(sizeof(IfNode));
        expect(TOKEN_IF);
        expect(TOKEN_LPAREN);
        if_node->condition = parse_expression();  // Parse the condition
        expect(TOKEN_RPAREN);
        if_node->trueBlock = parse_block();  // Parse the true block

        if (current_token.type == TOKEN_ELSE) {
            expect(TOKEN_ELSE);
            if_node->falseBlock = parse_block();  // Parse the false block
        } else {
            if_node->falseBlock = NULL;
        }

        stmt->node_type = NODE_IF;
        stmt->ifNode = if_node;
    } else if (current_token.type == TOKEN_FOR) {
        // Handle parsing of "for" loops here (similar to if_node)
        // Example skipped for brevity
    } else {
        fprintf(stderr, "Error: Unexpected token in statement: %s\n", current_token.value);
        free(stmt);
        exit(1);
    }

    return stmt;
}

// Function to parse an expression (basic implementation for variable assignment or arithmetic)
ExpressionNode* parse_expression() {
    ExpressionNode* expr = (ExpressionNode*)malloc(sizeof(ExpressionNode));

    if (current_token.type == TOKEN_IDENTIFIER) {
        expr->variable = strdup(current_token.value);
        next_token_wrapper();  // Move past identifier

        if (current_token.type == TOKEN_ASSIGN) {
            next_token_wrapper();  // Move past '='
            ExpressionNode* value_expr = (ExpressionNode*)malloc(sizeof(ExpressionNode));
            value_expr->value = atoi(current_token.value);
            value_expr->next = NULL;
            expr->next = value_expr;
            next_token_wrapper();  // Move past assigned value
        } else {
            fprintf(stderr, "Error: Expected '=' after identifier in assignment.\n");
            free(expr);
            exit(1);
        }
    } else {
        fprintf(stderr, "Error: Unexpected token in expression: %s\n", current_token.value);
        free(expr);
        exit(1);
    }

    return expr;
}
