// ast.c

#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Function to free memory allocated for a ClassNode
void free_class_node(ClassNode* class_node) {
    if (!class_node) return;

    // Free fields
    Field* field = class_node->fields;
    while (field) {
        Field* temp = field;
        field = field->next;
        free((char*)temp->type);
        free((char*)temp->name);
        if (temp->value) {
            free(temp->value); // Assuming the value needs to be freed (depends on the type)
        }
        free(temp);
    }

    // Free methods
    Method* method = class_node->methods;
    while (method) {
        Method* temp = method;
        method = method->next;

        // Free parameters
        ParameterNode* param = temp->parameters;
        while (param) {
            ParameterNode* temp_param = param;
            param = param->next;
            free((char*)temp_param->type);
            free((char*)temp_param->name);
            free(temp_param);
        }

        // Free the method body
        if (temp->body) {
            free_block_node(temp->body);
        }

        free((char*)temp->return_type);
        free((char*)temp->name);
        free(temp);
    }

    // Free the class name and the ClassNode itself
    free((char*)class_node->class_name);
    free(class_node);
}

// Function to free memory allocated for an IfNode
void free_if_node(IfNode* if_node) {
    if (!if_node) return;

    if (if_node->condition) {
        free_expression_node(if_node->condition);
    }
    if (if_node->trueBlock) {
        free_block_node(if_node->trueBlock);
    }
    if (if_node->falseBlock) {
        free_block_node(if_node->falseBlock);
    }

    free(if_node);
}

// Function to free memory allocated for a ForNode
void free_for_node(ForNode* for_node) {
    if (!for_node) return;

    if (for_node->initializer) {
        free_expression_node(for_node->initializer);
    }
    if (for_node->condition) {
        free_expression_node(for_node->condition);
    }
    if (for_node->update) {
        free_expression_node(for_node->update);
    }
    if (for_node->body) {
        free_block_node(for_node->body);
    }

    free(for_node);
}

// Function to free memory allocated for an ExpressionNode
void free_expression_node(ExpressionNode* expr) {
    if (!expr) return;

    if (expr->variable) {
        free(expr->variable);
    }

    // Recursively free linked expressions (e.g., in complex expressions like `a + b + c`)
    if (expr->next) {
        free_expression_node(expr->next);
    }

    free(expr);
}

// Function to free memory allocated for a BlockNode
void free_block_node(BlockNode* block) {
    while (block) {
        BlockNode* temp = block;
        block = block->next;

        switch (temp->node_type) {
            case NODE_IF:
                free_if_node(temp->ifNode);
                break;
            case NODE_FOR:
                free_for_node(temp->forNode);
                break;
            case NODE_EXPRESSION:
                free_expression_node(temp->expression);
                break;
            // Handle other node types if needed
            default:
                fprintf(stderr, "Error: Unsupported node type in block during free.\n");
                exit(1);
        }

        free(temp);
    }
}
