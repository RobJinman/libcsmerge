#ifndef __CHARSTRINGS_H__
#define __CHARSTRINGS_H__


#include <stdexcept>
#include <vector>
#include "Exception.hpp"
#include "Geometry.hpp"


namespace csmerge {


enum CsTokenType_t {
    PS_OPERATOR = 0,
    PS_OPERAND = 1
};


struct CsToken {
    CsToken(int num);
    CsToken(const char* str);
    CsToken(const std::string& str);
    CsToken(const CsToken& cpy);

    bool operator==(const CsToken& rhs) const;
    bool operator!=(const CsToken& rhs) const;

    CsTokenType_t type;
    int num;
    std::string str;
};


typedef std::vector<CsToken> Charstring;


std::ostream& operator<<(std::ostream& out, const CsToken& tok);
std::ostream& operator<<(std::ostream& out, const Charstring& cs);


class ParseError : public CsMergeException {
    public:
        ParseError()
            : CsMergeException("Parse error") {}

        ParseError(const std::string& msg)
            : CsMergeException(msg) {}
};


class UnrecognisedToken : public ParseError {
    public:
        UnrecognisedToken(const CsToken& token);

        virtual const char* what() const noexcept override;
        const CsToken& getToken() const noexcept;

    private:
        CsToken m_token;
};


class WrongNumberOfArguments : public ParseError {
    public:
        WrongNumberOfArguments(const std::string& msg, const std::string& tokenName, int numArgs);
        WrongNumberOfArguments(const std::string& tokenName, int numArgs);

        virtual const char* what() const noexcept override;
        const std::string& getTokenName() const noexcept;
        int getNumArgs() const noexcept;

    private:
        std::string m_tokenName;
        int m_numArgs;
};


class NotImplementedException : public ParseError {
    using ParseError::ParseError;
};


Charstring mergeCharstrings(const Charstring& cs1, const Charstring& cs2);
geometry::PathList parseCharstring(const Charstring& charstring);
Charstring generateCharstring(const geometry::PathList& paths);


}


#endif
