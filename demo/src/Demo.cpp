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

    m_scene = new QGraphicsScene;
    m_view = new QGraphicsView(m_scene);

    m_view->scale(0.5, -0.5);

    setCentralWidget(m_view);

    try {
        Charstring glyph({
            166, 243, "rmoveto", 117, 101, 64, 145, 26, 27, -1, -2, 15, "vhcurveto", -25, "vlineto", -216, -122, -64, -76, -63, -53, 41, 86, "vhcurveto", -77, -3, "rmoveto", -107, 71, -56, 62, -6, "vhcurveto", -2, "vlineto", -44, -14, -51, -7, -48, -24, "rrcurveto", -40, -20, -35, -34, -54, "vvcurveto", -108, 116, -45, 157, "vhcurveto", 189, 110, 72, 124, "hvcurveto", 60, -23, 70, 102, "vvcurveto", 291, "vlineto", 4, -30, -53, 4, -49, "hhcurveto", -199, -133, -87, -163, "hvcurveto", 189, -411, "rmoveto", -106, -91, 21, 69, 72, 100, 6, 99, 30, "hvcurveto", 81, 24, 83, 37, 36, 92, "rrcurveto", 2, "hlineto", -4, -103, 22, -37, -76, "vvcurveto", -89, -85, -46, -137, "vhcurveto", "endchar"
        });
/*
        PathList paths = parseCharstring(glyph);
        PathList linear = approx::toLinearPaths(paths);

        printPathList(linear);

        approx::cgal_approx::PolyList polyList1 = approx::toPolyList(linear);
*/

        Charstring watermark({
            0, -237, "rmoveto", 56.725, 0, "rlineto", 263.775, 404.50541269094174, "rlineto", 263.775, -404.50541269094174, "rlineto", 56.72500000000002, 0, "rlineto", -292.1375, 448.0, "rlineto", 292.1375, 448.0, "rlineto", -56.72500000000002, 0, "rlineto", -263.775, -404.50541269094174, "rlineto", -263.775, 404.50541269094174, "rlineto", -56.725, 0, "rlineto", 292.1375, -448.0, "rlineto", -292.1375, -448.0, "rlineto"
        });

        Charstring merged = mergeCharstrings(glyph, watermark);
        drawPaths(parseCharstring(merged));
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
    QPainterPath pp;

    for (const std::unique_ptr<Curve>& pCurve : path) {
        const Curve& curve = *pCurve;

        if (curve.type() == LineSegment::type) {
            const LineSegment& lseg = dynamic_cast<const LineSegment&>(curve);
//            std::cout << lseg << "\n";

            pp.moveTo(lseg.A().x, lseg.A().y);
            pp.lineTo(lseg.B().x, lseg.B().y);
        }
        else if (curve.type() == CubicBezier::type) {
            const CubicBezier& bezier = dynamic_cast<const CubicBezier&>(curve);
//            std::cout << bezier << "\n";

            pp.moveTo(bezier.A().x, bezier.A().y);
            pp.cubicTo(bezier.B().x, bezier.B().y, bezier.C().x, bezier.C().y, bezier.D().x, bezier.D().y);
        }
    }

    m_scene->addPath(pp, QPen(Qt::blue));
}

Demo::~Demo() {}

#include "Demo.moc"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Demo demo;
    demo.show();

    return app.exec();
}
