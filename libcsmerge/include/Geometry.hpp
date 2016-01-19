#ifndef __GEOMETRY_HPP__
#define __GEOMETRY_HPP__


#include <memory>
#include <vector>
#include <CGAL/basic.h>
#include <CGAL/Cartesian.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/Arr_Bezier_curve_traits_2.h>
#include <CGAL/Gps_traits_2.h>
#include <CGAL/General_polygon_set_2.h>
#include <CGAL/Polygon_2.h>
#ifdef APPROX_BEZIERS
#    include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#    include <CGAL/Polygon_2.h>
#    include <CGAL/Polygon_with_holes_2.h>
#    include <CGAL/Polygon_set_2.h>
#endif
#include "Exception.hpp"


namespace csmerge {
namespace geometry {


namespace cgal_wrap {


// The arrangement traits for bezier curves uses rational numbers for control
// points and algebraic numbers for roots.
typedef CGAL::CORE_algebraic_number_traits NtTraits;
typedef NtTraits::Rational Rational;
typedef NtTraits::Algebraic Algebraic;

// Instantiate traits templates that implement the geometry kernel concept,
// whose subtypes are used by the various CGAL data structures and algorithms.
typedef CGAL::Cartesian<Rational> RatKernel;
typedef CGAL::Cartesian<Algebraic> AlgKernel;

// Get the point type from the geometry kernel so we can define control points.
typedef RatKernel::Point_2 BezierRatPoint;

// Instantiate a traits class with the Arr_Bezier_curve_traits_2 template to
// implement the ArrangementTraits_2 concept.
//    - Arg3 should define nested types Integer, Rational, and Algebraic.
//    - Args 1 & 2 are geometric kernals templated with Arg3::Rational and
//      Arg3::Algebraic respectively.
typedef CGAL::Arr_Bezier_curve_traits_2<RatKernel, AlgKernel, NtTraits> Traits;

typedef Traits::Point_2 BezierPoint;
typedef Traits::Curve_2 BezierCurve;

// Models the GeneralPolygonSetTraits_2 concept. Inherits from its argument, Traits.
typedef CGAL::Gps_traits_2<Traits> BezierTraits;

// Get the x-monotone curve type from the arrangement traits type
typedef BezierTraits::X_monotone_curve_2 BezierXMonotoneCurve;

typedef BezierTraits::General_polygon_2 BezierPolygon;
typedef BezierTraits::General_polygon_with_holes_2 BezierPolygonWithHoles;
typedef CGAL::Gps_default_dcel<BezierTraits> BezierDcelTraits;
typedef CGAL::General_polygon_set_2<BezierTraits, BezierDcelTraits> BezierPolygonSet;

typedef std::vector<BezierPolygonWithHoles> PolyList;


}


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
        bool isClosed() const;

        const Curve& operator[](int idx) const;
        Curve& operator[](int idx);

        Point initialPoint() const;
        Point finalPoint() const;

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;

        ~Path();

    private:
        std::vector<std::unique_ptr<Curve>> m_curves;
};


std::ostream& operator<<(std::ostream& out, const Curve& curve);
std::ostream& operator<<(std::ostream& out, const Point& pt);
std::ostream& operator<<(std::ostream& out, const Path& path);


extern double FLOAT_PRECISION;
extern double MIN_LSEG_LENGTH;        // These are only used in the APPROX_BEZIERS build
extern double MAX_LSEGS_PER_BEZIER;   //

void initialise();
PathList computeUnion(const PathList& paths1, const PathList& paths2);


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


// ----- Private functions, exposed here for testing only -----

cgal_wrap::PolyList toPolyList(const PathList& paths);
PathList toPathList(const cgal_wrap::PolyList& polyList);
cgal_wrap::BezierCurve cubicBezierFromXMonoSection(const cgal_wrap::BezierXMonotoneCurve& mono);
PathList computeUnion(const PathList& paths1, const PathList& paths2);


// Namespace containing temporary solution due to bug in CGAL4.7. Bezier
// polygons are approximated by regular polygons.
#ifdef APPROX_BEZIERS
namespace approx {


namespace cgal_approx {


typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2 Point;
typedef CGAL::Polygon_2<Kernel> Polygon;
typedef CGAL::Polygon_with_holes_2<Kernel> PolygonWithHoles;
typedef CGAL::Polygon_set_2<Kernel> PolygonSet;
typedef std::vector<PolygonWithHoles> PolyList;


}


cgal_approx::PolyList toPolyList(const PathList& paths);
PathList toPathList(const cgal_approx::PolyList& polyList);
PathList toLinearPaths(const PathList& paths);
PathList computeUnion(const PathList& paths1, const PathList& paths2);


}
#endif

// ------------------------------------------------------------


}
}


#endif
