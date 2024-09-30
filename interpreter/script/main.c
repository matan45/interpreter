// main.c

#include "tests.h"

int main(int argc, char* argv[]) {
	printf("Running parser unit tests...\n");

	RUN_TEST(test_parse_function_with_parameters);
	RUN_TEST(test_parse_assignment);
	RUN_TEST(test_parse_if_statement);
	RUN_TEST(test_parse_for_loop);

	printf("All tests completed.\n");

    return 0;
}
