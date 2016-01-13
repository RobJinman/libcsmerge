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

static void printPolyList(const PolyList& polyList) {
    for (const cgal_wrap::BezierPolygonWithHoles& poly : polyList) {
        printPolyWithHoles(poly);
    }
}

static void printPolySet(const cgal_wrap::BezierPolygonSet& polySet) {
    PolyList polyList;
    polySet.polygons_with_holes(std::back_inserter(polyList));

    printPolyList(polyList);
}

static cgal_wrap::BezierPolygonSet toPolySet(const PolyList& polyList) {
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
        for (int j = 0; j < 5; ++j) {
            Charstring glyph2({
                472, 368, "rmoveto", -133, 225, 258, 66, -589, -66, 257, -225, -133, -66, 133, -302, 74, 302, 133, "hlineto", "endchar"
            });

            Charstring watermark2({
                0, -237, "rmoveto", 28.3625, 0, "rlineto", 274.1375, 425.96466584292557, "rlineto", 274.13750000000005, -425.96466584292557, "rlineto", 28.362499999999955, 0, "rlineto", -288.31875, 448.0, "rlineto", 288.31875, 448.0, "rlineto", -28.362499999999955, 0, "rlineto", -274.13750000000005, -425.96466584292557, "rlineto", -274.1375, 425.96466584292557, "rlineto", -28.3625, 0, "rlineto", 288.31875, -448.0, "rlineto", -288.31875, -448.0, "rlineto", 0, 237, "rmoveto"
            });

            PolyList polyList1 = toPolyList(parseCharstring(glyph2));
            PolyList polyList2 = toPolyList(parseCharstring(watermark2));

            auto set1 = toPolySet(polyList1);
            auto set2 = toPolySet(polyList2);

            printPolySet(set1);
            printPolySet(set2);

            std::cout << "Starting union\n";

            for (int i = 0; i < 3; ++i) {
                cgal_wrap::BezierPolygonSet set3;

                set3.join(set1);
                set3.join(set2);

                if (i == 0) {
                    printPolySet(set3);
                }
            }
        }
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
