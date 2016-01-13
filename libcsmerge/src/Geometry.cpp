#include <functional>
#include <sstream>
#include <CGAL/assertions_behaviour.h>
#include "Geometry.hpp"
#include "Types.hpp"


namespace csmerge {
namespace geometry {


class CgalException : public GeometryException {
    public:
        CgalException(const char* type, const char* expression,
            const char* file, int line, const char* explanation)
                : GeometryException(constructMsg(type, expression, file, line, explanation)) {}

    private:
        std::string constructMsg(const char* type, const char* expression,
            const char* file, int line, const char* explanation) const {

            std::stringstream ss;
            ss << "Error from CGAL: (type: " << type << ", expression: "
               << expression << ", file: " << file << ", line: " << line
               << ", explanation: " << explanation << ")";

            return ss.str();
        }
};


double FLOAT_PRECISION = 0.001;


NoncontiguousCurvesException::NoncontiguousCurvesException(const Point& pathEnd, const Point& curveStart)
    : GeometryException(constructMsg(pathEnd, curveStart)),
      pathEnd(pathEnd), curveStart(curveStart) {}

std::string NoncontiguousCurvesException::constructMsg(const Point& pathEnd,
    const Point& curveStart) const {

    std::stringstream ss;
    ss << "Paths must consist of contiguous curves; path end: ("
        << pathEnd.x << ", " << pathEnd.y << "), curve start: ("
        << curveStart.x << ", " << curveStart.y << ")";

    return ss.str();
}


static bool isLinear(const cgal_wrap::BezierCurve& curve) {
    return curve.number_of_control_points() == 2;
}

static Path toPath(const cgal_wrap::BezierPolygon& poly) {
    Path path;

    for (auto c = poly.curves_begin(); c != poly.curves_end(); ++c) {
        cgal_wrap::BezierCurve curve = cubicBezierFromXMonoSection(*c);
        size_t n = curve.number_of_control_points();

        if (isLinear(curve)) {
            Point A = curve.control_point(0);
            Point B = curve.control_point(n - 1);

            path.append(LineSegment(A, B));
        }
        else {
            assert(n == 4);

            Point A = curve.control_point(0);
            Point B = curve.control_point(1);
            Point C = curve.control_point(2);
            Point D = curve.control_point(3);

            path.append(CubicBezier(A, B, C, D));
        }
    }

    return path;
}

static void errorHandler(const char* type, const char* expression,
    const char* file, int line, const char* explanation) {

    throw CgalException(type, expression, file, line, explanation);
}

static void warningHandler(const char* type, const char* expression,
    const char* file, int line, const char* explanation) {

    throw CgalException(type, expression, file, line, explanation);
}

void initialise() {
    CGAL::set_error_behaviour(CGAL::THROW_EXCEPTION);
    CGAL::set_warning_behaviour(CGAL::CONTINUE);
    CGAL::set_error_handler(&errorHandler);
    CGAL::set_warning_handler(&warningHandler);
}

cgal_wrap::BezierCurve cubicBezierFromXMonoSection(const cgal_wrap::BezierXMonotoneCurve& mono) {
    cgal_wrap::BezierCurve supportCurve = mono.supporting_curve();

    double t0 = mono.parameter_range().first;
    double t1 = mono.parameter_range().second;

    if (t0 == 0.0 && t1 == 1.0) {
        return supportCurve;
    }

    // If the curve is a line segment, there's no need to use De Casteljau's
    if (supportCurve.number_of_control_points() == 2) {
        cgal_wrap::BezierRatPoint A = supportCurve.control_point(0);
        cgal_wrap::BezierRatPoint B = supportCurve.control_point(1);

        cgal_wrap::RatKernel::FT t0_(t0);
        cgal_wrap::RatKernel::FT t1_(t1);

        double ax = CGAL::to_double(A[0]);
        double ay = CGAL::to_double(A[1]);
        double bx = CGAL::to_double(B[0]);
        double by = CGAL::to_double(B[1]);

        cgal_wrap::BezierRatPoint A_(ax + (bx - ax) * t0_, ay + (by - ay) * t0_);
        cgal_wrap::BezierRatPoint B_(ax + (bx - ax) * t1_, ay + (by - ay) * t1_);

        std::list<cgal_wrap::BezierRatPoint> pts;
        pts.push_back(A_);
        pts.push_back(B_);

        return cgal_wrap::BezierCurve(pts.begin(), pts.end());
    }

    std::list<cgal_wrap::BezierRatPoint> ctrlPoints;

    for (int i = 0; i < supportCurve.number_of_control_points(); ++i) {
        ctrlPoints.push_back(supportCurve.control_point(i));
    }

    std::list<cgal_wrap::BezierRatPoint> leftCtrlPoints, rightCtrlPoints;

    CGAL::de_Casteljau_2(ctrlPoints.begin(), ctrlPoints.end(), t0,
        std::back_inserter(leftCtrlPoints), std::front_inserter(rightCtrlPoints));

    cgal_wrap::BezierCurve rightCurve(rightCtrlPoints.begin(), rightCtrlPoints.end());

    double t1_ = (t1 - t0) / (1.0 - t0);

    leftCtrlPoints.clear(); // We can reuse this list
    std::list<cgal_wrap::BezierRatPoint> rightRightCtrlPoints;

    CGAL::de_Casteljau_2(rightCtrlPoints.begin(), rightCtrlPoints.end(), t1_,
        std::back_inserter(leftCtrlPoints), std::front_inserter(rightRightCtrlPoints));

    return cgal_wrap::BezierCurve(leftCtrlPoints.begin(), leftCtrlPoints.end());
}

PathList toPathList(const PolyList& polyList) {
    PathList paths;

    for (auto i = polyList.begin(); i != polyList.end(); ++i) {
        const cgal_wrap::BezierPolygonWithHoles& poly = *i;
        const cgal_wrap::BezierPolygon& outer = poly.outer_boundary();

        paths.push_back(toPath(outer));

        for (auto j = poly.holes_begin(); j != poly.holes_end(); ++j) {
            paths.push_back(toPath(*j));
        }
    }

    return paths;
}

PolyList toPolyList(const PathList& paths) {
    cgal_wrap::Traits traits;
    cgal_wrap::Traits::Make_x_monotone_2 fnMakeXMonotone = traits.make_x_monotone_2_object();

    cgal_wrap::RatKernel ratKernel;
    cgal_wrap::RatKernel::Equal_2 fnEqual = ratKernel.equal_2_object();

    PolyList polyList; // The final polygons with holes
    cgal_wrap::BezierPolygon outerPoly;
    std::list<cgal_wrap::BezierPolygon> holes;
    std::list<cgal_wrap::BezierXMonotoneCurve> monoCurves;
    bool first = true;
    cgal_wrap::BezierRatPoint firstPoint;

    // For each path in the list
    for (auto i = paths.begin(); i != paths.end(); ++i) {
        const Path& path = *i;

        cgal_wrap::BezierRatPoint prevEndPoint;

        // For each curve in the path
        for (auto j = path.begin(); j != path.end(); ++j) {
            const Curve& curve = **j;

            std::list<cgal_wrap::BezierRatPoint> points;

            if (curve.type() == LineSegment::type) {
                const LineSegment& lseg = dynamic_cast<const LineSegment&>(curve);

                cgal_wrap::BezierRatPoint A = lseg.A();

                if (j != path.begin()) {
                    assert(A == prevEndPoint);
                }

                points.push_back(cgal_wrap::BezierRatPoint(A));
                points.push_back(cgal_wrap::BezierRatPoint(lseg.B()));
            }
            else if (curve.type() == CubicBezier::type) {
                const CubicBezier& bezier = dynamic_cast<const CubicBezier&>(curve);

                cgal_wrap::BezierRatPoint A = bezier.A();

                if (j != path.begin()) {
                    assert(A == prevEndPoint);
                }

                points.push_back(cgal_wrap::BezierRatPoint(A));
                points.push_back(cgal_wrap::BezierRatPoint(bezier.B()));
                points.push_back(cgal_wrap::BezierRatPoint(bezier.C()));
                points.push_back(cgal_wrap::BezierRatPoint(bezier.D()));
            }
            else {
                throw GeometryException("Curve type is not recognised");
            }

            bool bClosesCurve = false;

            // If the last point of the current curve is approximately equal to the first
            // point in the chain, we've completed a polygon. Set the last point to the
            // first to ensure they match exactly.
            if (!first && Point(points.back()) == Point(firstPoint)) {
                points.pop_back();
                points.push_back(firstPoint);

                bClosesCurve = true;
            }

            // Keep track of what the last point from the previous curve was
            prevEndPoint = points.back();

            cgal_wrap::BezierCurve cgalCurve(points.begin(), points.end());
            std::list<CGAL::Object> monoObjs;
            fnMakeXMonotone(cgalCurve, std::back_inserter(monoObjs));

//            std::cout << "No. mono-curves: " << monoObjs.size() << "\n";

            // Append the x-monotone curves to the list
            cgal_wrap::BezierXMonotoneCurve monoCurve;
            for (auto o = monoObjs.begin(); o != monoObjs.end(); ++o) {
                if (CGAL::assign(monoCurve, *o)) {
                    monoCurves.push_back(monoCurve);
                }
            }

            if (!first) {
                // If this curve closes the current chain, thereby creating a new polygon
                if (bClosesCurve) {

                    // Add the new polygon to the list

                    cgal_wrap::BezierPolygon subPoly(monoCurves.begin(), monoCurves.end());

                    if (subPoly.orientation() == CGAL::COUNTERCLOCKWISE) {
                        if (!outerPoly.is_empty()) {
                            polyList.push_back(cgal_wrap::BezierPolygonWithHoles(outerPoly, holes.begin(), holes.end()));
                            holes.clear();
                        }

                        outerPoly = subPoly;
                    }
                    else {
                        holes.push_back(subPoly);
                    }

                    monoCurves.clear();
                    first = true;
                }
            }
            else {
                // This is the first curve in the chain - store its source point
                firstPoint = cgalCurve.control_point(0);
                first = false;
            }
        }
    }

    polyList.push_back(cgal_wrap::BezierPolygonWithHoles(outerPoly, holes.begin(), holes.end()));

    return polyList;
}


Point::Point()
    : x(0), y(0) {}

Point::Point(const Point& pt)
    : x(pt.x), y(pt.y) {}

Point::Point(double x, double y)
    : x(x), y(y) {}

Point::Point(const cgal_wrap::BezierRatPoint& pt) {
    x = CGAL::to_double(pt[0]);
    y = CGAL::to_double(pt[1]);
}

Point Point::operator+(const Point& rhs) const {
    return Point(x + rhs.x, y + rhs.y);
}

Point Point::operator-(const Point& rhs) const {
    return Point(x - rhs.x, y - rhs.y);
}

Point& Point::operator=(const Point& rhs) {
    x = rhs.x;
    y = rhs.y;

    return *this;
}

Point& Point::operator=(const cgal_wrap::BezierRatPoint& rhs) {
    x = CGAL::to_double(rhs[0]);
    y = CGAL::to_double(rhs[1]);

    return *this;
}

Point& Point::operator+=(const Point& rhs) {
    return *this = *this + rhs;
}

Point& Point::operator-=(const Point& rhs) {
    return *this = *this - rhs;
}

bool Point::operator==(const Point& rhs) const {
    return fabs(x - rhs.x) <= FLOAT_PRECISION && fabs(y - rhs.y) <= FLOAT_PRECISION;
}

bool Point::operator!=(const Point& rhs) const {
    return !(*this == rhs);
}

Point::operator cgal_wrap::BezierRatPoint() const {
    return cgal_wrap::BezierRatPoint(x, y);
}


std::ostream& operator<<(std::ostream& out, const Point& pt) {
    out << "(" << pt.x << ", " << pt.y << ")";
    return out;
}


Curve::Curve(size_t typeId)
    : m_type(typeId) {}

size_t Curve::type() const {
    return m_type;
}

Curve::~Curve() {}


size_t LineSegment::type = std::hash<std::string>()("csmerge_LineSegment");


LineSegment::LineSegment(const Point& A, const Point& B)
    : Curve(LineSegment::type), m_a(A), m_b(B) {}

Curve* LineSegment::clone() const {
    return new LineSegment(*this);
}

const Point& LineSegment::A() const {
    return m_a;
}

const Point& LineSegment::B() const {
    return m_b;
}

Point LineSegment::initialPoint() const {
    return A();
}

Point LineSegment::finalPoint() const {
    return B();
}

void LineSegment::setInitialPoint(const Point& point) {
    m_a = point;
}

void LineSegment::print(std::ostream& out) const {
    out << "LineSegment[" << A() << ", " << B() << "]";
}

bool LineSegment::operator==(const Curve& rhs) const {
    if (rhs.type() != LineSegment::type) {
        return false;
    }

    const LineSegment& r = dynamic_cast<const LineSegment&>(rhs);

    return A() == r.A() && B() == r.B();
}

bool LineSegment::operator!=(const Curve& rhs) const {
    return !(*this == rhs);
}

LineSegment::~LineSegment() {}


size_t CubicBezier::type = std::hash<std::string>()("csmerge_CubicBezier");


CubicBezier::CubicBezier(const Point& A, const Point& B, const Point& C, const Point& D)
    : Curve(CubicBezier::type), m_a(A), m_b(B), m_c(C), m_d(D) {}

Curve* CubicBezier::clone() const {
    return new CubicBezier(*this);
}

const Point& CubicBezier::A() const {
    return m_a;
}

const Point& CubicBezier::B() const {
    return m_b;
}

const Point& CubicBezier::C() const {
    return m_c;
}

const Point& CubicBezier::D() const {
    return m_d;
}

Point CubicBezier::initialPoint() const {
    return A();
}

Point CubicBezier::finalPoint() const {
    return D();
}

void CubicBezier::setInitialPoint(const Point& point) {
    m_a = point;
}

void CubicBezier::print(std::ostream& out) const {
    out << "CubicBezier[" << A() << ", " << B() << ", " << C() << ", " << D() << "]";
}

bool CubicBezier::operator==(const Curve& rhs) const {
    if (rhs.type() != CubicBezier::type) {
        return false;
    }

    const CubicBezier& r = dynamic_cast<const CubicBezier&>(rhs);

    return A() == r.A() && B() == r.B() && C() == r.C() && D() == r.D();
}

bool CubicBezier::operator!=(const Curve& rhs) const {
    return !(*this == rhs);
}

CubicBezier::~CubicBezier() {}


std::ostream& operator<<(std::ostream& out, const Curve& curve) {
    curve.print(out);
    return out;
}


Path::Path() {}

Path::Path(Path&& cpy)
    : m_curves(std::move(cpy.m_curves)) {}

Path::Path(const Path& cpy) {
    for (const std::unique_ptr<Curve>& pCurve : cpy.m_curves) {
        m_curves.push_back(std::unique_ptr<Curve>(pCurve->clone()));
    }
}

void Path::append(const Curve& curve) {
    Curve* cpy = curve.clone();

    if (m_curves.size() > 0) {
        if (curve.initialPoint() != m_curves.back()->finalPoint()) {
//            try {
                throw NoncontiguousCurvesException(m_curves.back()->finalPoint(), curve.initialPoint());
//            }
//            catch (CsMergeException& ex) {
//                std::cout << "Error appending to path: " << ex.what() << "; Continuing ...\n"; // TODO
//            }
        }

        cpy->setInitialPoint(m_curves.back()->finalPoint());
    }

    m_curves.push_back(std::unique_ptr<Curve>(cpy));
}

bool Path::empty() const {
    return m_curves.size() == 0;
}

size_t Path::size() const {
    return m_curves.size();
}

const Curve& Path::operator[](int idx) const {
    return *m_curves[idx];
}

Curve& Path::operator[](int idx) {
    return *m_curves[idx];
}

// Joins the last point to the first with a line segment
void Path::close() {
    if (finalPoint() != initialPoint()) {
        std::unique_ptr<Curve> lseg(new LineSegment(finalPoint(), initialPoint()));
        m_curves.push_back(std::move(lseg));
    }
}

Point Path::initialPoint() const {
    return m_curves.size() > 0 ? m_curves.front()->initialPoint() : Point(0, 0);
}

Point Path::finalPoint() const {
    return m_curves.size() > 0 ? m_curves.back()->finalPoint() : Point(0, 0);
}

Path::iterator Path::begin() {
    return m_curves.begin();
}

Path::iterator Path::end() {
    return m_curves.end();
}

Path::const_iterator Path::begin() const {
    return m_curves.begin();
}

Path::const_iterator Path::end() const {
    return m_curves.end();
}

Path::~Path() {}


// Namespace containing temporary solution due to bug in CGAL4.7. Bezier
// polygons are approximated by regular polygons.
#ifdef APPROX_BEZIERS
namespace approx {


namespace cgal_wrap {


typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2                                   Point_2;
typedef CGAL::Polygon_2<Kernel>                           Polygon_2;


}


PathList Path::computeUnion(const PathList& paths1, const PathList& paths2) {
    // TODO
}


}
#endif


PathList Path::computeUnion(const PathList& paths1, const PathList& paths2) {
#ifdef APPROX_BEZIERS
    return approx::computeUnion(paths1, paths2);
#endif

    PolyList polyList1 = toPolyList(paths1);
    PolyList polyList2 = toPolyList(paths2);

    cgal_wrap::BezierPolygonSet polySet;

    for (auto i : polyList1) {
        polySet.join(i);
    }

    for (auto i : polyList2) {
        polySet.join(i);
    }

    PolyList polyList;
    polySet.polygons_with_holes(std::back_inserter(polyList));

    return toPathList(polyList);
}


}
}
