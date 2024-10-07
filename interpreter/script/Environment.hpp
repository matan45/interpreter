#pragma once

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

// Environment class to manage variable scope
class Environment : public std::enable_shared_from_this<Environment> {

private:
	std::map<std::string, std::any> values;
	std::shared_ptr<Environment> enclosing;
public:
	// Constructors
	Environment() : enclosing(nullptr) {}

	explicit Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

	// Get a variable from the environment
	std::any get(const Token& name);

	// Assign a value to an existing variable
	void assign(const Token& name, const std::any& value);

	// Define a new variable in the current scope
	void define(const std::string& name, const std::any& value);

	// Get the ancestor environment at a given distance
	std::shared_ptr<Environment> ancestor(int distance);

	// Get a variable from an ancestor at a given distance
	std::any getAt(int distance, const std::string& name);

	// Assign a value to a variable at a given distance
	void assignAt(int distance, const Token& name, const std::any& value);
	
};


