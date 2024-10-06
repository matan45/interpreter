#include "Scanner.hpp"
#include "Environment.hpp"

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


	auto global = std::make_shared<SharedEnvironment>();
	Token varToken("x", 1);
	global->define("x", 42.0);

	try {
		std::cout << "Value of x: " << std::get<double>(global->get(varToken)) << "\n";
	}
	catch (const RuntimeError& e) {
		std::cerr << "Runtime error: " << e.what() << "\n";
	}

	auto local = std::make_shared<SharedEnvironment>(global);
	Token localVarToken("y", 2);
	local->define("y", std::string("Hello"));

	try {
		std::cout << "Value of y: " << std::get<std::string>(local->get(localVarToken)) << "\n";
		std::cout << "Value of x in local scope: " << std::get<double>(local->get(varToken)) << "\n";
	}
	catch (const RuntimeError& e) {
		std::cerr << "Runtime error: " << e.what() << "\n";
	}

	std::cout << "Environment: " << local->toString() << "\n";

	return 0;

	return 0;
}