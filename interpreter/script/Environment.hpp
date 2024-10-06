#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <stdexcept>
#include <functional>
#include "RuntimeError.hpp"

// Forward declare Value since it will be used in the function type
struct Value;

// Define the type for lambda functions that return a Value
using FunctionType = std::function<Value(std::vector<Value>)>;

// Define the Value type to be used throughout the project
struct Value : public std::variant<
	std::monostate,    // Represents nil or undefined
	double,            // Numeric values
	std::string,       // String values
	FunctionType       // Lambda functions that return a Value
> {
	using variant::variant; // Inherit constructors from std::variant
};

// Environment class to manage variable scope
class Environment : public std::enable_shared_from_this<Environment> {

private:
	std::unordered_map<std::string, Value> values;
	std::shared_ptr<Environment> enclosing;
public:
	// Constructors
	Environment() : enclosing(nullptr) {}

	explicit Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

	// Get a variable from the environment
	Value get(const Token& name);

	// Assign a value to an existing variable
	void assign(const Token& name, const Value& value);

	// Define a new variable in the current scope
	void define(const std::string& name, const Value& value);

	// Get the ancestor environment at a given distance
	std::shared_ptr<Environment> ancestor(int distance);

	// Get a variable from an ancestor at a given distance
	Value getAt(int distance, const std::string& name);

	// Assign a value to a variable at a given distance
	void assignAt(int distance, const Token& name, const Value& value);
	// Overriding the toString() for debugging purposes
	std::string toString() const;
	
};

// To allow shared_from_this usage
class SharedEnvironment : public Environment, public std::enable_shared_from_this<SharedEnvironment> {
public:
	using Environment::Environment;
};

