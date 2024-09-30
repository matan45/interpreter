// utils.h
#pragma once

#include <stddef.h>

// Error handling utility function
void error(const char* message);

// Utility to duplicate a string (portable across different compilers)
char* str_dup(const char* source);

// Utility to concatenate two strings
char* str_concat(const char* str1, const char* str2);

// Utility to read the content of a file (useful for importing code from other files)
char* read_file(const char* filename);

