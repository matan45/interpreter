#include <iostream>
#include <any>
#include <memory>
#include <string>
#include <map>
#include <variant>
#include <stdexcept>
#include <utility>    // std::move
#include <functional>
#include "RuntimeError.hpp"
#include "Scanner.hpp"
#include "Token.hpp"
#include "Environment.hpp"

// Utility function to set up the global environment
void setupGlobalEnvironment(std::shared_ptr<Environment> global) {
	Token varToken("x", 1);
	global->define("x", 42.0);

	try {
		double value = std::any_cast<double>(global->get(varToken));
		std::cout << "Value of x: " << value << "\n";
	}
	catch (const RuntimeError& e) {
		std::cerr << "Runtime error: " << e.what() << "\n";
	}
}

// Utility function to set up the local environment and demonstrate scope resolution
void setupLocalEnvironment(std::shared_ptr<Environment> global) {
	auto local = std::make_shared<Environment>(global);
	Token localVarToken("y", 2);
	local->define("y", std::string("Hello"));

	try {
		std::string yValue = std::any_cast<std::string>(local->get(localVarToken));
		std::cout << "Value of y: " << yValue << "\n";
		double xValue = std::any_cast<double>(local->get(Token("x", 1)));
		std::cout << "Value of x in local scope: " << xValue << "\n";
	}
	catch (const RuntimeError& e) {
		std::cerr << "Runtime error: " << e.what() << "\n";
	}
}

// Utility function to set up and invoke a lambda function in the environment
void setupLambdaFunction(std::shared_ptr<Environment> global) {
	// Defining a lambda that takes a vector of `Value` (presumably a std::variant)
	auto greetLambda = [](std::vector<std::variant<int, double, std::string>> args) -> std::variant<int, double, std::string> {
		if (args.empty() || !std::holds_alternative<std::string>(args[0])) {
			throw std::runtime_error("Expected a string argument.");
		}
		std::string name = std::get<std::string>(args[0]);
		return "Hello, " + name + "!";
		};

	// Store lambda in environment
	global->define("greet", greetLambda);

	// Retrieving and invoking the lambda function
	try {
		std::any value = global->get(Token("greet", 3));
		auto lambda = std::any_cast<decltype(greetLambda)>(value);

		// Call the lambda with an argument
		std::vector<std::variant<int, double, std::string>> args = { "World" };
		auto result = lambda(args);

		// Printing the result
		if (std::holds_alternative<std::string>(result)) {
			std::cout << std::get<std::string>(result) << "\n";
		}
	}
	catch (const std::bad_any_cast& e) {
		std::cerr << "Bad any_cast: " << e.what() << "\n";
	}
	catch (const RuntimeError& e) {
		std::cerr << "Runtime error: " << e.what() << "\n";
	}
}

int main() {
	// Sample source code for scanning tokens
	std::string source = "var language = \"Lox\";\n"
		"print language + \" is fun!\";";

	// Create a Scanner object
	Scanner scanner(source);
	std::vector<Token> tokens = scanner.scanTokens();

	// Print scanned tokens for debugging purposes
	for (const Token& token : tokens) {
		std::cout << "Token: " << token.toString() << std::endl;
	}

	// Create the global environment
	auto global = std::make_shared<Environment>();

	// Setting up and testing the global environment
	setupGlobalEnvironment(global);

	// Setting up and testing a local environment
	setupLocalEnvironment(global);

	// Setting up and invoking a lambda function in the global environment
	setupLambdaFunction(global);

	return 0;
}
