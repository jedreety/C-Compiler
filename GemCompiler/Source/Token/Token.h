#pragma once
#include <string>
#include <string_view>
#include <variant>
#include <optional>
#include <vector>
#include <sstream>
#include <iostream>
#include <cctype>

namespace GC {

    //------------------------------------------------------------------------------
    // 1. Token Location
    //------------------------------------------------------------------------------
    struct TokenLocation {
        int line{ 1 };
        int column{ 1 };

        friend std::ostream& operator<<(std::ostream& os, const TokenLocation& loc) {
            os << loc.line << ":" << loc.column;
            return os;
        }
    };

    //------------------------------------------------------------------------------
    // 2. Token Literal (for carrying extra parsed data)
    //------------------------------------------------------------------------------
    using TokenLiteral = std::variant<std::monostate, int, double, char, std::string>;

    //------------------------------------------------------------------------------
    // 3. Token Types
    //------------------------------------------------------------------------------
    enum class TokenType {
        // Keywords
        Import,
        Iter,
        While,
        If,
        Else,
        Enum,
        Compare,
        On,
        Default,
        Public,
        Private,
        Continue,
        Break,
        ExitProgram,
        Vec,
        Tuple,
        Destroy,
        Delete,

        // Type Keywords (you can also treat these as identifiers if desired)
        U8, U16, U32, U64,
        I8, I16, I32, I64,
        F32, F64,
        Bool,
        Str,

        // Literals
        IntegerLiteral,
        FloatLiteral,
        StringLiteral,
        CharLiteral,

        // Identifier
        Identifier,

        // Operators
        Plus,           // +
        Minus,          // -
        Multiply,       // *
        Divide,         // /
        Modulo,         // %
        Assign,         // =
        Equal,          // ==
        NotEqual,       // !=
        Less,           // <
        LessEqual,      // <=
        Greater,        // >
        GreaterEqual,   // >=
        // Logical operators: now ?+ for AND and ?? for OR
        And,            // ?+ operator (logical AND)
        Or,             // ?? operator (logical OR)
        Not,            // !
        Arrow,          // ->
        BiArrow,        // <-> 
        Range,          // ..
        DoubleColon,    // ::

        // References:
        Ampersand,      // & for a mutable reference
        AmpersandAt,    // &@ for an immutable reference

        Pipe,           // |

        // Punctuation
        LeftParen,      // (
        RightParen,     // )
        LeftBrace,      // {
        RightBrace,     // }
        LeftBracket,    // [
        RightBracket,   // ]
        Comma,          // ,
        Semicolon,      // ;
        Colon,          // :
        Dot,            // .

        // Special symbols
        At,             // @
        Hash,           // #

        // End-of-File and Unknown
        EndOfFile,
        Unknown
    };

    //------------------------------------------------------------------------------
    // 4. The Token Class
    //------------------------------------------------------------------------------
    class Token {
    public:
        Token(TokenType type, std::string lexeme, TokenLocation location)
            : type_(type)
            , lexeme_(std::move(lexeme))
            , location_(location)
            , literal_(std::monostate{}) {}

        Token(TokenType type, std::string lexeme, TokenLocation location, TokenLiteral literal)
            : type_(type)
            , lexeme_(std::move(lexeme))
            , location_(location)
            , literal_(std::move(literal)) {}

        // Use the compiler-generated copy/move/destructors.
        Token(const Token&) = default;
        Token(Token&&) noexcept = default;
        Token& operator=(const Token&) = default;
        Token& operator=(Token&&) noexcept = default;
        ~Token() = default;

        // Accessors.
        TokenType getType() const noexcept { return type_; }
        const std::string& getLexeme() const noexcept { return lexeme_; }
        const TokenLocation& getLocation() const noexcept { return location_; }

        bool hasLiteral() const noexcept { return !std::holds_alternative<std::monostate>(literal_); }

        template<typename T>
        std::optional<T> getLiteral() const noexcept {
            if (auto ptr = std::get_if<T>(&literal_))
                return *ptr;
            return std::nullopt;
        }

        std::string toString() const {
            std::ostringstream oss;
            oss << "Token(" << tokenTypeToString(type_) << ", \"" << lexeme_
                << "\", at " << location_;
            if (hasLiteral())
                oss << ", literal: " << literalToString();
            oss << ")";
            return oss.str();
        }

    private:
        std::string tokenTypeToString(TokenType type) const {
            switch (type) {
                // Keywords
            case TokenType::Import:       return "import";
            case TokenType::Iter:         return "iter";
            case TokenType::While:        return "while";
            case TokenType::If:           return "if";
            case TokenType::Else:         return "else";
            case TokenType::Enum:         return "enum";
            case TokenType::Compare:      return "compare";
            case TokenType::On:           return "on";
            case TokenType::Default:      return "default";
            case TokenType::Public:       return "public";
            case TokenType::Private:      return "private";
            case TokenType::Continue:     return "continue";
            case TokenType::Break:        return "break";
            case TokenType::ExitProgram:  return "exitProgram";
            case TokenType::Vec:          return "vec";
            case TokenType::Tuple:        return "tuple";
            case TokenType::Destroy:      return "destroy";
            case TokenType::Delete:       return "delete";
                // Type Keywords
            case TokenType::U8:           return "u8";
            case TokenType::U16:          return "u16";
            case TokenType::U32:          return "u32";
            case TokenType::U64:          return "u64";
            case TokenType::I8:           return "i8";
            case TokenType::I16:          return "i16";
            case TokenType::I32:          return "i32";
            case TokenType::I64:          return "i64";
            case TokenType::F32:          return "f32";
            case TokenType::F64:          return "f64";
            case TokenType::Bool:         return "bool";
            case TokenType::Str:          return "str";
                // Literals
            case TokenType::IntegerLiteral: return "integer_literal";
            case TokenType::FloatLiteral:   return "float_literal";
            case TokenType::StringLiteral:  return "string_literal";
            case TokenType::CharLiteral:    return "char_literal";
                // Identifier
            case TokenType::Identifier:     return "identifier";
                // Operators
            case TokenType::Plus:           return "+";
            case TokenType::Minus:          return "-";
            case TokenType::Multiply:       return "*";
            case TokenType::Divide:         return "/";
            case TokenType::Modulo:         return "%";
            case TokenType::Assign:         return "=";
            case TokenType::Equal:          return "==";
            case TokenType::NotEqual:       return "!=";
            case TokenType::Less:           return "<";
            case TokenType::LessEqual:      return "<=";
            case TokenType::Greater:        return ">";
            case TokenType::GreaterEqual:   return ">=";
                // Logical operators
            case TokenType::And:            return "?+";
            case TokenType::Or:             return "??";
            case TokenType::Not:            return "!";
            case TokenType::Arrow:          return "->";
            case TokenType::BiArrow:        return "<->";
            case TokenType::Range:          return "..";
            case TokenType::DoubleColon:    return "::";
                // References
            case TokenType::Ampersand:      return "&";
            case TokenType::AmpersandAt:    return "&@";
            case TokenType::Pipe:           return "|";
                // Punctuation
            case TokenType::LeftParen:      return "(";
            case TokenType::RightParen:     return ")";
            case TokenType::LeftBrace:      return "{";
            case TokenType::RightBrace:     return "}";
            case TokenType::LeftBracket:    return "[";
            case TokenType::RightBracket:   return "]";
            case TokenType::Comma:          return ",";
            case TokenType::Semicolon:      return ";";
            case TokenType::Colon:          return ":";
            case TokenType::Dot:            return ".";
                // Special symbols
            case TokenType::At:             return "@";
            case TokenType::Hash:           return "#";
                // End-of-File and Unknown
            case TokenType::EndOfFile:      return "EOF";
            case TokenType::Unknown:        return "unknown";
            default:                        return "invalid";
            }
        }

        std::string literalToString() const {
            if (std::holds_alternative<int>(literal_))
                return std::to_string(std::get<int>(literal_));
            else if (std::holds_alternative<double>(literal_))
                return std::to_string(std::get<double>(literal_));
            else if (std::holds_alternative<char>(literal_))
                return std::string(1, std::get<char>(literal_));
            else if (std::holds_alternative<std::string>(literal_))
                return std::get<std::string>(literal_);
            return "";
        }

        TokenType type_;
        std::string lexeme_;
        TokenLocation location_;
        TokenLiteral literal_;
    };

    //------------------------------------------------------------------------------
    // 5. The Lexer Class Declaration
    //------------------------------------------------------------------------------
    class Lexer {
    public:
        explicit Lexer(const std::string& source)
            : source_(source), position_(0), location_{ 1, 1 } {}

        std::vector<Token> tokenize();

    private:
        bool isAtEnd() const { return position_ >= source_.length(); }
        char peek() const { return isAtEnd() ? '\0' : source_[position_]; }
        char peekNext() const { return (position_ + 1 >= source_.length()) ? '\0' : source_[position_ + 1]; }
        char peekNextN(size_t n) const { return (position_ + n >= source_.length()) ? '\0' : source_[position_ + n]; }
        char advance();
        void skipWhitespaceAndComments();
        void skipLineComment();
        void skipBlockComment();
        Token scanToken();
        Token identifierOrKeyword();
        Token number();
        Token stringLiteral();
        Token charLiteral();

        const std::string& source_;
        size_t position_;
        TokenLocation location_;
    };

    // A convenience function.
    std::vector<Token> Tokenize(const std::string& source);
}
