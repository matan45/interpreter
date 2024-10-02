#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "environment.h"

// Function prototypes for the interpreter
int interpret(ASTNode* node, Environment* env);
void interpret_program(ASTNode* program, Environment* env);
int interpret_function_call(ASTNode* node, Environment* env);
int interpret_if_statement(ASTNode* node, Environment* env);
int interpret_while_loop(ASTNode* node, Environment* env);
int interpret_do_while_loop(ASTNode* node, Environment* env);
int interpret_assignment(ASTNode* node, Environment* env);
int interpret_binary_op(ASTNode* node, Environment* env);
int interpret_return_statement(ASTNode* node, Environment* env);
int interpret_object_construction(ASTNode* node, Environment* env);
int interpret_object_destruction(ASTNode* node, Environment* env);
int interpret_method_call(ASTNode* node, Environment* env);

// Utility functions for managing the environment
Environment* create_environment(Environment* parent);
void free_environment(Environment* env);
int delete_object(Environment* env, const char* name);

int count_methods(Function* method);
int count_fields(ObjectField* field);

#endif // INTERPRETER_H

