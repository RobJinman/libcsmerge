#include <gtest/gtest.h>
#include <Geometry.hpp>


using namespace csmerge;
using namespace csmerge::geometry;


class GeometryTest : public testing::Test {
    public:
        virtual void SetUp() override {

        }

        virtual void TearDown() override {

        }
};


TEST_F(GeometryTest, pointPointEquality) {
    Point A(1.0, 2.0);
    Point B(-1.0, 2.0);
    Point C;
    Point D(-1.0, 2.0);
    Point E(0.0, 0.0);

    ASSERT_TRUE(A != B);
    ASSERT_TRUE(B != C);
    ASSERT_TRUE(B == D);
    ASSERT_TRUE(C == E);
}

TEST_F(GeometryTest, lineSegCopyCtor) {
    LineSegment lseg1(Point(1.0, 2.0), Point(3.0, 4.0));
    LineSegment lseg2(lseg1);

    ASSERT_TRUE(lseg1.A() == lseg2.A());
    ASSERT_TRUE(lseg1.B() == lseg2.B());
}

TEST_F(GeometryTest, lineSegLineSegEquality) {
    FLOAT_PRECISION = 0.001;

    LineSegment L1(Point(1.0, 2.0), Point(3.0, 4.0));
    LineSegment L2(Point(1.0011, 2.0), Point(3.0, 4.0));
    LineSegment L3(L2);
    LineSegment L4(Point(1.0009, 2.0), Point(3.0, 4.0));

    ASSERT_TRUE(L1 != L2);
    ASSERT_TRUE(L1 == L4);
    ASSERT_TRUE(L2 == L3);
}

#ifdef APPROX_BEZIERS
TEST_F(GeometryTest, toLinearPaths) {
    Path path;
    path.append(LineSegment(Point(-10, -10), Point(10, -10)));
    path.append(LineSegment(Point(10, -10), Point(10, 10)));
    path.append(LineSegment(Point(10, 10), Point(-10, 10)));
    path.append(LineSegment(Point(-10, 10), Point(-10, -10)));

    PathList paths1;
    paths1.push_back(path);

    PathList paths2 = approx::toLinearPaths(paths1);

    ASSERT_EQ(1, paths2.size());
    ASSERT_EQ(4, paths2[0].size());
    ASSERT_EQ(paths1[0][0], paths2[0][0]);
    ASSERT_EQ(paths1[0][1], paths2[0][1]);
    ASSERT_EQ(paths1[0][2], paths2[0][2]);
    ASSERT_EQ(paths1[0][3], paths2[0][3]);
}

TEST_F(GeometryTest, pathsToPoly) {
    Path path;
    path.append(LineSegment(Point(-10, -10), Point(10, -10)));
    path.append(LineSegment(Point(10, -10), Point(10, 10)));
    path.append(LineSegment(Point(10, 10), Point(-10, 10)));
    path.append(LineSegment(Point(-10, 10), Point(-10, -10)));

    PathList paths1;
    paths1.push_back(path);

    approx::cgal_approx::PolyList polyList = approx::toPolyList(paths1);
    ASSERT_EQ(1, polyList.size());

    const approx::cgal_approx::PolygonWithHoles& poly = polyList[0];

    ASSERT_FALSE(poly.is_unbounded());
    ASSERT_EQ(poly.holes_begin(), poly.holes_end());

    approx::cgal_approx::Polygon outer = poly.outer_boundary();

    ASSERT_EQ(4, outer.size());
    ASSERT_EQ(paths1[0][0].initialPoint(), Point(CGAL::to_double(outer[0].x()), CGAL::to_double(outer[0].y())));
    ASSERT_EQ(paths1[0][1].initialPoint(), Point(CGAL::to_double(outer[1].x()), CGAL::to_double(outer[1].y())));
    ASSERT_EQ(paths1[0][2].initialPoint(), Point(CGAL::to_double(outer[2].x()), CGAL::to_double(outer[2].y())));
    ASSERT_EQ(paths1[0][3].initialPoint(), Point(CGAL::to_double(outer[3].x()), CGAL::to_double(outer[3].y())));
}

TEST_F(GeometryTest, pathsToPolyAndBack) {
    Path path;
    path.append(LineSegment(Point(-10, -10), Point(10, -10)));
    path.append(LineSegment(Point(10, -10), Point(10, 10)));
    path.append(LineSegment(Point(10, 10), Point(-10, 10)));
    path.append(LineSegment(Point(-10, 10), Point(-10, -10)));

    PathList paths1;
    paths1.push_back(path);

    approx::cgal_approx::PolyList polyList = approx::toPolyList(paths1);
    ASSERT_EQ(1, polyList.size());
    ASSERT_EQ(4, polyList[0].outer_boundary().size());

    PathList paths2 = approx::toPathList(polyList);
    ASSERT_EQ(1, paths2.size());
    ASSERT_EQ(4, paths2[0].size());

    for (int i = 0; i < paths1[0].size(); ++i) {
        ASSERT_EQ(paths1[0][i], paths2[0][i]);
    }
}
#endif

#ifndef APPROX_BEZIERS
TEST_F(GeometryTest, pathsToPoly) {
    Path path;
    path.append(LineSegment(Point(-10, -10), Point(10, -10)));
    path.append(LineSegment(Point(10, -10), Point(10, 10)));
    path.append(LineSegment(Point(10, 10), Point(-10, 10)));
    path.append(LineSegment(Point(-10, 10), Point(-10, -10)));

    PathList paths1;
    paths1.push_back(path);

    cgal_wrap::PolyList polyList = toPolyList(paths1);
    ASSERT_EQ(1, polyList.size());

    const cgal_wrap::BezierPolygonWithHoles& poly = polyList[0];

    ASSERT_FALSE(poly.is_unbounded());
    ASSERT_EQ(poly.holes_begin(), poly.holes_end());

    auto outer = poly.outer_boundary();

    int i = 0;
    for (auto it = outer.curves_begin(); it != outer.curves_end(); ++it) {
        std::pair<double, double> src = it->source().approximate();
        std::pair<double, double> trg = it->target().approximate();

        const LineSegment& lseg = dynamic_cast<const LineSegment&>(path[i]);

        cgal_wrap::BezierCurve supCurve = it->supporting_curve();
        ASSERT_EQ(2, supCurve.number_of_control_points());

        Point A(supCurve.control_point(0));
        Point B(supCurve.control_point(1));

        ASSERT_EQ(lseg.A(), A);
        ASSERT_EQ(lseg.B(), B);

        ASSERT_EQ(std::make_pair(lseg.A().x, lseg.A().y), src);
        ASSERT_EQ(std::make_pair(lseg.B().x, lseg.B().y), trg);

        double t0 = it->parameter_range().first;
        double t1 = it->parameter_range().second;

        ASSERT_EQ(0.0, t0);
        ASSERT_EQ(1.0, t1);

        ++i;
    }
}

TEST_F(GeometryTest, pathsToPolyAndBack) {
    Path path;
    path.append(LineSegment(Point(-10, -10), Point(10, -10)));
    path.append(LineSegment(Point(10, -10), Point(10, 10)));
    path.append(LineSegment(Point(10, 10), Point(-10, 10)));
    path.append(LineSegment(Point(-10, 10), Point(-10, -10)));

    PathList paths1;
    paths1.push_back(path);

    cgal_wrap::PolyList polyList = toPolyList(paths1);
    ASSERT_EQ(1, polyList.size());

    PathList paths2 = toPathList(polyList);
    ASSERT_EQ(1, paths2.size());

    for (int i = 0; i < paths1[0].size(); ++i) {
        ASSERT_EQ(paths1[0][i], paths2[0][i]);
    }
}

TEST_F(GeometryTest, pathsToPolyWithBezier) {
    Path path;
    path.append(LineSegment(Point(-10, -10), Point(10, -10)));                            // L0
    path.append(CubicBezier(Point(10, -10), Point(7, -4), Point(13, 3), Point(10, 10)));  // B1
    path.append(LineSegment(Point(10, 10), Point(-10, 10)));                              // L2
    path.append(LineSegment(Point(-10, 10), Point(-10, -10)));                            // L3

    PathList paths1;
    paths1.push_back(path);

    cgal_wrap::PolyList polyList = toPolyList(paths1);
    ASSERT_EQ(1, polyList.size());

    const cgal_wrap::BezierPolygonWithHoles& poly = polyList[0];

    ASSERT_FALSE(poly.is_unbounded());
    ASSERT_EQ(poly.holes_begin(), poly.holes_end());

    auto outer = poly.outer_boundary();

    auto it = outer.curves_begin();
    ASSERT_TRUE(it != outer.curves_end());

    //------- L0 -------
    {
        std::pair<double, double> src = it->source().approximate();
        std::pair<double, double> trg = it->target().approximate();

        const LineSegment* lseg = dynamic_cast<const LineSegment*>(&path[0]);
        ASSERT_TRUE(lseg != nullptr);

        cgal_wrap::BezierCurve supCurve = it->supporting_curve();
        ASSERT_EQ(2, supCurve.number_of_control_points());

        Point A(supCurve.control_point(0));
        Point B(supCurve.control_point(1));

        ASSERT_EQ(lseg->A(), A);
        ASSERT_EQ(lseg->B(), B);

        ASSERT_EQ(std::make_pair(lseg->A().x, lseg->A().y), src);
        ASSERT_EQ(std::make_pair(lseg->B().x, lseg->B().y), trg);

        double t0 = it->parameter_range().first;
        double t1 = it->parameter_range().second;

        ASSERT_EQ(0.0, t0);
        ASSERT_EQ(1.0, t1);

        ++it;
        ASSERT_TRUE(it != outer.curves_end());
    }
    //------------------

    //------- B1 -------
    {
        const CubicBezier* bezier = dynamic_cast<const CubicBezier*>(&path[1]);
        ASSERT_TRUE(bezier != nullptr);

        cgal_wrap::BezierCurve supCurve = it->supporting_curve();
        ASSERT_EQ(4, supCurve.number_of_control_points());

        // The original bezier should now be broken into three x-monotone sub-curves

        // ------ Mono Curve 1 -----

        std::pair<double, double> src = it->source().approximate();
        std::pair<double, double> trg = it->target().approximate();
        std::cout << "Mono Curve 1: ("
            << src.first << ", " << src.second << ") -> ("
            << trg.first << ", " << trg.second << ")\n";

        double t0 = it->parameter_range().first;
        double t = it->parameter_range().second;

        ASSERT_EQ(0.0, t0);

        cgal_wrap::BezierCurve subCurve = cubicBezierFromXMonoSection(*it);
        ASSERT_EQ(4, subCurve.number_of_control_points());

        cgal_wrap::BezierRatPoint P1 = subCurve.control_point(3);

        ++it;
        ASSERT_TRUE(it != outer.curves_end());

        // ------ Mono Curve 2 -----

        src = it->source().approximate();
        trg = it->target().approximate();
        std::cout << "Mono Curve 2: ("
            << src.first << ", " << src.second << ") -> ("
            << trg.first << ", " << trg.second << ")\n";

        double t_ = it->parameter_range().first;
        double u = it->parameter_range().second;

        ASSERT_EQ(t, t_);

        subCurve = cubicBezierFromXMonoSection(*it);
        ASSERT_EQ(4, subCurve.number_of_control_points());

        cgal_wrap::BezierRatPoint P2 = subCurve.control_point(0);
        cgal_wrap::BezierRatPoint Q1 = subCurve.control_point(3);

        std::cout << "Comparing ("
            << CGAL::to_double(P1[0]) << ", " << CGAL::to_double(P1[1]) << ") with ("
            << CGAL::to_double(P2[0]) << ", " << CGAL::to_double(P2[1]) << ")\n";

        ASSERT_EQ(Point(P1), Point(P2));

        ++it;
        ASSERT_TRUE(it != outer.curves_end());

        // ------ Mono Curve 3 -----

        src = it->source().approximate();
        trg = it->target().approximate();
        std::cout << "Mono Curve 3: ("
            << src.first << ", " << src.second << ") -> ("
            << trg.first << ", " << trg.second << ")\n";

        double u_ = it->parameter_range().first;
        double t1 = it->parameter_range().second;

        ASSERT_EQ(u, u_);
        ASSERT_EQ(1.0, t1);

        subCurve = cubicBezierFromXMonoSection(*it);
        ASSERT_EQ(4, subCurve.number_of_control_points());

        cgal_wrap::BezierRatPoint Q2 = subCurve.control_point(0);

        std::cout << "Comparing ("
            << CGAL::to_double(Q1[0]) << ", " << CGAL::to_double(Q1[1]) << ") with ("
            << CGAL::to_double(Q2[0]) << ", " << CGAL::to_double(Q2[1]) << ")\n";

        ASSERT_EQ(Point(Q1), Point(Q2));

        ++it;
        ASSERT_TRUE(it != outer.curves_end());
    }
    //-----------------

    //-------L2 -------
    {
        std::pair<double, double> src = it->source().approximate();
        std::pair<double, double> trg = it->target().approximate();

        const LineSegment* lseg = dynamic_cast<const LineSegment*>(&path[2]);
        ASSERT_TRUE(lseg != nullptr);

        cgal_wrap::BezierCurve supCurve = it->supporting_curve();
        ASSERT_EQ(2, supCurve.number_of_control_points());

        Point A(supCurve.control_point(0));
        Point B(supCurve.control_point(1));

        ASSERT_EQ(lseg->A(), A);
        ASSERT_EQ(lseg->B(), B);

        ASSERT_EQ(std::make_pair(lseg->A().x, lseg->A().y), src);
        ASSERT_EQ(std::make_pair(lseg->B().x, lseg->B().y), trg);

        double t0 = it->parameter_range().first;
        double t1 = it->parameter_range().second;

        ASSERT_EQ(0.0, t0);
        ASSERT_EQ(1.0, t1);

        ++it;
        ASSERT_TRUE(it != outer.curves_end());
    }
    //------------------

    //------- L3 -------
    {
        std::pair<double, double> src = it->source().approximate();
        std::pair<double, double> trg = it->target().approximate();

        const LineSegment* lseg = dynamic_cast<const LineSegment*>(&path[3]);
        ASSERT_TRUE(lseg != nullptr);

        cgal_wrap::BezierCurve supCurve = it->supporting_curve();
        ASSERT_EQ(2, supCurve.number_of_control_points());

        Point A(supCurve.control_point(0));
        Point B(supCurve.control_point(1));

        ASSERT_EQ(lseg->A(), A);
        ASSERT_EQ(lseg->B(), B);

        ASSERT_EQ(std::make_pair(lseg->A().x, lseg->A().y), src);
        ASSERT_EQ(std::make_pair(lseg->B().x, lseg->B().y), trg);

        double t0 = it->parameter_range().first;
        double t1 = it->parameter_range().second;

        ASSERT_EQ(0.0, t0);
        ASSERT_EQ(1.0, t1);

        ++it;
        ASSERT_TRUE(it == outer.curves_end());
    }
    //------------------
}
#endif

TEST_F(GeometryTest, pathsToPolyAndBackWithBezier) {
    Path path;
    path.append(LineSegment(Point(-10, -10), Point(10, -10)));
    path.append(CubicBezier(Point(10, -10), Point(7, -4), Point(13, 3), Point(10, 10)));
    path.append(LineSegment(Point(10, 10), Point(-10, 10)));
    path.append(LineSegment(Point(-10, 10), Point(-10, -10)));

    PathList paths1;
    paths1.push_back(path);

    cgal_wrap::PolyList polyList = toPolyList(paths1);
    ASSERT_EQ(1, polyList.size());

    PathList paths2 = toPathList(polyList);
    ASSERT_EQ(1, paths2.size());
    ASSERT_EQ(6, paths2[0].size()); // The bezier should have been split into 3 x-monotone curves

    ASSERT_EQ(paths1[0][0], paths2[0][0]);
    ASSERT_EQ(paths1[0][1].initialPoint(), paths2[0][1].initialPoint());
    ASSERT_EQ(paths1[0][1].finalPoint(), paths2[0][3].finalPoint());
    ASSERT_EQ(paths1[0][2], paths2[0][4]);
    ASSERT_EQ(paths1[0][3], paths2[0][5]);
}

TEST_F(GeometryTest, pathsToPolyAndBackWithHole) {
    Path path1;
    path1.append(LineSegment(Point(-10, -10), Point(10, -10)));
    path1.append(LineSegment(Point(10, -10), Point(10, 10)));
    path1.append(LineSegment(Point(10, 10), Point(-10, 10)));
    path1.append(LineSegment(Point(-10, 10), Point(-10, -10)));

    Path path2;
    path2.append(LineSegment(Point(-5, -5), Point(-5, 5)));
    path2.append(LineSegment(Point(-5, 5), Point(5, 5)));
    path2.append(LineSegment(Point(5, 5), Point(5, -5)));
    path2.append(LineSegment(Point(5, -5), Point(-5, -5)));

    PathList paths1;
    paths1.push_back(path1);
    paths1.push_back(path2);

    cgal_wrap::PolyList polyList = toPolyList(paths1);
    ASSERT_EQ(1, polyList.size());

    PathList paths2 = toPathList(polyList);
    ASSERT_EQ(2, paths2.size());
    ASSERT_EQ(4, paths2[0].size());
    ASSERT_EQ(4, paths2[1].size());

    for (int i = 0; i < paths1.size(); ++i) {
        for (int j = 0; j < paths1[i].size(); ++j) {
            ASSERT_EQ(paths1[i][j], paths2[i][j]);
        }
    }
}

#ifndef APPROX_BEZIERS
TEST_F(GeometryTest, aGlyphToPoly) {
    Path path1;
    path1.append(LineSegment(Point(344, 0), Point(409, 0)));
    path1.append(CubicBezier(Point(409, 0), Point(403, 24), Point(400, 68), Point(400, 161)));
    path1.append(LineSegment(Point(400, 161), Point(400, 324)));
    path1.append(CubicBezier(Point(400, 324), Point(400, 437), Point(330, 485), Point(232, 485)));
    path1.append(CubicBezier(Point(232, 485), Point(180, 485), Point(121, 472), Point(66, 437)));
    path1.append(LineSegment(Point(66, 437), Point(94, 385)));
    path1.append(CubicBezier(Point(94, 385), Point(127, 405), Point(167, 424), Point(224, 424)));
    path1.append(CubicBezier(Point(224, 424), Point(283, 424), Point(326, 392), Point(326, 320)));
    path1.append(LineSegment(Point(326, 320), Point(326, 290)));
    path1.append(LineSegment(Point(326, 290), Point(236, 287)));
    path1.append(CubicBezier(Point(236, 287), Point(188, 285), Point(150, 280), Point(118, 264)));
    path1.append(CubicBezier(Point(118, 264), Point(70, 242), Point(38, 199), Point(38, 136)));
    path1.append(CubicBezier(Point(38, 136), Point(38, 45), Point(102, -10), Point(188, -10)));
    path1.append(CubicBezier(Point(188, -10), Point(247, -10), Point(293, 18), Point(330, 53)));
    path1.append(LineSegment(Point(330, 53), Point(344, 0)));

    Path path2;
    path2.append(LineSegment(Point(326, 234), Point(326, 114)));
    path2.append(CubicBezier(Point(326, 114), Point(304, 91), Point(260, 52), Point(201, 52)));
    path2.append(CubicBezier(Point(201, 52), Point(147, 52), Point(113, 88), Point(113, 140)));
    path2.append(CubicBezier(Point(113, 140), Point(113, 171), Point(127, 198), Point(154, 213)));
    path2.append(CubicBezier(Point(154, 213), Point(175, 224), Point(202, 230), Point(243, 231)));
    path2.append(LineSegment(Point(243, 231), Point(326, 234)));

    PathList paths;
    paths.push_back(path1);
    paths.push_back(path2);

    cgal_wrap::PolyList polyList = toPolyList(paths);
    ASSERT_EQ(1, polyList.size());

    for (const cgal_wrap::BezierPolygonWithHoles& poly : polyList) {
        const cgal_wrap::BezierPolygon& outer = poly.outer_boundary();

        std::cout << "Outer boundary:\n";
        for (auto i = outer.curves_begin(); i != outer.curves_end(); ++i) {
            const cgal_wrap::BezierXMonotoneCurve& curve = *i;
            std::cout << curve << "\n";
        }

        std::cout << "Holes:\n";
        for (auto i = poly.holes_begin(); i != poly.holes_end(); ++i) {
            const cgal_wrap::BezierPolygon& hole = *i;

            std::cout << "Hole:\n";
            for (auto j = hole.curves_begin(); j != hole.curves_end(); ++j) {
                const cgal_wrap::BezierXMonotoneCurve& curve = *j;
                std::cout << curve << "\n";
            }
        }
    }
}

TEST_F(GeometryTest, bGlyphToPoly) {
    Path path1;
    path1.append(CubicBezier(Point(201, -10), Point(351, -10), Point(461, 85), Point(461, 250)));
    path1.append(CubicBezier(Point(461, 250), Point(461, 404), Point(367, 485), Point(249, 485)));
    path1.append(CubicBezier(Point(249, 485), Point(204, 485), Point(173, 475), Point(142, 463)));
    path1.append(LineSegment(Point(142, 463), Point(142, 708)));
    path1.append(LineSegment(Point(142, 708), Point(69, 708)));
    path1.append(LineSegment(Point(69, 708), Point(69, 19)));
    path1.append(CubicBezier(Point(69, 19), Point(96, 4), Point(142, -10), Point(201, -10)));

    Path path2;
    path2.append(CubicBezier(Point(211, 53), Point(191, 53), Point(167, 57), Point(142, 66)));
    path2.append(LineSegment(Point(142, 66), Point(142, 397)));
    path2.append(CubicBezier(Point(142, 397), Point(159, 404), Point(188, 422), Point(239, 422)));
    path2.append(CubicBezier(Point(239, 422), Point(335, 422), Point(385, 350), Point(385, 247)));
    path2.append(CubicBezier(Point(385, 247), Point(385, 128), Point(314, 53), Point(211, 53)));

    PathList paths;
    paths.push_back(path1);
    paths.push_back(path2);

    cgal_wrap::PolyList polyList = toPolyList(paths);
    ASSERT_EQ(1, polyList.size());

    for (const cgal_wrap::BezierPolygonWithHoles& poly : polyList) {
        const cgal_wrap::BezierPolygon& outer = poly.outer_boundary();

        std::cout << "Outer boundary:\n";
        for (auto i = outer.curves_begin(); i != outer.curves_end(); ++i) {
            const cgal_wrap::BezierXMonotoneCurve& curve = *i;
            std::cout << curve << "\n";
        }

        std::cout << "Holes:\n";
        for (auto i = poly.holes_begin(); i != poly.holes_end(); ++i) {
            const cgal_wrap::BezierPolygon& hole = *i;

            std::cout << "Hole:\n";
            for (auto j = hole.curves_begin(); j != hole.curves_end(); ++j) {
                const cgal_wrap::BezierXMonotoneCurve& curve = *j;
                std::cout << curve << "\n";
            }
        }
    }
}
#endif

TEST_F(GeometryTest, simplePathsUnion) {
    Path path1;
    path1.append(LineSegment(Point(-10, -10), Point(10, -10)));
    path1.append(LineSegment(Point(10, -10), Point(10, 10)));
    path1.append(LineSegment(Point(10, 10), Point(-10, 10)));
    path1.append(LineSegment(Point(-10, 10), Point(-10, -10)));

    Path path2;
    path2.append(LineSegment(Point(-5, -5), Point(15, -5)));
    path2.append(LineSegment(Point(15, -5), Point(15, 15)));
    path2.append(LineSegment(Point(15, 15), Point(-5, 15)));
    path2.append(LineSegment(Point(-5, 15), Point(-5, -5)));

    PathList paths1;
    paths1.push_back(path1);

    PathList paths2;
    paths2.push_back(path2);

    PathList paths3 = computeUnion(paths1, paths2);

    for (auto i : paths3) {
        std::cout << i << "\n";
    }

    ASSERT_EQ(1, paths3.size());
    ASSERT_EQ(8, paths3[0].size());

    ASSERT_EQ(LineSegment(Point(-5, 15), Point(-5, 10)), paths3[0][0]);
    ASSERT_EQ(LineSegment(Point(-5, 10), Point(-10, 10)), paths3[0][1]);
    ASSERT_EQ(LineSegment(Point(-10, 10), Point(-10, -10)), paths3[0][2]);
    ASSERT_EQ(LineSegment(Point(-10, -10), Point(10, -10)), paths3[0][3]);
    ASSERT_EQ(LineSegment(Point(10, -10), Point(10, -5)), paths3[0][4]);
    ASSERT_EQ(LineSegment(Point(10, -5), Point(15, -5)), paths3[0][5]);
    ASSERT_EQ(LineSegment(Point(15, -5), Point(15, 15)), paths3[0][6]);
    ASSERT_EQ(LineSegment(Point(15, 15), Point(-5, 15)), paths3[0][7]);
}

TEST_F(GeometryTest, pathsWithHolesUnion) {
    Path path1;
    path1.append(LineSegment(Point(-10, -10), Point(10, -10)));
    path1.append(LineSegment(Point(10, -10), Point(10, 10)));
    path1.append(LineSegment(Point(10, 10), Point(-10, 10)));
    path1.append(LineSegment(Point(-10, 10), Point(-10, -10)));

    Path path2;
    path2.append(LineSegment(Point(-5, -5), Point(-5, 5)));
    path2.append(LineSegment(Point(-5, 5), Point(5, 5)));
    path2.append(LineSegment(Point(5, 5), Point(5, -5)));
    path2.append(LineSegment(Point(5, -5), Point(-5, -5)));

    PathList paths1;
    paths1.push_back(path1);
    paths1.push_back(path2);

    Path path3;
    path3.append(LineSegment(Point(-5, -5), Point(15, -5)));
    path3.append(LineSegment(Point(15, -5), Point(15, 15)));
    path3.append(LineSegment(Point(15, 15), Point(-5, 15)));
    path3.append(LineSegment(Point(-5, 15), Point(-5, -5)));

    Path path4;
    path4.append(LineSegment(Point(0, 0), Point(0, 10)));
    path4.append(LineSegment(Point(0, 10), Point(10, 10)));
    path4.append(LineSegment(Point(10, 10), Point(10, 0)));
    path4.append(LineSegment(Point(10, 0), Point(0, 0)));

    PathList paths2;
    paths2.push_back(path3);
    paths2.push_back(path4);

    PathList paths3 = computeUnion(paths1, paths2);

    ASSERT_EQ(2, paths3.size());
    ASSERT_EQ(8, paths3[0].size());
    ASSERT_EQ(4, paths3[1].size());

    ASSERT_EQ(LineSegment(Point(-5, 15), Point(-5, 10)), paths3[0][0]);
    ASSERT_EQ(LineSegment(Point(-5, 10), Point(-10, 10)), paths3[0][1]);
    ASSERT_EQ(LineSegment(Point(-10, 10), Point(-10, -10)), paths3[0][2]);
    ASSERT_EQ(LineSegment(Point(-10, -10), Point(10, -10)), paths3[0][3]);
    ASSERT_EQ(LineSegment(Point(10, -10), Point(10, -5)), paths3[0][4]);
    ASSERT_EQ(LineSegment(Point(10, -5), Point(15, -5)), paths3[0][5]);
    ASSERT_EQ(LineSegment(Point(15, -5), Point(15, 15)), paths3[0][6]);
    ASSERT_EQ(LineSegment(Point(15, 15), Point(-5, 15)), paths3[0][7]);
    ASSERT_EQ(LineSegment(Point(5, 5), Point(5, 0)), paths3[1][0]);
    ASSERT_EQ(LineSegment(Point(5, 0), Point(0, 0)), paths3[1][1]);
    ASSERT_EQ(LineSegment(Point(0, 0), Point(0, 5)), paths3[1][2]);
    ASSERT_EQ(LineSegment(Point(0, 5), Point(5, 5)), paths3[1][3]);
}

TEST_F(GeometryTest, bezierPathsWithHolesUnion) {
    Path path1;
    path1.append(LineSegment(Point(-10, -10), Point(10, -10)));
    path1.append(LineSegment(Point(10, -10), Point(10, 10)));
    path1.append(LineSegment(Point(10, 10), Point(-10, 10)));
    path1.append(LineSegment(Point(-10, 10), Point(-10, -10)));

    Path path2;
    path2.append(LineSegment(Point(-5, -5), Point(-5, 5)));
    path2.append(LineSegment(Point(-5, 5), Point(5, 5)));
    path2.append(CubicBezier(Point(5, 5), Point(15, 2), Point(-5, -2), Point(5, -5)));
    path2.append(LineSegment(Point(5, -5), Point(-5, -5)));

    PathList paths1;
    paths1.push_back(path1);
    paths1.push_back(path2);

    Path path3;
    path3.append(LineSegment(Point(-5, -5), Point(15, -5)));
    path3.append(LineSegment(Point(15, -5), Point(15, 15)));
    path3.append(LineSegment(Point(15, 15), Point(-5, 15)));
    path3.append(LineSegment(Point(-5, 15), Point(-5, -5)));

    Path path4;
    path4.append(LineSegment(Point(0, 0), Point(0, 10)));
    path4.append(LineSegment(Point(0, 10), Point(10, 10)));
    path4.append(LineSegment(Point(10, 10), Point(10, 0)));
    path4.append(LineSegment(Point(10, 0), Point(0, 0)));

    PathList paths2;
    paths2.push_back(path3);
    paths2.push_back(path4);

    PathList paths3 = computeUnion(paths1, paths2);

    ASSERT_EQ(2, paths3.size());
    ASSERT_EQ(8, paths3[0].size());
//    ASSERT_EQ(6, paths3[1].size());
/*
    ASSERT_EQ(LineSegment(Point(15, 15), Point(-5, 15)), paths3[0][0]);
    ASSERT_EQ(LineSegment(Point(-5, 15), Point(-5, 10)), paths3[0][1]);
    ASSERT_EQ(LineSegment(Point(-5, 10), Point(-10, 10)), paths3[0][2]);
    ASSERT_EQ(LineSegment(Point(-10, 10), Point(-10, -10)), paths3[0][3]);
    ASSERT_EQ(LineSegment(Point(-10, -10), Point(10, -10)), paths3[0][4]);
    ASSERT_EQ(LineSegment(Point(10, -10), Point(10, -5)), paths3[0][5]);
    ASSERT_EQ(LineSegment(Point(10, -5), Point(15, -5)), paths3[0][6]);
    ASSERT_EQ(LineSegment(Point(15, -5), Point(15, 15)), paths3[0][7]);
    ASSERT_EQ(LineSegment(Point(0, 5), Point(5, 5)), paths3[1][0]);
    ASSERT_EQ(LineSegment(Point(5, 5), Point(5, 0)), paths3[1][1]);
    ASSERT_EQ(LineSegment(Point(5, 0), Point(0, 0)), paths3[1][2]);
    ASSERT_EQ(LineSegment(Point(0, 0), Point(0, 5)), paths3[1][3]);*/
}
