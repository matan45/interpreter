#include "Environment.hpp"

std::variant<std::monostate, double, std::string> Environment::get(const Token& name)
{
	auto it = values.find(name.lexeme);
	if (it != values.end()) {
		return it->second;
	}

	if (enclosing != nullptr) {
		return enclosing->get(name);
	}

	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(const Token& name, const std::variant<std::monostate, double, std::string>& value)
{
	auto it = values.find(name.lexeme);
	if (it != values.end()) {
		values[name.lexeme] = value;
		return;
	}

	if (enclosing != nullptr) {
		enclosing->assign(name, value);
		return;
	}

	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::define(const std::string& name, const std::variant<std::monostate, double, std::string>& value)
{
	values[name] = value;
}

std::shared_ptr<Environment> Environment::ancestor(int distance)
{
	std::shared_ptr<Environment> environment = shared_from_this();
	for (int i = 0; i < distance; ++i) {
		if (environment->enclosing != nullptr) {
			environment = environment->enclosing;
		}
		else {
			throw std::runtime_error("Invalid environment chain distance.");
		}
	}
	return environment;
}

std::variant<std::monostate, double, std::string> Environment::getAt(int distance, const std::string& name)
{
	return ancestor(distance)->values[name];
}

void Environment::assignAt(int distance, const Token& name, const std::variant<std::monostate, double, std::string>& value)
{
	ancestor(distance)->values[name.lexeme] = value;
}

std::string Environment::toString() const
{
	std::string result = "{";
	for (const auto& pair : values) {
		result += pair.first + ": ";
		result += std::visit([](auto&& arg) -> std::string {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, std::monostate>) {
				return "nil";
			}
			else if constexpr (std::is_same_v<T, double>) {
				return std::to_string(arg);
			}
			else if constexpr (std::is_same_v<T, std::string>) {
				return arg;
			}
			}, pair.second);
		result += ", ";
	}
	result += "}";

	if (enclosing != nullptr) {
		result += " -> " + enclosing->toString();
	}

	return result;
}

