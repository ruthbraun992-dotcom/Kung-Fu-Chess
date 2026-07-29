#ifndef PARSE_ERROR_H
#define PARSE_ERROR_H

#include <stdexcept>
#include <string>
class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& code)
        : std::runtime_error(code), code_(code) {}

    const std::string& code() const { return code_; }

private:
    std::string code_;
};

#endif // PARSE_ERROR_H