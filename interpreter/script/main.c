// main.c

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    // Check if the user provided a source code file
   /* if (argc < 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }*/

    // Read the source code from the provided file
    const char* source_file = "../../test.txt";
    char* source_code = read_file(source_file);
    if (!source_code) {
        error("Failed to read the source code file.");
    }

    // Initialize lexer with the source code
    init_lexer(source_code);

    // Initialize parsing by getting the first token
    next_token_wrapper();

    // Parse the source code into a ClassNode
    ClassNode* class_node = parse_class();
    if (!class_node) {
        error("Failed to parse class.");
    }

    // Print parsed class information (for debugging purposes)
    printf("Parsed class: %s\n", class_node->class_name);

    Field* field = class_node->fields;
    while (field) {
        printf("Field: %s %s\n", field->type, field->name);
        field = field->next;
    }

    Method* method = class_node->methods;
    while (method) {
        printf("Method: %s %s\n", method->return_type, method->name);
        method = method->next;
    }

    // Create an object of the parsed class (if applicable)
    Object* my_object = create_object(class_node);
    printf("Created object of class %s\n", my_object->class_type->class_name);

    // Execute the main method of the class, if available
    execute_class(class_node);

    // Free all resources used during execution
    free_object(my_object);
    free_class_node(class_node);
    free_symbol_table();

    // Free the source code read from the file
    free(source_code);

    return 0;
}
