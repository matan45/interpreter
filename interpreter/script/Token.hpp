#pragma once

#include <string>
#include <variant>
#include <iostream>
#include <any>

#include "TokenType.hpp"

class Token {
public:
	TokenType type;
	std::string lexeme;
	std::any literal; // Can hold a string, a double, or nothing.
	int line;

	Token(TokenType type, std::string lexeme, std::any literal,
		int line)
		: type{ type }, lexeme{ std::move(lexeme) },
		literal{ std::move(literal) }, line{ line }
	{}

	Token(const std::string& lexeme, int line) : lexeme(lexeme), line(line) {}


	std::string toString() const {
		std::string literal_text;

		switch (type) {
		case (TokenType::IDENTIFIER):
			literal_text = lexeme;
			break;
		case (TokenType::STRING):
			literal_text = std::any_cast<std::string>(literal);
			break;
		case (TokenType::NUMBER):
			literal_text = std::to_string(std::any_cast<double>(literal));
			break;
		case (TokenType::TRUE):
			literal_text = "true";
			break;
		case (TokenType::FALSE):
			literal_text = "false";
			break;
		default:
			literal_text = "nil";
		}

		return toStringHelper(type) + " " + lexeme + " " + literal_text;
	}

private:
	inline std::string toStringHelper(TokenType type) const {
		static const std::string strings[] = {
			"LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
			"COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",
			"BANG", "BANG_EQUAL", "EQUAL", "EQUAL_EQUAL",
			"GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL",
			"IDENTIFIER", "STRING", "NUMBER", "AND", "CLASS", "ELSE",
			"FALSE", "FUN", "FOR", "IF", "NIL", "OR",
			"PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",
			"END_OF_FILE"
		};

		return strings[static_cast<int>(type)];
	}
};
