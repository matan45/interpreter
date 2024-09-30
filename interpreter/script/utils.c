// utils.c

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Error handling utility function
void error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

// Utility to duplicate a string (similar to strdup, but portable)
char* str_dup(const char* source) {
    if (source == NULL) {
        return NULL;
    }
    
    size_t length = strlen(source);
    char* copy = (char*)malloc(length + 1); // Allocate memory for the copy (+1 for null terminator)
    
    if (copy == NULL) {
        error("Memory allocation failed for string duplication");
    }

    strcpy(copy, source); // Copy the original string to the new memory location
    return copy;
}

// Utility to concatenate two strings
char* str_concat(const char* str1, const char* str2) {
    if (str1 == NULL || str2 == NULL) {
        error("Cannot concatenate null strings");
    }

    size_t length1 = strlen(str1);
    size_t length2 = strlen(str2);
    
    char* result = (char*)malloc(length1 + length2 + 1); // Allocate memory for the concatenated string (+1 for null terminator)
    
    if (result == NULL) {
        error("Memory allocation failed for string concatenation");
    }

    strcpy(result, str1);  // Copy the first string to the result
    strcat(result, str2);  // Append the second string to the result
    return result;
}

// Utility to read the content of a file
char* read_file(const char* filename) {
	FILE* file = fopen(filename, "rb");  // Open file in binary mode for broad compatibility
	if (file == NULL) {
		perror("Error opening file");
		return NULL;  // Return NULL if the file could not be opened
	}

	// Seek to the end of the file to determine its size
	if (fseek(file, 0, SEEK_END) != 0) {
		perror("Error seeking in file");
		fclose(file);
		return NULL;
	}

	long file_size = ftell(file);  // Get the size of the file
	if (file_size < 0) {
		perror("Error telling file size");
		fclose(file);
		return NULL;
	}

	rewind(file);  // Go back to the beginning of the file

	// Allocate memory to hold the file content plus a null-terminator
	char* buffer = (char*)malloc(sizeof(char) * (file_size + 1));
	if (buffer == NULL) {
		perror("Error allocating memory");
		fclose(file);
		return NULL;
	}

	// Read file content into buffer
	size_t read_size = fread(buffer, sizeof(char), file_size, file);
	if (read_size != file_size) {
		perror("Error reading file");
		free(buffer);  // Clean up allocated memory
		fclose(file);
		return NULL;
	}

	buffer[file_size] = '\0';  // Null-terminate the string

	fclose(file);  // Close the file

	return buffer;  // Return the dynamically allocated content
}
