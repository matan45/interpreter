#include "tests.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to create tokens from source code
Token** tokenize(const char* source_code, int* token_count) {
	Tokenizer* tokenizer = create_tokenizer((char*)source_code);
	Token** tokens = (Token**)malloc(100 * sizeof(Token*));
	*token_count = 0;

	Token* token = get_next_token(tokenizer);
	while (token->type != TOKEN_EOF) {
		tokens[(*token_count)++] = token;
		token = get_next_token(tokenizer);
	}

	tokens[(*token_count)++] = token; // Add EOF token

	free_tokenizer(tokenizer);
	return tokens;
}

// Helper function to free tokens
void free_tokens(Token** tokens, int token_count) {
	for (int i = 0; i < token_count; i++) {
		free_token(tokens[i]);
		free(tokens[i]);
	}
	free(tokens);
}

// Test the complete flow: Tokenizer -> Parser -> Interpreter
void test_full_flow() {
	printf("Running test_full_flow...\n");

	// Sample source code to tokenize, parse, and interpret
	const char* source_code = "int x = 10; if (x > 5) { x = x + 1; }";

	// 1. Tokenize the source code
	int token_count;
	Token** tokens = tokenize(source_code, &token_count);

	// 2. Parse the tokens to create an AST
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	// 3. Create an environment for the interpreter
	Environment* env = create_environment(NULL);

	// 4. Interpret the AST
	interpret_program(program, env);

	// 5. Validate the expected state of the environment
	Variable* var = lookup_variable(env, "x");
	assert(var != NULL);
	assert(var->value == 11); // Expect x to be 11 after increment

	// Cleanup
	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_full_flow passed!\n");
}

// Test for `while` loop with arithmetic operations and conditional checks
void test_while_loop_with_if() {
	printf("Running test_while_loop_with_if...\n");

	const char* source_code = "int i = 0; while (i < 10) { if (i % 2 == 0) { i = i + 3; } else { i = i + 1; } }";

	// 1. Tokenization
	int token_count;
	Token** tokens = tokenize(source_code, &token_count);

	// 2. Parsing
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	// 3. Interpretation
	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	// 4. Validation
	Variable* var = lookup_variable(env, "i");
	assert(var != NULL);
	assert(var->value == 12); // After the loop, i should be 12

	// Cleanup
	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_while_loop_with_if passed!\n");
}

// Test for `do-while` loop with arithmetic operations and conditionals
void test_do_while_loop() {
	printf("Running test_do_while_loop...\n");

	const char* source_code = "int count = 0; do { count = count + 2; } while (count < 10);";

	// 1. Tokenization
	int token_count;
	Token** tokens = tokenize(source_code, &token_count);

	// 2. Parsing
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	// 3. Interpretation
	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	// 4. Validation
	Variable* var = lookup_variable(env, "count");
	assert(var != NULL);
	assert(var->value == 10); // After the loop, count should be 10

	// Cleanup
	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_do_while_loop passed!\n");
}

// Test for `for` loop involving arithmetic operations and an `if` condition inside
void test_for_loop_with_if() {
	printf("Running test_for_loop_with_if...\n");

	const char* source_code = "int sum = 0; for (int j = 0; j < 5; j++) { if (j % 2 == 0) { sum = sum + j; } }";

	// 1. Tokenization
	int token_count;
	Token** tokens = tokenize(source_code, &token_count);

	// 2. Parsing
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	// 3. Interpretation
	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	// 4. Validation
	Variable* var = lookup_variable(env, "sum");
	assert(var != NULL);
	assert(var->value == 6); // Expected sum = 0 + 2 + 4 = 6

	// Cleanup
	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_for_loop_with_if passed!\n");
}

// Test class definition, object construction, method call, and deletion
void test_class_object_lifecycle() {
	printf("Running test_class_object_lifecycle...\n");

	// Define class Point, create an instance, and call a method
	const char* source_code =
		"class Point { "
		"    int x; int y; "
		"    function set_values(int a, int b) { x = a; y = b; } "
		"    function sum() { return x + y; } "
		"} "
		"Point p = new Point(); "
		"p.set_values(3, 4); "
		"int result = p.sum(); "
		"delete p;";

	// 1. Tokenization
	int token_count;
	Token** tokens = tokenize(source_code, &token_count);

	// 2. Parsing
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	// 3. Interpretation
	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	// 4. Validation
	Variable* result_var = lookup_variable(env, "result");
	assert(result_var != NULL);
	assert(result_var->value == 7); // Expected result: 3 + 4 = 7

	Object* obj = lookup_object(env, "p");
	assert(obj == NULL); // p should be deleted

	// Cleanup
	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_class_object_lifecycle passed!\n");
}

// Test function with parameters and without return type
void test_function_with_params() {
	printf("Running test_function_with_params...\n");

	const char* source_code =
		"function add(int a, int b) { int result = a + b; } "
		"add(3, 5);";

	int token_count;
	Token** tokens = tokenize(source_code, &token_count);
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_function_with_params passed!\n");
}

// Test function without parameters but with return type
void test_function_without_params() {
	printf("Running test_function_without_params...\n");

	const char* source_code =
		"function greet() { return \"Hello, World!\"; } "
		"string message = greet();";

	int token_count;
	Token** tokens = tokenize(source_code, &token_count);
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	//TODO add char* for string
	Variable* var = lookup_variable(env, "message");
	assert(var != NULL);
	assert(strcmp(var->value, "Hello, World!") == 0);

	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_function_without_params passed!\n");
}

// Test increment (++) and decrement (--) operators
void test_increment_decrement() {
	printf("Running test_increment_decrement...\n");

	const char* source_code = "int x = 5; x++; x--;";

	int token_count;
	Token** tokens = tokenize(source_code, &token_count);
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	Variable* var = lookup_variable(env, "x");
	assert(var != NULL);
	assert(var->value == 5); // After increment and decrement, x should be 5

	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_increment_decrement passed!\n");
}

// Test logical NOT operator (!)
void test_logical_not() {
	printf("Running test_logical_not...\n");

	const char* source_code = "boolean is_true = true; boolean is_false = !is_true;";

	int token_count;
	Token** tokens = tokenize(source_code, &token_count);
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	Variable* var = lookup_variable(env, "is_false");
	assert(var != NULL);
	assert(var->value == 0); // is_false should be false (0)

	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_logical_not passed!\n");
}

// Test class with data members and object manipulation
void test_class_with_data_members() {
	printf("Running test_class_with_data_members...\n");

	const char* source_code =
		"class Car { int speed; string color; } "
		"Car myCar = new Car(); myCar.speed = 100; myCar.color = \"red\";";

	int token_count;
	Token** tokens = tokenize(source_code, &token_count);
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	Object* car = lookup_object(env, "myCar");
	assert(car != NULL);
	ObjectField* speed = lookup_object_field(car, "speed");
	assert(speed != NULL);
	assert(speed->value == 100);

	ObjectField* color = lookup_object_field(car, "color");
	assert(color != NULL);
	assert(strcmp(color->name, "red") == 0);

	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_class_with_data_members passed!\n");
}

// Test class without data members and method call
void test_class_without_data_members() {
	printf("Running test_class_without_data_members...\n");

	const char* source_code =
		"class EmptyClass { function greet() { return \"Hi\"; } } "
		"EmptyClass obj = new EmptyClass(); string greeting = obj.greet();";

	int token_count;
	Token** tokens = tokenize(source_code, &token_count);
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	//TODO add char* for string
	Variable* greeting = lookup_variable(env, "greeting");
	assert(greeting != NULL);
	assert(strcmp(greeting->value, "Hi") == 0);

	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_class_without_data_members passed!\n");
}

// Test class with objects as data members
void test_class_with_object_as_member() {
	printf("Running test_class_with_object_as_member...\n");

	const char* source_code =
		"class Engine { int horsepower; } "
		"class Car { Engine engine; } "
		"Car myCar = new Car(); myCar.engine = new Engine(); myCar.engine.horsepower = 250;";

	int token_count;
	Token** tokens = tokenize(source_code, &token_count);
	Parser* parser = create_parser(tokens, token_count);
	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);

	Environment* env = create_environment(NULL);
	interpret_program(program, env);

	Object* car = lookup_object(env, "myCar");
	assert(car != NULL);

	ObjectField* engine_field = lookup_object_field(car, "engine");
	assert(engine_field != NULL);
	Object* engine = lookup_object(env, engine_field->name);
	assert(engine != NULL);

	ObjectField* horsepower = lookup_object_field(engine, "horsepower");
	assert(horsepower != NULL);
	assert(horsepower->value == 250);

	free_tokens(tokens, token_count);
	free_environment(env);
	free_ast_node(program);
	free_parser(parser);

	printf("test_class_with_object_as_member passed!\n");
}


// Run all tests
void run_all_tests() {
	test_full_flow();
	test_while_loop_with_if();
	test_do_while_loop();
	test_for_loop_with_if();
	test_class_object_lifecycle();
	test_function_with_params();
	test_function_without_params();
	test_increment_decrement();
	test_logical_not();
	test_class_with_data_members();
	test_class_without_data_members();
	test_class_with_object_as_member();
}


