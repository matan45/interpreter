// ast.h

#pragma once

#include "types.h"


// Function to free a ClassNode
void free_class_node(ClassNode* class_node);

// Function to free an IfNode
void free_if_node(IfNode* if_node);

// Function to free a ForNode
void free_for_node(ForNode* for_node);

// Function to free an ExpressionNode
void free_expression_node(ExpressionNode* expr);

// Function to free a BlockNode
void free_block_node(BlockNode* block);
