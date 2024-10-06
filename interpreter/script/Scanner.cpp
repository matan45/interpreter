#include "Scanner.hpp"


std::vector<Token> Scanner::scanTokens()
{
	while (!isAtEnd()) {
		start = current;
		scanToken();
	}
	tokens.emplace_back(TokenType::EOF_TYPE, "", "", line);
	return tokens;
}

std::unordered_map<std::string, TokenType> Scanner::createKeywordMap()
{
	return {
		{"and", TokenType::AND},
		{"class", TokenType::CLASS},
		{"else", TokenType::ELSE},
		{"false", TokenType::FALSE},
		{"for", TokenType::FOR},
		{"fun", TokenType::FUN},
		{"if", TokenType::IF},
		{"nil", TokenType::NIL},
		{"or", TokenType::OR},
		{"print", TokenType::PRINT},
		{"return", TokenType::RETURN},
		{"super", TokenType::SUPER},
		{"this", TokenType::THIS},
		{"true", TokenType::TRUE},
		{"var", TokenType::VAR},
		{"while", TokenType::WHILE}
	};

}

bool Scanner::isAtEnd() const
{
	return current >= source.length();
}

char Scanner::advance()
{
	return source[current++];
}

bool Scanner::match(char expected)
{
	if (isAtEnd() || source[current] != expected) return false;
	current++;
	return true;
}

char Scanner::peek() const
{
	return isAtEnd() ? '\0' : source[current];
}

char Scanner::peekNext() const
{
	if (current + 1 >= source.length()) return '\0';
	return source[current + 1];
}

void Scanner::scanToken()
{

	char c = advance();
	switch (c) {
	case '(': addToken(TokenType::LEFT_PAREN); break;
	case ')': addToken(TokenType::RIGHT_PAREN); break;
	case '{': addToken(TokenType::LEFT_BRACE); break;
	case '}': addToken(TokenType::RIGHT_BRACE); break;
	case ',': addToken(TokenType::COMMA); break;
	case '.': addToken(TokenType::DOT); break;
	case '-': addToken(TokenType::MINUS); break;
	case '+': addToken(TokenType::PLUS); break;
	case ';': addToken(TokenType::SEMICOLON); break;
	case '*': addToken(TokenType::STAR); break;

	case '!':
		addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
		break;
	case '=':
		addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
		break;
	case '<':
		addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
		break;
	case '>':
		addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
		break;

	case '/':
		if (match('/')) {
			while (peek() != '\n' && !isAtEnd()) advance();
		}
		else {
			addToken(TokenType::SLASH);
		}
		break;

	case ' ':
	case '\r':
	case '\t':
		break;

	case '\n':
		line++;
		break;

	case '"': string(); break;

	default:
		if (isDigit(c)) {
			number();
		}
		else if (isAlpha(c)) {
			identifier();
		}
		else {
			std::cerr << "Unexpected character at line " << line << ".\n";
		}
		break;
	}

}

void Scanner::identifier()
{
	while (isAlphaNumeric(peek())) advance();

	std::string text = source.substr(start, current - start);
	TokenType type = TokenType::IDENTIFIER;

	if (keywords.contains(text)) {
		type = keywords[text];
	}

	addToken(type);
}

void Scanner::number()
{
	while (isDigit(peek())) advance();

	if (peek() == '.' && isDigit(peekNext())) {
		advance(); // Consume '.'

		while (isDigit(peek())) advance();
	}

	addToken(TokenType::NUMBER, source.substr(start, current - start));
}

void Scanner::string()
{
	while (peek() != '"' && !isAtEnd()) {
		if (peek() == '\n') line++;
		advance();
	}

	if (isAtEnd()) {
		std::cerr << "Unterminated string at line " << line << ".\n";
		return;
	}

	advance(); // Consume the closing '"'
	std::string value = source.substr(start + 1, current - start - 2);
	addToken(TokenType::STRING, value);
}

bool Scanner::isAlpha(char c) const
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isAlphaNumeric(char c) const
{
	return isAlpha(c) || isDigit(c);
}

bool Scanner::isDigit(char c) const
{
	return c >= '0' && c <= '9';
}

void Scanner::addToken(TokenType type)
{
	addToken(type, "");
}

void Scanner::addToken(TokenType type, const std::string& literal)
{
	std::string text = source.substr(start, current - start);
	tokens.emplace_back(type, text, literal, line);
}