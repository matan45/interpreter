#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
#include "environment.h"
#include <stdio.h>  // Required for fprintf and stderr
#include <stdlib.h> // Required for malloc, free

// Example native function: Adds two integers
int native_add(int argc, void** argv) {
	if (argc != 2) {
		fprintf(stderr, "Error: native_add expects 2 arguments\n");
		exit(1);
	}
	return *((int*)argv[0]) + *((int*)argv[1]);
}

// Example native function: Sets an object's field value
int native_set_object_field(int argc, void** argv) {
	if (argc != 3) {
		fprintf(stderr, "Error: native_set_object_field expects 3 arguments\n");
		exit(1);
	}
	Object* obj = (Object*)argv[0];
	char* field_name = (char*)argv[1];
	int value = *((int*)argv[2]);

	set_object_field(obj, field_name, value);
	return 0;
}

int main() {
	// Define input script with object and native function usage
	char input[] =
		"function int main() { "
		"  MyClass obj = new MyClass(); "
		"  set_object_field(obj, \"x\", 10); "
		"  int value = add(5, obj.x); "
		"  print(value); "
		"}";

	// Step 1: Tokenization
	Tokenizer* tokenizer = create_tokenizer(input);
	if (!tokenizer) {
		fprintf(stderr, "Error: Failed to create tokenizer\n");
		return 1;
	}

	Token** tokens = malloc(100 * sizeof(Token*));  // Dynamic token array allocation
	int token_index = 0;

	Token* token;
	do {
		token = get_next_token(tokenizer);
		if (token_index >= 100) {
			tokens = realloc(tokens, (token_index + 50) * sizeof(Token*));  // Resize if needed
		}
		tokens[token_index++] = token;
	} while (token->type != TOKEN_EOF);

	// Step 2: Parsing
	Parser* parser = create_parser(tokens, token_index);
	if (!parser) {
		fprintf(stderr, "Error: Failed to create parser\n");
		return 1;
	}

	int statement_count = 0;
	ASTNode* program = parse_program(parser, &statement_count);
	if (!program) {
		fprintf(stderr, "Error: Failed to parse program\n");
		return 1;
	}

	// Step 3: Create Environment and Register Native Functions
	Environment* global_env = create_environment(NULL);
	if (!global_env) {
		fprintf(stderr, "Error: Failed to create global environment\n");
		return 1;
	}

	// Register native functions
	register_native_function(global_env, "add", native_add);
	register_native_function(global_env, "set_object_field", native_set_object_field);

	// Step 4: Interpretation
	interpret_program(program, global_env);

	// Cleanup
	free_parser(parser);
	free_environment(global_env);
	free_tokenizer(tokenizer);
	for (int i = 0; i < token_index; i++) {
		free_token(tokens[i]);
	}
	free(tokens);
	free_ast_node(program);

	return 0;
}
