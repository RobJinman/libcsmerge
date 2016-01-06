#include <gtest/gtest.h>
#include <Charstrings.hpp>
#include <Geometry.hpp>


using namespace csmerge;
using namespace csmerge::geometry;


class CharstringTest : public testing::Test {
    public:
        virtual void SetUp() override {

        }

        virtual void TearDown() override {

        }
};


TEST_F(CharstringTest, square) {
    Charstring cs({
        -10, -10, "rmoveto",
        20, "vlineto",
        20, "hlineto",
        -20, "vlineto",
        -20, "hlineto",
        "endchar"
    });

    PathList paths = parseCharstring(cs);
    ASSERT_EQ(1, paths.size());

    const Path& path = paths.front();
    ASSERT_EQ(4, path.size());

    ASSERT_EQ(LineSegment::type, path[0].type());
    const LineSegment& lseg0 = dynamic_cast<const LineSegment&>(path[0]);
    ASSERT_EQ(Point(-10, -10), lseg0.A());
    ASSERT_EQ(Point(-10, 10), lseg0.B());

    ASSERT_EQ(LineSegment::type, path[1].type());
    const LineSegment& lseg1 = dynamic_cast<const LineSegment&>(path[1]);
    ASSERT_EQ(Point(-10, 10), lseg1.A());
    ASSERT_EQ(Point(10, 10), lseg1.B());

    ASSERT_EQ(LineSegment::type, path[2].type());
    const LineSegment& lseg2 = dynamic_cast<const LineSegment&>(path[2]);
    ASSERT_EQ(Point(10, 10), lseg2.A());
    ASSERT_EQ(Point(10, -10), lseg2.B());

    ASSERT_EQ(LineSegment::type, path[3].type());
    const LineSegment& lseg3 = dynamic_cast<const LineSegment&>(path[3]);
    ASSERT_EQ(Point(10, -10), lseg3.A());
    ASSERT_EQ(Point(-10, -10), lseg3.B());
}

TEST_F(CharstringTest, squareNoEndchar) {
    Charstring cs({
        -10, -10, "rmoveto",
        20, "vlineto",
        20, "hlineto",
        -20, "vlineto",
        -20, "hlineto"
    });

    PathList paths = parseCharstring(cs);
    ASSERT_EQ(1, paths.size());

    const Path& path = paths.front();
    ASSERT_EQ(4, path.size());

    ASSERT_EQ(LineSegment::type, path[0].type());
    const LineSegment& lseg0 = dynamic_cast<const LineSegment&>(path[0]);
    ASSERT_EQ(Point(-10, -10), lseg0.A());
    ASSERT_EQ(Point(-10, 10), lseg0.B());

    ASSERT_EQ(LineSegment::type, path[1].type());
    const LineSegment& lseg1 = dynamic_cast<const LineSegment&>(path[1]);
    ASSERT_EQ(Point(-10, 10), lseg1.A());
    ASSERT_EQ(Point(10, 10), lseg1.B());

    ASSERT_EQ(LineSegment::type, path[2].type());
    const LineSegment& lseg2 = dynamic_cast<const LineSegment&>(path[2]);
    ASSERT_EQ(Point(10, 10), lseg2.A());
    ASSERT_EQ(Point(10, -10), lseg2.B());

    ASSERT_EQ(LineSegment::type, path[3].type());
    const LineSegment& lseg3 = dynamic_cast<const LineSegment&>(path[3]);
    ASSERT_EQ(Point(10, -10), lseg3.A());
    ASSERT_EQ(Point(-10, -10), lseg3.B());
}

TEST_F(CharstringTest, squareClosePath) {
    Charstring cs({
        -10, -10, "rmoveto",
        20, "vlineto",
        20, "hlineto",
        -20, "vlineto",
        "endchar"
    });

    PathList paths = parseCharstring(cs);
    ASSERT_EQ(1, paths.size());

    const Path& path = paths.front();
    ASSERT_EQ(4, path.size());

    ASSERT_EQ(LineSegment::type, path[0].type());
    const LineSegment& lseg0 = dynamic_cast<const LineSegment&>(path[0]);
    ASSERT_EQ(Point(-10, -10), lseg0.A());
    ASSERT_EQ(Point(-10, 10), lseg0.B());

    ASSERT_EQ(LineSegment::type, path[1].type());
    const LineSegment& lseg1 = dynamic_cast<const LineSegment&>(path[1]);
    ASSERT_EQ(Point(-10, 10), lseg1.A());
    ASSERT_EQ(Point(10, 10), lseg1.B());

    ASSERT_EQ(LineSegment::type, path[2].type());
    const LineSegment& lseg2 = dynamic_cast<const LineSegment&>(path[2]);
    ASSERT_EQ(Point(10, 10), lseg2.A());
    ASSERT_EQ(Point(10, -10), lseg2.B());

    ASSERT_EQ(LineSegment::type, path[3].type());
    const LineSegment& lseg3 = dynamic_cast<const LineSegment&>(path[3]);
    ASSERT_EQ(Point(10, -10), lseg3.A());
    ASSERT_EQ(Point(-10, -10), lseg3.B());
}

TEST_F(CharstringTest, charstringToPathsAndBack) {
    Charstring cs1({
        -10, -10, "rmoveto",
        20, "vlineto",
        20, "hlineto",
        -20, "vlineto",
        -20, "hlineto",
        "endchar"
    });

    PathList paths = parseCharstring(cs1);
    Charstring cs2 = generateCharstring(paths);

    Charstring expectedCs({
        -10, -10, "rmoveto",
        0, 20, "rlineto",
        20, 0, "rlineto",
        0, -20, "rlineto",
        -20, 0, "rlineto",
        "endchar"
    });

    ASSERT_EQ(expectedCs.size(), cs2.size());
    for (int i = 0; i < expectedCs.size(); ++i) {
        ASSERT_EQ(expectedCs[i], cs2[i]);
    }
}

TEST_F(CharstringTest, watermarkToPoly) {
    Charstring watermark({
        50, -240, "rmoveto",
        32, 0, "rlineto",
        198, 415, "rlineto",
        198, -415, "rlineto",
        32, 0, "rlineto",
        -214, 449, "rlineto",
        214, 449, "rlineto",
        -32, 0, "rlineto",
        -198, -415, "rlineto",
        -198, 415, "rlineto",
        -32, 0, "rlineto",
        214, -449, "rlineto",
        "endchar"
    });

    PathList paths = parseCharstring(watermark);
    ASSERT_EQ(1, paths.size());

    for (const std::unique_ptr<Curve>& pCurve : paths[0]) {
        std::cout << (*pCurve) << "\n";
    }
}
