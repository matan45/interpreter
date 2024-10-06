#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <stdexcept>
#include "RuntimeError.hpp"

// Environment class to manage variable scope
class Environment : public std::enable_shared_from_this<Environment> {

private:
	std::unordered_map<std::string, std::variant<std::monostate, double, std::string>> values;
	std::shared_ptr<Environment> enclosing;
public:
	// Constructors
	Environment() : enclosing(nullptr) {}

	explicit Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

	// Get a variable from the environment
	std::variant<std::monostate, double, std::string> get(const Token& name);

	// Assign a value to an existing variable
	void assign(const Token& name, const std::variant<std::monostate, double, std::string>& value);

	// Define a new variable in the current scope
	void define(const std::string& name, const std::variant<std::monostate, double, std::string>& value);

	// Get the ancestor environment at a given distance
	std::shared_ptr<Environment> ancestor(int distance);

	// Get a variable from an ancestor at a given distance
	std::variant<std::monostate, double, std::string> getAt(int distance, const std::string& name);

	// Assign a value to a variable at a given distance
	void assignAt(int distance, const Token& name, const std::variant<std::monostate, double, std::string>& value);
	// Overriding the toString() for debugging purposes
	std::string toString() const;
	
};

// To allow shared_from_this usage
class SharedEnvironment : public Environment, public std::enable_shared_from_this<SharedEnvironment> {
public:
	using Environment::Environment;
};

