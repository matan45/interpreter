#include "parse.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
	const char* code =
		"class MyClass {"
		"  int x;"
		"  void myMethod() {"
		"    if (x < 10) {"
		"      x = 15;"
		"    }"
		"    for (int i = 0; i < 3; i++) {"
		"      x = x + 1;"
		"    }"
		"  }"
		"}";

	// Point the global source pointer to the source code
	source = &code;

	// Initialize the lexer by getting the first token
	next_token_wrapper();

	// Parse the class definition from the source code
	ClassNode* class_node = parse_class();
	if (class_node == NULL) {
		printf("Error: Failed to parse class.\n");
		return 1;
	}

	// Print parsed class information
	printf("Parsed class: %s\n", class_node->class_name);

	// Print parsed fields
	Field* field = class_node->fields;
	while (field) {
		printf("Field: %s %s\n", field->type, field->name);
		field = field->next;
	}

	// Print parsed methods
	Method* method = class_node->methods;
	while (method) {
		printf("Method: %s %s\n", method->return_type, method->name);
		method = method->next;
	}

	// Create an object of the parsed class
	Object* my_object = create_object(class_node);
	printf("Created object of class %s\n", my_object->class_type->class_name);

	update_object_field(my_object, "x", 5);  // Set x = 5 for my_object
	execute_method(my_object, "myMethod");   // Execute method on object
	printf("Updated value of x: %d\n", lookup_object_field(my_object, "x"));

	// Cleanup
	free_object(my_object);
	free_class_node(class_node);
	clean_up();

	return 0;
}
