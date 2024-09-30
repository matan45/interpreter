// interpreter.c

#include "interpreter.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#define strdup _strdup
#endif

// Symbol table to keep track of variable values
static SymbolTable* symbol_table = NULL;

// Function to create an object from a class definition
Object* create_object(ClassNode* class_node) {
    if (!class_node) return NULL;

    Object* obj = (Object*)malloc(sizeof(Object));
    if (!obj) {
        error("Failed to allocate memory for Object");
    }

    obj->class_type = class_node;
    obj->field_values = NULL;

    // Initialize fields to default values (0 for int, etc.)
    Field* class_field = class_node->fields;
    while (class_field) {
        Field* new_field = (Field*)malloc(sizeof(Field));
        new_field->type = strdup(class_field->type);
        new_field->name = strdup(class_field->name);
        new_field->value = calloc(1, sizeof(int)); // Initialize to 0 for simplicity (assuming int type)
        new_field->next = obj->field_values;
        obj->field_values = new_field;

        class_field = class_field->next;
    }

    return obj;
}

// Function to free memory allocated for an object
void free_object(Object* obj) {
    if (!obj) return;

    Field* current_field = obj->field_values;
    while (current_field) {
        Field* next_field = current_field->next;
        free((char*)current_field->type);
        free((char*)current_field->name);
        free(current_field->value);
        free(current_field);
        current_field = next_field;
    }

    free(obj);
}

// Function to execute a class definition
void execute_class(ClassNode* class_node) {
    if (!class_node) return;

    printf("Executing class %s\n", class_node->class_name);

    // Execute each method in the class
    Method* method = class_node->methods;
    while (method) {
        if (strcmp(method->name, "main") == 0) {
            printf("Executing main method in class %s\n", class_node->class_name);
            execute_block(method->body);
        }
        method = method->next;
    }
}

// Function to execute a block of statements
void execute_block(BlockNode* block) {
    BlockNode* current = block;
    while (current) {
        switch (current->node_type) {
            case NODE_IF:
                execute_if(current->ifNode);
                break;
            case NODE_FOR:
                execute_for(current->forNode);
                break;
            case NODE_EXPRESSION:
                evaluate_expression(current->expression);
                break;
            default:
                error("Unsupported node type in block execution");
                break;
        }
        current = current->next;
    }
}

// Function to execute an if statement
void execute_if(IfNode* if_node) {
    if (!if_node) return;

    int condition_value = evaluate_expression(if_node->condition);
    if (condition_value) {
        execute_block(if_node->trueBlock);
    } else if (if_node->falseBlock) {
        execute_block(if_node->falseBlock);
    }
}

// Function to execute a for loop
void execute_for(ForNode* for_node) {
    if (!for_node) return;

    // Execute the initializer (e.g., int i = 0)
    evaluate_expression(for_node->initializer);

    // Loop while the condition is true
    while (evaluate_expression(for_node->condition)) {
        execute_block(for_node->body);
        evaluate_expression(for_node->update); // Execute update expression (e.g., i++)
    }
}

// Function to evaluate an expression and return the result
int evaluate_expression(ExpressionNode* expr) {
    if (!expr) {
        error("Null expression encountered in evaluation");
    }

    if (expr->variable) {
        // Check if it's an identifier, then get its value from the symbol table
        return lookup_variable(expr->variable);
    }

    // If it's a constant value, return the value
    return expr->value;
}

// Function to execute a method on an object
void execute_method(Object* obj, const char* method_name) {
    if (!obj || !method_name) return;

    Method* method = obj->class_type->methods;
    while (method) {
        if (strcmp(method->name, method_name) == 0) {
            printf("Executing method %s on object of class %s\n", method_name, obj->class_type->class_name);
            execute_block(method->body);
            return;
        }
        method = method->next;
    }

    printf("Error: Method %s not found in class %s\n", method_name, obj->class_type->class_name);
}

// Symbol Table Management Functions

// Function to look up a variable's value in the symbol table
int lookup_variable(const char* variable_name) {
    SymbolTable* current = symbol_table;
    while (current) {
        if (strcmp(current->variable_name, variable_name) == 0) {
            return current->value;
        }
        current = current->next;
    }
    fprintf(stderr, "Error: Undefined variable %s\n", variable_name);
    exit(1);
}

// Function to update or add a variable in the symbol table
void update_variable(const char* variable_name, int value) {
    SymbolTable* current = symbol_table;
    while (current) {
        if (strcmp(current->variable_name, variable_name) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    // If not found, add a new variable to the symbol table
    SymbolTable* new_var = (SymbolTable*)malloc(sizeof(SymbolTable));
    new_var->variable_name = str_dup(variable_name);
    new_var->value = value;
    new_var->next = symbol_table;
    symbol_table = new_var;
}

// Function to free the entire symbol table
void free_symbol_table() {
    SymbolTable* current = symbol_table;
    while (current) {
        SymbolTable* temp = current;
        current = current->next;
        free((char*)temp->variable_name);
        free(temp);
    }
    symbol_table = NULL;
}
