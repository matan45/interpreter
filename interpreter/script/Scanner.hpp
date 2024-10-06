#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <cctype>
#include "Token.hpp"

class Scanner
{
private:
	std::string source;
	std::vector<Token> tokens;
	int start = 0;
	int current = 0;
	int line = 1;

	std::unordered_map<std::string, TokenType> keywords;

public:
	explicit Scanner(const std::string& source) : source(source) {
		keywords = createKeywordMap();
	}

	std::vector<Token> scanTokens();
private:

	std::unordered_map<std::string, TokenType> createKeywordMap();

	bool isAtEnd() const;

	char advance();

	bool match(char expected);

	char peek() const;

	char peekNext() const;

	void scanToken();

	void identifier();

	void number();

	void string();

	bool isAlpha(char c) const;

	bool isAlphaNumeric(char c) const;

	bool isDigit(char c) const;

	void addToken(TokenType type);

	void addToken(TokenType type, const std::string& literal);
};


