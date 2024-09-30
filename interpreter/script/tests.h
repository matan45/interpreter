#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "lexer.h"
#include "parser.h"


// Simple macros to print test results
#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("Test failed: %s\n", message); \
            return 0; \
        } \
    } while (0)

#define RUN_TEST(test) \
    do { \
        if (test()) { \
            printf("%s passed.\n", #test); \
        } else { \
            printf("%s failed.\n", #test); \
        } \
    } while (0)

// A helper function to initialize the lexer and start parsing
void init_parser(const char* source) {
	init_lexer(source);
	next_token_wrapper();
}


int test_parse_function_with_parameters() {
	const char* source_code = "int add(int x, float y) { return x + y; }";

	init_parser(source_code);

	Method* method = parse_method();

	ASSERT(method != NULL, "Expected method to be parsed.");
	ASSERT(strcmp(method->name, "add") == 0, "Expected method name to be 'add'.");
	ASSERT(strcmp(method->return_type, "int") == 0, "Expected return type to be 'int'.");

	ParameterNode* param1 = method->parameters;
	ASSERT(param1 != NULL, "Expected first parameter to exist.");
	ASSERT(strcmp(param1->type, "int") == 0, "Expected first parameter type to be 'int'.");
	ASSERT(strcmp(param1->name, "x") == 0, "Expected first parameter name to be 'x'.");

	ParameterNode* param2 = param1->next;
	ASSERT(param2 != NULL, "Expected second parameter to exist.");
	ASSERT(strcmp(param2->type, "float") == 0, "Expected second parameter type to be 'float'.");
	ASSERT(strcmp(param2->name, "y") == 0, "Expected second parameter name to be 'y'.");

	ASSERT(param2->next == NULL, "Expected no more parameters.");

	return 1; // Test passed
}

int test_parse_assignment() {
	const char* source_code = "x = 10;";

	init_parser(source_code);

	StatementNode* statement = parse_statement();

	ASSERT(statement != NULL, "Expected statement to be parsed.");
	ASSERT(statement->node_type == NODE_ASSIGNMENT, "Expected node type to be NODE_ASSIGNMENT.");
	ASSERT(statement->expression != NULL, "Expected expression in assignment.");
	ASSERT(strcmp(statement->expression->variable, "x") == 0, "Expected assigned variable to be 'x'.");
	ASSERT(statement->expression->next != NULL, "Expected assignment value to exist.");
	ASSERT(statement->expression->next->value == 10, "Expected assigned value to be '10'.");

	return 1; // Test passed
}

int test_parse_if_statement() {
	const char* source_code = "if (x > 5) { int y = 10; } else { x = 20; }";

	init_parser(source_code);

	IfNode* if_node = parse_if();

	ASSERT(if_node != NULL, "Expected if statement to be parsed.");
	ASSERT(if_node->condition != NULL, "Expected condition in if statement.");

	ASSERT(if_node->trueBlock != NULL, "Expected true block to exist.");
	BlockNode* true_block = if_node->trueBlock;
	ASSERT(true_block->localVariable != NULL, "Expected local variable in true block.");
	ASSERT(strcmp(true_block->localVariable->name, "y") == 0, "Expected variable name to be 'y'.");

	ASSERT(if_node->falseBlock != NULL, "Expected else block to exist.");
	BlockNode* false_block = if_node->falseBlock;
	ASSERT(false_block->expression != NULL, "Expected assignment in else block.");
	ASSERT(strcmp(false_block->expression->variable, "x") == 0, "Expected assignment target to be 'x'.");
	ASSERT(false_block->expression->next->value == 20, "Expected assigned value to be '20'.");

	return 1; // Test passed
}

int test_parse_for_loop() {
	const char* source_code = "for (int i = 0; i < 10; i++) { x = x + 1; }";

	init_parser(source_code);

	ForNode* for_node = parse_for();

	ASSERT(for_node != NULL, "Expected for loop to be parsed.");
	ASSERT(for_node->initializer != NULL, "Expected initializer to be parsed.");
	ASSERT(strcmp(for_node->initializer->variable, "i") == 0, "Expected initializer variable to be 'i'.");
	ASSERT(for_node->initializer->value == 0, "Expected initializer value to be '0'.");

	ASSERT(for_node->condition != NULL, "Expected condition to be parsed.");
//	ASSERT(strcmp(for_node->condition->variable, "i") == 0, "Expected condition variable to be 'i'.");

	ASSERT(for_node->update != NULL, "Expected update to be parsed.");
	ASSERT(for_node->update->variable != NULL, "Expected update expression to be 'i++'.");

	ASSERT(for_node->body != NULL, "Expected body of for loop to exist.");
	BlockNode* body = for_node->body;
	ASSERT(body->expression != NULL, "Expected an assignment in the for loop body.");
	ASSERT(strcmp(body->expression->variable, "x") == 0, "Expected assignment target to be 'x'.");

	return 1; // Test passed
}

int test_parse_complex_expressions() {
	// Test expression: (x + 5) * (y - 3)
	const char* source_code = "(x + 5) * (y - 3)";
	init_lexer(source_code);
	ExpressionNode* expr = parse_expression();

	ASSERT(expr != NULL, "Expected a valid expression node.");
	ASSERT(expr->value == '*', "Expected '*' as the root operator of the expression.");
	ASSERT(expr->next != NULL, "Expected operator node to have a right-hand operand.");

	// Test logical expression: x > 5 && y < 10
	const char* source_code2 = "x > 5 && y < 10";
	init_lexer(source_code2);
	expr = parse_expression();

	ASSERT(expr != NULL, "Expected a valid expression node for logical operation.");
	ASSERT(expr->value == '&', "Expected '&&' represented by '&' as logical AND.");
	ASSERT(expr->next != NULL, "Expected a valid right-hand operand.");
	printf("test_parse_complex_expressions passed.\n");

	return 1; // Test passed
}

int test_parse_while_loop() {
	// Source code for a while loop
	const char* source_code = "while (x > 5) { x = x - 1; }";
	init_lexer(source_code);
	WhileNode* while_node = parse_while();

	ASSERT(while_node != NULL, "Expected a valid WhileNode.");
	ASSERT(while_node->condition != NULL, "Expected a valid condition in the while loop.");
	ASSERT(strcmp(while_node->condition->variable, "x") == 0, "Expected condition to reference variable 'x'.");
	ASSERT(while_node->body != NULL, "Expected a valid body block for the while loop.");
	printf("test_parse_while_loop passed.\n");
	return 1; // Test passed
}

int test_parse_do_while_loop() {
	// Source code for a do-while loop
	const char* source_code = "do { x = x + 1; } while (x < 10);";
	init_lexer(source_code);
	DoWhileNode* do_while_node = parse_do_while();

	ASSERT(do_while_node != NULL, "Expected a valid DoWhileNode.");
	ASSERT(do_while_node->body != NULL, "Expected a valid body block for the do-while loop.");
	ASSERT(do_while_node->condition != NULL, "Expected a valid condition for the do-while loop.");
	ASSERT(strcmp(do_while_node->condition->variable, "x") == 0, "Expected condition to reference variable 'x'.");
	printf("test_parse_do_while_loop passed.\n");
	return 1; // Test passed
}

int test_parse_switch_statement() {
	// Source code for a switch statement
	const char* source_code = "switch (x) { case 1: x = 10; break; case 2: x = 20; break; default: x = 0; }";
	init_lexer(source_code);
	SwitchNode* switch_node = parse_switch();

	ASSERT(switch_node != NULL, "Expected a valid SwitchNode.");
	ASSERT(switch_node->expression != NULL, "Expected a valid switch expression.");
	ASSERT(strcmp(switch_node->expression->variable, "x") == 0, "Expected switch expression to reference variable 'x'.");

	CaseNode* case_node = switch_node->cases;
	ASSERT(case_node != NULL, "Expected at least one case statement.");
	ASSERT(case_node->value == 1, "Expected the first case value to be '1'.");
	ASSERT(case_node->statements != NULL, "Expected statements in the first case.");

	case_node = case_node->next;
	ASSERT(case_node != NULL, "Expected a second case statement.");
	ASSERT(case_node->value == 2, "Expected the second case value to be '2'.");

	ASSERT(case_node->next != NULL, "Expected a default case.");
	ASSERT(case_node->next->value == -1, "Expected default case marker value.");
	printf("test_parse_switch_statement passed.\n");

	return 1; // Test passed
}

