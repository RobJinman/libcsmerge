#ifndef __GEOMETRY_HPP__
#define __GEOMETRY_HPP__


#include <memory>
#include <vector>
#include "Exception.hpp"
#include "Types.hpp"


namespace csmerge {
namespace geometry {


typedef std::vector<cgal_wrap::BezierPolygonWithHoles> PolyList;


struct Point {
    Point();
    Point(const Point& cpy);
    Point(double x, double y);
    Point(const cgal_wrap::BezierRatPoint& pt);

    Point operator+(const Point& rhs) const;
    Point operator-(const Point& rhs) const;
    Point& operator=(const Point& rhs);
    Point& operator=(const cgal_wrap::BezierRatPoint& rhs);
    Point& operator+=(const Point& rhs);
    Point& operator-=(const Point& rhs);
    bool operator==(const Point& rhs) const;
    bool operator!=(const Point& rhs) const;

    operator cgal_wrap::BezierRatPoint() const;

    double x;
    double y;
};


class Curve {
    public:
        Curve(size_t typeId);

        size_t type() const;
        virtual Point initialPoint() const = 0;
        virtual Point finalPoint() const = 0;
        virtual void setInitialPoint(const Point& point) = 0;
        virtual Curve* clone() const = 0;
        virtual void print(std::ostream& out) const = 0;
        virtual bool operator==(const Curve& rhs) const = 0;
        virtual bool operator!=(const Curve& rhs) const = 0;

        virtual ~Curve();

    private:
        size_t m_type;
};


class LineSegment : public Curve {
    public:
        static size_t type;

        LineSegment(const Point& A, const Point& B);

        const Point& A() const;
        const Point& B() const;
        virtual Point initialPoint() const override;
        virtual Point finalPoint() const override;
        virtual void setInitialPoint(const Point& point) override;
        virtual Curve* clone() const override;
        virtual void print(std::ostream& out) const override;
        virtual bool operator==(const Curve& rhs) const override;
        virtual bool operator!=(const Curve& rhs) const override;

        virtual ~LineSegment();

    private:
        Point m_a;
        Point m_b;
};


class CubicBezier : public Curve {
    public:
        static size_t type;

        CubicBezier(const Point& A, const Point& B, const Point& C, const Point& D);

        const Point& A() const;
        const Point& B() const;
        const Point& C() const;
        const Point& D() const;
        virtual Point initialPoint() const override;
        virtual Point finalPoint() const override;
        virtual void setInitialPoint(const Point& point) override;
        virtual Curve* clone() const override;
        virtual void print(std::ostream& out) const override;
        virtual bool operator==(const Curve& rhs) const override;
        virtual bool operator!=(const Curve& rhs) const override;

        virtual ~CubicBezier();

    private:
        Point m_a;
        Point m_b;
        Point m_c;
        Point m_d;
};


class Path;
typedef std::vector<Path> PathList;

// Wraps a contiguous sequence of curves.
//
class Path {
    public:
        typedef std::vector<std::unique_ptr<Curve>>::iterator iterator;
        typedef std::vector<std::unique_ptr<Curve>>::const_iterator const_iterator;

        Path();
        Path(Path&& cpy);
        Path(const Path& cpy);

        // The initial point of the input curve must match the final point
        // of the last curve in the path.
        void append(const Curve& curve);

        void close();
        bool empty() const;
        size_t size() const;

        const Curve& operator[](int idx) const;
        Curve& operator[](int idx);

        Point initialPoint() const;
        Point finalPoint() const;

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;

        static PathList computeUnion(const PathList& paths1, const PathList& paths2);

        ~Path();

    private:
        std::vector<std::unique_ptr<Curve>> m_curves;
};


std::ostream& operator<<(std::ostream& out, const Curve& curve);
std::ostream& operator<<(std::ostream& out, const Point& pt);


extern double FLOAT_PRECISION;

void initialise();
PathList toPathList(const PolyList& polyList);
PolyList toPolyList(const PathList& paths);
cgal_wrap::BezierCurve cubicBezierFromXMonoSection(const cgal_wrap::BezierXMonotoneCurve& mono);


class GeometryException : public CsMergeException {
    using CsMergeException::CsMergeException;
};


class NoncontiguousCurvesException : public GeometryException {
    public:
        NoncontiguousCurvesException(const Point& pathEnd, const Point& curveStart);

        Point pathEnd;
        Point curveStart;

    private:
        std::string constructMsg(const Point& pathEnd, const Point& curveStart) const;
};


}
}


#endif
