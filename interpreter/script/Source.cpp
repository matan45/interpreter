#include "Scanner.hpp"
#include "Environment.hpp"
#include "Token.hpp" 

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

	// Create the global environment
	auto global = std::make_shared<Environment>();
	Token varToken("x", 1);
	global->define("x", 42.0);

	try {
		std::cout << "Value of x: " << std::get<double>(global->get(varToken)) << "\n";
	}
	catch (const RuntimeError& e) {
		std::cerr << "Runtime error: " << e.what() << "\n";
	}

	// Create a local environment with 'global' as its enclosing environment
	auto local = std::make_shared<Environment>(global);
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

	// Create another environment
	auto global2 = std::make_shared<Environment>();

	// Define a lambda function that takes a vector of Values and returns a Value
	auto lambdaFunction = [](std::vector<Value> args) -> Value {
		if (!args.empty() && std::holds_alternative<std::string>(args[0])) {
			std::string greeting = "Hello, " + std::get<std::string>(args[0]) + "!";
			return greeting;
		}
		else {
			return std::string("Hello, stranger!");
		}
		};

	// Register the lambda in the environment
	global2->define("greetLambda", lambdaFunction);

	// Retrieve and call the lambda
	try {
		Token lambdaToken("greetLambda", 0); 
		Value value = global2->get(lambdaToken);

		if (auto func = std::get_if<std::function<Value(std::vector<Value>)>>(&value)) {
			// Call the lambda with an argument and store the result
			std::vector<Value> arguments = { std::string("World") };
			Value result = (*func)(arguments);

			// Print the result
			if (std::holds_alternative<std::string>(result)) {
				std::cout << std::get<std::string>(result) << "\n";
			}
		}
		else {
			std::cerr << "Error: 'greetLambda' is not a callable function.\n";
		}
	}
	catch (const RuntimeError& e) {
		std::cerr << "Runtime error: " << e.what() << "\n";
	}

	return 0;
}
