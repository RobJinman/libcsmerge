#include <CGAL/basic.h>
#include <CGAL/Cartesian.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/Arr_Bezier_curve_traits_2.h>
#include <CGAL/Gps_traits_2.h>
#include <CGAL/General_polygon_set_2.h>
#include <CGAL/Polygon_2.h>


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


}
