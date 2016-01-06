#ifndef __DEMO_HPP__
#define __DEMO_HPP__


#include <QMainWindow>
#include <list>
#include <Geometry.hpp>


class QGraphicsScene;
class QGraphicsView;

class Demo : public QMainWindow {
    Q_OBJECT

    public:
        Demo();

        virtual ~Demo();

    private:
        void drawPaths(const csmerge::geometry::PathList& paths);
        void drawPath(const csmerge::geometry::Path& path);

        QGraphicsScene* m_scene;
        QGraphicsView* m_view;
};


#endif
