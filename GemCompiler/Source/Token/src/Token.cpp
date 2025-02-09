#include "../Token.h"

namespace GC {

    //------------------------------------------------------------------------------
    // Utility: Advance the current position and update location.
    //------------------------------------------------------------------------------
    char Lexer::advance() {
        char c = source_[position_++];
        if (c == '\n') {
            location_.line++;
            location_.column = 1;
        }
        else {
            location_.column++;
        }
        return c;
    }

    //------------------------------------------------------------------------------
    // Skip a single-line comment starting with "||"
    //------------------------------------------------------------------------------
    void Lexer::skipLineComment() {
        while (!isAtEnd() && peek() != '\n')
            advance();
    }

    //------------------------------------------------------------------------------
    // Skip a block comment delimited by "|--" and "--|"
    //------------------------------------------------------------------------------
    void Lexer::skipBlockComment() {
        // We assume the initial "|--" has already been consumed.
        while (!isAtEnd()) {
            // Look for the end marker "--|"
            if (peek() == '-' && peekNext() == '-' && peekNextN(2) == '|') {
                // Consume the "--|"
                advance(); // '-'
                advance(); // '-'
                advance(); // '|'
                break;
            }
            advance();
        }
    }

    //------------------------------------------------------------------------------
    // Skip whitespace and comments.
    //------------------------------------------------------------------------------
    void Lexer::skipWhitespaceAndComments() {
        while (!isAtEnd()) {
            char c = peek();
            if (std::isspace(c)) {
                advance();
            }
            // Check for line comment marker "||"
            else if (c == '|' && peekNext() == '|') {
                // Consume "||" and then skip until end-of-line.
                advance(); // '|'
                advance(); // '|'
                skipLineComment();
            }
            // Check for block comment marker "|--"
            else if (c == '|' && peekNext() == '-' && peekNextN(2) == '-') {
                // Consume "|--"
                advance(); // '|'
                advance(); // '-'
                advance(); // '-'
                skipBlockComment();
            }
            else {
                break;
            }
        }
    }

    //------------------------------------------------------------------------------
    // Tokenize the entire source into a vector of tokens.
    //------------------------------------------------------------------------------
    std::vector<Token> Lexer::tokenize() {
        std::vector<Token> tokens;
        while (!isAtEnd()) {
            skipWhitespaceAndComments();
            if (isAtEnd())
                break;
            tokens.push_back(scanToken());
        }
        tokens.push_back(Token(TokenType::EndOfFile, "", location_));
        return tokens;
    }

    //------------------------------------------------------------------------------
    // scanToken: Reads the next token from the source.
    //------------------------------------------------------------------------------
    Token Lexer::scanToken() {
        skipWhitespaceAndComments(); // In case there’s trailing whitespace
        TokenLocation tokenStart = location_;
        char c = advance();

        // Handle single or multi-character tokens.
        switch (c) {
        case '+': return Token(TokenType::Plus, "+", tokenStart);
        case '-':
            if (!isAtEnd() && peek() == '>') {
                advance();
                return Token(TokenType::Arrow, "->", tokenStart);
            }
            return Token(TokenType::Minus, "-", tokenStart);
        case '*': return Token(TokenType::Multiply, "*", tokenStart);
        case '/': return Token(TokenType::Divide, "/", tokenStart);
        case '%': return Token(TokenType::Modulo, "%", tokenStart);
        case '=':
            if (!isAtEnd() && peek() == '=') {
                advance();
                return Token(TokenType::Equal, "==", tokenStart);
            }
            return Token(TokenType::Assign, "=", tokenStart);
        case '!':
            if (!isAtEnd() && peek() == '=') {
                advance();
                return Token(TokenType::NotEqual, "!=", tokenStart);
            }
            return Token(TokenType::Not, "!", tokenStart);
        case '<': {
            if (!isAtEnd() && peek() == '=') {
                advance();
                return Token(TokenType::LessEqual, "<=", tokenStart);
            }
            else if (!isAtEnd() && peek() == '-') {
                // Possible BiArrow "<->"
                advance(); // consume '-'
                if (!isAtEnd() && peek() == '>') {
                    advance();
                    return Token(TokenType::BiArrow, "<->", tokenStart);
                }
                else {
                    return Token(TokenType::Less, "<", tokenStart);
                }
            }
            return Token(TokenType::Less, "<", tokenStart);
        }
        case '>': {
            if (!isAtEnd() && peek() == '=') {
                advance();
                return Token(TokenType::GreaterEqual, ">=", tokenStart);
            }
            return Token(TokenType::Greater, ">", tokenStart);
        }
                // Logical operators starting with '?'
        case '?': {
            if (!isAtEnd()) {
                char next = peek();
                if (next == '+') {
                    advance();
                    return Token(TokenType::And, "?+", tokenStart);
                }
                else if (next == '?') {
                    advance();
                    return Token(TokenType::Or, "??", tokenStart);
                }
            }
            return Token(TokenType::Unknown, "?", tokenStart);
        }
        case '&': {
            // Now, check if this is a reference operator for an immutable reference.
            if (!isAtEnd() && peek() == '@') {
                advance(); // consume '@'
                return Token(TokenType::AmpersandAt, "&@", tokenStart);
            }
            return Token(TokenType::Ampersand, "&", tokenStart);
        }
        case '|': {
            return Token(TokenType::Pipe, "|", tokenStart);
        }
        case ':': {
            if (!isAtEnd() && peek() == ':') {
                advance();
                return Token(TokenType::DoubleColon, "::", tokenStart);
            }
            return Token(TokenType::Colon, ":", tokenStart);
        }
        case '.': {
            if (!isAtEnd() && peek() == '.') {
                advance();
                return Token(TokenType::Range, "..", tokenStart);
            }
            return Token(TokenType::Dot, ".", tokenStart);
        }
        case '(': return Token(TokenType::LeftParen, "(", tokenStart);
        case ')': return Token(TokenType::RightParen, ")", tokenStart);
        case '{': return Token(TokenType::LeftBrace, "{", tokenStart);
        case '}': return Token(TokenType::RightBrace, "}", tokenStart);
        case '[': return Token(TokenType::LeftBracket, "[", tokenStart);
        case ']': return Token(TokenType::RightBracket, "]", tokenStart);
        case ',': return Token(TokenType::Comma, ",", tokenStart);
        case ';': return Token(TokenType::Semicolon, ";", tokenStart);
        case '@': return Token(TokenType::At, "@", tokenStart);
        case '#': return Token(TokenType::Hash, "#", tokenStart);
        default:
            break;
        }

        // Number literal: digits (with optional fractional part).
        if (std::isdigit(c)) {
            std::string numberStr;
            numberStr.push_back(c);
            bool isFloat = false;
            while (!isAtEnd() && std::isdigit(peek()))
                numberStr.push_back(advance());
            if (!isAtEnd() && peek() == '.') {
                if (peekNext() != '.') {
                    isFloat = true;
                    numberStr.push_back(advance()); // consume the dot
                    while (!isAtEnd() && std::isdigit(peek()))
                        numberStr.push_back(advance());
                }
            }
            return isFloat
                ? Token(TokenType::FloatLiteral, numberStr, tokenStart)
                : Token(TokenType::IntegerLiteral, numberStr, tokenStart);
        }

        // String literal.
        if (c == '"')
            return stringLiteral();

        // Char literal.
        if (c == '\'')
            return charLiteral();

        // Identifier or keyword. Allow letters, digits, underscores, and also '@' or '#' in the beginning.
        if (std::isalpha(c) || c == '_' || c == '@' || c == '#') {
            std::string ident;
            ident.push_back(c);
            while (!isAtEnd() &&
                (std::isalnum(peek()) || peek() == '_' || peek() == '@' || peek() == '#'))
            {
                ident.push_back(advance());
            }
            // Check for reserved keywords.
            if (ident == "import")       return Token(TokenType::Import, ident, tokenStart);
            if (ident == "iter")         return Token(TokenType::Iter, ident, tokenStart);
            if (ident == "while")        return Token(TokenType::While, ident, tokenStart);
            if (ident == "if")           return Token(TokenType::If, ident, tokenStart);
            if (ident == "else")         return Token(TokenType::Else, ident, tokenStart);
            if (ident == "enum")         return Token(TokenType::Enum, ident, tokenStart);
            if (ident == "compare")      return Token(TokenType::Compare, ident, tokenStart);
            if (ident == "on")           return Token(TokenType::On, ident, tokenStart);
            if (ident == "default")      return Token(TokenType::Default, ident, tokenStart);
            if (ident == "public")       return Token(TokenType::Public, ident, tokenStart);
            if (ident == "private")      return Token(TokenType::Private, ident, tokenStart);
            if (ident == "continue")     return Token(TokenType::Continue, ident, tokenStart);
            if (ident == "break")        return Token(TokenType::Break, ident, tokenStart);
            if (ident == "exitProgram")  return Token(TokenType::ExitProgram, ident, tokenStart);
            if (ident == "vec")          return Token(TokenType::Vec, ident, tokenStart);
            if (ident == "tuple")        return Token(TokenType::Tuple, ident, tokenStart);
            if (ident == "destroy")      return Token(TokenType::Destroy, ident, tokenStart);
            if (ident == "delete")       return Token(TokenType::Delete, ident, tokenStart);
            // Type keywords.
            if (ident == "u8")           return Token(TokenType::U8, ident, tokenStart);
            if (ident == "u16")          return Token(TokenType::U16, ident, tokenStart);
            if (ident == "u32")          return Token(TokenType::U32, ident, tokenStart);
            if (ident == "u64")          return Token(TokenType::U64, ident, tokenStart);
            if (ident == "i8")           return Token(TokenType::I8, ident, tokenStart);
            if (ident == "i16")          return Token(TokenType::I16, ident, tokenStart);
            if (ident == "i32")          return Token(TokenType::I32, ident, tokenStart);
            if (ident == "i64")          return Token(TokenType::I64, ident, tokenStart);
            if (ident == "f32")          return Token(TokenType::F32, ident, tokenStart);
            if (ident == "f64")          return Token(TokenType::F64, ident, tokenStart);
            if (ident == "bool")         return Token(TokenType::Bool, ident, tokenStart);
            if (ident == "str")          return Token(TokenType::Str, ident, tokenStart);
            return Token(TokenType::Identifier, ident, tokenStart);
        }

        // If we haven’t recognized the character, return an unknown token.
        return Token(TokenType::Unknown, std::string(1, c), tokenStart);
    }

    //------------------------------------------------------------------------------
    // stringLiteral: Reads a string literal (allowing for simple escape sequences).
    //------------------------------------------------------------------------------
    Token Lexer::stringLiteral() {
        TokenLocation tokenStart = location_;
        std::string str;
        while (!isAtEnd() && peek() != '"') {
            if (peek() == '\\') {
                advance(); // skip the backslash
                if (!isAtEnd()) {
                    char escaped = advance();
                    str.push_back(escaped);
                }
            }
            else {
                str.push_back(advance());
            }
        }
        if (!isAtEnd())
            advance(); // consume closing quote
        return Token(TokenType::StringLiteral, str, tokenStart);
    }

    //------------------------------------------------------------------------------
    // charLiteral: Reads a character literal (allowing for simple escape sequences).
    //------------------------------------------------------------------------------
    Token Lexer::charLiteral() {
        TokenLocation tokenStart = location_;
        std::string str;
        while (!isAtEnd() && peek() != '\'') {
            if (peek() == '\\') {
                advance();
                if (!isAtEnd()) {
                    char escaped = advance();
                    str.push_back(escaped);
                }
            }
            else {
                str.push_back(advance());
            }
        }
        if (!isAtEnd())
            advance(); // consume closing quote
        return Token(TokenType::CharLiteral, str, tokenStart);
    }

    //------------------------------------------------------------------------------
    // Convenience function.
    //------------------------------------------------------------------------------
    std::vector<Token> Tokenize(const std::string& source) {
        Lexer lexer(source);
        return lexer.tokenize();
    }

} // namespace GC
