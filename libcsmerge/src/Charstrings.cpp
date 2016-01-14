#include <cassert>
#include <deque>
#include <list>
#include <algorithm>
#include <sstream>
#include "Geometry.hpp"
#include "Charstrings.hpp"
#include "Util.hpp"


using std::string;


namespace csmerge {


using namespace geometry;


typedef std::deque<CsToken> Stack;
typedef std::vector<CsToken> TokenList;


CsToken::CsToken(int num)
    : type(PS_OPERAND), num(num) {}

CsToken::CsToken(const char* str)
    : type(PS_OPERATOR), str(str) {}

CsToken::CsToken(const string& str)
    : type(PS_OPERATOR), str(str) {}

CsToken::CsToken(const CsToken& cpy)
    : type(cpy.type), num(cpy.num), str(cpy.str) {}

bool CsToken::operator==(const CsToken& rhs) const {
    if (type != rhs.type) {
        return false;
    }

    switch (type) {
        case PS_OPERATOR: return str == rhs.str;
        case PS_OPERAND: return num == rhs.num;
        default: assert(false);
    }
}

bool CsToken::operator!=(const CsToken& rhs) const {
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& out, const CsToken& tok) {
    if (tok.type == PS_OPERATOR) {
        out << tok.str;
    }
    else {
        out << tok.num;
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, const Charstring& cs) {
    out << "Charstring [";

    for (int i = 0; i < cs.size(); ++i) {
        out << cs[i];
        if (i + 1 < cs.size()) {
            out << ", ";
        }
    }

    out << "]\n";

    return out;
}


UnrecognisedToken::UnrecognisedToken(const CsToken& token)
    : ParseError("Unrecognised token"), m_token(token) {}

const char* UnrecognisedToken::what() const noexcept {
    static string msg;

    std::stringstream ss;
    ss << ParseError::what()
       << "(Token: str='" << m_token.str << "', num=" << m_token.num << ")";

    msg = ss.str();

    return msg.c_str();
}

const CsToken& UnrecognisedToken::getToken() const noexcept {
    return m_token;
}


WrongNumberOfArguments::WrongNumberOfArguments(const string& msg,
    const string& tokenName, int numArgs)
    : ParseError("Wrong number of arguments; " + msg),
      m_tokenName(tokenName),
      m_numArgs(numArgs) {}

WrongNumberOfArguments::WrongNumberOfArguments(const string& tokenName, int numArgs)
    : ParseError("Wrong number of arguments"),
      m_tokenName(tokenName),
      m_numArgs(numArgs) {}

const char* WrongNumberOfArguments::what() const noexcept {
    static string msg;

    std::stringstream ss;
    ss << ParseError::what()
       << "(Token: '" << m_tokenName << "', Num args found: " << m_numArgs << ")";

    msg = ss.str();

    return msg.c_str();
}

const string& WrongNumberOfArguments::getTokenName() const noexcept {
    return m_tokenName;
}

int WrongNumberOfArguments::getNumArgs() const noexcept {
    return m_numArgs;
}


static ParseError makeParseError(const string& msg, const string& tokName, int tokIdx,
    const Stack& stack, const CsMergeException* pEx) {

    std::stringstream ss;

    if (msg.size() > 0) {
        ss << msg << " ";
    }
    else {
        ss << "Parse error ";
    }

    ss << "(Token index: " << tokIdx << ", Token name: '" << tokName << "', Stack: [";
    for (unsigned int i = 0; i < stack.size(); ++i) {
        if (stack[i].type == PS_OPERATOR) {
            ss << stack[i].str;
        }
        else {
            ss << stack[i].num;
        }

        if (i + 1 < stack.size()) {
            ss << ", ";
        }
    }

    ss << "])";

    if (pEx != nullptr) {
        ss << "; " << pEx->what();
    }

    return ParseError(ss.str());
}

static ParseError makeParseError(const string& msg, const string& tokName, int tokIdx,
    const Stack& stack, const CsMergeException& ex) {

    return makeParseError(msg, tokName, tokIdx, stack, &ex);
}

static ParseError makeParseError(const string& tokName, int tokIdx, const Stack& stack,
    const CsMergeException& ex) {

    return makeParseError("", tokName, tokIdx, stack, ex);
}

static ParseError makeParseError(const string& msg, const string& tokName, int tokIdx,
    const Stack& stack) {

    return makeParseError(msg, tokName, tokIdx, stack, nullptr);
}

static ParseError makeParseError(const string& tokName, int tokIdx, const Stack& stack) {
    return makeParseError("", tokName, tokIdx, stack, nullptr);
}

static ParseError makeParseError(const string& msg) {
    return ParseError(msg);
}


static TokenList getArgs(Stack& stack) {
    TokenList vec;

    for (int i = static_cast<int>(stack.size() - 1); i >= 0; --i) {
        if (stack[i].type == PS_OPERAND) {
            vec.push_back(stack[i]);
            stack.pop_back();
        }
        else {
            break;
        }
    }

    std::reverse(vec.begin(), vec.end());
    return vec;
}

static void process(PathList& paths, Point& cursor, Stack& stack, const CsToken op) {
    assert(op.type == PS_OPERATOR);

    TokenList args = getArgs(stack);
    int nargs = args.size();

    if (paths.size() == 0) {
        paths.push_back(Path());
    }

    Path* path = &paths.back();
    Point z = path->finalPoint();

    if (op.str == "rmoveto") {
        if (nargs != 2) {
            throw WrongNumberOfArguments("rmoveto", nargs);
        }

        int x = args[0].num;
        int y = args[1].num;

        if (!path->empty()) {
            DBG_OUT("Starting new empty path\n"); 

            path->close();
            paths.push_back(Path());
            path = &paths.back();
        }

        DBG_OUT("Moving cursor from " << cursor << " to " << cursor + Point(x, y) << "\n"); 
        cursor += Point(x, y);
    }
    else if (op.str == "hmoveto") {
        if (nargs != 1) {
            throw WrongNumberOfArguments("hmoveto", nargs);
        }

        int x = args[0].num;

        if (!path->empty()) {
            DBG_OUT("Starting new empty path\n"); 

            path->close();
            paths.push_back(Path());
            path = &paths.back();
        }

        DBG_OUT("Moving cursor from " << cursor << " to " << cursor + Point(x, 0) << "\n"); 
        cursor += Point(x, 0);
    }
    else if (op.str == "vmoveto") {
        if (nargs != 1) {
            throw WrongNumberOfArguments("vmoveto", nargs);
        }

        int y = args[0].num;

        if (!path->empty()) {
            DBG_OUT("Starting new empty path\n"); 

            path->close();
            paths.push_back(Path());
            path = &paths.back();
        }

        DBG_OUT("Moving cursor from " << cursor << " to " << cursor + Point(0, y) << "\n"); 
        cursor += Point(0, y);
    }
    else if (op.str == "rlineto") {
        if (nargs % 2 != 0) {
            throw WrongNumberOfArguments("rlineto", nargs);
        }

        for (int i = 0; i < nargs; i += 2) {
            int x = args[i].num;
            int y = args[i + 1].num;

            const Point& A = cursor;
            Point B = cursor + Point(x, y);

            LineSegment lseg(A, B);

            DBG_OUT("Appending line segment: " << lseg << "\n"); 
            path->append(lseg);

            DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
            cursor = path->finalPoint();
        }
    }
    else if (op.str == "hlineto") {
        for (int i = 0; i < nargs; ++i) {
            const Point& A = cursor;

            if (i % 2 == 0) {
                int x = args[i].num;
                Point B = cursor + Point(x, 0);

                LineSegment lseg(A, B);

                DBG_OUT("Appending line segment: " << lseg << "\n"); 
                path->append(lseg);

                DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                cursor = path->finalPoint();
            }
            else {
                int y = args[i].num;
                Point B = cursor + Point(0, y);

                LineSegment lseg(A, B);

                DBG_OUT("Appending line segment: " << lseg << "\n"); 
                path->append(lseg);

                DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                cursor = path->finalPoint();
            }
        }
    }
    else if (op.str == "vlineto") {
        for (int i = 0; i < nargs; ++i) {
            const Point& A = cursor;

            if (i % 2 == 0) {
                int y = args[i].num;
                Point B = cursor + Point(0, y);

                LineSegment lseg(A, B);

                DBG_OUT("Appending line segment: " << lseg << "\n"); 
                path->append(lseg);

                DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                cursor = path->finalPoint();
            }
            else {
                int x = args[i].num;
                Point B = cursor + Point(x, 0);

                LineSegment lseg(A, B);

                DBG_OUT("Appending line segment: " << lseg << "\n"); 
                path->append(lseg);

                DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                cursor = path->finalPoint();
            }
        }
    }
    else if (op.str == "rrcurveto") {
        if (nargs % 6 != 0) {
            throw WrongNumberOfArguments("rrcurveto", nargs);
        }

        for (int i = 0; i < nargs; i += 6) {
            int bx = args[i].num;
            int by = args[i + 1].num;
            int cx = args[i + 2].num;
            int cy = args[i + 3].num;
            int dx = args[i + 4].num;
            int dy = args[i + 5].num;

            const Point& A = cursor;
            Point B = A + Point(bx, by);
            Point C = B + Point(cx, cy);
            Point D = C + Point(dx, dy);

            CubicBezier bezier(A, B, C, D);

            DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
            path->append(bezier);

            DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
            cursor = path->finalPoint();
        }
    }
    else if (op.str == "hhcurveto") {
        if (nargs % 4 != 0 && nargs % 4 != 1) {
            throw WrongNumberOfArguments("hhcurveto", nargs);
        }

        int i = 0;
        int by = 0;

        if (nargs % 4 == 1) {
            by = args[i].num;
            ++i;
        }

        for (; i < nargs; i += 4) {
            int bx = args[i].num;
            int cx = args[i + 1].num;
            int cy = args[i + 2].num;
            int dx = args[i + 3].num;

            const Point& A = cursor;
            Point B = A + Point(bx, by);
            Point C = B + Point(cx, cy);
            Point D = C + Point(dx, 0);

            CubicBezier bezier(A, B, C, D);

            DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
            path->append(bezier);

            DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
            cursor = path->finalPoint();

            by = 0;
        }
    }
    else if (op.str == "hvcurveto") {
        if (nargs % 8 == 4 || nargs % 8 == 5) {
            int bx = args[0].num;
            int cx = args[1].num;
            int cy = args[2].num;
            int dx = 0;
            int dy = args[3].num;

            if (nargs == 5) {
                dx = args[4].num;
            }

            const Point& A = cursor;
            Point B = A + Point(bx, 0);
            Point C = B + Point(cx, cy);
            Point D = C + Point(dx, dy);

            CubicBezier bezier(A, B, C, D);

            DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
            path->append(bezier);

            DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
            cursor = path->finalPoint();

            // Number of curves remaining
            int n = (nargs - 4) / 8;

            for (int cv = 0; cv < n; ++cv) {
                int i = 4 + cv * 8;

                int by = args[i].num;
                int cx = args[i + 1].num;
                int cy = args[i + 2].num;
                int dx = args[i + 3].num;

                const Point& A = cursor;
                Point B = A + Point(0, by);
                Point C = B + Point(cx, cy);
                Point D = C + Point(dx, 0);

                CubicBezier bezier(A, B, C, D);

                DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
                path->append(bezier);

                DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                cursor = path->finalPoint();

                {
                    int bx = args[i + 4].num;
                    int cx = args[i + 5].num;
                    int cy = args[i + 6].num;
                    int dx = 0;
                    int dy = args[i + 7].num;

                    if (cv == n - 1 && nargs % 8 == 5) {
                        dx = args[i + 8].num;
                    }

                    const Point& A = cursor;
                    Point B = A + Point(bx, 0);
                    Point C = B + Point(cx, cy);
                    Point D = C + Point(dx, dy);

                    CubicBezier bezier(A, B, C, D);

                    DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
                    path->append(bezier);

                    DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                    cursor = path->finalPoint();
                }
            }
        }
        else if (nargs >= 8 && nargs % 8 == 0 || nargs % 8 == 1) {
            int n = nargs / 8;

            for (int cv = 0; cv < n; ++cv) {
                int i = cv * 8;

                int bx = args[i].num;
                int cx = args[i + 1].num;
                int cy = args[i + 2].num;
                int dy = args[i + 3].num;

                const Point& A = cursor;
                Point B = A + Point(bx, 0);
                Point C = B + Point(cx, cy);
                Point D = C + Point(0, dy);

                CubicBezier bezier(A, B, C, D);

                DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
                path->append(bezier);

                DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                cursor = path->finalPoint();

                {
                    int by = args[i + 4].num;
                    int cx = args[i + 5].num;
                    int cy = args[i + 6].num;
                    int dx = args[i + 7].num;
                    int dy = 0;

                    if (cv == n - 1 && nargs % 8 == 1) {
                        dy = args[i + 8].num;
                    }

                    const Point& A = cursor;
                    Point B = A + Point(0, by);
                    Point C = B + Point(cx, cy);
                    Point D = C + Point(dx, dy);

                    CubicBezier bezier(A, B, C, D);

                    DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
                    path->append(bezier);

                    DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                    cursor = path->finalPoint();
                }
            }
        }
        else {
            throw WrongNumberOfArguments("hhcurveto", nargs);
        }
    }
    else if (op.str == "rcurveline") {
        if (nargs < 8 && nargs % 6 != 2) {
            throw WrongNumberOfArguments("rcurveline", nargs);
        }

        int n = (nargs - 2) / 6;

        for (int cv = 0; cv < n; ++cv) {
            int i = cv * 6;

            for (int j = 0; j < 6; ++j) {
                stack.push_back(args[i + j].num);
            }

            process(paths, cursor, stack, "rrcurveto");
        }

        stack.push_back(args[nargs - 2]);
        stack.push_back(args[nargs - 1]);

        process(paths, cursor, stack, "rlineto");
    }
    else if (op.str == "rlinecurve") {
        if (nargs < 8 && nargs % 2 != 0) {
            throw WrongNumberOfArguments("rlinecurve", nargs);
        }

        int n = (nargs - 6) / 2;

        for (int cv = 0; cv < n; ++cv) {
            int i = cv * 2;

            stack.push_back(args[i].num);
            stack.push_back(args[i + 1].num);

            process(paths, cursor, stack, "rlineto");
        }

        stack.push_back(args[nargs - 6]);
        stack.push_back(args[nargs - 5]);
        stack.push_back(args[nargs - 4]);
        stack.push_back(args[nargs - 3]);
        stack.push_back(args[nargs - 2]);
        stack.push_back(args[nargs - 1]);

        process(paths, cursor, stack, "rrcurveto");
    }
    else if (op.str == "vhcurveto") {
        if (nargs % 8 == 4 || nargs % 8 == 5) {
            int by = args[0].num;
            int cx = args[1].num;
            int cy = args[2].num;
            int dx = args[3].num;
            int dy = 0;

            if (nargs == 5) {
                dy = args[4].num;
            }

            const Point& A = cursor;
            Point B = A + Point(0, by);
            Point C = B + Point(cx, cy);
            Point D = C + Point(dx, dy);

            CubicBezier bezier(A, B, C, D);

            DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
            path->append(bezier);

            DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
            cursor = path->finalPoint();

            // Number of curves remaining
            int n = (nargs - 4) / 8;

            for (int cv = 0; cv < n; ++cv) {
                int i = 4 + cv * 8;

                int bx = args[i].num;
                int cx = args[i + 1].num;
                int cy = args[i + 2].num;
                int dy = args[i + 3].num;

                const Point& A = cursor;
                Point B = A + Point(bx, 0);
                Point C = B + Point(cx, cy);
                Point D = C + Point(0, dy);

                CubicBezier bezier(A, B, C, D);

                DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
                path->append(bezier);

                DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                cursor = path->finalPoint();

                {
                    int by = args[i + 4].num;
                    int cx = args[i + 5].num;
                    int cy = args[i + 6].num;
                    int dy = 0;
                    int dx = args[i + 7].num;

                    if (cv == n - 1 && nargs % 8 == 5) {
                        dy = args[i + 8].num;
                    }

                    const Point& A = cursor;
                    Point B = A + Point(0, by);
                    Point C = B + Point(cx, cy);
                    Point D = C + Point(dx, dy);

                    CubicBezier bezier(A, B, C, D);

                    DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
                    path->append(bezier);

                    DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                    cursor = path->finalPoint();
                }
            }
        }
        else if (nargs >= 8 && nargs % 8 == 0 || nargs % 8 == 1) {
            int n = nargs / 8;

            for (int cv = 0; cv < n; ++cv) {
                int i = cv * 8;

                int by = args[i].num;
                int cx = args[i + 1].num;
                int cy = args[i + 2].num;
                int dx = args[i + 3].num;

                const Point& A = cursor;
                Point B = A + Point(0, by);
                Point C = B + Point(cx, cy);
                Point D = C + Point(dx, 0);

                CubicBezier bezier(A, B, C, D);

                DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
                path->append(bezier);

                DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                cursor = path->finalPoint();

                {
                    int bx = args[i + 4].num;
                    int cx = args[i + 5].num;
                    int cy = args[i + 6].num;
                    int dx = 0;
                    int dy = args[i + 7].num;

                    if (cv == n - 1 && nargs % 8 == 1) {
                        dx = args[i + 8].num;
                    }

                    const Point& A = cursor;
                    Point B = A + Point(bx, 0);
                    Point C = B + Point(cx, cy);
                    Point D = C + Point(dx, dy);

                    CubicBezier bezier(A, B, C, D);

                    DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
                    path->append(bezier);

                    DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
                    cursor = path->finalPoint();
                }
            }
        }
        else {
            throw WrongNumberOfArguments("vhcurveto", nargs);
        }
    }
    else if (op.str == "vvcurveto") {
        if (nargs % 4 != 0 && nargs % 4 != 1) {
            throw WrongNumberOfArguments("vvcurveto", nargs);
        }

        int bx = 0;
        if (nargs % 4 == 1) {
            bx = args[0].num;
        }

        int n = nargs / 4;

        for (int cv = 0; cv < n; ++cv) {
            int i = cv * 4;

            if (nargs % 4 == 1) {
                ++i;
            }

            int by = args[i].num;
            int cx = args[i + 1].num;
            int cy = args[i + 2].num;
            int dy = args[i + 3].num;

            const Point& A = cursor;
            Point B = A + Point(bx, by);
            Point C = B + Point(cx, cy);
            Point D = C + Point(0, dy);

            CubicBezier bezier(A, B, C, D);

            DBG_OUT("Appending cubic bezier: " << bezier << "\n"); 
            path->append(bezier);

            DBG_OUT("Moving cursor from " << cursor << " to " << path->finalPoint() << "\n");
            cursor = path->finalPoint();

            bx = 0;
        }
    }
    else if (op.str == "flex") {
        throw NotImplementedException("Token 'flex' is not implemented.");
    }
    else if (op.str == "hflex") {
        throw NotImplementedException("Token 'hflex' is not implemented.");
    }
    else if (op.str == "hflex1") {
        throw NotImplementedException("Token 'hflex1' is not implemented.");
    }
    else if (op.str == "flex1") {
        throw NotImplementedException("Token 'flex1' is not implemented.");
    }
    else if (op.str == "endchar") {
        if (nargs != 0) {
            throw WrongNumberOfArguments("endchar", nargs);
        }

        path->close();
    }
    else {
        throw UnrecognisedToken(op);
    }
}

static void unparseLineSegment(Charstring& cs, const Point& cursor, const LineSegment& lseg) {
    assert(lseg.initialPoint() == cursor);

    Point P = lseg.B() - cursor;

    Charstring extra({
        static_cast<int>(P.x),
        static_cast<int>(P.y),
        "rlineto"
    });

    cs.insert(cs.end(), extra.begin(), extra.end());
}

static void unparseCubicBezier(Charstring& cs, const Point& cursor, const CubicBezier& bezier) {
    assert(bezier.initialPoint() == cursor);

    Point B = bezier.B() - cursor;
    Point C = bezier.C() - bezier.B();
    Point D = bezier.D() - bezier.C();

    Charstring extra({
        static_cast<int>(B.x), static_cast<int>(B.y),
        static_cast<int>(C.x), static_cast<int>(C.y),
        static_cast<int>(D.x), static_cast<int>(D.y),
        "rrcurveto"
    });

    cs.insert(cs.end(), extra.begin(), extra.end());
}

PathList parseCharstring(const Charstring& charstring) {
    PathList paths;
    Point cursor(0, 0);
    Stack stack;

    for (int i = 0; i < charstring.size(); ++i) {
        const CsToken& tok = charstring[i];

        try {
            if (tok.type == PS_OPERAND) {
                stack.push_back(tok);
            }
            else if (tok.type == PS_OPERATOR) {
                process(paths, cursor, stack, tok);
            }
        }
        catch (CsMergeException& ex) {
            throw makeParseError(tok.str, i, stack, ex);
        }
    }

    if (!stack.empty()) {
        throw makeParseError("Redundant arguments on stack");
    }

    return paths;
}

Charstring generateCharstring(const PathList& paths) {
    Charstring cs;
    Point cursor(0, 0);

    for (auto i = paths.begin(); i != paths.end(); ++i) {
        const Path& path = *i;

        for (auto j = path.begin(); j != path.end(); ++j) {
            const Curve& curve = **j;
            Point p = curve.initialPoint();

            if (p != cursor) {
                Point p_ = p - cursor;

                Charstring extra({
                    static_cast<int>(p_.x),
                    static_cast<int>(p_.y),
                    "rmoveto"
                });

                cs.insert(cs.end(), extra.begin(), extra.end());

                cursor = p;
            }

            if (curve.type() == LineSegment::type) {
                const LineSegment& lseg = dynamic_cast<const LineSegment&>(curve);
                unparseLineSegment(cs, cursor, lseg);
            }
            else if (curve.type() == CubicBezier::type) {
                const CubicBezier& bezier = dynamic_cast<const CubicBezier&>(curve);
                unparseCubicBezier(cs, cursor, bezier);
            }

            cursor = curve.finalPoint();
        }
    }

    cs.push_back("endchar");
    return cs;
}

Charstring mergeCharstrings(const Charstring& cs1, const Charstring& cs2) {
    PathList paths1 = parseCharstring(cs1);
    PathList paths2 = parseCharstring(cs2);
    PathList paths3 = computeUnion(paths1, paths2);

    return generateCharstring(paths3);
}


}
