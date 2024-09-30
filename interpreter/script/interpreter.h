// interpreter.h

#pragma once


#include "types.h"
#include "ast.h"

// Function to execute a class definition
void execute_class(ClassNode* class_node);

// Function to execute an if statement
void execute_if(IfNode* if_node);

// Function to execute a for loop
void execute_for(ForNode* for_node);

// Function to execute a block of code (e.g., method body or a block within an if statement)
void execute_block(BlockNode* block);

// Function to evaluate an expression (returning an integer result)
int evaluate_expression(ExpressionNode* expr);

// Function to execute a method on a class object
void execute_method(Object* obj, const char* method_name);

// Function to create an instance of a class
Object* create_object(ClassNode* class_node);

// Function to free the resources used by an Object
void free_object(Object* obj);

// Symbol table management
int lookup_variable(const char* variable_name);
void update_variable(const char* variable_name, int value);
void free_symbol_table();

