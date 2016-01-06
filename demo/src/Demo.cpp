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


void printCharstring(const Charstring& cs) {
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

static void printPolyList(const PolyList& polyList) {
    for (const cgal_wrap::BezierPolygonWithHoles& poly : polyList) {
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
}

Demo::Demo() {
    initialise();
    FLOAT_PRECISION = 20.0;

    m_scene = new QGraphicsScene;
    m_view = new QGraphicsView(m_scene);

    m_view->scale(0.5, -0.5);

    setCentralWidget(m_view);

    try {
        Charstring watermark({
0, -240, "rmoveto", 16.25, 0, "rlineto", 414.25, 440.8532110091743, "rlineto", 414.25, -440.8532110091743, "rlineto", 16.25, 0, "rlineto", -422.375, 449.5, "rlineto", 422.375, 449.5, "rlineto", -16.25, 0, "rlineto", -414.25, -440.8532110091743, "rlineto", -414.25, 440.8532110091743, "rlineto", -16.25, 0, "rlineto", 422.375, -449.5, "rlineto", -422.375, -449.5, "rlineto", 0, 240, "rmoveto"
        });

//        drawPaths(parseCharstring(CHARSTRINGS[3]));

        PathList paths1 = parseCharstring(CHARSTRINGS[6]);
        PathList paths2 = parseCharstring(watermark);

        std::cout << "PATHS 1:\n";
        for (const Path& path : paths1) {
            std::cout << "PATH:\n";
            for (const std::unique_ptr<Curve>& pCurve : path) {
                std::cout << (*pCurve) << "\n";
            }
        }

        std::cout << "PATHS 2:\n";
        for (const Path& path : paths2) {
            std::cout << "PATH:\n";
            for (const std::unique_ptr<Curve>& pCurve : path) {
                std::cout << (*pCurve) << "\n";
            }
        }

        PolyList shape1 = toPolyList(paths1);
        PolyList shape2 = toPolyList(paths2);

        std::cout << "SHAPE 1:\n";
        printPolyList(shape1);

        std::cout << "SHAPE 2:\n";
        printPolyList(shape2);

        cgal_wrap::BezierPolygonSet polySet;

        for (auto i : shape1) {
            polySet.join(i);
        }

        for (auto i : shape2) {
            polySet.join(i);
        }

        PolyList polyList;
        polySet.polygons_with_holes(std::back_inserter(polyList));

        std::cout << "SHAPE 3:\n";
        printPolyList(polyList);

        PathList result = toPathList(polyList);

        for (const Path& path : result) {
            for (const std::unique_ptr<Curve>& pCurve : path) {
                std::cout << (*pCurve) << "\n";
            }
        }

//        Charstring merged = mergeCharstrings(CHARSTRINGS[2], watermark);
//        PathList paths = parseCharstring(merged);

        drawPaths(result);

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
            std::cout << lseg << "\n";

            pp.moveTo(lseg.A().x, lseg.A().y);
            pp.lineTo(lseg.B().x, lseg.B().y);
        }
        else if (curve.type() == CubicBezier::type) {
            const CubicBezier& bezier = dynamic_cast<const CubicBezier&>(curve);
            std::cout << bezier << "\n";

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
