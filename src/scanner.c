/*
 * MIT License
 * Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
 * See LICENSE file in the root directory for full license text.
*/
#include "scanner.h"

//shared scanner
Scanner scanner;

void scanner_init(C_STR source)
{
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

static bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_';
}

static bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

static bool isAtEnd()
{
	return *scanner.current == '\0';
}

static char advance() {
	scanner.current++;
	return scanner.current[-1];
}

static char peek() {
	return *scanner.current;
}

static char peekNext() {
	if (isAtEnd()) return '\0';
	return scanner.current[1];
}

static bool match(char expected) {
	if (isAtEnd()) return false;
	if (*scanner.current != expected) return false;
	scanner.current++;
	return true;
}

static Token makeToken(TokenType type) {
	return (Token) {
		.type = type,
			.start = scanner.start,
			.length = (uint32_t)(scanner.current - scanner.start),
			.line = scanner.line
	};
}

static Token errorToken(C_STR message) {
	return (Token) {
		.type = TOKEN_ERROR,
			.start = message,
			.length = (uint32_t)strlen(message),
			.line = scanner.line
	};
}

static TokenType skipWhitespace() {
	while (true) {
		char c = peek();
		switch (c) {
		case ' ':
		case '\r':
		case '\t':
			advance();
			break;
		case '\n':
			scanner.line++;
			advance();
			break;
		case '/': {
			switch (peekNext())
			{
			case '/': {
				// A comment goes until the end of the line.
				while (peek() != '\n' && !isAtEnd()) advance();
				break;
			}
			case '*': {
				// A comment goes until */.
				advance();
				while (true) {
					while (peek() != '*' && !isAtEnd()) {
						if (peek() == '\n') scanner.line++;
						advance();
					}
					if (isAtEnd()) {
						return TOKEN_UNTERMINATED_COMMENT;
					}
					else if (peek() == '*') {
						advance();
						if (isAtEnd()) return TOKEN_UNTERMINATED_COMMENT;

						if (peek() == '/') {
							advance();
							break;//jump to outer while
						}
					}
				}
				break;
			}
			default: return TOKEN_ERROR;
			}
			break;
		}
		default:
			return TOKEN_IGNORE;
		}
	}
}

static TokenType checkKeyword(uint32_t start, uint32_t length, C_STR rest, TokenType type) {
	if (scanner.current - scanner.start == start + length &&
		memcmp(scanner.start + start, rest, length) == 0) {
		return type;
	}

	return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
	switch (scanner.start[0]) {
	case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
	case 'b': {
		if ((scanner.current - scanner.start > 2) && (scanner.start[1] == 'r')) {
			switch (scanner.start[2]) {
			case 'e':return checkKeyword(3, 2, "ak", TOKEN_BREAK);
			case 'a':return checkKeyword(3, 3, "nch", TOKEN_BRANCH);
			}
		}
		break;
	}
	case 'c': {
		if (scanner.current - scanner.start > 1) {
			switch (scanner.start[1]) {
			case 'l': return checkKeyword(2, 3, "ass", TOKEN_CLASS);
			case 'o': return checkKeyword(2, 6, "ntinue", TOKEN_CONTINUE);
			}
		}
		break;
	}
	case 'f': {
		if (scanner.current - scanner.start > 1) {
			switch (scanner.start[1]) {
			case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
			case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
			case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
			}
		}
		break;
	}
	case 'n': {
		if (scanner.current - scanner.start > 1) {
			switch (scanner.start[1]) {
			case 'i': return checkKeyword(2, 1, "l", TOKEN_NIL);
			case 'o': return checkKeyword(2, 2, "ne", TOKEN_NONE);
			}
		}
		break;
	}
	case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
	case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
	case 't': {
		if (scanner.current - scanner.start > 1) {
			switch (scanner.start[1]) {
			case 'h':return checkKeyword(2, 2, "is", TOKEN_THIS);
			case 'y': return checkKeyword(2, 4, "peof", TOKEN_TYPE_OF);
			case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
			}
		}
		break;
	}
	case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
	}

	return TOKEN_IDENTIFIER;
}

static TokenType checkModule(uint32_t start, uint32_t length, C_STR rest, TokenType type) {
	if (scanner.current - scanner.start == start + length &&
		memcmp(scanner.start + start, rest, length) == 0) {
		return type;
	}

	return TOKEN_NIL;
}

//convert
static TokenType builtinType() {
	if ((scanner.start[0] == '@') && (scanner.current - scanner.start > 1)) {
		switch (scanner.start[1])
		{
		case 'a':return checkModule(2, 4, "rray", TOKEN_MODULE_ARRAY);
		case 's': {
			if (scanner.current - scanner.start > 2) {
				switch (scanner.start[2])
				{
				case 't':return checkModule(3, 4, "ring", TOKEN_MODULE_STRING);
				case 'y':return checkModule(3, 1, "s", TOKEN_MODULE_SYSTEM);
				}
			}
			break;
		}
		}
	}

	return TOKEN_NIL;
}

static Token mention() {
	while (isAlpha(peek()) || isDigit(peek())) advance();

	TokenType type = builtinType();

	if (type == TOKEN_NIL) {
		return errorToken("Unexpected module (Available modules : @array, @string, @system).");
	}

	return makeToken(type);
}

static Token identifier() {
	while (isAlpha(peek()) || isDigit(peek())) advance();
	return makeToken(identifierType());
}

static bool isBinDigit(char c) {
	return c == '0' || c == '1';
}

static bool isHexDigit(char c) {
	return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static Token number() {
	switch (peek())
	{
	case 'b':
	case 'B':
		advance(); // Skip '0'
		advance(); // Skip 'b' or 'B'

		if (!isBinDigit(peek())) {
			return errorToken("Invalid bin number format.");
		}

		do {
			advance();
		} while (isBinDigit(peek()));

		return makeToken(TOKEN_NUMBER_BIN);
	case 'x':
	case 'X':
		advance(); // Skip '0'
		advance(); // Skip 'x' or 'X'

		if (!isHexDigit(peek())) {
			return errorToken("Invalid hex number format.");
		}

		do {
			advance();
		} while (isHexDigit(peek()));

		return makeToken(TOKEN_NUMBER_HEX);
	default:
		break;
	}

	// Parse the number based on its base.
	// Decimal number.
	while (isDigit(peek())) {
		advance();
	}

	// Look for a fractional part.
	if (peek() == '.' && isDigit(peekNext())) {
		// Consume the ".".
		advance();

		while (isDigit(peek())) {
			advance();
		}
	}

	// Look for scientific notation (e or E).
	if (peek() == 'e' || peek() == 'E') {
		// Consume the 'e' or 'E'.
		advance();

		// Check for an optional sign (+ or -) in the exponent.
		if (peek() == '+' || peek() == '-') {
			advance(); // Consume the sign.
		}

		// Ensure there are digits in the exponent.
		if (!isDigit(peek())) {
			return errorToken("Expected digit after 'e' or 'E'.");
		}

		// Parse the exponent digits.
		while (isDigit(peek())) {
			advance();
		}
	}

	return makeToken(TOKEN_NUMBER);
}

static Token string() {
	bool isEscapeString = false;

	for (char c = '\0'; (c = peek()) != '"' && !isAtEnd(); advance()) {
		switch (c)
		{
		case '\n':
			scanner.line++;
			break;
		case '\\': {
			if (isAtEnd()) return errorToken("Unterminated string.");

			advance(); // skip "\\"

			if (!isEscapeString) {
				switch (peek()) {
				case '"':  // '\"'
				case '\\': // '\\'
					isEscapeString = true;
				default:
					break;
				}
			}
		}
		default:
			break;
		}
	}

	if (isAtEnd()) return errorToken("Unterminated string.");

	// The closing quote.
	advance();
	return makeToken(isEscapeString ? TOKEN_STRING_ESCAPE : TOKEN_STRING);
}

Token scanToken()
{
	//we don't need this
	switch (skipWhitespace())
	{
	case TOKEN_UNTERMINATED_COMMENT:
		return errorToken("Expect '*/' after comment.");
	default:
		break;
	}

	scanner.start = scanner.current;

	if (isAtEnd()) return makeToken(TOKEN_EOF);

	char c = advance();

	if (isAlpha(c)) return identifier();
	if (isDigit(c)) return number();

	switch (c) {
	case '(': return makeToken(TOKEN_LEFT_PAREN);
	case ')': return makeToken(TOKEN_RIGHT_PAREN);
	case '{': return makeToken(TOKEN_LEFT_BRACE);
	case '}': return makeToken(TOKEN_RIGHT_BRACE);
	case '[': return makeToken(TOKEN_LEFT_SQUARE_BRACKET);
	case ']': return makeToken(TOKEN_RIGHT_SQUARE_BRACKET);
	case ';': return makeToken(TOKEN_SEMICOLON);
	case ':': return makeToken(TOKEN_COLON);
	case ',': return makeToken(TOKEN_COMMA);
	case '.': return makeToken(TOKEN_DOT);
	case '-': return makeToken(TOKEN_MINUS);
	case '+': return makeToken(TOKEN_PLUS);
	case '/': return makeToken(TOKEN_SLASH);
	case '*': return makeToken(TOKEN_STAR);
	case '%': return makeToken(TOKEN_PERCENT);
	case '@':
		return isAlpha(peek()) ? mention() : errorToken("Expected module name after @.");
	case '!':
		return makeToken(
			match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
	case '=':
		return makeToken(
			match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
	case '<': {
		if (!match('<')) {
			return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		}
		else {
			return makeToken(TOKEN_BIT_SHL);
		}
	}
	case '>': {
		if (!match('>')) {
			return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
		}
		else {
			return makeToken(match('>') ? TOKEN_BIT_SHR : TOKEN_BIT_SAR);
		}
	}
	case '&': return makeToken(TOKEN_BIT_AND);
	case '|': return makeToken(TOKEN_BIT_OR);
	case '~': return makeToken(TOKEN_BIT_NOT);
	case '^': return makeToken(TOKEN_BIT_XOR);
	case '"': return string();
	}

	if (!isAtEnd()) {
		advance();//stop deadly looping
		return errorToken("Unexpected character. ");
	}
	else {
		return makeToken(TOKEN_EOF);
	}
}