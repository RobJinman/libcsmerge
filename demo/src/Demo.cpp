#include <iostream>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <Charstrings.hpp>
#include <Geometry.hpp>
#include "Demo.hpp"
#include "Glyphs.hpp"


using namespace csmerge;
using namespace csmerge::geometry;
using namespace std;


static void printCharstring(const Charstring& cs) {
    std::cout << "[";
    for (auto i : cs) {
        switch (i.type) {
            case PS_OPERATOR:
                std::cout << i.str << " ";
                break;
            case PS_OPERAND:
                std::cout << i.num << " ";
                break;
        }
    }
    std::cout << "]\n";
}

static void printPathList(const PathList& pathList) {
    for (const Path& path : pathList) {
        std::cout << path;
    }
}

static void printPolyWithHoles(const cgal_wrap::BezierPolygonWithHoles poly) {
    const cgal_wrap::BezierPolygon& outer = poly.outer_boundary();

    std::cout << "OUTER:\n";
    for (auto i = outer.curves_begin(); i != outer.curves_end(); ++i) {
        const cgal_wrap::BezierXMonotoneCurve& curve = *i;
        std::cout << curve << "\n";
    }

    for (auto j = poly.holes_begin(); j != poly.holes_end(); ++j) {
        const cgal_wrap::BezierPolygon& hole = *j;

        std::cout << "HOLE:\n";
        for (auto i = hole.curves_begin(); i != hole.curves_end(); ++i) {
            const cgal_wrap::BezierXMonotoneCurve& curve = *i;
            std::cout << curve << "\n";
        }
    }
}

static void printPolyList(const cgal_wrap::PolyList& polyList) {
    for (const cgal_wrap::BezierPolygonWithHoles& poly : polyList) {
        printPolyWithHoles(poly);
    }
}

static void printPolySet(const cgal_wrap::BezierPolygonSet& polySet) {
    cgal_wrap::PolyList polyList;
    polySet.polygons_with_holes(std::back_inserter(polyList));

    printPolyList(polyList);
}

static cgal_wrap::BezierPolygonSet toPolySet(const cgal_wrap::PolyList& polyList) {
    cgal_wrap::BezierPolygonSet set;

    for (const cgal_wrap::BezierPolygonWithHoles& poly : polyList) {
        if (is_valid_polygon_with_holes(poly, set.traits())) {
            set.join(poly);
        }
        else {
            std::cout << "Polygon is not valid\n";
        }
    }

    return set;
}

Demo::Demo() {
    initialise();
//    FLOAT_PRECISION = 20.0;
    MAX_LSEGS_PER_BEZIER = 8;
    MIN_LSEG_LENGTH = 20;

    m_scene = new QGraphicsScene;
    m_view = new QGraphicsView(m_scene);

    m_view->scale(2.0, -2.0);

    setCentralWidget(m_view);

    try {
        Charstring glyph({
            215, 450, "rmoveto", 3, -27, 0, -81, -96, "vvcurveto", -95, -1, -98, -2, -25, "vhcurveto", -90, -3, "rlineto", -25, 265, 25, "vlineto", -92, 3, "rlineto", -1, 24, -2, 75, 110, "vvcurveto", 0, "vlineto", 94, 1, 98, 1, 21, "vhcurveto", 91, "hlineto", 16, 19, -29, -86, 32, "hvcurveto", 12, "hlineto", -7, 156, "rlineto", -5, "hlineto", -28, -11, "rlineto", -342, "hlineto", -28, 11, "rlineto", -5, "hlineto", -7, -156, "rlineto", 12, "hlineto", 86, 32, 19, 29, 16, "hhcurveto", 132, 143, "rmoveto", -39, 42, -47, 47, -36, 27, -17, -3, "rcurveline", 35, -46, 48, -76, 26, -48, "rrcurveto", -3, 10, 12, -1, 8, "hhcurveto", 8, 12, 1, 3, 10, "hvcurveto", 26, 48, 48, 76, 35, 46, -17, 3, "rcurveline", -36, -27, -47, -47, -39, -42, "rrcurveto", "endchar"
        });

        PathList paths = parseCharstring(glyph);
        std::cout << paths.size() << " paths\n";

        PathList linear = approx::toLinearPaths(paths);
        std::cout << linear.size() << " linear paths\n";

        PathList linear1;
        linear1.push_back(linear[0]);

//        PathList linear2;
//        linear2.push_back(linear[1]);

        printPathList(linear1);

        drawPaths(linear1);

        approx::cgal_approx::PolyList polyList = approx::toPolyList(linear1);
/*
        for (const approx::cgal_approx::PolygonWithHoles& poly : polyList) {
            const approx::cgal_approx::Polygon& outer = poly.outer_boundary();

            std::cout << "OUTER:\n";
            for (auto i = outer.vertices_begin(); i != outer.vertices_end(); ++i) {
                std::cout << *i << "\n";
            }

            for (auto j = poly.holes_begin(); j != poly.holes_end(); ++j) {
                const approx::cgal_approx::Polygon& hole = *j;

                std::cout << "HOLE:\n";
                for (auto i = hole.vertices_begin(); i != hole.vertices_end(); ++i) {
                    std::cout << *i << "\n";
                }
            }
        }*/
/*
        Charstring watermark({
            0, -220, "rmoveto", 26.50830707571863, 0, "rlineto", 229.49169292428138, 431.1019673401231, "rlineto", 229.49169292428138, -431.1019673401231, "rlineto", 26.50830707571862, 0, "rlineto", -242.74584646214066, 456.0, "rlineto", 242.74584646214066, 456.0, "rlineto", -26.50830707571862, 0, "rlineto", -229.49169292428138, -431.10196734012305, "rlineto", -229.49169292428138, 431.10196734012305, "rlineto", -26.50830707571863, 0, "rlineto", 242.7458464621407, -456.0, "rlineto", "endchar"
        });

        Charstring merged = mergeCharstrings(glyph, watermark);
        drawPaths(parseCharstring(merged));*/
    }
    catch (CsMergeException& ex) {
        std::cerr << ex.what() << "\n";
    }
}

void Demo::drawPaths(const PathList& paths) {
    for (const Path& path : paths) {
        drawPath(path);
    }
}

void Demo::drawPath(const Path& path) {
    int i = 0;
    for (const std::unique_ptr<Curve>& pCurve : path) {
        const Curve& curve = *pCurve;

        QPen colour;
        if (i == 0) {
            colour = QPen(Qt::red);
        }
        else {
            colour = i % 2 == 0 ? QPen(Qt::black) : QPen(Qt::blue);
        }

        if (curve.type() == LineSegment::type) {
            const LineSegment& lseg = dynamic_cast<const LineSegment&>(curve);

            QPainterPath pp;
            pp.moveTo(lseg.A().x, lseg.A().y);
            pp.lineTo(lseg.B().x, lseg.B().y);

            m_scene->addPath(pp, colour);
        }
        else if (curve.type() == CubicBezier::type) {
            const CubicBezier& bezier = dynamic_cast<const CubicBezier&>(curve);

            QPainterPath pp;
            pp.moveTo(bezier.A().x, bezier.A().y);
            pp.cubicTo(bezier.B().x, bezier.B().y, bezier.C().x, bezier.C().y, bezier.D().x, bezier.D().y);

            m_scene->addPath(pp, colour);
        }

        ++i;
    }
}

Demo::~Demo() {}

#include "Demo.moc"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Demo demo;
    demo.show();

    return app.exec();
}
