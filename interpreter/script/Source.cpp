#include "Scanner.hpp"

int main() {
	// Sample Lox code to scan
	std::string source = "var language = \"Lox\";\n"
		"print language + \" is fun!\";";

	// Create a Scanner object
	Scanner scanner(source);

	// Scan the tokens
	std::vector<Token> tokens = scanner.scanTokens();

	// Print the tokens
	for (const Token& token : tokens) {
		std::cout << "Type: " << token.toString() << std::endl;
	}

	return 0;
}